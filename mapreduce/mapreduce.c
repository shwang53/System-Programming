/**
 * mapreduce Lab
 * CS 241 - Spring 2019
 */


#include "utils.h"
#include <alloca.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char ** argv) {
	if (argc != 6) {
		print_usage();
        exit(1);
	}
	int num = atoi(argv[5]);

    // Create an input pipe for each mapper.
 	int mapper_pipe[2];
	pipe(mapper_pipe);


    // Open the output file.
	int input_start = open(argv[1], O_RDONLY);
    int output_end = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU);


    // Start a splitter process for each mapper.
	pid_t splitter_id[num];
	pid_t mapper_id[num];
	for (int i = 0; i < num; i++) {
		int splitter_pipe[2];
		pipe(splitter_pipe);
		splitter_id[i] = fork();
		if(splitter_id[i] == 0){
			close(mapper_pipe[1]);
			close(mapper_pipe[0]);
			close(splitter_pipe[0]);

			dup2(input_start, 0);
			dup2(splitter_pipe[1], 1);

			char splitter_idx[2];
    		sprintf(splitter_idx, "%d", i);
			execlp("./splitter", "./splitter", argv[1], argv[5], splitter_idx, (char *)NULL);
			exit(1);
		}


		//Start the mapper processe.
		mapper_id[i] = fork();
		if (mapper_id[i] == 0) {
    		close(mapper_pipe[0]);
   	 		close(splitter_pipe[1]);

			dup2(splitter_pipe[0], 0);
			dup2(mapper_pipe[1], 1);

        	execlp(argv[3], argv[3], (char *)NULL);
			exit(1);
		}
		//parent
    	close(splitter_pipe[0]);
    	close(splitter_pipe[1]);
	}


    // Start the reducer process.
    // take one reducer to complete the task
	pid_t reducer_id = fork();

    // Wait for the reducer to finish.
	if (reducer_id == 0) {
    	close(mapper_pipe[1]);

    	dup2(mapper_pipe[0], 0);
    	dup2(output_end, 1);

    	execlp(argv[4], argv[4], (char *)NULL);
    	exit(1);
	} else {
        close(mapper_pipe[0]);
        close(mapper_pipe[1]);
    	for (int i = 0; i < num; i++) {
    		int s_s;
    		waitpid(splitter_id[i], &s_s, 0);
    		if (WIFEXITED(s_s) && WEXITSTATUS(s_s)) {
        		print_nonzero_exit_status("./splitter", WEXITSTATUS(s_s));
    		}
    	}
		for (int i = 0; i < num; i++) {
    		int m_s;
    		waitpid(mapper_id[i], &m_s, 0);
    		if (WIFEXITED(m_s) && WEXITSTATUS(m_s)) {
        	print_nonzero_exit_status(argv[3], WEXITSTATUS(m_s));
    		}
    	}
		int r_s;
    	waitpid(reducer_id, &r_s, 0);

    	if (WIFEXITED(r_s) && WEXITSTATUS(r_s)) {
    		print_nonzero_exit_status(argv[4], WEXITSTATUS(r_s));
		}
   	}
    // Print nonzero subprocess exit codes.
	print_num_lines(argv[2]);
	close(input_start);
	close(output_end);

    // Count the number of lines in the output file.
    return 0;
}

