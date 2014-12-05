#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "helper.c"


pthread_mutex_t qi_mutex;
pthread_mutex_t qo_mutex;

struct Node* qi=0;
struct Node* qo=0;

int isockfd,osockfd,n;
struct sockaddr_in saddr_me, saddr_you;
socklen_t len;


char sip[256];	/*sender ip*/
char sp[256];	/*sender port*/

char *rip;	/*receiver ip*/
char *rp;	/*receiver port*/


float interv;
float pc,PC,pd,PD,po,PO;		/*PC PD PO are the user entered thresholds, pc pd po are generated*/



void *s2r_listen(){
	char buf1[BUFSZ];
	char data[BUFSZ];
	int seqnum=0, mysum=0;
	char *s=0;
 	char tbuf[100];	
	srand(time(0));
	
	for(;;){
		n = recvfrom(isockfd,buf1,BUFSZ,0,(struct sockaddr *)&saddr_me,&len);
		s=(char *)inet_ntoa(saddr_me.sin_addr);
		
		sprintf(sip,"%s",s);
		sprintf(sp,"%d",ntohs(saddr_me.sin_port));

		
		fix_string(n, buf1);
		printf("%s RECV (%s:%s) %s",timestamp(tbuf),sip,sp,buf1);
		parse_udp(buf1, &seqnum, &mysum, data);
	

		/*----------------------------------------Packet Tampering--------------------------------------*/

		pc = ((float)rand())/RAND_MAX;
		pd = ((float)rand())/RAND_MAX;
		po = ((float)rand())/RAND_MAX;



		if(pd>PD){												/*droped*/
			printf(" packet was DROPPED %0.2f\n",pd);
			continue;
			
		}else if(pc>PC){										/*corrupted*/
			printf(" packet was CORRUPTED %0.2f\n",pc);
			data[1]='@';data[4]='3';
			sprintf(buf1,"%d\7%d\7%s", seqnum, mysum, data);
			
			pthread_mutex_lock(&qi_mutex);
			enqueue(buf1,&qi);
			pthread_mutex_unlock(&qi_mutex);

			
		}else if(po>PO){										/*reordered*/
			printf(" packet was REORDERED %0.2f\n",po);
			sprintf(buf1,"%d\7%d\7%s", seqnum, mysum, data);
			
			pthread_mutex_lock(&qi_mutex);
			enqueue_wrong(buf1,&qi);
			pthread_mutex_unlock(&qi_mutex);


		} else{													
			printf(" packet was UNHARMED\n"); 			/*lucky*/
			sprintf(buf1,"%d\7%d\7%s", seqnum, mysum, data);
			
			pthread_mutex_lock(&qi_mutex);
			enqueue(buf1,&qi);
			pthread_mutex_unlock(&qi_mutex);
		}

		/*----------------------------------------------------------------------------------------------*/
	}
} 
void *s2r_send(){
	char *s=0;
 	char tbuf[100];	
	for(;;){
		sleep(interv);
		for(;;){

			pthread_mutex_lock(&qi_mutex);
			s=iterate(qi,0);
			pthread_mutex_unlock(&qi_mutex);
			if(strlen(s)>0){
				sendto(osockfd,s,strlen(s),0,(struct sockaddr *)&saddr_you,sizeof(saddr_you));
				printf("%s SENT (%s:%s) %s\n",timestamp(tbuf),rip,rp,s);

				pthread_mutex_lock(&qi_mutex);
				dequeue(&qi);
				pthread_mutex_unlock(&qi_mutex);
			}
		}	
	}
}
void *r2s_listen(){
	char buf2[BUFSZ];
	char data[BUFSZ];
 	char tbuf[100];	
	int seqnum=0, mysum=0;
	for(;;){

		n = recvfrom(osockfd,buf2,BUFSZ,0,NULL,NULL);
		fix_string(n, buf2);
		
		printf("%s RECV (%s:%s) %s",timestamp(tbuf),rip,rp,buf2);
		parse_udp(buf2, &seqnum, &mysum, data);
	

		/*----------------------------------------Packet Tampering--------------------------------------*/

		pc = ((float)rand())/RAND_MAX;
		pd = ((float)rand())/RAND_MAX;
		po = ((float)rand())/RAND_MAX;


		if(pd>PD){												/*droped*/
			printf(" Ack was DROPPED %0.2f\n",pd);
			continue;
			
		}else if(pc>PC){										/*corrupted*/
			printf(" Ack was CORRUPTED %0.2f\n",pc);
			data[0]='@';data[2]='#';
			sprintf(buf2,"%d\7%d\7%s", seqnum, mysum, data);

			pthread_mutex_lock(&qo_mutex);
			enqueue(buf2,&qo);
			pthread_mutex_unlock(&qo_mutex);
			
		}else if(po>PO){										/*reordered*/
			printf(" Ack was REORDERED %0.2f\n",po);
			sprintf(buf2,"%d\7%d\7%s", seqnum, mysum, data);

			pthread_mutex_lock(&qo_mutex);
			enqueue_wrong(buf2,&qo);
			pthread_mutex_unlock(&qo_mutex);

		} else{													
			printf(" Ack was UNHARMED\n"); 			/*lucky*/
			sprintf(buf2,"%d\7%d\7%s", seqnum, mysum, data);
			
			pthread_mutex_lock(&qo_mutex);
			enqueue(buf2,&qo);
			pthread_mutex_unlock(&qo_mutex);
		}

		/*----------------------------------------------------------------------------------------------*/
		
	}
}
void *r2s_send(){
	char *s=0;
 	char tbuf[100];	
	for(;;){
			pthread_mutex_lock(&qo_mutex);
			s=iterate(qo,0);
			pthread_mutex_unlock(&qo_mutex);

		if(strlen(s)>0){
			sendto(isockfd,s,strlen(s),0,(struct sockaddr *)&saddr_me,len);
			printf("%s SENT (%s:%s) %s\n",timestamp(tbuf),sip,sp,s);

			pthread_mutex_lock(&qo_mutex);
			dequeue(&qo);
			pthread_mutex_unlock(&qo_mutex);
		}
	}
}

