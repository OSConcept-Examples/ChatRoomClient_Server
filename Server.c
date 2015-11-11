/**********************************************************************
* Chatroom|Server
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
struct View_t{
	int View_id[100];
	int Count;
};
struct Client_t{
	int Client_id;
	char NickName[100];
};
struct Identification{
	int flag;
	char NickName[100];
};
struct View_t vt;
int Server_id;
int Sin;
time_t t;
void termination_handler(){
	char ch[100];
	strcpy(ch,"\\q");
	int i;
	for(i=0;i<vt.Count;i++){
		write(vt.View_id[i],&ch,100);
		close(vt.View_id[i]);
	}
	close(Server_id);
	close(Sin);
	exit(0);
}
void *interact_function(void *data){
	struct sigaction handler;
	handler.sa_handler=termination_handler;
	handler.sa_flags=0;
	sigemptyset(&handler.sa_mask);
	sigaction(SIGINT,&handler,NULL);

	struct Client_t ct=*(struct Client_t*)data;
	char messages[100];
	while(1){
		char message[100];
		read(ct.Client_id,&message,100);
		if(message[0]=='\\'&&message[1]=='q'){
			t= time(NULL);
			struct tm *tm = localtime(&t);
			sprintf(messages,">>%s - %s currently offline",asctime(tm),ct.NickName);
			printf("%s\n",messages);
			write(Sin,messages,strlen(messages));
			write(Sin,"\n",strlen("\n"));
			int i;
			for(i=0;i<vt.Count;i++){
				write(vt.View_id[i],&messages,100);
			}
			close(ct.Client_id);
			close(Sin);
			pthread_exit(0);
			break;
		}else
		{
			int i;
			t= time(NULL);
			struct tm *tm = localtime(&t);
			sprintf(messages,">>%s - %s: %s",asctime(tm),ct.NickName,message);
			write(Sin,messages,strlen(messages));
			write(Sin,"\n",strlen("\n"));
			for(i=0;i<vt.Count;i++){
				if(vt.View_id[i]!=ct.Client_id)
					write(vt.View_id[i],&messages,100);
			}
		}
	}
}
void Nofication(char *NickName){
	char messages[100];
	t= time(NULL);
	struct tm *tm = localtime(&t);
	sprintf(messages,">>%s - %s currently online",asctime(tm),NickName);
	write(Sin,messages,strlen(messages));
	write(Sin,"\n",strlen("\n"));
	printf("%s\n",messages);
	int i;
	for(i=0;i<vt.Count;i++){
		write(vt.View_id[i],&messages,100);
	}
}
int main(int argc, char const *argv[])
{
	if(argc!=2){
		printf("Retry again: %s <port>\n",argv[0] );
		return 1;
	}
	int Client_View_id;
	Server_id=socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in Server_addr,Client_View_addr;
	Server_addr.sin_family=AF_INET;
	Server_addr.sin_port=atoi(argv[1]);
	Server_addr.sin_addr.s_addr=INADDR_ANY;
	bind(Server_id,(struct sockaddr*)&Server_addr,sizeof(Server_addr));
	listen(Server_id,10);
	pthread_t tid;
	struct Client_t ct;
	vt.Count=0;
	char temp[100];
	struct Identification t;
	Sin= open("chat.log",O_RDWR|O_CREAT,0666);
	while(1){
		int Client_View_len;
		Client_View_id=accept(Server_id,(struct sockaddr*)&Client_View_addr,&Client_View_len);
		int flag;
		read(Client_View_id,&t,sizeof(t));
		if(t.flag==1){
			ct.Client_id=Client_View_id;
			strcpy(ct.NickName,t.NickName);
			Nofication(t.NickName);
			vt.View_id[vt.Count++]=Client_View_id;
			pthread_create(&tid,NULL,interact_function,(void *)&ct);
		}
	}
	return 0;
}
