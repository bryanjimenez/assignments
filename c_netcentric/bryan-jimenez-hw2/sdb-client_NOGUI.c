#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFFSZ 256

void syserr(char* msg) { perror(msg); exit(-1); }

void show_help(){
	printf("Shared Drawing Board Client\n");
	printf("sdb-client 1.0 Bryan Jimenez\n");
	printf("Usage:\n");
	printf("\tsdb-client <hostname> <port> <alias>\n");
}

void listener(void * fd){
	int n;
	char buf[BUFFSZ];
	int sockfd = (int)fd;
	fd_set active_set;

	
	FD_ZERO(&active_set);


	for( ; ; ){

		FD_SET(sockfd, &active_set);
		select(sockfd+1, &active_set, NULL, NULL, NULL);

		if((n = recv(sockfd, buf, sizeof(buf), 0)) < 0) 
		syserr("failed to receive from client");
		else if(n==0) syserr("server disconnected");
		if(buf[n-1]=='\n') buf[n-1]='\0';

		printf("%s\n",buf);
		FD_ZERO(&active_set);

	}
}

int main(int argc, char* argv[]) {
	int sockfd, portno, n;
	struct hostent* server;
	struct sockaddr_in serv_addr;
	char buf[BUFFSZ];
	char *s;


	if(argc != 4) {
		show_help();
		return 1;
	}
	server = gethostbyname(argv[1]);
	
	if(!server) {
		fprintf(stderr, "ERROR: no such host: %s\n", argv[1]);
		return 2;
	}
	portno = atoi(argv[2]);

	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sockfd < 0) syserr("can't open socket");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr = *((struct in_addr*)server->h_addr);
	serv_addr.sin_port = htons(portno);

	if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
	syserr("can't connect to server");

	sprintf(buf,"%s\n",argv[3]);
	
	n = send(sockfd, buf, strlen(buf), 0);
	if(n < 0) syserr("can't send to server");

	pthread_t incoming;
	pthread_create(&incoming, NULL, (void *)listener, (void *)sockfd);
	
	for( ; ; ){

		s=fgets(buf, BUFFSZ-1, stdin);

		n = send(sockfd, s, strlen(s), 0);
		if(n < 0) syserr("can't send to server");
	}

	return 0;
}
