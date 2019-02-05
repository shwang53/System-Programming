/**
 * Vector Lab
 * CS 241 - Spring 2019
 */
 
#include "vector.h"
#include <stdio.h>
#include "callbacks.h"
#include <assert.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {
    // Write your test cases here
	printf("HI just testing out \n");

//	vector* vec_ = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
	 
	//TEST
//	int a = 1;
//	int *b = &a;
//	vector_insert(test, 0, b);
//	printf("%d\n",*(int*)vector_get(test,0)); 
/*

	//SH
		// Write your test cases here

        vector * vec_  = vector_create(int_copy_constructor, int_destructor, int_default_constructor);

        size_t cap = vector_capacity(vec_);

        //vector_reserve() test case
        vector_reserve(vec_, 10);
        cap = vector_capacity(vec_);
        assert(cap >= 10);
        printf("Capacity: %zu\n", cap);

        //vector_size() test case
        printf("Size: %zu\n ", vector_size(vec_));

        //vector_push_back test case
        for(size_t i = 0; i < 6; i++) {
                int* x = malloc(sizeof(int));
                *x = i;
                vector_push_back(vec_,x);
        }
        for(size_t i = 0; i < 6; i++) {
                printf("%d\n", *(int *)vector_get(vec_, i));
        }
        printf("size: %zu\n",  vector_size(vec_));

//      vector_erase(vec_, 3);
        //vector_at test
        int** ab = (int**)vector_at(vec_, 1);
        printf("at 1: %d\n", **ab);

        //vector_set test
        int dd = 8;
        int *aaa = &dd;
        vector_set(vec_, 2, aaa);
	
	printf("DID I GOT THIS ? \n");

        //pop_back test
        vector_pop_back(vec_);
	 printf("DID I GOT THIS ? \n");
        printf("New Last element has to be 4: %d\n", **(int**)vector_back(vec_));
        //vector_front test
        int** b = (int**) vector_front(vec_);
        printf("front value: %d\n", **b);

        //vector_back test
        int** c = (int**) vector_back(vec_);
        printf("back value: %d\n", **c);

        //at test
        int** g = (int**) vector_at(vec_, 3);
        printf("at value: %d\n", **g);

			  //insert test
        for(size_t i =0; i< vector_size(vec_); i++) {
                printf("%d\n",*(int*) vector_get(vec_, i));
        }
        int d = 9;
        int* e = &d;
        vector_insert(vec_, 3, e);
        printf("size after insert %zu\n", vector_size(vec_));
        for(size_t i = 0; i < vector_size(vec_); i++) {
                printf("%d\n", *(int *)vector_get(vec_, i));
        }
        //resize test
        vector_resize(vec_, 10);
        printf("%zu\n", vector_size(vec_));

        vector_resize(vec_, 40);
        printf("Resized size: %zu, capacity: %zu\n", vector_size(vec_), vector_capacity(vec_));

        vector_resize(vec_, 3);
         printf("Resized size: %zu, capacity: %zu\n", vector_size(vec_), vector_capacity(vec_));
        for(size_t i = 0; i < vector_size(vec_); i++) {
                printf("%d\n", *(int*)vector_get(vec_,i));
        }
//      vector_erase(vec_,2);
        for(size_t i=0; i < vector_size(vec_); i++) {
                printf("%d\n", *(int*)vector_get(vec_,i));
        }
//      vector_clear(vec_);
        printf("size after clear: %zu\n", vector_size(vec_));
        vector_destroy(vec_);
*/
        return 0;
	 return 0;



}
