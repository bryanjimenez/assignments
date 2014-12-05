#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define SIZE 1024

void pUsage(char* argv[]){
	printf("This program opens a file and replaces all occurances\n");
	printf("of <string_a> by <string_b>\n");
	printf("USAGE: %s <string_a> <string_b> <filename>\n", argv[0]);
}

void replaceAll(char* line, char* before, char* after){
	int i,c;
	int afterLen=strlen(after);
	int beforeLen=strlen(before);
	int diff=afterLen-beforeLen;
	char* lineEnd;

	while(line=strstr(line, before)){		 
		
		lineEnd=line;
		c=0;	
						
		if(diff<0){
			while(*lineEnd)
				*(++lineEnd+diff)=*lineEnd;
		
		}
		if(diff>0){
			while(*lineEnd){lineEnd++; c++;}

			for(i=0;i<(diff+c);i++)
				*(lineEnd+diff)=*lineEnd--;
				
		}
			
		for(i=0;i<afterLen;i++)
			*line++=*after++;
		after-=i;
		
	}
}

int main(int argc, char* argv[]){

FILE* fp;
char* line;
char* after;
char* before;
char* found;

if(argc!=4){
	pUsage(argv);	
	return -1;
}
else if(fp=fopen(argv[3],"r")){

	before=argv[1];
	after=argv[2];
	
	do{
		line=(char*)malloc(SIZE*strlen(after)/strlen(before));
		//printf("%s",line);
	
		replaceAll(line,before,after);

		printf("%s",line);			
	
		free(line);	
	
	}while(fgets(line,SIZE,fp));
	fclose(fp);
	
	return 0;
}
else{
	printf("File \"%s\" does not exist\n", argv[3]);
	return -1;
}

}
