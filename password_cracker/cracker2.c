
#include "cracker2.h"
#include "format.h"
#include "utils.h"
#include "queue.h"

#include <crypt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


pthread_mutex_t mutex_c;
pthread_barrier_t barr;
pthread_rwlock_t rw_f;
int theImportant = 0;

typedef struct thread_st {
    int    tid;
    queue   *q;
    int    success;
    int    count_thread;
    int    fails;
} thread_st;


typedef struct pcrack_st {
    char *user;
    int  password_length;
    char *clare;
    int  unknown_length;
    char *prefix;
    char *password;
    long count_hash;
    int  prefix_length;

} pcrack_st;



pcrack_st *initializing(const char *user, const char *clare, const char *clue);
char *realCracking(pcrack_st *temp, int tid, int count_thread, long *count_hash);


void *realCracking_start(void *arg)
{
    thread_st *tempArgs = (thread_st*)arg;
    int tid    = tempArgs->tid;
    queue *task_queue   = tempArgs->q;
    
    pcrack_st  *temp  = NULL;
    long   count_hash    = 0;
    char  *password = NULL;
    
  
    while (true) {
        
        temp = (pcrack_st*)queue_pull(task_queue);
        
        if (temp == NULL)
            break;
        
        password = realCracking(temp, tempArgs->tid, tempArgs->count_thread, &count_hash);
        
        pthread_mutex_lock(&mutex_c);
        if (password != NULL) {
            v2_print_thread_result(tid, count_hash, 0);
            temp->password = password;
        } else if (password == NULL && theImportant == 1) {
            v2_print_thread_result(tid, count_hash, 1);
        } else {
            v2_print_thread_result(tid, count_hash, 2);
        }
       
        temp->count_hash += count_hash;
        
        pthread_mutex_unlock(&mutex_c);
        pthread_barrier_wait(&barr);
    }
    
    return NULL;
}

pcrack_st *initializing(const char *user, const char *clare, const char *clue)
{
    
    pcrack_st *temp = malloc(sizeof(pcrack_st));
    memset(temp, 0, sizeof(pcrack_st));
    
    
    int length1 = strlen(user);
    int length2 = strlen(clare);
    
    temp->user = malloc(sizeof(char) * (length1 + 1));
    temp->clare = malloc(sizeof(char) * (length2 + 1));
    
    memcpy(temp->user, user, length1 + 1);
    memcpy(temp->clare, clare, length2 + 1);
    
    temp->prefix_length  = getPrefixLength(clue);
    temp->password_length      = strlen(clue);
    
    temp->unknown_length = temp->password_length - temp->prefix_length;
    
    temp->prefix         = malloc(sizeof(char) * (temp->prefix_length + 1));
    
    memset(temp->prefix, 0, temp->prefix_length + 1);
    memcpy(temp->prefix, clue, temp->prefix_length);
    
    return temp;
}

int start(size_t count_thread) {
   
    char bufallow[769], user[257], clare[257], clue[257];

    pcrack_st *temp_task;
    queue *task_queue;
    
    task_queue = queue_create(count_thread);
    pthread_t*   threads[count_thread];
    thread_st* tempArgs[count_thread];
    

    
    double       cpu_time_s, cpu_time_e, wall_time_s, wall_time_e;
    int          result;
        size_t i;

    pthread_barrier_init(&barr, NULL, count_thread + 1);
    pthread_rwlock_init(&rw_f, NULL);
    pthread_mutex_init(&mutex_c, NULL);
    
    for (i=0; i<count_thread; i++) {
        tempArgs[i]  = malloc(sizeof(thread_st));
        
        threads[i] = malloc(sizeof(pthread_t));
        
        memset(tempArgs[i], 0, sizeof(thread_st));
        
        tempArgs[i]->tid = i + 1;
        tempArgs[i]->count_thread = count_thread;
        
        tempArgs[i]->q   = task_queue;
        
        pthread_create(threads[i], NULL, &realCracking_start, (void*)tempArgs[i]);
    }

    while(fgets(bufallow, 768, stdin) != NULL) {
        
        sscanf(bufallow, "%s %s %s", user, clare, clue);
        temp_task = initializing(user, clare, clue);
        
        pthread_rwlock_wrlock(&rw_f);
        theImportant = 0;
        pthread_rwlock_unlock(&rw_f);
        
        cpu_time_s = getCPUTime();
        wall_time_s = getTime();
        
        v2_print_start_user(temp_task->user);

        for (i=0; i<count_thread; i++) {
            queue_push(task_queue, (void*)temp_task);
        }
  
        pthread_barrier_wait(&barr);
        cpu_time_e   = getCPUTime();
        wall_time_e  = getTime();
        
        result = temp_task->password == NULL ? 1 : 0;
        v2_print_summary(temp_task->user, temp_task->password, temp_task->count_hash, wall_time_e - wall_time_s, cpu_time_e - cpu_time_s, result);
        
        free(temp_task->user);
        free(temp_task->clare);
        free(temp_task->prefix);
        free(temp_task->password);
        free(temp_task);

    }
    
    for (i=0; i<count_thread; i++) {
        queue_push(task_queue, NULL);
    }
    
    for (i=0; i<count_thread; i++) {
        pthread_join(*(threads[i]), NULL);
    }
    
    for (i=0; i<count_thread; i++) {
        free(threads[i]);
        free(tempArgs[i]);
    }
    
    pthread_barrier_destroy(&barr);
    pthread_rwlock_destroy(&rw_f);
    pthread_mutex_destroy(&mutex_c);
    
    queue_destroy(task_queue);
    return 0; // DO NOT change the return code since AG uses it to check if your
    // program exited normally
}


char *realCracking(pcrack_st *temp, int tid, int count_thread, long *count_hash)
{
    long idx;
    long count;
    
    char *pw_try = malloc(sizeof(char) * (temp->password_length + 1));
    
    
    
    
    memset(pw_try,   0, temp->password_length+1);
    memset(pw_try, 'a', temp->password_length);
    memcpy(pw_try, temp->prefix, temp->prefix_length);
    
    
    getSubrange(temp->unknown_length, count_thread, tid, &idx, &count);
    
    
    for (int i=0; i<idx; i++) {
        incrementString(pw_try);
    }
    
    pthread_mutex_lock(&mutex_c);
    v2_print_thread_start(tid, temp->user, idx, pw_try);
    pthread_mutex_unlock(&mutex_c);
    
    
    long i = 0;
    const char *encrypted;
    struct crypt_data crypt_data;
    
    crypt_data.initialized = 0;
    
    for (i=0; i<count; i++) {
        encrypted = crypt_r(pw_try, "xx", &crypt_data);
        
        if (strcmp(encrypted, temp->clare) != 0) {
	
		incrementString(pw_try);
        } else {
		
	    char *result = malloc(sizeof(char) * (temp->password_length + 1));
            memset(result, 0, temp->password_length+1);
            memcpy(result, pw_try, temp->password_length);
            *count_hash = i+1;

            pthread_rwlock_wrlock(&rw_f);
            theImportant = 1;
            pthread_rwlock_unlock(&rw_f);

            free(pw_try);
            return result;

        }

        
        pthread_rwlock_rdlock(&rw_f);
 

       if (theImportant) {
            *count_hash = i+1;

            free(pw_try);

            pthread_rwlock_unlock(&rw_f);
            
            return NULL;
        }

        pthread_rwlock_unlock(&rw_f);
    }
    
    *count_hash = count;
    
    free(pw_try);
    return NULL;
}


