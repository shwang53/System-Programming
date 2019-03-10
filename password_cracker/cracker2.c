/**
 * Password Cracker Lab
 * CS 241 - Spring 2019
 */
 
#include "cracker2.h"
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
#include <semaphore.h>

pthread_mutex_t mmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t mbarrier;
char** names;
char** hashes;
char** ff;
size_t total;
long finished;

void* pcrack(void* t_id) {
    struct crypt_data cd;
    cd.initialized = 0;
    int ntid = (long)t_id;
    
    while (1) {
	pthread_mutex_lock(&mmutex);
	long proc_it = finished;
	size_t tn = total;
	pthread_mutex_unlock(&mmutex);

	if (proc_it == -1) {
	    break;
	}	
	char* name_it = names[proc_it];
	char* hash_it = hashes[proc_it];
	char* ff_it = ff[proc_it];

	char* dot = strstr(ff_it, ".");
	int fflen = strlen(ff_it);
	char* end = &ff_it[fflen-1];
	int dot_count = end - dot + 1;
	
	long start = 0;
	long count = 0;
	getSubrange(dot_count, tn, ntid+1, &start, &count);
	setStringPosition(dot, start);

	v2_print_thread_start((ntid+1), name_it, start, *ff);

	long it = 0;
	long mid = 0;
	long fb = 0;
	for (it = 0; it < count; it++) {
	    pthread_mutex_lock(&mmutex);
	    if (finished) {
		mid = 1;
	    }
	    pthread_mutex_unlock(&mmutex);
	    if (mid) {
		break;
	    }
	    const char* crpyted = crypt_r(ff_it, "xx", &cd);
	    if (strcmp(crpyted, hash_it) == 0) {
		pthread_mutex_lock(&mmutex);
		finished = 1;
		pthread_mutex_unlock(&mmutex);
		fb = 1;
		break;
	    }
	    incrementString(ff_it);
	}
	if (fb+mid == 0) {
	    v2_print_thread_result(ntid+1, it, 2);
	}
	else if (mid) {
	    v2_print_thread_result(ntid+1, it+1, 1);
	}
	else if (mid) {
	    v2_print_thread_result(ntid+1, it+1, 0);
	}
	pthread_barrier_wait(&mbarrier);
	pthread_barrier_wait(&mbarrier);
	//proc_it = -1;
    }
    return NULL;
}
int start(size_t thread_count) {
    // TODO your code here, make sure to use thread_count!
    // Remember to ONLY crack passwords in other threads
  names = (char**) malloc(1024*sizeof(char*));
    hashes = (char**) malloc(1024*sizeof(char*));
    ff = (char**) malloc(1024*sizeof(char*));
    for (int d = 0; d < 1024; d++) {
	names[d] = (char*)malloc(1024*sizeof(char));
	hashes[d] = (char*)malloc(1024*sizeof(char));
	ff[d] = (char*)malloc(1024*sizeof(char));
    }
    
    pthread_barrier_init(&mbarrier, NULL, thread_count + 1);

    char* line = NULL;
    size_t len = 0;
    ssize_t nread = 0;
    long idx = 0;

    while ((nread = (getline(&line, &len, stdin))) != -1) {
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

	free(line);
	line = NULL;
	idx++;
    }
    free(line);
    line = NULL;

    pthread_t workers[thread_count];
    for (size_t b = 0; b < thread_count; b++) {
	pthread_create(&workers[b], NULL, &pcrack, (void*)b);
    }

    for (long c = 0; c < idx; c++) {
	double nt = getTime();
	double cpt = getCPUTime();
	pthread_mutex_lock(&mmutex);
	v2_print_start_user(names[c]);
	pthread_mutex_unlock(&mmutex);

	char* mystr = NULL;
	int hc = 0;
	pthread_barrier_wait(&mbarrier);
	pthread_mutex_lock(&mmutex);
	v2_print_summary(names[c], mystr, hc, getTime()-nt, getCPUTime()-cpt, 0);
	pthread_mutex_unlock(&mmutex);
	pthread_barrier_wait(&mbarrier);
    }

    for (unsigned long j = 0; j < thread_count; j++) {
	pthread_join(workers[j], NULL);
    }

    for (int l = 0; l < 1024; l++) {
	free(names[l]);
	free(hashes[l]);
	free(ff[l]);
	names[l] = NULL;
	hashes[l] = NULL;
	ff[l] = NULL;
    }
    free(names);
    free(hashes);
    free(ff);
    names = NULL;
    hashes = NULL;
    ff = NULL;

    pthread_barrier_destroy(&mbarrier);
    pthread_mutex_destroy(&mmutex);
	
	  return 0; // DO NOT change the return code since AG uses it to check if your
              // program exited normally
}
