/**
 * Password Cracker Lab
 * CS 241 - Spring 2019
 */
 
#include "cracker1.h"
#include "format.h"
#include "utils.h"


#include "queue.h"
#include <stdbool.h>
#include <crypt.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>

pthread_mutex_t mmutex = PTHREAD_MUTEX_INITIALIZER;
queue* tasks;
char** names;
char** hashes;
char** ff;
long finished;

void* pcrack(void* t_id) {
    struct crypt_data cd;
    cd.initialized = 0;
    int ntid = (long)t_id;
    long match = 0;

    while (1) {
	pthread_mutex_lock(&mmutex);
	long done = finished;
	pthread_mutex_unlock(&mmutex);
	if (done == 1) {
	    break;
	}

	long check = (long)queue_pull(tasks);
	if (check == -1) {
	    pthread_mutex_lock(&mmutex);
	    finished = 1;
	    pthread_mutex_unlock(&mmutex);
	    break;
	}
	
	char* name_it = names[check];
	char* hash_it = hashes[check];
	char* ff_it = ff[check];
	v1_print_thread_start((ntid+1), name_it);

	char* dot = strstr(ff_it, ".");
	int fflen = strlen(ff_it);
	char* end = &ff_it[fflen-1];
	int dot_count = end - dot + 1;
	int total = pow(26, dot_count);

        
	char rem[dot_count+1];
	memset(rem, 'a', dot_count);
	rem[dot_count] = 0;

	long foundit = 0;
	long k;
	for(k = 0; k < total; k++) {
	    int l = k;
	    int pos = dot_count - 1;

	    while (l > 0) {
		int t = l % 26;
		rem[pos] = t + rem[pos];
		pos--;
		l = (l - t) / 26;
	    }

	    strcpy(dot, rem);
	    const char* crypted = crypt_r(ff_it, "xx", &cd);
	    if (strcmp(crypted, hash_it) == 0) {
		v1_print_thread_result((ntid+1), name_it, ff_it, k+1, getThreadCPUTime(), 0);
		foundit = 1;
		match = match + 1;
		break;
	    }
	    memset(rem, 'a', dot_count);
	}
	if (foundit == 0) {
	    v1_print_thread_result((ntid+1), name_it, ff_it, k, getThreadCPUTime(), 1);
	}
    }
    return (void*)match;
}

int start(size_t thread_count) {
    // TODO your code here, make sure to use thread_count!
    // Remember to ONLY crack passwords in other threads
tasks = queue_create(1024);

    names = (char**) malloc(1024*sizeof(char*));
    hashes = (char**) malloc(1024*sizeof(char*));
    ff = (char**) malloc(1024*sizeof(char*));

    for (int c = 0; c < 1024; c++) {
	names[c] = (char*) malloc(1024*sizeof(char));
    	hashes[c] = (char*) malloc(1024*sizeof(char));
    	ff[c] = (char*) malloc(1024*sizeof(char));
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t nread = 0;
    long idx = 0;

    while ((nread = (getline(&line, &len, stdin))) != -1) {
	//set last line to null
	if (line[nread-1] == '\n') {
	    line[nread-1] = '\0';
	}
	
	//push in hashes
	char* t1 = strstr(line, " ");
	*t1 = '\0';
	strcpy(names[idx], line);
	//push in ff
	char* t2 = strstr(t1+1, " ");
	*t2 = '\0';
	
	strcpy(hashes[idx], t1+1);
	strcpy(ff[idx], t2+1);
	
	queue_push(tasks, (void*)idx);
	
	free(line);
	line = NULL;
	idx++;
    }
    free(line);
    line = NULL;

    queue_push(tasks, (void*)-1);
    queue_push(tasks, (void*)-1);
    queue_push(tasks, (void*)-1);
    queue_push(tasks, (void*)-1);

    pthread_t workers[thread_count];

    for (size_t b = 0; b < thread_count; b++) {
	pthread_create(&workers[b], NULL, &pcrack, (void*)b);
    }

    long numrec = 0;
    void* retval;
    for (size_t a = 0; a < thread_count; a++) {
	pthread_join(workers[a], &retval);
	numrec = (long)retval + numrec;
    }

    v1_print_summary(numrec, idx - numrec);
    pthread_mutex_destroy(&mmutex);
    queue_destroy(tasks);
    
    for (int c = 0; c < 1024; c++) {
	free(names[c]);
	free(hashes[c]);
	free(ff[c]);
	names[c] = NULL;
	hashes[c] = NULL;
	ff[c] = NULL;
    }
    free(names);
    free(hashes);
    free(ff);
    names = NULL;
    hashes = NULL;
    ff = NULL;

    return 0; // DO NOT change the return code since AG uses it to check if your
              // program exited normally
}
