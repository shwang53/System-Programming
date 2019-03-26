/**
 * Parallel Make Lab
 * CS 241 - Spring 2019
 */
 
/*
#include "format.h"
#include "graph.h"
#include "parmake.h"
#include "parser.h"

int parmake(char *makefile, size_t num_threads, char **targets) {
    // good luck!
    return 0;
}
*/


#include "format.h"
#include "graph.h"
#include "parmake.h"
#include "parser.h"
#include "queue.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <fcntl.h>
#include "set.h"
#include "vector.h"
#include "dictionary.h"

int exec_file(graph* graph, char* it);
int exec_cmd(graph* graph, char* it);
void detect_cycles(graph *graph, char* it, dictionary* dict, vector* dup, vector* visited);
void trans_cycle(graph* graph, char* it, dictionary* dict, vector* cycle);
void put_in_graph(graph* graph, queue* myqueue, char* it, vector* old, vector* tasks, vector* in_q);
void* doem(void* param);
int is_file(char* fname);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
graph* mygraph;

typedef struct mystruc {
    queue* myqueue;
    vector* in_q;
    vector* tasks;
} mystruct;

void* doem(void* param) {
    mystruct* mys = (mystruct*)param;
    queue* myqueue = mys->myqueue;
    vector* in_q = mys->in_q;
    vector* tasks = mys->tasks;
    vector* pars;
 SP:
    while(true) {
	char* mynode = queue_pull(myqueue);
	if (!strcmp(mynode, "")) {
	    queue_push(myqueue, mynode);
	    queue_push(myqueue, mynode);
	    queue_push(myqueue, mynode);
	    return NULL;
	}

	bool flag = false;
	VECTOR_FOR_EACH(tasks, var, {
		int found = strcmp((char*)var, mynode);
		if (found == 0) {
		    flag = true;
		}
	    });
	if (!flag) {
	    continue;
	}

	rule_t* myrule = (rule_t*) graph_get_vertex_value(mygraph, mynode);
	vector* mynei = graph_neighbors(mygraph, mynode);

	if (myrule->state == 1 || myrule->state == -1) {
	    goto PG;
	}

	if (vector_size(mynei) != 0) {
	    bool node_flag = true;
	    FILE* myfile = fopen(mynode, "r");
	    if (myfile) {
		fclose(myfile);
		for (size_t i = 0; i < vector_size(mynei); i++) {
		    char* itvec = vector_get(mynei, i);
		    rule_t* itrul = (rule_t*)graph_get_vertex_value(mygraph, itvec);
		    pthread_mutex_lock(&mutex);
		    if (itrul->state == 0) {
			queue_push(myqueue, mynode);
			pthread_mutex_unlock(&mutex);
			goto SP;
		    }
		    while(itrul->state == 0) {
			pthread_cond_wait(&cond, &mutex);
		    }
		    pthread_mutex_unlock(&mutex);
		    FILE* myfile2 = fopen(itvec, "r");
		    if (myfile2) {
			fclose(myfile2);
			struct stat fsvi;
			struct stat fsi;
			stat(itvec, &fsvi);
			stat(mynode, &fsi);
			if (difftime(fsi.st_mtime, fsvi.st_mtime) > 0) {
			    node_flag = false;
			    pthread_mutex_lock(&mutex);
			    myrule->state = 1;
			    pthread_mutex_unlock(&mutex);
			    continue;
			}
		    }
		    else {
			if (itrul->state == -1) {
			    node_flag = false;
			    pthread_mutex_lock(&mutex);
			    myrule->state = -1;
			    pthread_mutex_unlock(&mutex);
			    break;
			}
			else if (itrul->state == 1) {
			    continue;
			}
		    }
		}
	    }
	    else {
		for (size_t i = 0; i < vector_size(mynei); i++) {
		    char* itvec = vector_get(mynei, i);
		    rule_t* itrul = (rule_t*)graph_get_vertex_value(mygraph, itvec);
		    pthread_mutex_lock(&mutex);
		    if (itrul->state == 0) {
			queue_push(myqueue, mynode);
			pthread_mutex_unlock(&mutex);
			goto SP;
		    }
		    while (itrul->state == 0) {
			pthread_cond_wait(&cond, &mutex);
		    }
		    pthread_mutex_unlock(&mutex);
		    if (itrul->state == 1) {
			continue;
		    }
		    else if (itrul->state == -1) {
			node_flag = false;
			myrule->state = -1;
			break;
		    }
		}
	    }
	    if (node_flag) {
		vector* command = myrule->commands;
		size_t c_size = vector_size(command);
		int res = 1;
		for (size_t i = 0; i < c_size; i++) {
		    char* ccmd = vector_get(command, i);
		    int ret = system(ccmd);
		    if (ret != 0) {
			res = -1;
		    }
		}
		if (res <= 0) {
		    myrule->state = -1;
		}
		else {
		    myrule->state = 1;
		}
	    }
	}
	else {
	    vector* command = myrule->commands;
	    size_t c_size = vector_size(command);
	    int sres = 1;
	    for (size_t i = 0; i < c_size; i++) {
		char* ccmd = vector_get(command, i);
		int ret = system(ccmd);
		if (ret != 0) {
		    sres = -1;
		}
	    }
	    pthread_mutex_lock(&mutex);
	    if (sres > 0) {
		myrule->state = 1;
	    }
	    else {
		myrule->state = -1;
	    }
	    pthread_mutex_unlock(&mutex);
	}
    PG:
	pars = graph_antineighbors(mygraph, mynode);
	for (size_t i = 0; i < vector_size(pars); i++) {
	    char* mytit = vector_get(pars, i);
	    bool flag = false;

	    VECTOR_FOR_EACH (in_q, var, {
		    int found = strcmp((char*)var, mytit);
		    if (found == 0) {
			flag = true;
		    }
		});
	    if (!flag) {
		vector_push_back(in_q, mytit);
		queue_push(myqueue, mytit);
	    }
	}
	pthread_cond_broadcast(&cond);
	vector_destroy(pars);
	vector_destroy(mynei);
    }
    return NULL;
}


