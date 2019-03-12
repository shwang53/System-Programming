/**
 * Password Cracker Lab
 * CS 241 - Spring 2019
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


