/**
 * Vector Lab
 * CS 241 - Spring 2019
 */
 
#include "sstring.h"
#include "vector.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <string.h>

struct sstring {
    // Anything you want
	size_t leng;
	char* words;
	size_t pos;
};

sstring *cstr_to_sstring(const char *input) {
    // your code goes her
	  sstring* test = malloc(sizeof(sstring));
  	 test->pos = 0;
 	
  	if ( input != NULL) {
   		 test->words = malloc(strlen(input) + 1);
   		 test->leng = strlen(input);
		// test->words = input; //fixed
 	 }

	 else if ( input == NULL) {
                 test->words = NULL;
                 test->leng = 0;
         }
    
   	 return test;
}

char *sstring_to_cstr(sstring *input) {
    // your code goes here
   	 char* test = malloc(input->leng + 1);
	size_t i;
 	 for (i = 0; i < input->leng; i++) {
   		 test[i] = input->words[i];
  	}
 	 test[i] = '\0';
   	 return test;
}

int sstring_append(sstring *this, sstring *addition) {
    // your code goes here
      size_t n;
 //	 if (addition->length == 0) {
 //	  	 n = strlen(addition->words);
 //	 }
 //	 else {
  // 		 n = addition->leng;
// 	 }

	n = (addition->leng == 0) ? strlen(addition->words) : addition->leng;
 	 this->words = realloc(this->words, (this->leng + n));
	 memcpy(this->words + this->leng, addition->words, n);
	 
	 this->leng += n;
    
   	 return this->leng;
}

vector *sstring_split(sstring *this, char delimiter) {
    // your code goes here
    return NULL;
}

int sstring_substitute(sstring *this, size_t offset, char *target,
                       char *substitution) {
    // your code goes here
    return -1;
}

char *sstring_slice(sstring *this, int start, int end) {
    // your code goes here
	 char* temp = malloc(sizeof(end - start + 1));
 	 
	size_t i;
	size_t min = (int)this->leng <(int) end+1 ? this->leng : end+1;
 	 for ( i = 0; i < min; i++) {
   		 temp[i] = this->words[i + start];
 	 }
 	 temp[i] = '\0';
   	 return temp;
}

void sstring_destroy(sstring *this) {
    // your code goes here
	 if (this != NULL) {
   		 if (this->words) {
     			 free(this->words);
   		 }
 	 	  free(this);
 	 }
	  this = NULL;

}