int main(int argc, char**argv)
{
	pthread_t s2r_l;
	pthread_t s2r_s;
	
	pthread_t r2s_l;
	pthread_t r2s_s;
	
	pthread_mutex_init(&qi_mutex, NULL);
	pthread_mutex_init(&qo_mutex, NULL);
   if (argc != 8)
      usage("usage:  twister <listenPort> <receiverIP> <receiverPort> <PC> <PD> <PO> <interval>");


	rip=argv[2];	
	rp=argv[3];
	PC=atof(argv[4]);
	PD=atof(argv[5]);
	PO=atof(argv[6]);
	interv=atof(argv[7]);
	

	
   if((isockfd=socket(AF_INET,SOCK_DGRAM,0)) < 0)
   	haltwith("socket failed");

   bzero(&saddr_me,sizeof(saddr_me));
   saddr_me.sin_family = AF_INET;
   saddr_me.sin_addr.s_addr=htonl(INADDR_ANY);
   saddr_me.sin_port=htons(atoi(argv[1]));
   
   if(bind(isockfd,(struct sockaddr *)&saddr_me,sizeof(saddr_me))<0)
		haltwith("bind failed");


   if((osockfd=socket(AF_INET,SOCK_DGRAM,0)) < 0)
   	haltwith("socket failed");

   bzero(&saddr_you,sizeof(saddr_you));
   saddr_you.sin_family = AF_INET;
   saddr_you.sin_addr.s_addr=inet_addr(rip);
   saddr_you.sin_port=htons(atoi(rp));

	len = sizeof(saddr_me);
	
	pthread_create(&s2r_l, NULL,s2r_listen, NULL);
	pthread_create(&s2r_s, NULL,s2r_send, NULL);
	pthread_create(&r2s_l, NULL,r2s_listen, NULL);
	pthread_create(&r2s_s, NULL,r2s_send, NULL);

   pthread_join(s2r_l, NULL);
   pthread_join(s2r_s, NULL);
	pthread_join(r2s_l, NULL);
	pthread_join(r2s_s, NULL);

	pthread_mutex_destroy(&qi_mutex);
	pthread_mutex_destroy(&qo_mutex);
}
