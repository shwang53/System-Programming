/**
 * Perilous Pointers Lab
 * CS 241 - Spring 2019
 */
 
#include "part2-functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * (Edit this function to print out the "Illinois" lines in
 * part2-functions.c in order.)
 */
int main() {
    // your code here
	first_step(81); // 1

	int target = 132;
	int *second = &target;
	second_step(second); // 2

	target = 8942;
	int **third = &second;
	double_step(third); // 3

	char *temp = malloc(5 + sizeof(int));
   	 * (int *)(temp + 5) = 15;
   	 strange_step(temp); // 4
   	 free(temp); temp = NULL;

	char *temp2 = malloc(4*sizeof(char));
   	 temp2[3] = 0;
   	 empty_step((void *)temp2); // 5
	 free(temp2); temp2 = NULL;  

	char *s = malloc(4*sizeof(char));
	char *s2 = s;
	s2[3] = 'u';
	two_step(s,s2); //6
	 free(s); s = NULL; 

	char first [15] = {1,1,9,17,1,1,1,1,1,1,1,1,1,1,1};
	char *second2 = first + 2;
	char *third2 = second2 + 2;
	three_step(first, second2, third2); // 7

	second2 = first;
	third2 = first;
	step_step_step(first, second2, third2); // 8

	int b = 65;
	char * a = "A" ;
//	printf("%c %d " ,*a, *a) ;
	it_may_be_odd(a,b); // 9
	
	 char ten[] = " ,CS241";
   	 tok_step(ten);	// 10
	
	char *orange =  malloc(4* sizeof(int));
	 char *blue = orange;  
	orange[0] = 1;
	orange[1] = 3;
	orange[2] = 3;
	orange[3] = 2;	
	the_end(orange,blue);
	free(orange); orange = NULL;
   
 return 0;
}
