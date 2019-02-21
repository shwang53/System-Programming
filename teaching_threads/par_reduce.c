/**
 * Teaching Threads Lab
 * CS 241 - Spring 2019
 */
 
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "reduce.h"
#include "reducers.h"

static int * lis_;
static int baseCase_;
static reducer function_;
static size_t _num_threads;
static size_t elemLen_;


/* You might need a struct for each task ... */
typedef struct _reduce_struct {
    pthread_t temp;
    size_t index_;
    int result;
} reducer_t;

/* You should create a start routine for your threads. */
void * routine(void * temp) {
    
   
    reducer_t * data = (reducer_t *)temp;
    size_t left_bound = (elemLen_ / _num_threads) * data->index_;
    size_t right_bound;
    
    right_bound = (data->index_ == _num_threads - 1) ? elemLen_ : (elemLen_ / _num_threads) * (data->index_ + 1);

    size_t tempsize = right_bound - left_bound;
    data->result = reduce(lis_ + left_bound, tempsize, function_, baseCase_);

    return temp;
}


int par_reduce(int *list, size_t list_len, reducer reduce_func, int base_case, size_t num_threads) {
    
    if (list_len <= num_threads || num_threads == 1)
        return reduce(list, list_len, reduce_func, base_case);
    
    int result = base_case;
    
    lis_ = list;
    _num_threads = num_threads;
    elemLen_ = list_len;
    function_ = reduce_func;
    baseCase_ = base_case;

    reducer_t * array = calloc(num_threads, sizeof(reducer_t));

    void * status;
    for (size_t i = 0; i < num_threads; i++) {
        array[i].index_ = i;
        pthread_create(&array[i].temp, NULL, routine, &array[i]);
    }

    for (size_t i = 0; i < num_threads; i++) {
        pthread_join(array[i].temp, &status);
        result = reduce_func(result, array[i].result);
    }

    free(array);
    return result;
}

