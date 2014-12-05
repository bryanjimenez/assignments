/* This example is stolen from Dr. Raju Rangaswami's original 4338
demo and modified to fit into our lecture. */

#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>


#define MAX_ARGS 20
#define BUFSIZ 1024

int get_args(char* cmdline, char* args[]) {
	int i = 0;

	/* if no args */
	if((args[0] = strtok(cmdline, "\n\t ")) == NULL) 
		return 0; 

	while((args[++i] = strtok(NULL, "\n\t ")) != NULL) {
		if(i >= MAX_ARGS) {
			printf("Too many arguments!\n");
			exit(1);
		}
	}
	/* the last one is always NULL */
	return i;
}

void execute(char* cmdline) {

	int pid, pid2, async, i, pflag=0;
	char* args[MAX_ARGS];
	int iFile =-1,oFile=-1, pp[2];
	int nargs = get_args(cmdline, args);

	if(nargs <= 0) 
		return;

	if(!strcmp(args[0], "quit") || !strcmp(args[0], "exit")) 
		exit(0);

	/* check if async call */
	if(!strcmp(args[nargs-1], "&")) { async = 1; args[--nargs] = 0; }
	else async = 0;

	for(i=0;i<nargs;i++){
		if(!strcmp(args[i], ">")){
			oFile = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
			if(oFile < 0) { perror("open failed!!"); exit(-1); }
			args[i++]=0;
			args[i]=0;  
		}
		else if(!strcmp(args[i], ">>")){
			oFile = open(args[i+1], O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
			if(oFile < 0) { perror("open failed!!"); exit(-1); }
			args[i++]=0;
			args[i]=0;	  
		}		
		else if(!strcmp(args[i], "<")){
			iFile = open(args[i+1], O_RDONLY);
			if(iFile < 0) { perror("open failed!!"); exit(-1); }
			args[i++]=0;
			args[i]=0;
		}	
		else if(!strcmp(args[i], "|")){
			args[i++]=0;
			pflag=1;

			if(pipe(pp) < 0) { perror("pipe failed"); exit(-1); }

			pid2 = fork();
			if(pid2 == 0) { //child process 

				dup2(pp[0], STDIN_FILENO);
				close(pp[1]);

				execvp(args[i], &args[i]);
				// return only when exec fails 
				perror("exec failed");
				exit(-1);
			} 
			else if(pid2 > 0) { // parent process 

			} 
			else { // error occurred 
				perror("pipe fork failed");
				exit(1);
			}
			break;
		}			
	}


	pid = fork();
	if(pid == 0) { /* child process */

		if(!pflag){
			if(iFile>0){dup2(iFile, STDIN_FILENO);close(iFile);}
			if(oFile>0){dup2(oFile, STDOUT_FILENO);close(oFile);}
		}

		dup2(pp[1], STDOUT_FILENO);
		close(pp[0]);

		execvp(args[0], args);
		/* return only when exec fails */
		perror("exec failed");
		exit(-1);
	} 
	else if(pid > 0) { /* parent process */
		close(pp[0]);
		close(pp[1]);

		if(!async) waitpid(pid, NULL, 0);
		else printf("this is an async call\n");
		
		if(pid2) 
			waitpid(pid2, NULL, 0);

	} 
	else { /* error occurred */
		perror("fork failed");
		exit(1);
	}
}

int main (int argc, char* argv []){
	char cmdline[BUFSIZ];

	for(;;) {
		printf("COP4338$ ");
		if(fgets(cmdline, BUFSIZ, stdin) == NULL) {
			perror("fgets failed");
			exit(1);
		}
		execute(cmdline) ;
	}
	return 0;
}
