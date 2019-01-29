/**
 * Extreme Edge Cases Lab
 * CS 241 - Spring 2019
 */
 
#include "camelCaser.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

char **camel_caser(const char *input_str) {
	if( !input_str){
		return NULL;
	}	
    
	char temp;
        int iter_i  = 0;
	int iter_s  = 0;   
	int count_s = 0;
	int count_c = 0;
        char input_char;
        bool shouldCapital  = false, isFirstChar  = true;



	// char temp = input str (from beginning to the end) and check if theres a punc symbol
	// count the sentences needed

	temp = input_str[iter_i];
  	while ( temp != 0 ) {
  	      if (ispunct(temp)) {
        	    count_s++;
              }    
	iter_i++;
	temp = input_str[iter_i];
    }


	// allocate output memoy on heap ( # of sentences + null terminated ) * size of char pointer
    char **output_s = malloc((count_s + 1) * sizeof(char*)); //when to free ?
	// set each pointer as NULL
    output_s[count_s] = NULL;

   
 
    	 iter_i = 0;
     	 temp = input_str[iter_i]; 
   	
	 while (temp != 0 ) {
		if (isspace(temp)) {
		// no change
        	}
		else if (ispunct(temp)) {
                // allocate each sentence on heap memory : # of char + 1 * size of char
                    output_s[iter_s] = malloc((count_c + 1));
                // set last char of the sentence as NULL
                    output_s[iter_s][count_c] = '\0';
                 
                 iter_s++; 
                 count_c = 0;
                }  
		else {
        	   	 count_c++;
       		 }
	
		iter_i ++;
		temp = input_str[iter_i];
    	} // now, done with memory allocating.

    
    iter_i = 0, iter_s = 0; count_c = 0;
    temp = input_str[0];

    while (temp != 0) {
        if (output_s[iter_s] == NULL) break;
	
	if (isspace(temp)) {
            shouldCapital = true;
        } 

	else if (ispunct(temp)) {
            isFirstChar = true;
            shouldCapital = false;

            iter_s++; count_c = 0;
        } 

	else {
            if (isalpha(temp)) {
		//만약 대문자이고&& 첫번째 문자도 아니면, 대문자로 변환
		//만약 소문자이거나 첫번째 문자이면, 소문자로 변
           	if( shouldCapital == true && isFirstChar == false ){
			input_char = toupper(temp);
		}else{
			input_char = tolower(temp);
		} 
	   } 

	   else {
		// 아무런 케이스도 아니면 걍 옮긴다.
                input_char = temp;
            }
    
	    shouldCapital = false; isFirstChar = false;
            output_s[iter_s][count_c] = input_char;
            count_c++;
        }
	iter_i ++;
	temp = input_str[iter_i];
    }

    return output_s;
}

void destroy(char **result) {
	int i = 0;
	char *temp = result[i];
	while(temp){
		free(temp);
		temp = NULL;
		i++;
		temp = result[i];
	}
	free(result);
	result = NULL;

    return;
}