void put_in_graph(graph* graph, queue* myqueue, char* it, vector* old, vector* tasks, vector* in_q) {
    if (!it) {
	return;
    }
    vector_push_back(tasks, it);
    vector* myadj = graph_neighbors(graph, it);
    bool flag = false;
    VECTOR_FOR_EACH(old, var, {
	int found = strcmp((char*)var, it);
	if (found == 0) {
	    flag = true;
	}
    });
    
    if (vector_size(myadj) == 0 && !flag) {
	vector_push_back(old, it);
	vector_push_back(in_q, it);
	queue_push(myqueue, it);
    }
    for (size_t i = 0; i < vector_size(myadj); i++) {
	put_in_graph(mygraph, myqueue, vector_get(myadj, i), old, tasks, in_q);
    }
    vector_destroy(myadj);
}

int exec_file(graph* graph, char* it) {
    rule_t* myrule = (rule_t*)graph_get_vertex_value(graph, it);
    int out = 1;

    bool flag = true;
    vector* adj_iter = graph_neighbors(graph, it);
    if (vector_size(adj_iter)) {
	for (size_t i = 0; i < vector_size(adj_iter); i++) {
	    char* myvirt_iter = vector_get(adj_iter, i);
	    FILE* myfile = fopen(myvirt_iter, "r");
	    if (myfile) {
		fclose(myfile);
		struct stat fsvi;
		struct stat fsi;
		stat(myvirt_iter, &fsvi);
		stat(it, &fsi);
		if (difftime(fsi.st_mtime, fsvi.st_mtime) <= 0) {
		    int newout = exec_file(graph, myvirt_iter);
		    if (newout < 0) {
			out = newout;
		    }
		} else {
		    flag = false;
		    myrule->state = 1;
		    continue;
		}
	    } else {
		rule_t* trule = (rule_t*)graph_get_vertex_value(graph, myvirt_iter);
		if (trule->state > 0) {
		    continue;
		}
		int newout = exec_cmd(graph, myvirt_iter);
		if (newout < 0) {
		    out = newout;
		}
	    }
	}
	if (out > 0 && flag) {
	    vector* cmds = myrule->commands;
	    for (size_t i = 0; i < vector_size(cmds); i++) {
		char* mycmd = vector_get(cmds, i);
		int sysout = system(mycmd);
		if (sysout) {
		    myrule->state = -1;
		    out = -1;
		}
	    }
	}
    }
    else {
	vector* cmds = myrule->commands;
	for (size_t i = 0; i < vector_size(cmds); i++) {
	    char* mycmd = vector_get(cmds, i);
	    int sysout = system(mycmd);
	    if (sysout) {
		myrule->state = -1;
		out = -1;
	    }
	}
	myrule->state = out;
    }
    vector_destroy(adj_iter);
    return out;
}

