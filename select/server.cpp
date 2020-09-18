
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
	int listenfd, connfd, clilen, maxfdp1, n, n2, t, cnt;
	char c, c2;
	fd_set rset;
	vector<Cli> v;
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
		FD_ZERO(&rset);
		FD_SET(listenfd, &rset);
		maxfdp1=listenfd+1;
		for(int i=0; i<v.size(); i++)
			FD_SET(v[i].fd, &rset), maxfdp1=max(maxfdp1, v[i].fd+1);
		printf("ser: select\n");
		select(maxfdp1, &rset, NULL, NULL, NULL);
		if(FD_ISSET(listenfd, &rset))
		{
			printf("ser: accept\n");
			connfd=accept(listenfd, (sockaddr*)&cliaddr, (socklen_t*)&clilen);
			v.push_back(connfd);
		}
		for(int i=0; i<v.size(); i++)
		{
			printf("ser: check i=%d fd=%d vsize=%d\n", i, v[i].fd, v.size());
			if(FD_ISSET(v[i].fd, &rset))
			{
				printf("ser: found i=%d fd=%d\n", i, v[i].fd);
				n=read(v[i].fd, recvline, N);
				if(n<=0)
				{
					printf("ser: disconnect cli\n");
					close(v[i].fd);
					swap(v[i], v.back());
					v.pop_back();
					i--;
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
					v[i].M[filename];
					vector<char> &data=v[i].M[filename];
					data.insert(data.end(), recvline+p2+1, recvline+n);
					siz-=(n-p2-1);
					while(siz>0)
					{
						n=read(v[i].fd, recvline, N);
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
					vector<char> &data=v[i].M[string(recvline+4)];
					so=to_string(data.size())+" ";
					printf("data.size=%d\n", data.size());
					write(v[i].fd, so.c_str(), so.length());
					for(int ii=0; ii<data.size(); ii+=N)
					{
						for(int j=0; j<N && ii+j<data.size(); j++)
							sendline[j]=data[ii+j];
						write(v[i].fd, sendline, min(N, (int)data.size()-ii));
					}
					printf("ser: get completed\n");
				}
				else if(si=="LIS")
				{
					printf("ser: lis\n");
					so=to_string(v[i].M.size())+" ";
					for(auto it=v[i].M.begin(); it!=v[i].M.end(); it++)
						so+=it->first+"\n";
					write(v[i].fd, so.c_str(), so.length());
					printf("ser: lis completed\n");
				}
				else
				{
					printf("ser: error command\n");
				}
				break;
			}
		}
	}

}
