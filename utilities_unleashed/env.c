/**
 * Utilities Unleashed Lab
 * CS 241 - Spring 2019
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include "format.h"

extern char **bing;

int check(char c){

	return isalnum(c) || c == '_';
}

char **making (const char *s1, const char *nope) {
        char *s = strdup(s1);

        size_t al = 1;
        size_t used = 0;

        char **temp = calloc(al, sizeof(char*));
        char * assi;
        char *rest = s;

        while ((assi = strsep(&rest, nope)) != NULL) {
                if (used == al) {
                        al *= 2;
                        assi = realloc(temp, al * sizeof(char *));
                }
                temp[used++] = strdup(assi);
        }

        if (used == 0) {
                free(assi);
                temp = NULL;
        } else {
                temp = realloc(assi, used * sizeof(char *));
        }

        free(s);
        return temp;
}

void setup(char* temp, int leng){
	int i=0;
	int j=0;
	
 	// int ddidx = 0;
          //   for (; ddidx < argc - 1; ddidx++)
            //     if (!strcmp(argv[ddidx], "--")) break;
	//found equal sign =
	for(;j<(int)strlen(temp);j++){
		if(temp[j] == '='){
			break;
		}
	} // token[j] is =, j is digit of =
	
	char* var = strndup(temp, j); //dest_var is duplicate of variable name.
//	int length1 = (int)strlen(var);
	
	for(i=0; i< leng+1; i++){ //check if dest var has valid character.
		if(!check(var[i])){
			free(var); var = NULL;
			free(temp); temp = NULL;
		}
	}


  	char * result = malloc(1);

	for(i = j + 1; i<leng+1;i++){ //대입값의 범위. = 부터 시작

		char * start = &temp[i]; 
		
		if(temp[i] == '%'){ //만약 퍼센트를 만나면 또 다른 대입연ㅅ나 ?
			while(check(temp[i+1])) {
				 i++;
			 }
		

			char * final = &temp[i];

	 	 	char * nam = strndup(start, final - start + 1);
	 	 	char * vol =  getenv(nam);

	 	 	if(vol!=NULL){
		 	 	result = realloc(result, strlen(result) + strlen(vol) + 1);
		 	 	sprintf(result, "%s%s", result, vol);
	 	 	}
		} 

		else {
	 	 	result = realloc(result, strlen(result) + 2);	
		 	sprintf(result, "%s%c", result, temp[i]);			
		}
	}
	setenv(var, result, 1);
	free(var);
}


int main(int argc, char *argv[]) {
	pid_t childid = fork();
	if(childid  == -1){
		print_fork_failed();
	
	}
	if(childid >0){
		int status;
		waitpid(childid, &status, 0);
	}
	if(childid ==0){
		// if there is only one argument, print out all environ vars.

		int ddidx = 0;
   		 for (; ddidx < argc - 1; ddidx++)
       		 if (!strcmp(argv[ddidx], "--")) break;
		
		if(argc == 1){
			char** ptr = bing;
			while(*ptr){
				printf("%s\n", *ptr);
				ptr +=1;
			}
			exit(0);
		}

		//if there are two args, exit.
		else if(argc == 2){
		print_env_usage();
	
		}


		else if(argc >2){

			for(int i=1; i<ddidx-1; i++){
				char* copy = strdup(argv[i]);
                        	char** output = making(copy, ",");
                        
                        	while(*output){
                                	setup(*output,ddidx);
                        	       	 output++;
                        	}
			}
		
			if(execvp(argv[argc-1], argv+(argc-1)) <0){
				print_exec_failed();
				
			}
		}			
	}
	return 0; 
}

