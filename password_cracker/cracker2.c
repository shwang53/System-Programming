/**
 * Password Cracker Lab
 * CS 241 - Spring 2019
 */
/* 
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
*/

/**
 * Machine Problem: Password Cracker
 * CS 241 - Fall 2017
 */

#include "cracker2.h"
#include "format.h"
#include "utils.h"
#include "queue.h"

#include <crypt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int g_found_pw = 0;
pthread_rwlock_t g_rw_found;   // using a mutex on write will cause thread sanitizer to throw a warning, but 
                               // it is unnecessary to have mutex on all read operations where only 1 thread 
                               // will write to the global variable for a user. Thus, we will use a read write
                               // lock
pthread_mutex_t g_mt_console;
pthread_barrier_t g_barrier;

typedef struct task_st {
    char *username;
    char *digest;
    char *prefix;
    int  prefix_length;
    int  pw_length;
    int  unknown_length;

    char *pw;
    long hash_count;
} task;

typedef struct thread_args_st {
    int    tid;
    int    thread_count;
    queue *q;
    int    passes;
    int    fails;
} thread_args;

// Create a task object that contains all the information about cracking one password
task *create_task(const char *username, const char *digest, const char *hint)
{
    // Allocate and initialize an object
    task *t = malloc(sizeof(task));
    memset(t, 0, sizeof(task)); 
    
    int length = 0;
    // allocate space for username, and copy from temp buffer
    length = strlen(username);
    t->username = malloc(sizeof(char) * (length + 1));   // +1 is for null termination
    memcpy(t->username, username, length + 1);

    // allocate space for digest, and copy from temp buffer
    length = strlen(digest);
    t->digest = malloc(sizeof(char) * (length + 1));
    memcpy(t->digest, digest, length + 1);

    // allocate space to store the prefix, also record information about password
    t->prefix_length  = getPrefixLength(hint);
    t->pw_length      = strlen(hint);
    t->unknown_length = t->pw_length - t->prefix_length;
    t->prefix         = malloc(sizeof(char) * (t->prefix_length + 1));
    memset(t->prefix, 0, t->prefix_length + 1);
    memcpy(t->prefix, hint, t->prefix_length);

    return t;
}

void delete_task(task *t)
{
    free(t->username);
    free(t->digest);
    free(t->prefix);
    free(t->pw);
    free(t);
}

char *crack_pw(task *t, int tid, int thread_count, long *hash_count)
{
    long start_idx;
    long attempt_count;

    // create the initial password attempt string
    char *pw_attempt = malloc(sizeof(char) * (t->pw_length + 1));
    memset(pw_attempt,   0, t->pw_length+1);  // initialize to 0s
    memset(pw_attempt, 'a', t->pw_length);  // fill with 'a' except for null termination
    memcpy(pw_attempt, t->prefix, t->prefix_length);

    // determine the number of attempts on a thread
    getSubrange(t->unknown_length, thread_count, tid, &start_idx, &attempt_count);

    // set the attempt string to the appropriate start
    for (int i=0; i<start_idx; i++) {
        incrementString(pw_attempt);
    }

    pthread_mutex_lock(&g_mt_console);
    v2_print_thread_start(tid, t->username, start_idx, pw_attempt);
    pthread_mutex_unlock(&g_mt_console);

    // brute force password cracking
    long i = 0;
    const        char *hashed;
    struct crypt_data  cdata;
    cdata.initialized = 0;

    for (i=0; i<attempt_count; i++) {
        hashed = crypt_r(pw_attempt, "xx", &cdata);
        
        // stop when the generated hash matches the actual hash
        if (strcmp(hashed, t->digest) == 0) {
            char *result = malloc(sizeof(char) * (t->pw_length + 1));
            memset(result, 0, t->pw_length+1);
            memcpy(result, pw_attempt, t->pw_length);
            *hash_count = i+1; 

            pthread_rwlock_wrlock(&g_rw_found);
            g_found_pw = 1;
            pthread_rwlock_unlock(&g_rw_found);

            free(pw_attempt);
            return result;
        } else {
            incrementString(pw_attempt);
        }

        pthread_rwlock_rdlock(&g_rw_found);
        if (g_found_pw) {
            *hash_count = i+1;
            free(pw_attempt);
            pthread_rwlock_unlock(&g_rw_found);

            return NULL;
        }
        pthread_rwlock_unlock(&g_rw_found);
    }

    *hash_count = attempt_count;

    free(pw_attempt);
    return NULL;
}

