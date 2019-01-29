/**
 * Extreme Edge Cases Lab
 * CS 241 - Spring 2019
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "camelCaser.h"
#include "camelCaser_tests.h"


/*
 * Testing function for various implementations of camelCaser.
 *
 * @param  camelCaser   A pointer to the target camelCaser function.
 * @param  destroy      A pointer to the function that destroys camelCaser
 * output.
 * @return              Correctness of the program (0 for wrong, 1 for correct).
 */
int test_camelCaser(char **(*camelCaser)(const char *),
                    void (*destroy)(char **)) {
    // TODO: Return 1 if the passed in function works properly; 0 if it doesn't.
	char* temp;
	int i;

	// // Testcase 1, normal sentances.
	char *expected_1[5] = { "thisMachineProblem", "isThis","gettingHarder","iDoNotKnow",NULL};
   	 char **cameledInput_1 = (*camelCaser)("THis MachINE PRoblem. Is THIS, getting harder? I do not know.");

	// string compare, if not equal return 0 and free. 
	i = 0;
	temp = cameledInput_1[0];
	while (temp) {
		if (strcmp(temp, expected_1[i])) {
			 (*destroy)(cameledInput_1);              		 
			return 0;
		}
		i++;
		temp = cameledInput_1[i];
	}

	(*destroy)(cameledInput_1);
	
	// Testcase 2, number combination.
	 char *expected_2[5] = { "myphoneNumberIs773", "729", "xxxx", "isForRe7al", NULL};
         char **cameledInput_2 = (*camelCaser)("MyPHONE NUmber is 773-729-xxxx. IS for Re7al?");

        // string compare, if not equal return 0 and free. 
        i = 0;
        temp = cameledInput_2[0];
        while ((temp)) {
                if (strcmp(temp, expected_2[i])) {
                         (*destroy)(cameledInput_2);     
                        return 0;
                }
                i++;
                temp = cameledInput_2[i];
        }

        (*destroy)(cameledInput_2);

   	// Testcase 3, lots of punctuation.
         char *expected_3[7] = { "", "", "lotsOfPunc", "oh", "yesHah", "a77", NULL};
         char **cameledInput_3 = (*camelCaser)(". ! lots of Punc. OH-YES hah/a77!");

        // string compare, if not equal return 0 and free. 
        i = 0;
        temp = cameledInput_3[0];
        while ((temp)) {
                if (strcmp(temp, expected_3[i])) {
                         (*destroy)(cameledInput_3);     
                        return 0;
                }
                i++;
                temp = cameledInput_3[i];
        }

        (*destroy)(cameledInput_3);

 	//Testcase 4, null
	char **cameledInput_4 = (*camelCaser)(NULL);
        if (cameledInput_4 != NULL){
		return 0;
	}
        
	//Testcase 5, wierd 
	char **cameledInput_5 = (*camelCaser)("STRANgeThing\x1f give it a Try.");
	char *expected_5[2] = {"strangething\x1FGiveItATry", NULL };
	// string compare, if not equal return 0 and free. 
        i = 0;
        temp = cameledInput_5[0];
        while ((temp)) {
                if (strcmp(temp, expected_5[i])) {
                         (*destroy)(cameledInput_5);
                        return 0;
                }
                i++;
                temp = cameledInput_5[i];
        }

        (*destroy)(cameledInput_5);
	
	//Testcase 6, non alpha
	   char **cameledInput_6 = (*camelCaser)("66HELLO, LOOOKKATTHAT BBOY!");
        char *expected_6[3] = {"66hello","loookkatthatBboy", NULL };
        // string compare, if not equal return 0 and free. 
        i = 0;
        temp = cameledInput_6[0];
        while ((temp)) {
                if (strcmp(temp, expected_6[i])) {
                         (*destroy)(cameledInput_6);
                        return 0;
                }
                i++;
                temp = cameledInput_6[i];
        }

        (*destroy)(cameledInput_6);

	//Testcase 7, no include string without the punc mark at the end.
	char **cameledInput_7 = (*camelCaser)("This has punc mark. Nice. But this is not");
        char *expected_7[3] = {"thisHasPuncMark","nice", NULL };
        // string compare, if not equal return 0 and free. 
        i = 0;
        temp = cameledInput_7[0];
        while ((temp)) {
                if (strcmp(temp, expected_7[i])) {
                         (*destroy)(cameledInput_7);
                        return 0;
                }
                i++;
                temp = cameledInput_7[i];
        }

        (*destroy)(cameledInput_7);
	return 1;
}
