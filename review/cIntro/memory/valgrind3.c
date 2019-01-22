#include <stdlib.h>

int main() {
  int* examplePointer = malloc(3 * sizeof(int));

  examplePointer++;

  int* segFaultPointer = NULL;
  *segFaultPointer = 5;

  examplePointer--;
  free(examplePointer);
  return 0;
}
