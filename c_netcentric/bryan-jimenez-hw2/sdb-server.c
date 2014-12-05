#include	"unp.h"
#include <pthread.h>

#define BUFFSZ 1024
#define MAXNAMESZ 64
#define MAXCLIENT 32


/*------------------------GLOBALS------------------------*/
char *colors[MAXCLIENT]={"Air Force blue (USAF)","Air superiority blue","Alabama Crimson","Alice blue","Alizarin crimson","Alloy orange",
"Almond","Amaranth","Amazon","Amber","SAE/ECE Amber (color)","American rose","Amethyst","Android green","Anti-flash white",
"Antique brass","Antique bronze","Antique fuchsia","Antique ruby","Antique white","Ao (English)","Apple green","Apricot","Aqua",
"Aquamarine","Army green","Arsenic","Arylide yellow"};

pthread_mutex_t draw_mutex;
pthread_mutex_t client_mutex;

int d_num=0;
char draw_buf[BUFFSZ*10];

int c_num=0;
struct node *client[MAXCLIENT];
char client_buf[MAXCLIENT*MAXNAMESZ];


int x_dim, y_dim, fdmax=0;

fd_set master_set;
/*------------------------GLOBALS------------------------*/

void syserr(char* msg) { perror(msg); exit(-1); }

struct node {
	int *i;
	char *name;
	char *color;
};


/*
Client list constructor
*/
void initClient(){
	int i;
	for(i=0;i<MAXCLIENT;i++)
		client[i]=0;
}

/*
inserts a client struc into the array
*/
struct node *addClient(char *name){
	int i;

	struct node* c = malloc(sizeof(struct node));
	c->name = malloc(strlen(name)+1);

	strcpy(c->name,name);

	for(i=0;i<MAXCLIENT;i++)
		if(!client[i]){
			client[i]=c;
			c->i = malloc(sizeof(i));
			*(c->i)=i;
			c->color = malloc(strlen(colors[i])+1);		
			strcpy(c->color,colors[i]);
			break;
		}

	c_num++;			
	return c;
}

/*
removes client from array
*/
void delClient(struct node *me){
	char* start=strstr(client_buf,me->name);
	char* end=strstr(client_buf,me->color)+strlen(me->color)+1;
	
	strcpy(start,end);
	
	client[*(me->i)]=0;
	free(me->i);
	free(me->name);
	free(me->color);
	free(me);
	c_num--;
}
/*
figure out the termination type for the incoming string
*/
void fix_string(int n, char *buf){
	if(buf[n-2]=='\r' && buf[n-1]=='\n') buf[n-2]='\0';	/* \r\n (LF+CR) for telnet */
	else if(buf[n-1]=='\n') buf[n-1]='\0';						/* \n  for sdb-client*/
}

