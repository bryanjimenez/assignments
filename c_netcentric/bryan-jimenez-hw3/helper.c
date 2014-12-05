#define BUFSZ 256

    
char *timestamp(char *tbuf) {

    time_t now = time(0);
    strftime (tbuf, 100, "%H:%M:%S", localtime (&now));

    return tbuf;
}



void haltwith(char *e){
	perror(e);
	exit(1);
}

void usage(char *u){
	printf("%s\n",u);
	exit(1);
}

/*----------------------------------Linked list queue-----------------------------------------------*/

struct Node {
	char* content;
	struct Node* next;
	struct Node* prev;
};
/*
regular enqueue, packet goes at end of the line
*/
void enqueue(char* str,struct Node** q)
{
	struct Node* i = (struct Node*)malloc(sizeof(struct Node));
	i->content = (char*)malloc(strlen(str)+1);
	strcpy(i->content, str);
	i->next=i->prev=i;

	if(!(*q))
		*q = i;
	else{
		i->next=(*q)->next;
		i->prev=(*q);
		(*q)->next=i;
		i->next->prev=i;
	}
}
/*
will enqueue a packet one ahead
*/
void enqueue_wrong(char* str,struct Node** q)
{
	struct Node* i = (struct Node*)malloc(sizeof(struct Node));
	i->content = (char*)malloc(strlen(str)+1);
	strcpy(i->content, str);
	i->next=i->prev=i;

	if(!(*q))
		*q = i;
	else{
		i->next=(*q)->next->next;
		i->prev=(*q)->next;
		(*q)->next->next=i;
		i->next->prev=i;
	}
}



char *iterate(struct Node* q, int i){
	
	if(!q)
		return "";
	
	if(i==0)
		return q->content;
	else
		return iterate(q->prev,--i);
}

void dequeue(struct Node** q){
	if(*q){
		free((*q)->content);
		if((*q)->prev!=(*q)){
			(*q)->prev->next=(*q)->next;
			(*q)=(*q)->prev;
			free((*q)->next->prev);
			(*q)->next->prev=(*q);
		}
		else{
			free(*q);
			*q=0;
		}
	}
}
/*-----------------------------------------------------------------------------------------------------*/
/*--------------------------------------UDP PARSER-----------------------------------------------------*/

void parse_udp(char *msg, int *seqnum, int *mysum, char *data){
	char *tok = (char *)malloc(strlen(msg)+1);
	char *freeme = tok;
	strcpy(tok,msg);
      
	if(!strstr(tok,"\7"))
		return;		

	tok = strtok(tok, "\7");
	if(seqnum)
		*seqnum = atoi(tok);				
				
	tok = strtok(NULL, "\7");
	if(mysum)
		*mysum = atoi(tok);		
		
	tok = strtok(NULL, "\7");
	if(data)
		strcpy(data,tok);
	
	
	free(freeme);
}
/*-----------------------------------------------------------------------------------------------------*/

/*
figure out the termination type for the incoming string
*/
void fix_string(int n, char *buf){
	if(buf[n-2]=='\r' && buf[n-1]=='\n') buf[n-2]='\0';	/* \r\n (LF+CR) for telnet */
	else if(buf[n-1]=='\n') buf[n-1]='\0';						/* \n  for sdb-client*/
	else buf[n]=0;
}

/*
very simple checksum
*/
int checksum(char *data){  
	int i,checksum=0;
	
	for(i=0;i<strlen(data);i++)
		checksum+=data[i];
	
	return checksum;	
}

