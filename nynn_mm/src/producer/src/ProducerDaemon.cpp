// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "Producer.h"
#include <nynn_mm_config.h>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/concurrency/Thread.h>
#include <thrift/concurrency/PosixThreadFactory.h>
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/server/TNonblockingServer.h>


using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;

using boost::shared_ptr;

using namespace  ::nynn::mm;

class ProducerHandler : virtual public ProducerIf {
 public:
  ProducerHandler() {
    // Your initialization goes here
  }

  bool report(const std::vector<int32_t> & subgraphKeys, const std::string& hostname) {
    // Your implementation goes here
    printf("report\n");
  }

  void getHost(std::string& _return, const int32_t subgraphKey) {
    // Your implementation goes here
    printf("getHost\n");
  }

  int32_t insertPrev(const int32_t vtxno, const int32_t nextBlkno, const std::vector<int8_t> & xblk) {
    // Your implementation goes here
    printf("insertPrev\n");
  }

  int32_t insertNext(const int32_t vtxno, const int32_t prevBlkno, const std::vector<int8_t> & xblk) {
    // Your implementation goes here
    printf("insertNext\n");
  }

  bool remove(const int32_t vtxno, const int32_t blkno) {
    // Your implementation goes here
    printf("remove\n");
  }

  int32_t unshift(const int32_t vtxno, const std::vector<int8_t> & newXHeadBlk) {
    // Your implementation goes here
    printf("unshift\n");
  }

  bool shift(const int32_t vtxno) {
    // Your implementation goes here
    printf("shift\n");
  }

  int32_t push(const int32_t vtxno, const std::vector<int8_t> & newXTailBlk) {
    // Your implementation goes here
    printf("push\n");
  }

  bool pop(const int32_t vtxno) {
    // Your implementation goes here
    printf("pop\n");
  }

};

int main(int argc, char **argv) {
	int port =strtoul(getenv("PRODUCER_PORT"),NULL,0);

	boost::shared_ptr<ProducerHandler> handler(new ProducerHandler());
	boost::shared_ptr<TProcessor> processor(new ProducerProcessor(handler));
	//boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
	boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
	boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
	boost::shared_ptr<ThreadFactory> pthreadFactory(new PosixThreadFactory());

	boost::shared_ptr<ThreadManager> threadManager=ThreadManager::newSimpleThreadManager(10,4);
	threadManager->threadFactory(pthreadFactory);

	TNonblockingServer server(processor,transportFactory,transportFactory,protocolFactory,protocolFactory,port,threadManager);
	threadManager->start();
	server.serve();
	return 0;
}
