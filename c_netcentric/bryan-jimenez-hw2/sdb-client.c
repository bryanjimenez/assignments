#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

int x=0,y=0;

#define BUFFSZ 1024

char draw_buf[BUFFSZ*10];
char last_draw[256];




void syserr(char* msg) { perror(msg); exit(-1); }
gboolean draw_stuff(GtkWidget *widget); 

void mytok(char *src, int delim, int i, char *dest){
	
	char *start, *end=strchr(src,delim);
	int x,len;
	if(!strlen(src) || end=='\0')
		return;
		
	for(x=0;x<i;x++){
		start=end+1;
		end=strchr(end+1,delim);
		if(end==0)									/*fails if I use NULL*/
			end=src+strlen(src);
	}
		
	len=end-start;

	
	for(x=0;x<len;x++)
	*(dest+x)=*(start+x);
	dest[len]='\0';
	
	/*printf("mytok:\t'%s'\n",token);*/

}

void show_help(){
	printf("Shared Drawing Board Client\n");
	printf("sdb-client 1.0 Bryan Jimenez\n");
	printf("Usage:\n");
	printf("\tsdb-client <hostname> <port> <alias>\n");
}
/*
listens for message from the server
*/
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
		strcpy(draw_buf,buf);
		printf("%s\n",buf);
		FD_ZERO(&active_set);

	}
}

/*
drawing function
checks the draw buffer for updates and draws every second
*/
gboolean draw_stuff(GtkWidget *widget) {
	GdkGC *gc;
	gc=gdk_gc_new(GDK_DRAWABLE(widget->window));

	GdkColor color;
	color.red = 0xffff;
	color.green = 0xffff;
	color.blue = 0xffff;
	
	gdk_gc_set_line_attributes(gc, 3, GDK_LINE_SOLID, GDK_CAP_ROUND, GDK_JOIN_ROUND);
	
	char shape[128],x1[4],y1[4],x2[4],y2[4],r[4];
	

	if(strcmp(last_draw,draw_buf)&& strlen(draw_buf)){

		mytok(draw_buf, ' ', 1,shape);
		
		if(!strcmp(shape,"line")){
			mytok(draw_buf, ' ', 2,x1);
			mytok(draw_buf, ' ', 3,y1);
			mytok(draw_buf, ' ', 4,x2);
			mytok(draw_buf, ' ', 5,y2);

			gdk_draw_line(GDK_DRAWABLE(widget->window), gc, atoi(x1), atoi(y1), atoi(x2), atoi(y2));
		}
		else if(!strcmp(shape,"circle")){
			mytok(draw_buf, ' ', 2,x1);
			mytok(draw_buf, ' ', 3,y1);
			mytok(draw_buf, ' ', 4,r);

			gdk_draw_arc(GDK_DRAWABLE(widget->window), gc,FALSE,atoi(x1)-atoi(r), atoi(y1)-atoi(r),2*atoi(r),2*atoi(r),0, 64 * 360);
		}
		else if(!strcmp(shape,"rectangle")){
			mytok(draw_buf, ' ', 2,x1);
			mytok(draw_buf, ' ', 3,y1);
			mytok(draw_buf, ' ', 4,x2);
			mytok(draw_buf, ' ', 5,y2);
			
			gdk_draw_rectangle(GDK_DRAWABLE(widget->window), gc,FALSE, atoi(x1), atoi(y1), atoi(x2)-atoi(x1), atoi(y2)-atoi(y1));
		}
		else if(!strcmp(shape,"point")){
			mytok(draw_buf, ' ', 2,x1);
			mytok(draw_buf, ' ', 3,y1);
			
			//gdk_draw_point(GDK_DRAWABLE(widget->window), gc, atoi(x1), atoi(y1));
			gdk_draw_rectangle(GDK_DRAWABLE(widget->window), gc,TRUE, atoi(x1), atoi(y1), 3, 3);
		}
		else if(!strcmp(shape,"clear\n") || !strcmp(shape,"clear")){

			gtk_widget_modify_bg(widget, GTK_STATE_NORMAL, &color);
		}
		

		strcpy(last_draw,draw_buf);
	}

	return TRUE;
}

/*
gui builder/initializer
*/
int draw() {
	GtkWidget *window;
	gtk_init(NULL, NULL);

	GdkColor color;
	color.red = 0xffff;
	color.green = 0xffff;
	color.blue = 0xffff;

	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(window, 640, 460);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_title(GTK_WINDOW(window), "Shared Drawing Board");
	gtk_widget_modify_bg(window, GTK_STATE_NORMAL, &color);
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	

	g_timeout_add(1000, (GSourceFunc) draw_stuff, (gpointer) window);
	
	gtk_widget_show_all(window);
	
	draw_stuff(window);
	
	gtk_widget_show_all(window);

	
	
	gtk_main();

	return 0;
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
	pthread_t paint;
	pthread_create(&incoming, NULL, (void *)listener, (void *)sockfd);
	pthread_create(&paint, NULL, (void *)draw, NULL);

	
	for( ; ; ){

		s=fgets(buf, BUFFSZ-1, stdin);
		strcpy(draw_buf,"me ");
		strcat(draw_buf,s);
		
		n = send(sockfd, s, strlen(s), 0);

		if(n < 0) syserr("can't send to server");
	}

	return 0;
}
