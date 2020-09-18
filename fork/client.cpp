
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

int main(int argc, char **argv)
{
	int sockfd, n, n2, t, cnt;
	char c, c2;
	FILE *fp, *fp2;
	char sendline[N], recvline[N], buf[N];
	
	if((sockfd=socket(AF_INET, SOCK_STREAM, 0))<0)
	{
		printf("err socket\n");
		return 0;
	}
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr)<=0)
	{
		printf("err inet_pton");
		close(sockfd);
		return 0;
	}
	servaddr.sin_port=htons(atoi(argv[2]));
	if(connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr))<0)
	{
		printf("err connect\n");
		close(sockfd);
		return 0;
	}
	
	for(; ; )
	{
		n=read(fileno(stdin), recvline, N);
		recvline[n-1]=0, n--;
		string si=recvline, si2, si3, si4, so, so2, so3, so4;
		if(si[0]=='P')
		{
			si=si.substr(si.find_first_of(" ")+1);
			si2=si.substr(si.find_first_of(" ")+1);
			si=si.substr(0, si.find_first_of(" "));
			fp=fopen(si.c_str(), "rb");
			vector<char> data;
			while((n2=read(fileno(fp), sendline, N))>0)
				data.insert(data.end(), sendline, sendline+n2);
			fclose(fp);
			so="PUT "+si2+" "+to_string(data.size())+" ";
			write(sockfd, so.c_str(), so.length());
			for(int i=0; i<data.size(); i+=N)
			{
				for(int j=0; j<N && i+j<data.size(); j++)
					sendline[j]=data[i+j];
				write(sockfd, sendline, min(N, (int)data.size()-i));
			}
			printf("PUT %s %s succeeded\n", si.c_str(), si2.c_str());
		}
		else if(si[0]=='G')
		{
			si=si.substr(si.find_first_of(" ")+1);
			si2=si.substr(si.find_first_of(" ")+1);
			si=si.substr(0, si.find_first_of(" "));
			fp=fopen(si2.c_str(), "wb");
			so="GET "+si;
			write(sockfd, so.c_str(), so.length());
			n=read(sockfd, recvline, N);
			int p=0;
			for(; recvline[p]!=' '; p++)
				;
			recvline[p]=0;
			int siz=stoi(string(recvline));
			write(fileno(fp), recvline+p+1, n-p-1);
			siz-=(n-p-1);
			while(siz>0)
			{
				n=read(sockfd, recvline, N);
				write(fileno(fp), recvline, n);
				siz-=n;
			}
			fclose(fp);
			printf("GET %s %s succeeded\n", si.c_str(), si2.c_str());
		}
		else if(si[0]=='L')
		{
			so="LIS ";
			write(sockfd, so.c_str(), so.length());
			n=read(sockfd, recvline, N);
			int p=0;
			for(; recvline[p]!=' '; p++)
				;
			recvline[p]=0;
			int lineno=stoi(string(recvline));
			recvline[n]=0;
			printf("%s", recvline+p+1);
			for(int i=p+1; i<n; i++)
				lineno-=recvline[i]=='\n';
			while(lineno>0)
			{
				n=read(sockfd, recvline, N);
				recvline[n]=0;
				printf("%s", recvline);
				for(int i=0; i<n; i++)
					lineno-=recvline[i]=='\n';
			}
			printf("LIST succeeded\n");
		}
		else if(si[0]=='E')
		{
			close(sockfd);
			return 0;
		}
		else
		{
			printf("err command\n");
		}
	}

}
