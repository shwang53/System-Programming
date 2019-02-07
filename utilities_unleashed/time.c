/**
 * Utilities Unleashed Lab
 * CS 241 - Spring 2019
 */
 
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "format.h"
#include <stdio.h>

int main(int argc, char *argv[]) {

	if(argc < 2){
		print_time_usage();
		return 0;
	}
	
	pid_t child_pid = fork();

	if(child_pid < 0){
		print_fork_failed();
	}
	
		
	if(child_pid == 0){ 
      //     	printf("Hi I am a child ! \n");

		 if(execvp(argv[1], argv+1) <0){
                        print_exec_failed();
 
                }
        }


	if(child_pid > 0){
 	//	printf("Hi I am an adult ! \n");		
		struct timespec start, end;
		int status;

	        clock_gettime(CLOCK_MONOTONIC, &start);   

		waitpid(child_pid, &status, 0);

		clock_gettime(CLOCK_MONOTONIC, &end); 

		double duration = end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec)/1000000000.0;
		display_results(argv, duration);
	}

	return 0;
}