int exec_cmd(graph* graph, char* it) {
    rule_t* myrule = (rule_t*)graph_get_vertex_value(graph, it);
    int out = 1;

    bool flag = true;
    vector* adj_iter = graph_neighbors(graph, it);
    if (vector_size(adj_iter)) {	
	for (size_t i = 0; i < vector_size(adj_iter); i++) {
	    char* myvirt_iter = vector_get(adj_iter, i);
	    FILE* myfile = fopen(myvirt_iter, "r");
	    if (myfile) {
		fclose(myfile);
		int newout = exec_file(graph, myvirt_iter);
		if (newout < 0) {
		    flag = false;
		    myrule->state = -1;
		    break;
		}
	    }
	    else {
		rule_t* trule = (rule_t*)graph_get_vertex_value(graph, myvirt_iter);
		if (trule->state > 0) {
		    continue;
		}
		int newout = exec_cmd(graph, myvirt_iter);
		if (newout < 0) {
		    out = newout;
		}
	    }
	}
	if (out > 0 && flag) {
	    vector* cmds = myrule->commands;
	    for (size_t i = 0; i < vector_size(cmds); i++) {
		char* mycmd = vector_get(cmds, i);
		int sysout = system(mycmd);
		if (sysout) {
		    myrule->state = -1;
		    out = -1;
		}
	    }
	}
    }
    else {
	vector* cmds = myrule->commands;
	for (size_t i = 0; i < vector_size(cmds); i++) {
	    char* mycmd = vector_get(cmds, i);
	    int sysout = system(mycmd);
	    if (sysout) {
		myrule->state = -1;
		out = -1;
	    }
	}
	myrule->state = out;
    }
    vector_destroy(adj_iter);
    return out;
}

void detect_cycles(graph *graph, char* it, dictionary* dict, vector* dup, vector* visited) {
    vector_push_back(visited, it);
    vector* adj_iter = graph_neighbors(graph, it);
    for (size_t i = 0; i < vector_size(adj_iter); i++) {
	char* vert_iter = vector_get(adj_iter, i);
	bool flag = false;
	VECTOR_FOR_EACH(visited, var, {
		int found = strcmp((char*)var, vert_iter);
		if (found == 0) {
		    flag = true;
		}
	});

	if (flag) {
	    vector_push_back(dup, it);
	    continue;
	}
	
	dictionary_set(dict, vert_iter, it);
	detect_cycles(graph, vert_iter, dict, dup, visited);
	size_t idx = 0;
	VECTOR_FOR_EACH(visited, var, {
		int found = strcmp((char*)var, vert_iter);
		if (found == 0) {
		    break;
		}
		idx++;
	});
	vector_erase(visited, idx);
    }
    vector_destroy(adj_iter);
}

void trans_cycle(graph* graph, char* it, dictionary* dict, vector* cycle) {
    //base case
    if (!strcmp(it, "")) {
	return;
    }
    vector_push_back(cycle, it);
    if (graph_contains_vertex(graph, it)) {
	graph_remove_vertex(graph, it);
    }
    char* temp = dictionary_get(dict, it);
    trans_cycle(graph, temp, dict, cycle);
}

