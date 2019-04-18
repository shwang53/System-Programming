/**
 * Savvy Scheduler
 * CS 241 - Spring 2019
 */
#include "libpriqueue/libpriqueue.h"
#include "libscheduler.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "print_functions.h"
typedef struct _job_info {
    int id;
    int priority;

    double arrival_time;
    double start_time;
    double end_time;
    double remaining_time;
    double running_time;
    /* Add whatever other bookkeeping you need into this struct. */
} job_info;

double total_turnaround_time;
double total_response_time;
int total_job_num;
double total_waiting_time;

priqueue_t pqueue;
scheme_t pqueue_scheme;
comparer_t comparision_func;

void scheduler_start_up(scheme_t s) {
    switch (s) {
    case FCFS:
        comparision_func = comparer_fcfs;
        break;
    case PRI:
        comparision_func = comparer_pri;
        break;
    case PPRI:
        comparision_func = comparer_ppri;
        break;
    case PSRTF:
        comparision_func = comparer_psrtf;
        break;
    case RR:
        comparision_func = comparer_rr;
        break;
    case SJF:
        comparision_func = comparer_sjf;
        break;
    default:
        printf("Did not recognize scheme\n");
        exit(1);
    }
    priqueue_init(&pqueue, comparision_func);
    pqueue_scheme = s;
    // Put any set up code you may need here
}

static int break_tie(const void *a, const void *b) {
    return comparer_fcfs(a, b);
}

int comparer_fcfs(const void *a, const void *b) {
    job_info * a_job = ((job *)a)->metadata;
    job_info * b_job = ((job *)b)->metadata;
    if (a_job->arrival_time < b_job->arrival_time) return -1;
    if (a_job->arrival_time > b_job->arrival_time) return 1;
    return 0;
}

int comparer_ppri(const void *a, const void *b) {
    // Complete as is
    return comparer_pri(a, b);
}

int comparer_pri(const void *a, const void *b) {
    job_info * a_job = ((job *)a)->metadata;
    job_info * b_job = ((job *)b)->metadata;
    if (a_job->priority < b_job->priority) return -1;
    if (a_job->priority > b_job->priority) return 1;
    return break_tie(a, b);    
}

int comparer_psrtf(const void *a, const void *b) {
     job_info * a_job = ((job *)a)->metadata;
    job_info * b_job = ((job *)b)->metadata;
    if (a_job->remaining_time < b_job->remaining_time) return -1;
    if (a_job->remaining_time > b_job->remaining_time) return 1;
    return break_tie(a, b);
}

int comparer_rr(const void *a, const void *b) {
    job_info * a_job = ((job *)a)->metadata;
    job_info * b_job = ((job *)b)->metadata;
    if (a_job->end_time == b_job->end_time) return break_tie(a, b);
    if (a_job->end_time == -1 || a_job->end_time < b_job->end_time) return -1;
    else return 1;
}

int comparer_sjf(const void *a, const void *b) {
    job_info * a_job = ((job *)a)->metadata;
    job_info * b_job = ((job *)b)->metadata;
    if (a_job->running_time < b_job->running_time) return -1;
    if (a_job->running_time > b_job->running_time) return 1;
    return break_tie(a, b);
}

// Do not allocate stack space or initialize ctx. These will be overwritten by
// gtgo
void scheduler_new_job(job *newjob, int job_number, double time,
                       scheduler_info *sched_data) {
    // TODO complete me!
    newjob->metadata = malloc(sizeof(job_info));
    job_info * info = newjob->metadata;
    info->id = job_number;
    info->priority = sched_data->priority;
    info->arrival_time = time;
    info->remaining_time = sched_data->running_time;
    info->running_time = sched_data->running_time;
    info->start_time = -1;
    info->end_time = -1;
    priqueue_offer(&pqueue, newjob);
}

job *scheduler_quantum_expired(job *job_evicted, double time) {
    // TODO complete me!
        if (!job_evicted) {
        job * j = priqueue_poll(&pqueue);
        if (!j) return NULL;
        job_info * i = j->metadata;
        i->start_time = time;
        i->end_time = time;
        return j;
    }
    if (comparision_func == comparer_fcfs || comparision_func == comparer_pri || comparision_func == comparer_sjf) {
        return job_evicted;
    }
    job_info * evict_info = job_evicted->metadata;
    evict_info->remaining_time -= time - evict_info->end_time;
    priqueue_offer(&pqueue, job_evicted);
    job * j = priqueue_poll(&pqueue);
    job_info * i = j->metadata;
    if (i->start_time == -1) i->start_time = time;
    i->end_time = time;
    return j;
    
}

void scheduler_job_finished(job *job_done, double time) {
    // TODO complete me!
    job_info * i = job_done->metadata;
    total_waiting_time += time - i->arrival_time - i->running_time;
    total_turnaround_time += time - i->arrival_time;
    total_response_time += i->start_time - i->arrival_time;
    free(i);
    total_job_num++;
}

static void print_stats() {
    fprintf(stderr, "turnaround     %f\n", scheduler_average_turnaround_time());
    fprintf(stderr, "total_waiting  %f\n", scheduler_average_waiting_time());
    fprintf(stderr, "total_response %f\n", scheduler_average_response_time());
}

double scheduler_average_waiting_time() {
    // TODO complete me!
    return total_waiting_time / total_job_num;
}

double scheduler_average_turnaround_time() {
    // TODO complete me!
    return total_turnaround_time / total_job_num;
}

double scheduler_average_response_time() {
    // TODO complete me!
    return total_response_time / total_job_num;
}

void scheduler_show_queue() {
    // Implement this if you need it!
}

void scheduler_clean_up() {
    priqueue_destroy(&pqueue);
    print_stats();
}
