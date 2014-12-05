#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "helper.c"


int sockfd;
struct sockaddr_in saddr_me,saddr_you;
socklen_t len;
pthread_mutex_t q_mutex;

float interv;
int winmax,winsz=0;
struct Node* q=0;


void *fillthequeue(){
	int seqnum=0;
	char buf2[BUFSZ];   
	char data[BUFSZ];
	for(;;){
		if(winsz<winmax){
			sprintf(data,"packet %d",seqnum);
			sprintf(buf2,"%d\7%d\7%s",seqnum++,checksum(data),data);

			pthread_mutex_lock(&q_mutex);
			enqueue(buf2,&q);
			pthread_mutex_unlock(&q_mutex);
			
			winsz++;
		}
	}
}
void *sendfromqueue(){
	int i;
	char *s;
 	char tbuf[100];	
	for(;;){ 
		sleep(interv);
		for(i=0;i<winmax;i++){ 

			pthread_mutex_lock(&q_mutex);
			s=iterate(q,i);
			pthread_mutex_unlock(&q_mutex);
			
			sendto(sockfd,s,strlen(s),0,(struct sockaddr *)&saddr_you,sizeof(saddr_you));
			printf("%s SENT %s\n",timestamp(tbuf),s);
		}
	}
} 
void *listenforack(){
	int imysum=0,ack=0,n;
	char buf1[BUFSZ];
	char idata[BUFSZ];
 	char tbuf[100];	
	for(;;){
		n=recvfrom(sockfd,buf1,BUFSZ,0,NULL,NULL);
		fix_string(n, buf1);
		printf("%s RECV %s\n",timestamp(tbuf),buf1);

		parse_udp(buf1, NULL, &imysum, idata);

		if(imysum==checksum(idata)){
			pthread_mutex_lock(&q_mutex);
			parse_udp(iterate(q,0), &ack, NULL, NULL);
			pthread_mutex_unlock(&q_mutex);


			while(ack<atoi(buf1)){
				pthread_mutex_lock(&q_mutex);
				dequeue(&q);
				pthread_mutex_unlock(&q_mutex);

				winsz--;
				pthread_mutex_lock(&q_mutex);
				parse_udp(iterate(q,0), &ack, NULL, NULL);
				pthread_mutex_unlock(&q_mutex);
			}
		}
	}
}
  
     
int main(int argc, char**argv)
{
	pthread_t fill;
	pthread_t send;
	pthread_t listen;

	pthread_mutex_init(&q_mutex, NULL);

   if (argc != 6)
      usage("usage:	sender <listenPort> <destIP> <destPort> <interval> <win size>");

	interv=atof(argv[4]);
	winmax=atoi(argv[5]);
	

   if((sockfd=socket(AF_INET,SOCK_DGRAM,0)) < 0)
   	haltwith("socket failed");
   	
   bzero(&saddr_me,sizeof(saddr_me)); 
   saddr_me.sin_family = AF_INET;
   saddr_me.sin_addr.s_addr=htonl(INADDR_ANY);
   saddr_me.sin_port=htons(atoi(argv[1]));
    
   bzero(&saddr_you,sizeof(saddr_you));
   saddr_you.sin_family = AF_INET;
   saddr_you.sin_addr.s_addr=inet_addr(argv[2]);
   saddr_you.sin_port=htons(atoi(argv[3]));
     
	if(bind(sockfd, (struct sockaddr*)&saddr_me, sizeof(saddr_me)) < 0) 
		haltwith("can't bind");
   
	pthread_create(&fill, NULL,fillthequeue, NULL);
	pthread_create(&send, NULL,sendfromqueue, NULL);
	pthread_create(&listen, NULL,listenforack, NULL);

   pthread_join(fill, NULL);
   pthread_join(send, NULL);
   pthread_join(listen, NULL);

	pthread_mutex_destroy(&q_mutex);
}
