#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "unp.h"



#define BUFFSZ 1024
#define MYPORT 55555
#define WWWFOLDER "www"
#define SERVERNAME bryan
#define CONNECTION close

/*
params: src buffer is the where where searching
params: delim is the delimiter to split the buffer by
params: i is the ith token to return
*/
char *mytok(char *src, char *delim, int i){
	
	char *token, *start=src, *end=strchr(src,delim);
	int x,len;

	for(x=0;x<i;x++){
		start=end+1;
		end=strchr(end+1,delim);
		if(end==0)									/*fails if I use NULL*/
			end=src+strlen(src);
	}
		
	len=end-start;
	token=(char*) malloc(sizeof(char)*(len)+1);
	
	for(x=0;x<len;x++)
	*(token+x)=*(start+x);
	token[len]='\0';
	
	/*printf("mytok:\t'%s'\n",token);*/
	return token;

}
/*
Debug
this is to print a buffer char by char to make sure files are being written correctly
*/
void printb(void *src, int n){
	int i;

	printf("'");
	for(i = 0;i < n;++i){
		printf("%c", ((char *)src)[i]);}
	printf("'");	
}

/*
params: a buffer and a character to search for
returns: index where the character is first found
*/
int charat(char *source, char search){
	int x;
	for(x=0; x<strlen(source); x++)
		if( *(source+x)==search)
			return x;
	return -1;
}
                              
