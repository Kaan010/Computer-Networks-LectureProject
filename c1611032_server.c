//Kaan Kalan 201611032
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#define SIZE sizeof(struct sockaddr_in)
#define MAX 50

int client[MAX];
int ActiveClients = 0;
void findMax(int *maxfd) {
    int i;
    *maxfd = client[0];
    for (i = 1; i < MAX; i++)
        if (client[i] > *maxfd)
            *maxfd = client[i];
}
int is_correct(char** id,char* taken_id,int k){
	int i;
	for(i=0;i<k;i++){
		if(strncmp(taken_id,id[i],strlen(taken_id)-1)==0)
				return i;
	}
	return -5;
}
void print_date(){
	char datetime[25];
	time_t _now;
	struct tm *now;
	time(&_now);
	now=localtime(&_now);
	strftime(datetime,25,"%Y-%m-%d %I:%M:%S %p",now);
	
	setvbuf(stdout,NULL,_IONBF,0);
	printf("[%s]",datetime);
}

int main() {
	
	char* id[100];
	char* password[100];
	int grades[100];
	memset(id,0,10*sizeof(char*));
	memset(password,0,10*sizeof(char*));
	
	char readed_text[255];
	int readed_int;
	FILE *fp=NULL;
	fp=fopen("grades.txt","r");
	
	int k=0;//taken total input number
	while(!feof(fp)){
		fscanf(fp,"%s",readed_text);
		id[k]=malloc(strlen(readed_text)+1);
		if(id[k])
			strcpy(id[k],readed_text);
		
		fscanf(fp,"%s",readed_text);
		password[k]=malloc(strlen(readed_text)+1);
		if(password[k])
			strcpy(password[k],readed_text);
			
		fscanf(fp,"%d",&grades[k]);
			
		k++;
	}
	
	
	struct sockaddr_in dest[50];
	
	int sockfd, maxfd, nread, found, i, j;
	char buf[128];
	
	fd_set fds;
	struct sockaddr_in server = { AF_INET, 2000, INADDR_ANY };
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
   		printf("Error creating SOCKET\n");
    		return (0);
	}
	if (bind(sockfd, (struct sockaddr *) &server, SIZE) == -1) {
    		printf("bind failed\n");
    		return (0);
	}
	if (listen(sockfd, MAX) == -1) {
    		printf("listen failed\n");
    		return (0);
	}
	
		
	int client_steps[100]={0};
	int cl_holder[100];
	char wrong_ID[100][100];
	
	int a;
	
	findMax(&maxfd);
	for (;;) {
		findMax(&maxfd);
		maxfd = (maxfd > sockfd ? maxfd : sockfd) + 1;
		FD_ZERO(&fds);
		FD_SET(sockfd, &fds);
		for (i = 0; i < MAX; i++)
			if (client[i] != 0)
				FD_SET(client[i], &fds);

		select(maxfd, &fds, (fd_set *) 0, (fd_set *) 0, (struct timeval *) 0);
		
		for (i = 0; i < MAX; i++)
			if (FD_ISSET(client[i], &fds)) {
				nread = recv(client[i], buf, sizeof(buf), 0);
				
				if (nread < 1) {
					close(client[i]);
					client[i] = 0;
					ActiveClients--;
				} 
				else{
					buf[nread]='\0';
					if(client_steps[i]==-1){ //kullanıcı adı yanlıs girdiyse
						a=-1;	//hatalı id
						send(client[i],&a,sizeof(int),0);
						print_date();
						printf(" incorrect username \"%.*s\" to %s\n",(int)strlen(wrong_ID[i]),wrong_ID[i],inet_ntoa(dest->sin_addr));
						client_steps[i]=0;
					}
					else if(client_steps[i]==1){ //kullanıcı adı doğru girdiyse
						if(cl_holder[i]==is_correct(password,buf,k)){	//dogru girdi ise (cl_holder'da 													//id'yi buldugu id indexi var
							send(client[i],&grades[cl_holder[i]],sizeof(int),0);
							print_date();
							printf(" replied grade for %s to %s\n",id[cl_holder[i]],inet_ntoa(dest->sin_addr));
							close(client[i]);
							client[i] = 0;
							ActiveClients--;
							client_steps[i]=-2;
						}
						else{	
							a=-2;	//hatalı password
							send(client[i],&a,sizeof(int),0);
							print_date();
							printf(" incorrect password \"%.*s\" for \"%s\" to %s\n",(int)strlen(buf)-1,buf,id[cl_holder[i]],inet_ntoa(dest->sin_addr));
							
						}
					}
					else if(client_steps[i]==0){ //kullanıcı adı girmediyse
						cl_holder[i]=is_correct(id,buf,k);
						if(cl_holder[i]!=-5)
							client_steps[i]=1;
						else{
							client_steps[i]=-1;
							strncpy(wrong_ID[i],buf,strlen(buf)-1);
						}
						a=-3;
						send(client[i],&a,sizeof(int),0);
					}
					else continue;			
				}	
			
			}			
			
		/* if there is a request for a new connection */	
		if (FD_ISSET(sockfd, &fds)) {
			if (ActiveClients < MAX) {
				found = 0;
				for (i = 0; i < MAX && !found; i++){
					if (client[i] == 0) {
						socklen_t socksize=sizeof(struct sockaddr_in);	
						client[i] = accept(sockfd, (struct sockaddr *)&dest[i], &socksize);//client addresini alabilmek için NULL değilde bu şekilde bi dest struct'ı tanımladık.
						found = 1;
						ActiveClients++;
						client_steps[i]=0;
					}
				}
			}  
		}
    	}
    	
}

