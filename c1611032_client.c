#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>

#define SIZE sizeof(struct sockaddr_in)

int main() {
	int sockfd, nread;
	char buf[128], enter, resp;
	int buf_int;
	fd_set fds;
	char IP[20];
	struct sockaddr_in server = { AF_INET, 2000 };
	setvbuf(stdout,NULL,_IONBF,0);
	printf("\nEnter server address:");
	scanf("%s%c", IP, &enter);
	server.sin_addr.s_addr = inet_addr(IP);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("Error creating SOCKET\n");
		return (0);
	}

	if (connect(sockfd, (struct sockaddr *) &server, SIZE) == -1) {
		printf("Connect failed\n");
		return (0);
	}
	printf("Connected to the server.\n");
	setvbuf(stdout,NULL,_IONBF,0);
	printf("Username:");
	
	do {
		
		FD_ZERO(&fds);
		FD_SET(sockfd, &fds);
		FD_SET(0, &fds);
		/* Wait for some input. */
		select(sockfd + 1, &fds, (fd_set *) 0, (fd_set *) 0, (struct timeval *) 0);

		/* If either device has some input,read it and copy it to the other. */
		if (FD_ISSET(sockfd, &fds)) {
			nread = recv(sockfd, &buf_int, sizeof(buf_int), 0);
			/* If error or eof, terminate. */
			if (nread < 1) {
				close(sockfd);
				exit(0);
			}
			//buf[nread] = 0;
			
			if(buf_int==-3){
				setvbuf(stdout,NULL,_IONBF,0);
				printf("Password:");
			}
			else if(buf_int==-2){
				printf("Password is incorrect, Please retry.\n");
				setvbuf(stdout,NULL,_IONBF,0);
				printf("Password:");
			}
			else if(buf_int==-1){
				printf("Username does not exist.\n");
				setvbuf(stdout,NULL,_IONBF,0);
				printf("Username:");
			}
			else{
				printf("Your grade is %d.\nBye.\n",buf_int);
				close(sockfd);
				exit(0);
			}
			
		}
		
		if (FD_ISSET(0, &fds)) {
			
			nread = read(0, buf, sizeof(buf));
			/* If error or eof, terminate. */
			if (nread < 1) {
				close(sockfd);
				exit(0);
			} else if ((buf[0] == 'e' || buf[0] == 'E') && nread == 2) {
				close(sockfd);
				exit(0);
			} else
				send(sockfd, buf, nread, 0);
		}
		
	} while (1);
}
