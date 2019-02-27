/**
 * Critical Concurrency Lab
 * CS 241 - Spring 2019
 */
 
#include "queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * This queue is implemented with a linked list of queue_nodes.
 */
typedef struct queue_node {
    void *data;
    struct queue_node *next;
} queue_node;

struct queue {
    /* queue_node pointers to the head and tail of the queue */
    queue_node *head, *tail;

    /* The number of elements in the queue */
    ssize_t size;

    /**
     * The maximum number of elements the queue can hold.
     * max_size is non-positive if the queue does not have a max size.
     */
    ssize_t max_size;

    /* Mutex and Condition Variable for thread-safety */
    pthread_cond_t cv;
    pthread_mutex_t m;
};

queue *queue_create(ssize_t max_size) {
    /* Your code here */
//    return NULL;
	    queue * q = malloc(sizeof(queue));
    if (!q) return NULL;
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    q->max_size = max_size;
    pthread_mutex_init(&q->m, NULL);
    pthread_cond_init(&q->cv, NULL);
    return q;

}

void queue_destroy(queue *this) {
    /* Your code here */
	  queue_node * tmp;
    while (this->head) {
        tmp = this->head;
        if (this->size == 1) {
            this->head = NULL;
        } else {
            this->head = tmp->next;
        }
        free(tmp);
    }
    this->head = NULL;
    this->tail = NULL;
    pthread_mutex_destroy(&this->m);
    pthread_cond_destroy(&this->cv);
    free(this);
}

void queue_push(queue *this, void *data) {
    /* Your code here */
	queue_node * entry = malloc(sizeof(queue_node));
    entry->data = data;
    entry->next = NULL;
    pthread_mutex_lock(&this->m);
    if (this->max_size > 0) {
        while (this->size == this->max_size) {
            pthread_cond_wait(&this->cv, &this->m);
        }
    }
    if (!this->head) {
        this->head = entry;
        this->tail = entry;
    } else {
        this->tail->next = entry;
        this->tail = entry;
    }
    this->size++;
    pthread_cond_signal(&this->cv);
    pthread_mutex_unlock(&this->m);
}

void *queue_pull(queue *this) {
    /* Your code here */
 //   return NULL;

   queue_node * entry;
    void * data;
    pthread_mutex_lock(&this->m);
    while (!this->size) {
        pthread_cond_wait(&this->cv, &this->m);
    }
    entry = this->head;
    if (this->size == 1) {
        this->head = NULL;
        this->tail = NULL;
    } else {
        this->head = entry->next;
    }
    this->size--;
    pthread_cond_signal(&this->cv);
    pthread_mutex_unlock(&this->m);
    data = entry->data;
    free(entry);
    return data;
}