// arg is of type thread_args
void *pw_cracker_start(void *arg)
{
    thread_args *t_args = (thread_args*)arg;
    int          tid    = t_args->tid;
    queue *task_queue   = t_args->q;

    task  *t  = NULL;
    long   hash_count    = 0;
    char  *pw = NULL;
    
    // The PW cracker sits idle until there is an new entry into the task queue
    while (1) {
        // Check if there is an entry to the queue
        t = (task*)queue_pull(task_queue);

        if (t == NULL) {
            break;
        }
      
        // Crack password, would not stop until we found a solution
        pw = crack_pw(t, t_args->tid, t_args->thread_count, &hash_count);

        pthread_mutex_lock(&g_mt_console);
        if (pw != NULL) {
            v2_print_thread_result(tid, hash_count, 0);
            t->pw = pw;
        } else if (pw == NULL && g_found_pw == 1) {
            v2_print_thread_result(tid, hash_count, 1);
        } else {
            v2_print_thread_result(tid, hash_count, 2);
        }

        // Also update the result, it will the share same mutex as console
        t->hash_count += hash_count;

        pthread_mutex_unlock(&g_mt_console);

        // For each password cracked, all threads wait until we move on to the next password
        pthread_barrier_wait(&g_barrier);
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
    task *temp_task;
    queue *task_queue;
    
    task_queue = queue_create(thread_count);
    pthread_t*   threads[thread_count];
    thread_args* t_args[thread_count];
    size_t       i;

    double       cpu_start_time, cpu_end_time;
    double       wall_start_time, wall_end_time;
    int          result;

    // Barrier initialization
    pthread_barrier_init(&g_barrier, NULL, thread_count + 1);
    pthread_rwlock_init(&g_rw_found, NULL);
    pthread_mutex_init(&g_mt_console, NULL);

    for (i=0; i<thread_count; i++) {
        // Start the thread
        threads[i] = malloc(sizeof(pthread_t));
        t_args[i]  = malloc(sizeof(thread_args));
        memset(t_args[i], 0, sizeof(thread_args));
        t_args[i]->tid = i + 1;
        t_args[i]->thread_count = thread_count;
        t_args[i]->q   = task_queue;
        
        pthread_create(threads[i], NULL, &pw_cracker_start, (void*)t_args[i]); 
    }
    
    // The main thread will read the inputs and populate task queue
    while(fgets(line_buffer, 768, stdin) != NULL) {
        sscanf(line_buffer, "%s %s %s", username, digest, hint);
        temp_task = create_task(username, digest, hint);
        
        pthread_rwlock_wrlock(&g_rw_found);
        g_found_pw = 0;
        pthread_rwlock_unlock(&g_rw_found);

        cpu_start_time = getCPUTime();
        wall_start_time = getTime();

        v2_print_start_user(temp_task->username);

        // Push the task into queue x amount of times, where x = thread_count
        for (i=0; i<thread_count; i++) {
            queue_push(task_queue, (void*)temp_task);
        }

        // Wait until all threads finish cracking the current password
        pthread_barrier_wait(&g_barrier);
        cpu_end_time   = getCPUTime();
        wall_end_time  = getTime();

        result = temp_task->pw == NULL ? 1 : 0;
        v2_print_summary(temp_task->username, temp_task->pw, temp_task->hash_count, wall_end_time - wall_start_time, cpu_end_time - cpu_start_time, result);

        delete_task(temp_task);
    }

    // push NULL elements to terminate threads, each element will terminate one thread
    for (i=0; i<thread_count; i++) {
        queue_push(task_queue, NULL);
    }

    // Join all threads
    for (i=0; i<thread_count; i++) {
        pthread_join(*(threads[i]), NULL);
    }

    for (i=0; i<thread_count; i++) {
        free(threads[i]);
        free(t_args[i]);
    }

    pthread_barrier_destroy(&g_barrier);
    pthread_rwlock_destroy(&g_rw_found);
    pthread_mutex_destroy(&g_mt_console);

    queue_destroy(task_queue);
    return 0; // DO NOT change the return code since AG uses it to check if your
              // program exited normally
}
