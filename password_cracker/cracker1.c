/**
 * Password Cracker Lab
 * CS 241 - Spring 2019
 */
/* 
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
*/

/**
 * Machine Problem: Password Cracker
 * CS 241 - Fall 2017
 */

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
    
//    int  prefix_leng;
    int  pw_leng;
    int  unknown_leng;

    char *user;
    char *digest;

    char *prefix;
    int  prefix_leng;

} pcrack_st;

// Create a task object that contains all the information about cracking one password
pcrack_st *initializing(const char *user, const char *digest, const char *hint)
{
    // Allocate and initialize an object
    pcrack_st *temp = malloc(sizeof(pcrack_st));
    memset(temp, 0, sizeof(pcrack_st)); 
    
    int length = 0;
    // allocate space for username, and copy from temp buffer
    length = strlen(user);
    temp->user = malloc(sizeof(char) * (length + 1));   // +1 is for null termination
    memcpy(temp->user, user, length + 1);

    // allocate space for digest, and copy from temp buffer
    length = strlen(digest);
    temp->digest = malloc(sizeof(char) * (length + 1));
    memcpy(temp->digest, digest, length + 1);

    // allocate space to store the prefix, also record information about password
    temp->prefix_leng  = getPrefixLength(hint);
    temp->pw_leng      = strlen(hint);
    temp->unknown_leng = temp->pw_leng - temp->prefix_leng;
    temp->prefix         = malloc(sizeof(char) * (temp->prefix_leng + 1));
    memset(temp->prefix, 0, temp->prefix_leng + 1);
    memcpy(temp->prefix, hint, temp->prefix_leng);

    return temp;
}

void delete_pcrack(pcrack_st *temp) 
{
    free(temp->user);
    free(temp->digest);
    free(temp->prefix);
    free(temp);
}

char *realCracking(pcrack_st *temp, int tid, int thread_count, long *hash_count)
{
    long start_idx;
    long attempt_count;

    // create the initial password attempt string
    char *pw_attempt = malloc(sizeof(char) * (temp->pw_leng + 1));
    memset(pw_attempt,   0, temp->pw_leng+1);  // initialize to 0s
    memset(pw_attempt, 'a', temp->pw_leng);  // fill with 'a' except for null termination
    memcpy(pw_attempt, temp->prefix, temp->prefix_leng);

    // determine the number of attempts on a thread
    getSubrange(temp->unknown_leng, thread_count, tid, &start_idx, &attempt_count);

    // set the attempt string to the appropriate start
    for (int i=0; i<start_idx; i++) {
        incrementString(pw_attempt);
    }

    // brute force password cracking
    long i = 0;
    const        char *hashed;
    struct crypt_data  cdata;
    cdata.initialized = 0;

    for (i=0; i<attempt_count; i++) {
        hashed = crypt_r(pw_attempt, "xx", &cdata);
        
        // stop when the generated hash matches the actual hash
        if (strcmp(hashed, temp->digest) == 0) {
            char *result = malloc(sizeof(char) * (temp->pw_leng + 1));
            memset(result, 0, temp->pw_leng+1);
            memcpy(result, pw_attempt, temp->pw_leng);
            *hash_count = i+1; 

            free(pw_attempt);
            return result;
        } else {
            incrementString(pw_attempt);
        }
    }

    *hash_count = attempt_count;

    free(pw_attempt);
    return NULL;
}

// arg is of type thread_args
void *realCracking_start(void *arg)
{
    thread_st *t_args = (thread_st*)arg;
    int          tid    = t_args->tid;
    queue *task_queue   = t_args->q;

    pcrack_st  *t  = NULL;
    long   hash_count    = 0;
    char  *pw = NULL;
    double start_time   = 0.0;
    double end_time     = 0.0;
    
    // The PW cracker sits idle until there is an new entry into the task queue
    while (1) {
        // Check if there is an entry to the queue
        t = (pcrack_st*)queue_pull(task_queue);

        if (t == NULL) {
            break;
        }
      
        v1_print_thread_start(tid, t->user); 

        // Crack password, would not stop until we found a solution
        start_time = getThreadCPUTime();
        pw = realCracking(t, 1, 1, &hash_count);
        end_time   = getThreadCPUTime();

        if (pw != NULL) {
            v1_print_thread_result(tid, t->user, pw, hash_count, end_time - start_time, 0);
            t_args->success++;
        } else {
            v1_print_thread_result(tid, t->user, pw, hash_count, end_time - start_time, 1);
            t_args->fails++;
        }
        free(pw);
        delete_pcrack(t);
    }

    return NULL;
}

int start(size_t thread_count) {
    // Remember to ONLY crack passwords in other threads
    char line_buffer[769];
    char username[257];
    char digest[257];
    char hint[257];
    
    // Task Queue
    pcrack_st *temp_task;
    queue *task_queue;
    
    task_queue = queue_create(thread_count);
    pthread_t*   threads[thread_count];
    thread_st* t_args[thread_count];
    size_t       i;

    for (i=0; i<thread_count; i++) {
        // Start the thread
        threads[i] = malloc(sizeof(pthread_t));
        t_args[i]  = malloc(sizeof(thread_st));
        memset(t_args[i], 0, sizeof(thread_st));
        t_args[i]->tid = i + 1;
        t_args[i]->q   = task_queue;
        
        pthread_create(threads[i], NULL, &realCracking_start, (void*)t_args[i]); 
    }
    
    // The main thread will read the inputs and populate task queue
    while(fgets(line_buffer, 768, stdin) != NULL) {
        sscanf(line_buffer, "%s %s %s", username, digest, hint);
        temp_task = initializing(username, digest, hint);
        
        queue_push(task_queue, (void*)temp_task);
    }

    // push NULL elements to terminate threads, each element will terminate one thread
    for (i=0; i<thread_count; i++) {
        queue_push(task_queue, NULL);
    }

    // Join all threads
    for (i=0; i<thread_count; i++) {
        pthread_join(*(threads[i]), NULL);
    }

    // Print summary
    int num_recovered = 0;
    int num_failed    = 0;
    for (i=0; i<thread_count; i++) {
        num_recovered += t_args[i]->success;
        num_failed    += t_args[i]->fails;
    }
    v1_print_summary(num_recovered, num_failed);

    for (i=0; i<thread_count; i++) {
        free(threads[i]);
        free(t_args[i]);
    }

    queue_destroy(task_queue);
    return 0; // DO NOT change the return code since AG uses it to check if your
              // program exited normally
}
