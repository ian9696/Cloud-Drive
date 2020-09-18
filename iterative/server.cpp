
#include<stdio.h>
#include<iostream>
#include<algorithm>
#include<string.h>
#include<set>
#include<string>
#include<unistd.h>
#include<sys/select.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
#include<vector>
#include<stdlib.h>
#include<fstream>
#include<map>
using namespace std;

const int N=1000000;
const int LISTENQ=1000000;

struct Cli
{
	int fd;
	map<string, vector<char>> M;
	Cli(int _fd=-1){fd=_fd;}
};

int main(int argc, char **argv)
{
	int listenfd, connfd, clilen, n, n2, t, cnt;
	char c, c2;
	char sendline[N], recvline[N], buf[N];

	printf("ser: socket\n");
	if((listenfd=socket(AF_INET, SOCK_STREAM, 0))<0)
	{
		printf("err socket\n");
		return 0;
	}
	struct sockaddr_in cliaddr, servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(atoi(argv[1]));
	printf("ser: bind\n");
	bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));
	printf("ser: listen\n");
	listen(listenfd, LISTENQ);
	for(; ; )
	{
		printf("ser: accept\n");
		connfd=accept(listenfd, (sockaddr*)&cliaddr, (socklen_t*)&clilen);
		Cli cli(connfd);
		printf("ser: found fd=%d\n", cli.fd);
		for(; ; )
		{
			n=read(cli.fd, recvline, N);
			if(n<=0)
			{
				printf("ser: disconnect cli\n");
				close(cli.fd);
				break;
			}
			recvline[3]=0;
			string si=recvline, si2, si3, si4, so, so2, so3, so4;
			if(si=="PUT")
			{
				printf("ser: put\n");
				int p=4;
				for(; recvline[p]!=' '; p++)
					;
				recvline[p]=0;
				string filename=recvline+4;
				int p2=p+1;
				for(; recvline[p2]!=' '; p2++)
					;
				recvline[p2]=0;
				int siz=atoi(recvline+p+1);
				printf("siz=%d\n", siz);
				cli.M[filename];
				vector<char> &data=cli.M[filename];
				data.insert(data.end(), recvline+p2+1, recvline+n);
				siz-=(n-p2-1);
				while(siz>0)
				{
					n=read(cli.fd, recvline, N);
					data.insert(data.end(), recvline, recvline+n);
						siz-=n;
				}
				printf("~siz=%d\n", siz);
				printf("ser: PUT completed\n");
			}
			else if(si=="GET")
			{
				printf("ser: get\n");
				recvline[n]=0;
				printf("get filename=%s\n", string(recvline+4).c_str());
				vector<char> &data=cli.M[string(recvline+4)];
				so=to_string(data.size())+" ";
				printf("data.size=%d\n", data.size());
				write(cli.fd, so.c_str(), so.length());
				for(int i=0; i<data.size(); i+=N)
				{
					for(int j=0; j<N && i+j<data.size(); j++)
						sendline[j]=data[i+j];
					write(cli.fd, sendline, min(N, (int)data.size()-i));
				}
				printf("ser: get completed\n");
			}
			else if(si=="LIS")
			{
				printf("ser: lis\n");
				so=to_string(cli.M.size())+" ";
				for(auto it=cli.M.begin(); it!=cli.M.end(); it++)
					so+=it->first+"\n";
				write(cli.fd, so.c_str(), so.length());
				printf("ser: lis completed\n");
			}
			else
			{
				printf("ser: error command\n");
			}
		}
	}

}
