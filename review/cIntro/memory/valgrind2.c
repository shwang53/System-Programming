#include <stdlib.h>

struct node {
  struct node* child;
};

int main() {
  struct node* root = (struct node*) malloc(sizeof(struct node));
  
  struct node* temporary = root;
  for(int i = 0; i < 2; i++) {
    temporary->child = (struct node*) malloc(sizeof(struct node));

    temporary = temporary->child;
  }

  free(root);
  return 0;
}
