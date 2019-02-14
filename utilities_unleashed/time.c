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
/*
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
*/

#include <stdio.h>
#include <unistd.h>

char* compiler = "gcc";

int main(int argc, const char * argv[]) {
    
    const char* target1 = argv[1];
    const char* target2 = argv[2];
     const char* target3 = argv[3];
    const char* target4 = argv[4];
    
    pid_t child1, child2;
    
    child1 = fork();
    
        printf("hi i m an adult \n" );
    
    if (child1 == 0) {
        
        printf("hi i m child 1 \n" );

        execlp(target1, target1, target2,  (char*)NULL);
	  printf("failed \n" );
        //여기서 exec을 어떻게 쓰면될까 ??????????
        //어떻게 써야 커맨드를 알아듣고 echo도 하고 sleep도 하고 ??
        
	  child2 = fork();

//        if (child2 == 0) {
             execlp(target3, target3 ,target4, (char*)NULL);
             printf("failed2 \n" );

  //      }


    }
    
//    
  //      child2 = fork();
        
    //    if (child2 == 0) {
      //       execlp(target3, target3 ,target4, (char*)NULL);
        //     printf("failed2 \n" );
            
      //  }
        
    
    int status = 0;
    waitpid(child1, &status, 0);
   // if(WIFEXITED(status) && WEXITSTATUS(status) ==0);

    
    
  
}

