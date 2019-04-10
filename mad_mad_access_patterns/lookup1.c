/**
 * Mad Mad Access Patterns
 * CS 241 - Spring 2019
 */
#include "tree.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>

#define BTNSIZE (sizeof(BinaryTreeNode))

/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses fseek() and fread() to access the data.

  ./lookup1 <data_file> <word> [<word> ...]
*/

static uint32_t node_offset;
static int is_found;

static void search(FILE * file, uint32_t offset, char * word);

int main(int argc, char **argv) {
    if (argc < 3) {
        printArgumentUsage();
        exit(1);
    }

    FILE * data_file = fopen(argv[1], "r");
    if (!data_file) {
        openFail(argv[1]);
        exit(2);
    }
    fseek(data_file, 0, SEEK_SET);
    char * buf = malloc(4 * sizeof(char) + 1);
    fread(buf, sizeof(char), 4, data_file);
    buf[4] = '\0';
    if (strcmp(buf, BINTREE_HEADER_STRING)) {
        formatFail(argv[1]);
        exit(2);
    }
    free(buf);

    for (int i = 2; i < argc; i++) {
        char * word = argv[i];
        node_offset = 0;
        is_found = 0;
        search(data_file, BINTREE_ROOT_NODE_OFFSET, word);
        if (is_found) {
            fseek(data_file, node_offset, SEEK_SET);
            BinaryTreeNode * found = malloc(BTNSIZE);
            fread(found, BTNSIZE, 1, data_file);
            printFound(word, found->count, found->price);
            free(found);
        } else {
            printNotFound(word);
        }
    }

    fclose(data_file);
    return 0;
}

static void search(FILE * file, uint32_t offset, char * word) {
    fseek(file, offset + 16, SEEK_SET);
    char * buf = malloc(256);
    size_t n = 0;
    int c;
    c = fgetc(file);
    while (c != EOF && c != '\0') {
        buf[n++] = (char)c;
        c = fgetc(file);
    }
    buf[n] = '\0';
    int result = strcmp(word, buf);
    if (!result) {
        is_found = 1;
        node_offset = offset;
        free(buf);
        return;
    }
    free(buf);

    fseek(file, offset, SEEK_SET);
    BinaryTreeNode * node = malloc(BTNSIZE);
    fread(node, BTNSIZE, 1, file);
    if (node->left_child && result < 0 && !is_found) {
        search(file, node->left_child, word);
    }
    if (node->right_child && result > 0 && !is_found) {
        search(file, node->right_child, word);
    }
    free(node);
}