void show_help(){
	printf("Shared Drawing Board Server\n");
	printf("sdb-server 1.0 Bryan Jimenez\n");
	printf("Usage:\n");
	printf("\tsdb-server <server-port> <board-dim-x> <board-dim-y>\n");
}
/*
thread function for incoming connections to server
*/
void newclient(void * fd){			/*Client joining the chat*/

	int connfd = (int)fd;
	char buf[BUFFSZ];
	char myMsg[BUFFSZ];

	struct node *me;
	
	int n,i;

	fd_set active_set;
		

	/*-------------------------------Get name of new client-----------------------------*/
	n = recv(connfd, buf, sizeof(buf), 0);	
	if(n < 0) syserr("can't receive from client");
	fix_string(n, buf);
   pthread_mutex_lock(&client_mutex);
		me=addClient(buf);
   pthread_mutex_unlock(&client_mutex);
	/*-----------------------------------------------------------------------------------*/
	
	/*----------------------------Anouncing new client in chat---------------------------*/
	sprintf(buf, "%s (%s) joined\n",me->name, me->color);
	for(i=0;i<fdmax+1;i++)
		if(FD_ISSET(i, &master_set)&& i!=connfd)
		if((n= send(i, buf, strlen(buf), 0)) < 0) syserr("can't send to server");
	/*-----------------------------------------------------------------------------------*/
	
	/*---------------------------------My Name Color-------------------------------------*/
	sprintf(buf, "%d\n%s %s\n",c_num,me->name,me->color);
	n = send(connfd, buf, strlen(buf), 0);
	if(n < 0) syserr("can't send to client");
	/*-----------------------------------------------------------------------------------*/
	
	/*---------------------------------Client List---------------------------------------*/
	n = send(connfd, client_buf, strlen(client_buf), 0);
	if(n < 0) syserr("can't send to client");
	sprintf(&client_buf[strlen(client_buf)],"%s %s\n", me->name,me->color);
	/*-----------------------------------------------------------------------------------*/
	
	/*---------------------------------Draw Count----------------------------------------*/
	sprintf(buf,"%d %d %d\n",d_num,x_dim,y_dim);
	n = send(connfd, buf, strlen(buf), 0);
	if(n < 0) syserr("can't send to client");
	/*-----------------------------------------------------------------------------------*/
	
	/*---------------------------------Draw History--------------------------------------*/
	n = send(connfd, draw_buf, strlen(draw_buf), 0);
	if(n < 0) syserr("can't send to client");
	/*-----------------------------------------------------------------------------------*/
	
	
	FD_ZERO(&active_set);

	
	for( ; ; ){
		active_set=master_set;
	
		FD_SET(connfd, &active_set);
		Select(connfd+1, &active_set, NULL, NULL, NULL);

		if((n = recv(connfd, buf, sizeof(buf), 0)) < 0) /*Something failed*/
			syserr("failed to receive from client");
		else if(n==0) {											/*Client disconnected*/
			sprintf(buf,"%s left\n",me->name);
			FD_CLR(connfd, &master_set);

			for(i=0;i<fdmax+1;i++)									/*Broadcast client exit*/
				if(FD_ISSET(i, &master_set))
				if((n= send(i, buf, strlen(buf), 0)) < 0) syserr("can't send to server");
	
		   pthread_mutex_lock(&client_mutex);	
				delClient(me);
		   pthread_mutex_unlock(&client_mutex);
			return;
		}
		else {														/*Message received*/
			fix_string(n, buf);
			if(strcmp(buf,"clear")==0){							/*Message is clear*/
				//printf("%s cleared the boards\n",me->name);
				//strcpy(draw_buf,"");
				draw_buf[0]='\0';
				d_num=0;
				sprintf(myMsg, "%s %s\n",me->name,buf);
				
				for(i=0;i<fdmax+1;i++)								/*Broadcast client message*/
					if(FD_ISSET(i, &master_set)&& i!=connfd)
					if((n= send(i, myMsg, strlen(myMsg), 0)) < 0) syserr("can't send to server");
				
			}else{														/*Message is shape*/
				sprintf(myMsg, "%s %s\n",me->name,buf);
				
				//printf("%s",myMsg);
			   pthread_mutex_lock(&draw_mutex);
					strcat(draw_buf,myMsg);
					d_num++;
			   pthread_mutex_unlock(&draw_mutex);
			   
				for(i=0;i<fdmax+1;i++)								/*Broadcast client message*/
					if(FD_ISSET(i, &master_set)&& i!=connfd)
					if((n= send(i, myMsg, strlen(myMsg), 0)) < 0) syserr("can't send to server");
			}
		}
	}
}

int main(int argc, char *argv[]){

	int listenfd;
	pthread_mutex_init(&draw_mutex, NULL);
	pthread_mutex_init(&client_mutex, NULL);
	
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	
	if(argc !=4){
		show_help();
		return 1;
	}

	x_dim=atoi(argv[2]);
	y_dim=atoi(argv[3]);
	
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	FD_ZERO(&master_set);
		
	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		
		int connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
		fdmax = connfd>fdmax ? connfd : fdmax;
		FD_SET(connfd, &master_set);
			
		pthread_t incoming;
		pthread_create(&incoming, NULL, (void *)newclient, (void *)connfd);
	}
	
	pthread_mutex_destroy(&draw_mutex);
	pthread_mutex_destroy(&client_mutex);
}
