#ifndef NYNN_MM_SUBGRAPH_STORAGE_BY_SATANSON
#define NYNN_MM_SUBGRAPH_STORAGE_BY_SATANSON
#include<nynn_mm_common.h>
#include<nynn_mm_types.h>
using namespace nynn::mm::common;
using namespace nynn::mm;
namespace nynn{namespace mm{

struct Vertex;
struct Edge;

template<
	uint32_t LOG2_BLOCKSZ,
	uint32_t LOG2_OVERFLOW_BLOCKSZ,
	uint32_t OVERFLOW_BLOCKNO_MIN,
	uint32_t BLOCKNO_MAX,
	uint32_t VERTEX_INTERVAL_WIDTH,
	uint32_t OVERFLOW_NUM,
	uint32_t MONITOR_NUM
>class SubgraphStorage;


/* blkno(block number)space is [0,BLOCKNO_MAX).
 * BLOCKNO_MAX<=(1<<32)-1.
 * INVALID_BLOCKNO=(1<<32)-1	 
 * [0,OVERFLOW_BLOCKNO_MIN) mapped into volume.
 * [OVERFLOW_BLOCKNO_MIN,BLOCKNO_MAX) mapped into overflow.
 *  */
template<
	uint32_t LOG2_BLOCKSZ,
	uint32_t LOG2_OVERFLOW_BLOCKSZ,
	uint32_t OVERFLOW_BLOCKNO_MIN,
	uint32_t BLOCKNO_MAX,
	uint32_t VERTEX_INTERVAL_WIDTH,
	uint32_t OVERFLOW_NUM,
	uint32_t MONITOR_NUM
>class SubgraphStorage{
public:
	static uint32_t const INVALID_BLOCKNO=~0L;
	static uint32_t const INVALID_VERTEXNO=~0L;
	static uint32_t const BLOCKSZ=1<<LOG2_BLOCKSZ;
	static uint32_t const OVERFLOW_BLOCKSZ=1<<LOG2_OVERFLOW_BLOCKSZ;
	static uint32_t const OVERFLOW_BLOCKNO_MASK=(1<<LOG2_OVERFLOW_BLOCKSZ-LOG2_BLOCKSZ)-1;
	static uint32_t const VOLUME_SIZE=OVERFLOW_BLOCKNO_MIN*BLOCKSZ;

	typedef nynn::mm::BlockType<BLOCKSZ> Block;

	struct SuperBlock
	{ 
		struct Data{
			uint32_t m_numOfFreeBlks;
			uint32_t m_headBlkno;
			uint32_t m_supBlkno;
			Vertex   m_vertices[VERTEX_INTERVAL_WIDTH];
		}*m_data;

		explicit SuperBlock(void *superblk=NULL ) { loadData(superblk); }
		
		static uint32_t const SIZE=sizeof(Data);
		void     loadData(void*superblk) { m_data=static_cast<Data*>(superblk); }
		uint32_t getSupBlkno()const { return m_data->m_supBlkno; }
		void     setSupBlkno(uint32_t blkno){ m_data->m_supBlkno=blkno; }
		uint32_t getHeadBlkno()const { return m_data->m_headBlkno; }
		void     setHeadBlkno(uint32_t blkno) { m_data->m_headBlkno=blkno; }
		void     resetNumberOfFreeBlks() { m_data->m_numOfFreeBlks=0; }
		void     incNumberOfFreeBlks() { m_data->m_numOfFreeBlks++; }
		void     decNumberOfFreeBlks() { m_data->m_numOfFreeBlks--; }
		uint32_t getNumberOfFreeBlks() { return m_data->m_numOfFreeBlks;}
		uint32_t getMinVtxno()const { return m_data->m_vertices[0].m_source; }
		
		void     setMinVtxno(uint32_t minVtxno)
		{
			if (minVtxno%VERTEX_INTERVAL_WIDTH!=0){
				throwNynnException("Minimal vertex no must be multiple of VERTEX_INTERVAL_WIDTH!");
			}			
			for (uint32_t vtxno=minVtxno;vtxno<minVtxno+VERTEX_INTERVAL_WIDTH;vtxno++){
				m_data->m_vertices[vtxno-minVtxno]=Vertex(vtxno);
			}
		}

		bool containVertex(uint32_t vtxno)
		{
			uint32_t minVtxno=getMinVtxno();
			return minVtxno<=vtxno && vtxno<minVtxno+VERTEX_INTERVAL_WIDTH;
		}

		Vertex& getVertex(uint32_t vtxno){
			if (!containVertex(vtxno)){
				throwNynnException("No specified vertex is found in this subgraph!");
			}
			return m_data->m_vertices[vtxno-getMinVtxno()];			
		}

		uint32_t getNumberOfEdgesByVtxno(uint32_t vtxno) { return getVertex(vtxno).m_nedges; }
		uint32_t getHeadBlknoByVtxno(uint32_t vtxno) { return getVertex(vtxno).m_blkno; }
		uint32_t getDataByVtxno(uint32_t vtxno) { return getVertex(vtxno).m_data; }
	};

	struct Overflow{
		struct OverflowEntry
		{
			uint32_t m_overflowBlkno;
			uint32_t m_hit;
			void*    m_overflowBlk;
			OverflowEntry(uint32_t no=INVALID_BLOCKNO,void* addr=0):
				m_overflowBlkno(no),m_hit(0),m_overflowBlk(addr){}
		};
		OverflowEntry m_table[OVERFLOW_NUM];

		static uint32_t const SIZE=OVERFLOW_NUM;
		static uint32_t mapping(uint32_t x)
		{
			uint32_t y=x;
			y  -= OVERFLOW_BLOCKNO_MIN;
			y >>= LOG2_OVERFLOW_BLOCKSZ-LOG2_BLOCKSZ;
			y  %= Overflow::SIZE;
			return y;
		}	
		void* get(uint32_t blkno)
		{
			uint32_t idx=mapping(blkno);
			if (m_table[idx].m_overflowBlkno==blkno) {
				m_table[idx].m_hit++;
				return m_table[idx].m_overflowBlk; 
			}
			
			return NULL;
		}
		void set(uint32_t blkno,void*newOverflowBlk)
		{
			uint32_t idx=mapping(blkno);
			void* oldOverflowBlk=m_table[idx].m_overflowBlk;
			if (oldOverflowBlk!=NULL){
				munmap(oldOverflowBlk,OVERFLOW_BLOCKSZ);
			}
			m_table[idx].m_overflowBlkno=blkno;
			m_table[idx].m_overflowBlk=newOverflowBlk;
			m_table[idx].m_hit=0;
		}
	};


//------------------------------------------------------------------------------
	SubgraphStorage(const string &basedir):m_basedir(basedir)
	{
		try{
			// initialize mutexes.

			string superblkPath=basedir+"/superblock.dat";
			string volumePath=basedir+"/volume.dat";
			string overflowPath=basedir+"/overflow";

			MmapFile superblkFile(superblkPath);
			MmapFile volumeFile(volumePath);
			// initialize superblock.
			m_superblk.loadData(superblkFile.getBaseAddress());

			// initialize overflow.
			glob_t g;
			g.gl_offs=0;
			string overflowBlkPathPattern=overflowPath+"/block*.dat";

			int retcode=glob(overflowBlkPathPattern.c_str(),0,0,&g);
			if (retcode!=0 && retcode!=GLOB_NOMATCH){
				string errinfo=string("Fail to invoke glob!")+"("+strerr(errno)+")";
				throwNynnException(errinfo.c_str());
			}
			uint32_t numOfPreloadedOverflowBlk=OVERFLOW_NUM<g.gl_pathc?
				                               OVERFLOW_NUM:g.gl_pathc;
			for (uint32_t i=0;i<numOfPreloadedOverflowBlk;i++){
				const char* path=g.gl_pathv[i];
				const char* strBlkno=path+strlen(path)-strlen("0xffff0000.dat");
				uint32_t blkno=strtoul(strBlkno,NULL,16);

				MmapFile overflowBlkFile(path);
				setOverflowBlock(blkno,overflowBlkFile.getBaseAddress());
			}
			globfree(&g);
			// initialize volume. 
			m_volume = volumeFile.getBaseAddress();
			
		}catch(NynnException &err){
			throwNynnException("Fail to construct SubgraphStorage object!");
		}
	}

	uint32_t require()
	{
		nynn::mm::common::ExclusiveSynchronization es(&m_superblkRWLock);

		uint32_t blkno=requireAllocated();
		return blkno==INVALID_BLOCKNO?requireUnallocated():blkno;
	}

	void release(uint32_t blkno)
	{
		nynn::mm::common::ExclusiveSynchronization es(&m_superblkRWLock);
		releaseRaw(blkno);
	}

	void readBlock(uint32_t blkno,Block& blk)
	{
		nynn::mm::common::SharedSynchronization ss(&m_superblkRWLock);
		nynn::mm::common::Synchronization s(&m_monitors[blkno%MONITOR_NUM]);
		Block *srcBlk=getBlock(blkno);
		if (srcBlk==NULL)throwNynnException("Fail to get specified block(getBlock)!");
		memcpy(&blk,srcBlk,sizeof(Block));
	}
	
	void writeBlock(uint32_t blkno,Block& blk)
	{
		nynn::mm::common::SharedSynchronization ss(&m_superblkRWLock);
		nynn::mm::common::Synchronization s(&m_monitors[blkno%MONITOR_NUM]);
		Block *destBlk=getBlock(blkno);
		if (destBlk==NULL)throwNynnException("Fail to get specified block(getBlock)!");
		memcpy(destBlk,&blk,sizeof(Block));
	}

	/*"SuperBlock" /path/to/basedir/superblock.dat
	 *"Volume"     /path/to/basedir/volume.dat
	 *"Overflow"   /path/to/basedir/overflow/block0xabcdabcd.dat
	 * */
	static void format(const string &path)
	{
		try{
			string superblkPath=path+"/superblock.dat";
			string volumePath=path+"/volume.dat";
			cout<<superblkPath<<"("<<SuperBlock::SIZE<<")"<<endl;
			cout<<volumePath<<"("<<VOLUME_SIZE<<")"<<endl;
			MmapFile superblkFile(superblkPath,SuperBlock::SIZE);
			MmapFile volumeFile(volumePath,VOLUME_SIZE);
			string overflowDir=path+"/overflow";
			if (mkdir(overflowDir.c_str(),0770)!=0) {
				throwNynnException(strerr(errno).c_str());
			}
			void *superblk = superblkFile.getBaseAddress();
			void *volume = volumeFile.getBaseAddress();

			SubgraphStorage subgraph(path,superblk,volume);	
		}catch(NynnException &err){
			throwNynnException(err.what());
		}
	}

private:
	SubgraphStorage(const string &basedir,void* superblk,void* volume)
	try
	:m_basedir(basedir),m_superblk(superblk),m_volume(volume)
	{

		m_superblk.resetNumberOfFreeBlks();
		m_superblk.setHeadBlkno(INVALID_BLOCKNO);
		m_superblk.setSupBlkno(OVERFLOW_BLOCKNO_MIN);
		for (uint32_t i=OVERFLOW_BLOCKNO_MIN;i>0;i--)release(i-1);

		const char *path=basedir.c_str();
		const char *strMinVtxno=path+strlen(path)-strlen("0xffff0000");
		uint32_t minVtxno=strtoul(strMinVtxno,NULL,16);
		m_superblk.setMinVtxno(minVtxno);

		cout<<"format subgraph:"<<endl;
		cout<<"blocksz:("<<BLOCKSZ<<")"<<endl;
		cout<<"overflow_blocksz:("<<OVERFLOW_BLOCKSZ<<")"<<endl;
		cout<<"overflow_blockno_min:("<<OVERFLOW_BLOCKNO_MIN<<")"<<endl;
		cout<<"superblk:("<<SuperBlock::SIZE<<")"<<endl;
		cout<<"volume:("<<VOLUME_SIZE<<"/"<<OVERFLOW_BLOCKNO_MIN<<")"<<endl;
		cout<<"\tfree blocks:("<<m_superblk.getNumberOfFreeBlks()<<")"<<endl;
		cout<<"\thead blkno:("<<m_superblk.getHeadBlkno()<<")"<<endl;
		cout<<"\tsup blkno:("<<m_superblk.getSupBlkno()<<")"<<endl;
	}catch (NynnException &err){
		throwNynnException("Fail to initialize new SubgraphStorage object!");
	}

	bool isFlat(uint32_t blkno) 
	{ 
		return 0<=blkno && blkno<=OVERFLOW_BLOCKNO_MIN; 
	}

	bool isOverflow(uint32_t blkno) 
	{ 
		return OVERFLOW_BLOCKNO_MIN<=blkno && blkno<BLOCKNO_MAX; 
	}

	
	bool isInvalid(uint32_t blkno) 
	{ 
		return BLOCKNO_MAX<=blkno<=INVALID_BLOCKNO; 
	} 

	Block*  getBlock(uint32_t blkno)
	{
		if (isFlat(blkno)) {
			return getVolumeBlock(blkno);
		}else if (isOverflow(blkno) && blkno<m_superblk.getSupBlkno() ){
			void *overflowBlk=getOverflowBlock(blkno);
			if (overflowBlk==NULL){
				//replace old overflow block with new one.
				MmapFile mf(getOverflowBlkPath(blkno));
				overflowBlk=mf.getBaseAddress();
				setOverflowBlock(blkno,overflowBlk);
			}
			return static_cast<Block*>(overflowBlk)+(blkno&OVERFLOW_BLOCKNO_MASK);
		}else {
			return NULL;
		}
	}
	

	string   getOverflowBlkPath(uint32_t blkno)
	{
		if (isOverflow(blkno)) {
			stringstream ss;
			blkno&=~OVERFLOW_BLOCKNO_MASK;
			ss<<m_basedir<<"/overflow/block0x";
			ss<<hex<<nouppercase<<setw(8)<<setfill('0');
			ss<<blkno<<".dat";
			return ss.str();

		}
		log_d("blkno=%d",blkno);
		throwNynnException("blkno is invalid or not overflow");
	}


	Block* getVolumeBlock(uint32_t blkno)
	{
		return static_cast<Block*>(m_volume)+blkno;
	}
	/* get Overflow Block.
	 * policy 1: no cache.
	 * policy 2: direct mapping cache
	 * policy 3: full associative cache
	 * policy 4: grouped mmapping cache
	 * policy 5: LRU cache
	 * policy 6: read all overflow blocks in.
	 * */
	void*  getOverflowBlock(uint32_t blkno)
	{
		return m_overflow.get(blkno&~OVERFLOW_BLOCKNO_MASK);
	}

	void   setOverflowBlock(uint32_t blkno,void* newOverflowBlk)
	{
		m_overflow.set(blkno&~OVERFLOW_BLOCKNO_MASK,newOverflowBlk);
	}

	uint32_t requireAllocated()
	{
		uint32_t headBlkno=m_superblk.getHeadBlkno();
		//allocated space is exhausted.
		if (headBlkno == INVALID_BLOCKNO){
			return INVALID_BLOCKNO;
		}

		//head block available when reachs its bottom.
		m_superblk.decNumberOfFreeBlks();
		Block *headBlk=getBlock(headBlkno);
		if (headBlk->isAtBottom()){
			m_superblk.setHeadBlkno(getBlock(headBlkno)->popIndex());
			return headBlkno;
		}
		//head block has more than one indexes.
		return headBlk->popIndex();
	}

	uint32_t requireUnallocated()
	{
		uint32_t supBlkno=m_superblk.getSupBlkno();
		string path=getOverflowBlkPath(supBlkno);
		MmapFile mf(path.c_str(),OVERFLOW_BLOCKSZ);
		void* overflowBlk=mf.getBaseAddress();
		setOverflowBlock(supBlkno,overflowBlk);
		m_superblk.setSupBlkno(supBlkno+OVERFLOW_BLOCKNO_MASK+1);
		//insert free block into free list in reverse older.	
		for (uint32_t i=0;i<OVERFLOW_BLOCKNO_MASK+1;i++){
			releaseRaw(supBlkno+OVERFLOW_BLOCKNO_MASK-i);	
		}

		return requireAllocated();
	}

	void releaseRaw(uint32_t blkno)
	{
		uint32_t headBlkno=m_superblk.getHeadBlkno();
		Block *blk=getBlock(blkno);
		if (blk==NULL){
			throwNynnException("The released block is NULL!");
		}
		//free list is empty or its head block is full.
		if (headBlkno==INVALID_BLOCKNO||getBlock(headBlkno)->isAtTop()) {
			blk->initIndexBlock();
			blk->pushIndex(headBlkno);
			m_superblk.setHeadBlkno(blkno);
		//head block is not full.
		}else{
		 	getBlock(headBlkno)->pushIndex(blkno);
		}
		m_superblk.incNumberOfFreeBlks();
		return;

	}
	
private:
	string      m_basedir;
	SuperBlock  m_superblk; 
	void*       m_volume;
	Overflow    m_overflow;
	RWLock 	    m_superblkRWLock;
	Monitor 	m_monitors[MONITOR_NUM];
};

}}
#endif

