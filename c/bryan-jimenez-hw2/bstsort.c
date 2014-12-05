#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define SIZE 1024

//GLOBAL
int (*comp)(char*, char*)=&strcasecmp;
struct Node* head = 0;

//END GLOBAL

struct Node {
	char* content;
	struct Node* prev;
	struct Node* next;
};

void inOrderPrint(struct Node* n)
{
	if(n->prev)
		inOrderPrint(n->prev);
	
	printf("%s",n->content);
		
	if(!(n->next))
		return;
	else
		inOrderPrint(n->next);
}

void postOrderFree(struct Node* n)
{
	if(n->next)
		postOrderFree(n->next);
	
	free(n->content);	
		
	if(!(n->prev)){
		free(n);	
		return;
	}
	else
		postOrderFree(n->prev);
}

//n= current node, i= insertion
void rInsert(struct Node* n, struct Node* i)
{
	if((*comp)(i->content,n->content)>0){		//insert after this one

		if(n->next)
			rInsert(n->next,i);
		else
			n->next=i;
	}
	else{													//insert before this one

		if(n->prev)
			rInsert(n->prev,i);
		else{
			n->prev=i;

			}
	}
}

void insert(char* str)
{
	struct Node* i = (struct Node*)malloc(sizeof(struct Node));
	i->content = (char*)malloc(strlen(str)+1);
	strcpy(i->content, str);
	i->next=i->prev=0;

	if(!head) 
		head = i;
	else
		rInsert(head,i);
}

void printTree()
{
	inOrderPrint(head);
}
void cleanTree()
{
	postOrderFree(head);
}


int main(int argc, char* argv[])
{
	//FILE *fout= stdout;	//dont need it here
	//FILE *fin= stdin;		//dont need it here
	
	int o;
	char line[SIZE];

	while ((o = getopt(argc, argv, "co:")) != -1) {
		switch(o) {
			case 'c':			//case sensitive
				comp=&strcmp;
				break;
			case 'o':			//output to file
				stdout = fopen(optarg, "w");
				if(!stdout) { 
					printf("ERROR: can't open source file to read: %s\n", optarg);
					return 2;
				}	
				break;
			case '?':			//unknown switch
				default:
				printf("USAGE: %s [-c] [-o output_file_name] input_file_name\n", argv[0]);
				return 0;
		}
	}

	argc -= optind;
	argv += optind;

	stdin = argc ? fopen(argv[0],"r") : stdin;

	while(fgets(line,SIZE,stdin))
		insert(line);

	printTree();
	cleanTree();
	
	fclose(stdout);
	fclose(stdin);

	return 0;
}
