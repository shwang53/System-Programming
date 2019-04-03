/**
 * Charming Chatroom
 * CS 241 - Spring 2019
 */
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "utils.h"
static const size_t MESSAGE_SIZE_DIGITS = 4;

char *create_message(char *name, char *message) {
    int name_len = strlen(name);
    int msg_len = strlen(message);
    char *msg = calloc(1, msg_len + name_len + 4);
    sprintf(msg, "%s: %s", name, message);

    return msg;
}

ssize_t get_message_size(int socket) {
    int32_t size;
    ssize_t read_bytes =
        read_all_from_socket(socket, (char *)&size, MESSAGE_SIZE_DIGITS);
    if (read_bytes == 0 || read_bytes == -1)
        return read_bytes;

    return (ssize_t)ntohl(size);
}

// You may assume size won't be larger than a 4 byte integer
ssize_t write_message_size(size_t size, int socket) {
    // Your code here
	    ssize_t mes_size = htonl(size);
    ssize_t wats_ret = write_all_to_socket(socket, (char *)&mes_size, MESSAGE_SIZE_DIGITS);
    if (wats_ret == 0 || wats_ret == -1) {
        return wats_ret;
    }
    return mes_size;

    return 9001;
}

ssize_t read_all_from_socket(int socket, char *buffer, size_t count) {
    // Your Code Here
	ssize_t read_bytes = count;
    while (read_bytes) {
        ssize_t r = read(socket, buffer + count - read_bytes, read_bytes);
        if (r == -1 && errno == EINTR) { continue; }
        read_bytes -= r;
    }
    return count;
    return 9001;
}

ssize_t write_all_to_socket(int socket, const char *buffer, size_t count) {
    // Your Code Here
   	 ssize_t write_bytes = count;
    while (write_bytes) {
        ssize_t w = write(socket, buffer + count - write_bytes, write_bytes);
        if (w <= 0) {
            if (w == -1 && errno == EINTR) { continue; }
            perror(strerror(w));
            return w;
        }
        write_bytes -= w;
    }
    return count;
	 return 9001;
}
