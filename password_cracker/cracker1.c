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


/*

#include "cracker1.h"
#include "format.h"
#include "utils.h"
#include "queue.h"

#include <stdio.h>
#include <string.h>
#include <crypt.h>

typedef struct thread_st {
    int    tid;
    queue  *q;

    int    success;
    int    fails;
} thread_st;

typedef struct pcrack_st {
    
    int  pw_leng;
    int  unknown_leng;

    char *user;
    char *clare;

    char *prefix;
    int  prefix_leng;

} pcrack_st;


pcrack_st *initializing(const char *user, const char *clare, const char *clue);
char *realCracking(pcrack_st *temp, int tid, int count_thread, long *count_hash);
void *realCracking_start(void *arg)
{
    thread_st *tempArgs = (thread_st*)arg;

    queue *tempQ   = tempArgs->q;
    int tempTid    = tempArgs->tid;

    long   countingHash    = 0;
    char  *password = NULL;
    double time_s   = 0.0;
    pcrack_st  *temp  = NULL;
    double time_e     = 0.0;

    while (true) {

        temp = (pcrack_st*)queue_pull(tempQ);

        if (!temp)
            break;

        v1_print_thread_start(tempTid, temp->user);
        time_s = getThreadCPUTime();
        password = realCracking(temp, 1, 1, &countingHash);
        time_e   = getThreadCPUTime();


        if (!password) {
            v1_print_thread_result(tempTid, temp->user, password, countingHash, time_e - time_s, 1);
            tempArgs->fails++;
        }else{
            v1_print_thread_result(tempTid, temp->user, password, countingHash, time_e - time_s, 0);
            tempArgs->success++;
        }

        free(password);
        free(temp->user);
        free(temp->clare);
        free(temp->prefix);
        free(temp);
    }

    return NULL;
}

pcrack_st *initializing(const char *user, const char *clare, const char *clue)
{

    pcrack_st *temp = malloc(sizeof(pcrack_st));
    memset(temp, 0, sizeof(pcrack_st));

    int length1 = strlen(clare);
    int length2 = strlen(user);

    temp->clare = malloc(sizeof(char) * (length1 + 1));
    temp->user = malloc(sizeof(char) * (length2 + 1));

    memcpy(temp->clare, clare, length1 + 1);
    memcpy(temp->user, user, length2 + 1);

    temp->pw_leng      = strlen(clue);
    temp->prefix_leng  = getPrefixLength(clue);

    temp->unknown_leng = temp->pw_leng - temp->prefix_leng;

    temp->prefix         = malloc( sizeof(char) * (temp->prefix_leng + 1));

    memset(temp->prefix, 0, temp->prefix_leng + 1);
    memcpy(temp->prefix, clue, temp->prefix_leng);

    return temp;
}


int start(size_t thread_count) {
   
    char line_buffer[769], user[257], clare[257], clue[257];
    
    pcrack_st *temp_task;
    queue *task_queue;
     size_t i;
    task_queue = queue_create(thread_count);
    pthread_t*   threads[thread_count];
    thread_st* t_args[thread_count];
    

    for (i=0; i<thread_count; i++) {
        threads[i] = malloc(sizeof(pthread_t));
    
        t_args[i]  = malloc(sizeof(thread_st));
        memset(t_args[i], 0, sizeof(thread_st));
    
        t_args[i]->tid = i + 1;
        t_args[i]->q   = task_queue;
        
        pthread_create(threads[i], NULL, &realCracking_start, (void*)t_args[i]); 
    }
    
    while(fgets(line_buffer, 768, stdin) != NULL) {
        sscanf(line_buffer, "%s %s %s", user, clare, clue);
        temp_task = initializing(user, clare, clue);
        
        queue_push(task_queue, (void*)temp_task);
    }

    for (i=0; i<thread_count; i++) 
        queue_push(task_queue, NULL);
    
    for (i=0; i<thread_count; i++)
        pthread_join(*(threads[i]), NULL);

    int num_r = 0;
    int num_f = 0;

    for (i=0; i<thread_count; i++) {
        num_r += t_args[i]->success;
        num_f += t_args[i]->fails;
    }

    v1_print_summary(num_r, num_f);

    for (i=0; i<thread_count; i++) {
        free(threads[i]);
        free(t_args[i]);
    }

    queue_destroy(task_queue);

    return 0; // DO NOT change the return code since AG uses it to check if your
              // program exited normally


}


char *realCracking(pcrack_st *temp, int tid, int count_thread, long *count_hash)
{   
    long idx;
    long count;
    
    char *pw_try = malloc(sizeof(char) * (temp->pw_leng + 1));
    memset(pw_try,   0, temp->pw_leng+1);
    memset(pw_try, 'a', temp->pw_leng); 
    memcpy(pw_try, temp->prefix, temp->prefix_leng);
    
    getSubrange(temp->unknown_leng, count_thread, tid, &idx, &count);
    
    
    for (int i=0; i<idx; i++) 
        incrementString(pw_try);
    
    long i = 0;  
    const        char *encrypted;
    struct crypt_data  crypt_data;
    
    crypt_data.initialized = 0;
    
    for (i=0; i<count; i++) {
        encrypted = crypt_r(pw_try, "xx", &crypt_data);
        
        if (strcmp(encrypted, temp->clare) == 0) {
            char *result = malloc(sizeof(char) * (temp->pw_leng + 1));
            memset(result, 0, temp->pw_leng+1);
            memcpy(result, pw_try, temp->pw_leng);
            *count_hash = i+1;
            
            free(pw_try);
            return result;
        } else {
            incrementString(pw_try);
        }
    }
    
    *count_hash = count;
    
    free(pw_try);
    return NULL;
}

*/
