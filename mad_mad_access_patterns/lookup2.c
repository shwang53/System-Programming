/**
 * Mad Mad Access Patterns
 * CS 241 - Spring 2019
 */
#include "tree.h"
#include "utils.h"

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses mmap to access the data.

  ./lookup2 <data_file> <word> [<word> ...]
*/

static void * addr;
static uint32_t node_offset;
static int is_found;

static void search(uint32_t offset, char * word);
int main(int argc, char **argv) {
    
    if (argc < 3) {
        printArgumentUsage();
        exit(1);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        openFail(argv[1]);
        exit(2);
    }

    int file_size = lseek(fd, 0, SEEK_END);
    addr = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == (void *)-1) {
        mmapFail(argv[1]);
        exit(2);
    }

    char * temp = addr;
    if (strncmp(temp, BINTREE_HEADER_STRING, 4)) {
        formatFail(argv[1]);
        exit(2);
    }

    for (int i = 2; i < argc; i++) {
        char * word = argv[i];
        node_offset = 0;
        is_found = 0;
        search(BINTREE_ROOT_NODE_OFFSET, word);
        if (is_found) {
            BinaryTreeNode * found = addr + node_offset;
            printFound(word, found->count, found->price);
        } else {
            printNotFound(word);
        }
    }

    munmap(addr, file_size);
    close(fd);
     
   return 0;
}

static void search(uint32_t offset, char * word) {
    char * temp = addr + offset + 16;
    char * buf = malloc(256);
    size_t n = 0;
    while (temp[n] != '\0') {
        buf[n] = temp[n];
        n++;
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

    uint32_t * bt_array = addr + offset;
    uint32_t left = bt_array[0], right = bt_array[1];
    if (left && result < 0 && !is_found) {
        search(left, word);
    }
    if (right && result > 0 && !is_found) {
        search(right, word);
    }
}
