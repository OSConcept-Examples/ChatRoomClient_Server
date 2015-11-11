/**********************************************************************
* Chatroom|Client
* version: 2.0
* November, 6th, 2015
*
* This lib was written by thangdn
* Contact:thangdn.tlu@outlook.com
*
* Every comment would be appreciated.
*
* If you want to use parts of any code of mine:
* let me know and
* use it!
**********************************************************************/
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
void GetString(char *s){
	int i=0;
	char ch;
	do
	{
		read(1,&ch,1);
		s[i++]=ch;
	}while(ch!='\n');
	s[--i]='\0';
}
struct Identification{
	int flag;
	char NickName[100];
};
int Client_id;
int Sin;
time_t d;
void termination_handler(){
	write(Client_id,"\\q",strlen("\\q"));
	close(Client_id);
	close(Sin);
	exit(0);
}

void interact_function(void *data){
	struct sigaction handler;
	handler.sa_handler=termination_handler;
	handler.sa_flags=0;
	sigemptyset(&handler.sa_mask);
	sigaction(SIGINT,&handler,NULL);

	int *Client_id = (int *)data;
	while(1){
		char ch[100];
		if(read(*Client_id,&ch,100)>0)
		{
			if(ch[0]=='\\'&&ch[1]=='q'){
				close(Client_id);
				close(Sin);
				pthread_exit(0);
				return;
			}
			write(Sin,&ch,strlen(ch));
			write(Sin,"\n",strlen("\n"));
			write(1,&ch,strlen(ch));
			write(1,"\n",strlen("\n"));
		}
	}
	close(Client_id);
}
int main(int argc, char const *argv[])
{

	if(argc!=4){
		printf("Entry again: %s <IP Address> <port> <Your NickName>\n",argv[0] );
		return 1;
	}
	Client_id=socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in Client_addr;
	Client_addr.sin_family=AF_INET;
	Client_addr.sin_port=atoi(argv[2]);
	inet_pton(AF_INET,argv[1],&Client_addr.sin_addr);
	int connects=connect(Client_id,(struct sockaddr*)&Client_addr,sizeof(Client_addr));
	if(connects<0)
	{
		printf("Could not connect to server!\n");
		return 0;
	}
	pthread_t tid;
	pthread_create(&tid,NULL,interact_function,&Client_id);
	struct Identification t;
	t.flag=1;
	strcpy(t.NickName,argv[3]);
	write(Client_id,&t,sizeof(t));
	Sin= open("mychat.log",O_RDWR|O_CREAT,0666);
	while(1){
		char ch[100];
		write(1,argv[3],strlen(argv[3]));
		write(1,": ",strlen(": "));
		GetString(ch);
		if(strcmp(ch,"\0")==0) continue;
		else
		{
			write(Client_id,&ch,100);
			d= time(NULL);
			struct tm *tm = localtime(&d);
			char message[100];
			sprintf(message,">>%s - %s: %s\n",asctime(tm),t.NickName,ch);
			write(Sin,&message,strlen(message));
		}
		if(strcmp(ch,"\\q")==0){
			break;
		}
	}
	close(Client_id);
	close(Sin);
	return 0;
}
