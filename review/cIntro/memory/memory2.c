#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main() {

  char* string1 = malloc(10);
  printf("%p\n", &string1);
  free(string1);

  char* string2 = realloc(string1, 5);
  printf("%p\n", &string1);
  free(string1);

  return 0;  
}