int main(int argc, char **argv)
{

	int listenfd, connfd, n, headerSize, fileSize=0;
	char buffer[BUFFSZ];
	char *response;
	char *filepath;
	char *filename;
	char *filetype;
	
	char *status200= "HTTP/1.0 200 OK\r\n";
	char *status400= "HTTP/1.0 400 Bad Request\r\n";
	char *status404= "HTTP/1.0 404 Not Found\r\n";
	char *status403= "HTTP/1.0 403 Forbidden\r\n";
	char *server	= "Server: SERVERNAME\r\nConnection: CONNECTION\r\nContent-Length: ";
	char *html		= "\r\nContent-Type: text/html\r\n\r\n";
	char *jpeg		= "\r\nContent-Type: image/jpeg\r\n\r\n";
	char *gif 		= "\r\nContent-Type: image/gif\r\n\r\n";
	char *ico 		= "\r\nContent-Type: image/x-icon\r\n\r\n";
	char *js 		= "\r\nContent-Type: text/javascript\r\n\r\n";
	char *css 		= "\r\nContent-Type: text/css\r\n\r\n";
	char *txt 		= "\r\nContent-Type: text/plain\r\n\r\n";
	/*Add more mimetypes here, but also add filetype in Match file with mimetypes section*/


	FILE *file;
	int fileErr;
	char *mode = "r";
			
	pid_t childpid;

	socklen_t clilen;

	struct sockaddr_in cliaddr, servaddr;

	listenfd = Socket (AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
	servaddr.sin_port = htons (MYPORT);
	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
	Listen(listenfd, LISTENQ);
	
	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
		if ( (childpid = Fork()) == 0) { /* child process */
			Close(listenfd);				/* close listening socket */			
			n = recv(connfd, buffer, sizeof(buffer), 0);
			  						
			if(n < 0) printf("can't receive from client"); 
			else buffer[n] = '\0';

			/*printf("-----------------------------------\n");
  			printf("SERVER GOT MESSAGE:\n%s", buffer);  */
			
			
			/*check for "GET " and "HTTP/1." in the first line of REQUEST*/
			if(strstr(buffer,"GET ")==NULL || strstr(buffer," HTTP/1.")==NULL){
				fprintf(stderr,"400! %s Bad Request\n");
				strcpy(buffer,status400);

				send(connfd, buffer, strlen(buffer)+fileSize, 0);
				exit(1);			
			}
			
			
			filename=mytok(buffer, ' ', 1);
			
			/*-----------------------Do address redirects here--------------------------*/
			if(strcmp(filename,"/")==0) filename="/index.html";
			
			
			/*--------------------------------------------------------------------------*/
			
			/*-------------------Attach the WWWFOLDER to the path-----------------------*/
			filepath=(char *)malloc(sizeof(WWWFOLDER)+sizeof(char)*strlen(filename));
			strcpy(filepath,WWWFOLDER);
			strcat(filepath,filename);
			/*printf("filename:\t'%s'\n",filename);*/
			
			/*--------------------------------------------------------------------------*/
			
			memset(buffer,0,sizeof(buffer));
			/*printf("buffer:\t'%s'\n",buffer);*/
			
			file = fopen(filepath, mode);			

			/*----------------------No object or permission issues----------------------*/
			if (file == NULL) {
				fileErr = errno;
				
				if(fileErr == EACCES){/*Check if it was due to permission*/
					fprintf(stderr,"403! '%s' is Forbidden\n", filename);
					strcpy(buffer,status404);

					free(filepath);
					free(filename);
					filename="/403.html";
					filepath=(char *)malloc(sizeof(WWWFOLDER)+sizeof(char)*strlen(filename));

					strcpy(filepath,WWWFOLDER);
					strcat(filepath,filename);
					file = fopen(filepath, mode);
					if (file == NULL){
						fprintf(stderr,"we cant find our 403 page!\n");
						send(connfd, buffer, strlen(buffer)+fileSize, 0);
						exit(1);
					}	
				}
				else{/*Assume that all other errors mean the file is not there..*/
					fprintf(stderr, "404! '%s' is Not Found\n", filename);
					strcpy(buffer,status404);

					free(filepath);
					free(filename);
					filename="/404.html";
					filepath=(char *)malloc(sizeof(WWWFOLDER)+sizeof(char)*strlen(filename));

					strcpy(filepath,WWWFOLDER);
					strcat(filepath,filename);
					file = fopen(filepath, mode);
					if (file == NULL){
						fprintf(stderr,"we cant find our 404 page!\n");
						send(connfd, buffer, strlen(buffer)+fileSize, 0);
						exit(1);
					}
				}
			}
			/*--------------------------------------------------------------------------*/
			else
				strcpy(buffer,status200);

			
			/*extract filetype*/
			if(index(filename,'.')!=NULL)
				filetype=mytok(filename, '.', 1);
			else
				filetype="";
				
			strcat(buffer,server);
			
			fseek(file, 0, SEEK_END);
			fileSize = ftell(file);
			rewind(file);
			
		
			sprintf(buffer+strlen(buffer), "%d", fileSize);			
				
			/*-----------------------Match file with mimetypes-------------------------*/
			if (strcmp(filetype,"html")==0)
				strcat(buffer,html);
			
			else if (strcmp(filetype,"jpeg")==0)
				strcat(buffer,jpeg);
			
			else if (strcmp(filetype,"gif")==0)
				strcat(buffer,gif);
			
			else if (strcmp(filetype,"ico")==0)
				strcat(buffer,ico);
				
			else if (strcmp(filetype,"js")==0)
				strcat(buffer,js);
				
			else if (strcmp(filetype,"css")==0)
				strcat(buffer,css);			
						
			else if (strcmp(filetype,"txt")==0 || strcmp(filetype,"")==0)
				strcat(buffer,txt);			
			/*If you add more filetypes here also define them at the beginning of main()*/
			/*--------------------------------------------------------------------------*/
				
			/*printf("buff:\t'%s'\n",buffer);*/
			
			
			response=(char*) malloc(sizeof(buffer)+fileSize);
			memset(response,0,sizeof(response));
			
			memcpy(response, buffer, strlen(buffer)+1);
			headerSize=strlen(response);
			fread(response+strlen(response), 1, fileSize, file);
			fclose(file);
			
			
			/*
			printf("-----------------------------------\n");
			if (strcmp(filetype,"js")==0)
			printb(response,headerSize+fileSize);
 			printf("\n-----------------------------------\n");
 			*/
 			
			n = send(connfd, response, headerSize+fileSize, 0);			
			if(n < 0) printf("can't send to client"); 
			
			free(filename);
			free(filetype);
			free(filepath);
			free(response);
			
			exit (0);
		}
		Close(connfd);						/* parent closes connected socket */
	}
}
