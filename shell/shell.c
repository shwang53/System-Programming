/**
 * Shell Lab
 * CS 241 - Spring 2019
 */


#include "format.h"
#include "shell.h"
#include "vector.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>

typedef struct process {
    char *command;
    char *status;
    pid_t pid;
} process;

static pid_t children[10];
static int it;
static int temp;
static int fd;


void sigkillf() {
    for (int i = 0; i < 10; i++) {
	if (children[i] > 0) {
	    kill(children[i], SIGKILL);
	}
    }
}

void setczero() {
    for (int i = 0; i < 10; i++) {
	children[i] = 0;
    }
}

int execute(char* buf, vector* histv, int prompt, int cmd) {
    if (it == 1) {
	return 0;
    }

	return 1;
}


int shell(int argc, char *argv[]) {
    // TODO: This is the entry point for your shell.
    if (argc == 1 || argc == 2 || argc == 4 || argc > 5) {
	//invalid input
	print_usage();
	return 0;
    }
    
    signal(SIGINT, sigkillf);
    setczero();
    it = 0;
    temp = 0;
	
	char *full_path = get_full_path(argv[optind]);
//    vector* histv = string_vector_create();
    
    bool fexe = false;
    bool his = false;

    char* f_n = calloc(1024, sizeof(char));
    char* h_n = calloc(1024, sizeof(char));

    for (int i = 1; i <= argc; i++) {
	if (!strcmp(argv[i], "-h")) {
	    his = true;
	    strcpy(h_n, argv[1]);
	    fd = open(h_n, O_CREAT | O_RDWR | O_TRUNC, 0666);
	    signal(SIGINT, SIG_IGN);
		break;
	}
	if (!strcmp(argv[i], "-f")) {
	    fexe = true;
	    strcpy(f_n, argv[1]);
		break;
	}
    }
   
	//load history 
    if (fexe) {
	FILE* fd = fopen(full_path, "r");
	if (!fd) {
	    print_script_file_error();
	    return 0;
	}

	char* line = NULL;
	size_t len = 0;
	ssize_t nread = 0;
//	 vector* histv = string_vector_create();

	    char* dir = NULL;
	dir = getcwd(dir, 1024);
	while ((nread = (getline(&line, &len, fd))) != -1) {
	    if (line[nread - 1] == '\n') {
		line[nread - 1] = '\0';
	    }
	    char bb[2][1024];
	    char* t1 = NULL;
    char* t2 = NULL;
	    int choose;
            if ((t1 = strstr(line, "&&"))) {
		*t1 = '\0';
		t2 = t1 - 1;
		while (*t2 == ' ') {
		    t2--;
		}
		char t3 = t2[1];
		t2[1] = '\0';
		strcpy(bb[0], line);
		t2[1] = t3;
		*t1 = '&';
		t1 = t1 + 2;
		while (*t1 == ' ') {
		    t1++;
		}
		strcpy(bb[1], t1);
		choose = 0;//1
	    }
	    else if ((t1 = strstr(line, "||"))) {
		*t1 = '\0';
		t2 = t1 - 1;
		while (*t2 == ' ') {
		    t2--;
		}
		char t3 = t2[1];
		t2[1] = '\0';
		strcpy(bb[0], line);
		t2[1] = t3;
		*t1 = '|';
		t1 = t1 + 2;
		while (*t1 == ' ') {
		    t1++;
		}
		strcpy(bb[1], t1);
		choose = 1;//2
	    }
	    else if ((t1 = strstr(line, ";"))) {
		*t1 = '\0';
		t2 = t1 - 1;
		while (*t2 == ' ') {
		    t2--;
		}
		char t3 = t2[1];
		t2[1] = '\0';
		strcpy(bb[0], line);
		t2[1] = t3;
		*t1 = ';';
		t1 = t1 + 2;
		while (*t1 == ' ') {
		    t1++;
		}
		strcpy(bb[1], t1);
		choose = 2;//3
	    }
	    else {
		choose = 3;
	    }
/*

	    if (choose == 0) {
		if (run(bb[0][histv], 1, 1) == 0) {
		    run(bb[1][histv], 1, 1);
		}
	    }
	    else if (choose == 1) {
		if (run(bb[0][histv], 1, 1) == 0) {
		    continue;
		}
		else {
		    run(bb[1][histv], 1, 1);
		}
	    }
	    else if (choose == 2) {
		run(bb[0][histv], 1, 1);
                run(bb[1][histv], 1, 1);
	    }
	    else if (choose == 3) {
		run(line, 1, 1);
	    }

	    free(line);
	    line = NULL;
	}
	free(line);
	line = NULL;

	if (his) {
	    
	}


	    
	    vector_push_back(hiv, line);
	    print_prompt(dir, getpid());
	    printf("%s\n", line);

	    if (strncmp(line, "cd", 2) == 0) {
		char* cdir = line + 3;
		if (chdir(cdir) == -1 || cdir == NULL) {
		    print_no_directory(cdir);
		}
		else {
		    strcat(dir, "/");
		    strcat(dir, cdir);
		}
	    }
	    else if (strncmp(line, "!", 1)) {
		if (strcmp(line, "!history")) {
		    for (size_t i = 0; i < vector_size(hiv); i++) {
			print_history_line(i, vector_get(hiv, i));
		    }
		}
		else {
		    char* temp = NULL;
		    char* cmd = line + 1;
		    VECTOR_FOR_EACH(hiv, thing, {
			    if (strncmp(cmd, thing, 1) == 0) {
				temp = thing;
			    }
			});
		    printf("%s\n", temp);
		}
	    }
	    else {
		vector_push_back(hiv, line);
		pid_t mc = fork();
		int stat;
		if (mc > 0) {
		    if (setpgid(mc, mc) == -1) {
			print_setpgid_failed();
			exit(1);
		    }
		    children[it] = mc;
		    it++;
		    waitpid(it, &stat, 0);
		}
		else if (mc < 0) {
		    print_fork_failed();
		}
		else {
		    char* mbuf[1024];
		    char* mt = strtok(line, " ");
		    int i = 0;
		    for (; mt != NULL; i++) {
			mbuf[i] = mt;
			mt = strtok(NULL, " ");
		    }
		    mbuf[i] = NULL;
		

		print_command_executed(getpid());
		int err = execvp(*mbuf, mbuf);
		if (err == -1) {
		    print_exec_failed(line);
		}
		exit(9);
		}
	    }
	    
    }
    free(dir);
    free(line);
    dir = NULL;
    line = NULL;
    vector_destroy(hiv);
    fclose(fd);
    exit(0);    

	execute_f(f_n, his, histv);
    }
    
    else if (his) {
	char* buffer = NULL;
	size_t len = 0;
	ssize_t nread = 0;
	while ((nread = getline(&buffer, &len, stdin)) != -1) {
	    if (buffer[nread - 1] == '\n') {
		buffer[nread - 1] = '\0';
	    }
	    if (len > 0) {
		execute_f(buffer, his, histv);
	    }
	}
	free(buffer);
	buffer = NULL;
    }
    if (feof(stdin)) {
	if (his == true) {
	    for (size_t j = 0; j < vector_size(histv); j++) {
		dprintf(fd, "%s\n", (char*)vector_get(histv, j));
	    }
	}
    }
    
*/

}
} 
    return 0;
}


