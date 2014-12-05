#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "helper.c"




int main(int argc, char**argv)
{
   int sockfd,n;
   struct sockaddr_in saddr_me,saddr_you;
   socklen_t len= sizeof(saddr_you);
   char mesg[BUFSZ];
 	char tbuf[100];	
	int seqnum=0,iseqnum;
	int mysum;
	char data[BUFSZ];

   if (argc != 2)
      usage("usage:  receiver <listenPort>");

   if((sockfd=socket(AF_INET,SOCK_DGRAM,0)) < 0)
   	haltwith("socket failed");

   bzero(&saddr_me,sizeof(saddr_me));
   saddr_me.sin_family = AF_INET;
   saddr_me.sin_addr.s_addr=htonl(INADDR_ANY);
   saddr_me.sin_port=htons(atoi(argv[1]));

   
	if(bind(sockfd, (struct sockaddr*)&saddr_me, sizeof(saddr_me)) < 0) 
		haltwith("bind failed");

   for (;;)
   {
      n = recvfrom(sockfd,mesg,BUFSZ,0,(struct sockaddr *)&saddr_you,&len);
		fix_string(n, mesg);

      printf("%s RECV: %s",timestamp(tbuf),mesg);

		parse_udp(mesg, &iseqnum, &mysum, data);
		
		if(mysum==checksum(data)&& seqnum==iseqnum){
			seqnum++;
			printf("\n");	
		}
		else if(seqnum!=iseqnum)
			printf(" bad order detected\n");	
		else	
			printf(" corruption detected\n");	
      
      sprintf(data,"ACK %d",seqnum);      
		sprintf(mesg,"%d\7%d\7%s", seqnum,checksum(data),data);      
		mesg[strlen(mesg)] = 0;

      sendto(sockfd,mesg,strlen(mesg),0,(struct sockaddr *)&saddr_you,sizeof(saddr_you));
      printf("%s SENT: %s\n",timestamp(tbuf),mesg);

   }
}
