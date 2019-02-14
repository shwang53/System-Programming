/**
 * Mini Valgrind Lab
 * CS 241 - Spring 2019
 */
 
#include "mini_valgrind.h"
#include <stdio.h>
#include <string.h>

meta_data * head;
size_t total_memory_requested;
size_t total_memory_freed;
size_t invalid_addresses;

void *mini_malloc(size_t request_size, const char *filename,
                  void *instruction) {
    // your code here
    if (!request_size) return NULL;
   
	 meta_data * temp = malloc(sizeof(meta_data) + request_size);
   
	 if (!temp) return NULL;
		        	
		total_memory_requested += request_size;
	    	temp->request_size = request_size;
      	    	temp->filename = filename;
 	 	temp->instruction = instruction;

 		temp->next = head;
                head = temp;
    return (void *)(temp + 1);

}

void *mini_calloc(size_t num_elements, size_t element_size,
                  const char *filename, void *instruction) {
    // your code here
        size_t bytesNum = num_elements * element_size;

	    if (bytesNum / num_elements != element_size) { return NULL; }
   
		 meta_data * temp  = calloc(sizeof(meta_data) + bytesNum, 1);
    
		if (!temp) return NULL;
  	


    total_memory_requested += bytesNum;
    temp->request_size = bytesNum;
    temp->filename = filename;
    temp->instruction = instruction;
	
	temp->next = head;
        head = temp;
    
    return (void *)(temp + 1);
}

void *mini_realloc(void *payload, size_t request_size, const char *filename,
                   void *instruction) {
    // your code here
        if (!payload) { 
		return mini_malloc(request_size, filename, instruction); 
	}
	
    	if (!request_size) {
        	mini_free(payload);
       		 return NULL;
	}
	   int is_valid = 0;
        size_t oldsize = 0;


   	 meta_data * tmp = head;
   	 meta_data * prev = NULL;
   

    while (tmp) {

        if (tmp + 1 == payload) {
            oldsize = tmp->request_size;
            is_valid  = 1;
            break;
        }

        prev = tmp;
        tmp = tmp->next;
    }

    if (!is_valid) {
        invalid_addresses++;
        return NULL;
    }

    if (prev) prev->next = tmp->next;
    else head = tmp->next;


    meta_data * temp = (meta_data *)realloc(tmp, sizeof(meta_data) + request_size);
    if (!temp) return NULL;

    if (request_size <= temp->request_size) total_memory_freed += (oldsize - request_size);
    else total_memory_requested += (request_size - oldsize);


   

    temp->request_size = request_size;
    temp->filename = filename;
    temp->instruction = instruction;

	 temp->next = head;
         head = temp;
   
 return (void *)(temp + 1);
}

void mini_free(void *payload) {
    // your code her
	    if (!payload) return;
    meta_data * tmp = head;
    meta_data * prev = NULL;

    while (tmp) {
        if (tmp + 1 == payload) {
         
	   total_memory_freed += tmp->request_size;
            if (prev) {
                prev->next = tmp->next;
                free(tmp);
                return;
            } else {
                head = tmp->next;
                free(tmp);
                return;
            }
        }

        prev = tmp;
        tmp = tmp->next;
    }
    invalid_addresses++;
    return;

}