int parmake(char *makefile, size_t num_threads, char **targets) {
    // good luck!
    //parse the makefile into dependency graph
    mygraph = parser_parse_makefile(makefile, targets);
    vector* myvert = graph_vertices(mygraph);
    char* fn = vector_get(myvert, 0);
    char* fn_temp = calloc(1024, sizeof(char));
    strcpy(fn_temp, vector_get(myvert, 1));
    
    //to detect cycles
    dictionary* mydict = string_to_string_dictionary_create();
    vector* visited = vector_create(NULL, NULL, NULL);
    vector* dup = vector_create(NULL, NULL, NULL);
    detect_cycles(mygraph, fn, mydict, dup, visited);

    vector* cycles = vector_create(NULL, NULL, NULL);
    VECTOR_FOR_EACH(dup, it, {
	trans_cycle(mygraph, it, mydict, cycles);
    });

    dictionary_destroy(mydict);
    vector_destroy(visited);
    vector_destroy(dup);

    if (!targets[0]) {
	bool flag = false;
	VECTOR_FOR_EACH(cycles, var, {
		int found = strcmp((char*)var, fn_temp);
		if (found == 0) {
		    flag = true;
		}
	});
	if (flag) {
	    print_cycle_failure(fn_temp);
	}	
    }
    else {
	for (char** iter = targets; *iter != NULL; iter++) {
	    bool flag = false;
	    VECTOR_FOR_EACH(cycles, var, {
		int found = strcmp((char*)var, *iter);
		if (found == 0) {
		    flag = true;
		}
	    });
	    if (flag) {
		print_cycle_failure(*iter);
	    }
	}
    }

    vector_destroy(myvert);

    vector* mynodes = graph_vertices(mygraph);
    
    if (num_threads == 1) {
	if (targets[0]) {
	    for (char** tit = targets; *tit != NULL; tit++) {
		bool flag = false;
		VECTOR_FOR_EACH(cycles, var, {
		    int found = strcmp((char*)var, *tit);
		    if (found == 0) {
			flag = true;
		    }
		});
		if (flag) {
		    continue;
		}
		FILE* myfile = fopen(*tit, "r");
		if (myfile) {
		    fclose(myfile);
		    exec_file(mygraph, *tit);
		} else {
		    exec_cmd(mygraph, *tit);
		}
	    }
	}
	else {
	    bool flag = false;
	    VECTOR_FOR_EACH(cycles, var, {
		int found = strcmp((char*)var, fn_temp);
		if (found == 0) {
		    flag = true;
		}
	    });
	    if (!flag) {
		FILE* myfile = fopen(fn_temp, "r");
		if (myfile) {
		    fclose(myfile);
		    exec_file(mygraph, fn_temp);
		}
		else {
		    exec_cmd(mygraph, fn_temp);
		}
	    }
	}
        
	vector_destroy(cycles);
	vector_destroy(mynodes);
	graph_destroy(mygraph);
	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mutex);
	return 0;
    }

    if (!targets[0]) {
	vector* old = vector_create(NULL, NULL, NULL);
	vector* in_q = vector_create(NULL, NULL, NULL);
	vector* tasks = vector_create(NULL, NULL, NULL);
	queue* myqueue = queue_create(-1);
	put_in_graph(mygraph, myqueue, vector_get(mynodes, 0), old, tasks, in_q);
	mystruct mys;
	mys.myqueue = myqueue;
	mys.in_q = in_q;
	mys.tasks = tasks;

	pthread_t mythreads[1024];
	for (size_t i = 0; i < num_threads; i++) {
	    pthread_create(&mythreads[i], NULL, &doem, (void*)&mys);
	}

	for (size_t i = 0; i < num_threads; i++) {
	    pthread_join(mythreads[i], NULL);
	}

	queue_destroy(myqueue);
	vector_destroy(old);
	vector_destroy(tasks);
	vector_destroy(in_q);
    }
    else {
	vector* old = vector_create(NULL, NULL, NULL);
	vector* in_q = vector_create(NULL, NULL, NULL);
	vector* tasks = vector_create(NULL, NULL, NULL);
	queue* myqueue = queue_create(-1);
	mystruct mys;
	mys.myqueue = myqueue;
	mys.in_q = in_q;
	mys.tasks = tasks;
	
	pthread_t mythreads[1024];
	for (char** it = targets; *it != NULL; it++) {
    	    bool flag = false;
	    VECTOR_FOR_EACH(cycles, var, {
		int found = strcmp((char*)var, *it);
		if (found == 0) {
		    flag = true;
		}
	    });
	    if (flag) {
		continue;
	    }
	    put_in_graph(mygraph, myqueue, *it, old, tasks, in_q);
	}
	
	for (size_t i = 0; i < num_threads; i++) {
	    pthread_create(&mythreads[i], NULL, &doem, (void*)&mys);
	}

	for (size_t i = 0; i < num_threads; i++) {
	    pthread_join(mythreads[i], NULL);
	}

	queue_destroy(myqueue);
	vector_destroy(old);
	vector_destroy(tasks);
	vector_destroy(in_q);
    }

    vector_destroy(cycles);
    vector_destroy(mynodes);
    graph_destroy(mygraph);
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
    free(fn_temp);
    fn_temp = NULL;

    return 0;
}

