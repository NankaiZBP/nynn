#include<nynn.h>
int main(int argc,char*argv[])
{
	char wbuff[4092];
	char *rbuff;
	size_t rbuffsize;
	uint32_t inetaddr[2];
	inet_pton(AF_INET,argv[1],&inetaddr[0]);
	inet_pton(AF_INET,argv[2],&inetaddr[1]);
	while(true){
		memset(wbuff,0,4092);
		if(cin.eof())break;
		cin.getline(wbuff,4092);
		cout<<"strlen(wbuff)="<<strlen(wbuff)<<endl;
		nynn_write(inetaddr,2,wbuff,strlen(wbuff)+1);
		if(nynn_read(&rbuff,&rbuffsize)!=0){
			cout<<"failed to read!"<<endl;
			continue;
		}
		cout<<rbuff<<endl;
		delete rbuff;
	}
	return 0;
}
