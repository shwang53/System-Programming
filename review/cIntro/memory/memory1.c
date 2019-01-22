#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main() {

  char* string1 = malloc(10);
  memset(string1, 0, 10);
  strncpy(string1, "test", 4);
  printf("%s\n", string1);
  free(string1);

  char* string2 = calloc(6, 1);
  strncpy(string2, "test", 4);
  printf("%s\n", string2);
  free(string2);

  return 0;  
}
