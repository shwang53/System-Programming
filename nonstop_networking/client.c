/**
 * Nonstop Networking
 * CS 241 - Spring 2019
 */

/*
#include "common.h"
#include "format.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
char **parse_args(int argc, char **argv);
verb check_args(char **args);
int main(int argc, char **argv) {
    // Good luck!
}
*/

/**
 * Given commandline argc and argv, parses argv.
 *
 * argc argc from main()
 * argv argv from main()
 *
 * Returns char* array in form of {host, port, method, remote, local, NULL}
 * where `method` is ALL CAPS
 */

/*
char **parse_args(int argc, char **argv) {
    if (argc < 3) {
        return NULL;
    }

    char *host = strtok(argv[1], ":");
    char *port = strtok(NULL, ":");
    if (port == NULL) {
        return NULL;
    }

    char **args = calloc(1, 6 * sizeof(char *));
    args[0] = host;
    args[1] = port;
    args[2] = argv[2];
    char *temp = args[2];
    while (*temp) {
        *temp = toupper((unsigned char)*temp);
        temp++;
    }
    if (argc > 3) {
        args[3] = argv[3];
    }
    if (argc > 4) {
        args[4] = argv[4];
    }

    return args;
}
*/
/**
 * Validates args to program.  If `args` are not valid, help information for the
 * program is printed.
 *
 * args     arguments to parse
 *
 * Returns a verb which corresponds to the request method
 */

/*
verb check_args(char **args) {
    if (args == NULL) {
        print_client_usage();
        exit(1);
    }

    char *command = args[2];

    if (strcmp(command, "LIST") == 0) {
        return LIST;
    }

    if (strcmp(command, "GET") == 0) {
        if (args[3] != NULL && args[4] != NULL) {
            return GET;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "DELETE") == 0) {
        if (args[3] != NULL) {
            return DELETE;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "PUT") == 0) {
        if (args[3] == NULL || args[4] == NULL) {
            print_client_help();
            exit(1);
        }
        return PUT;
    }

    // Not a valid Method
    print_client_help();
    exit(1);
}

*/




/**
* Networking Lab
* CS 241 - Fall 2018
*/

#include "common.h"
#include "format.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

char **parse_args(int argc, char **argv);
verb check_args(char **args);

int main(int argc, char **argv) {
    // Good luck!
    
    char **command = parse_args(argc, argv);
    verb operation = check_args(command);

    //TCP Client
    int s;
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);    
    if (sock_fd == -1) {
	fprintf(stderr, "Failed to create socket\n");
	exit(1);
    }
    
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    
    s = getaddrinfo(command[0], command[1], &hints, &result);
    if (s != 0) {
	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
	exit(1);
    }
    
    if (connect(sock_fd, result->ai_addr, result->ai_addrlen) == -1) {
	perror("connect");
	exit(2);
    }

    freeaddrinfo(result);
    
    //verb is LIST
    if (operation == LIST) {
	char buffer[1000000];
	dprintf(sock_fd, "LIST\n");
	shutdown(sock_fd, SHUT_WR);
	
	size_t bytes_read = 0;
	char* it = buffer;
	while ((bytes_read = read(sock_fd, it, 1)) > 0) {
	    it = it + bytes_read;
	}
	size_t len = 0;
	char* temp = NULL;
	if (strstr(buffer, "OK") == buffer) {
	    for (int i = 14; i >= 0; i -= 2) {
		len += buffer[i / 2 + 3] << (i * 4);
	    }
	    temp = (buffer + 11);
	    write(1, (buffer + 11), len);
	}
	else if (strstr(buffer, "ERROR") == buffer) {
	    temp = NULL;
	    char* t1 = strstr(buffer, "\n");
	    t1 = strstr(t1+1, "\n");
	    char t2 = *t1;
	    *t1 = 0;
	    print_error_message(buffer+6);
	    *t1 = t2;
	}
	else {
	    temp = (char*)(-1);
	    print_invalid_response();
	}
    }
    else if (operation == GET) {
	int lfd = open(command[4], O_CREAT | O_TRUNC | O_RDWR, 0777);
	dprintf(sock_fd, "Get %s\n", command[3]);
	shutdown(sock_fd, SHUT_WR);
	char buff[1024];
	memset(buff, 0, 1024);
	int bytes_read = 0;
	while (1) {
	    int err_nom = errno;
	    bytes_read = read(sock_fd, buff, 1024);
	    if (bytes_read >= 0) {
		break;
	    }
	    if (bytes_read == -1 && errno == EAGAIN) {
		errno = err_nom;
		continue;
	    }
	    else {
		fprintf(stderr, "Read failed!\n");
		exit(1);
	    }
	    errno = err_nom;
	}
	size_t len = 0;
	size_t true_len = bytes_read - 11;
	char* temp = NULL;
	if (strstr(buff, "OK") == buff) {
    	    for (int i = 14; i >= 0; i -= 2) {
		len += buff[i / 2 + 3] << (i * 4);
	    }
	    temp = (buff + 11);
	    write(lfd, temp, (bytes_read-11));
	    char buffer2[65536];
	    memset(buffer2, 0, 65536);
	    while (1) {
		memset(buffer2, 0, 65536);
		int err_nom = errno;
		int bytes_read2 = read(sock_fd, buffer2, 65536);
		if (bytes_read2 == 0) {
		    break;
		}
		else if (bytes_read2 > 0) {
		    write(lfd, buffer2, bytes_read2);
		    true_len += bytes_read2;
		}
		else {
		    if (bytes_read == -1 && errno == EAGAIN) {
			errno = err_nom;
			continue;
		    }
		}
		errno = err_nom;
	    }
	    if (true_len > len) {
		print_received_too_much_data();
	    }
	    else if (true_len < len) {
		print_too_little_data();
	    }
	}
	else if (temp == NULL) {
	    char* t1 = strstr(buff, "\n");
	    t1 = strstr((t1+1), "\n");
	    char t2 = *t1;
	    *t1 = 0;
	    print_error_message(buff+6);
	    *t1 = t2;
	}
	else {
	    print_invalid_response();
	}
    }
    else if (operation == DELETE) {
	dprintf(sock_fd, "DELETE %s\n", command[3]);
	shutdown(sock_fd, SHUT_WR);
	char buff[1024];
	memset(buff, 0, 1024);
	char* it = buff;
	int bytes_read = 0;
	while ((bytes_read = read(sock_fd, it, 1)) > 0) {
	    it = it + bytes_read;
	}
	if (strstr(buff, "OK") == buff) {
	    print_success();
	}
	else if (strstr(buff, "ERROR") == buff) {
	    char* t1 = strstr(buff, "\n");
	    t1 = strstr((t1+1), "\n");
	    char t2 = *t1;
	    *t1 = 0;
	    print_error_message(buff+6);
	    *t1 = t2;
	}
	else {
	    print_invalid_response();
	}
    }
    else if (operation == PUT) {
	FILE* lf = fopen(command[4], "rb");
	if (lf == NULL) {
	    exit(-1);
	}
	struct stat s;
	stat(command[4], &s);
	size_t len = s.st_size;
	unsigned char lenarr[8];
	for (int i = 0; i < 8; i++) {
	    lenarr[i] = 0xFF & len;
	    len = len >> 8;
	}
	fprintf(stderr, "file size : %zu\n", len);
	dprintf(sock_fd, "PUT %s\n", command[3]);
	for (int i = 0; i < 8; i++) {
	    dprintf(sock_fd, "%c", lenarr[i]);
	}
	char sfbuff[10000];
	memset(sfbuff, 0, 10000);
	if (len >= 9990) {
	    while ((len = fread(sfbuff, 1, 9900, lf))) {
		write(sock_fd, sfbuff, len);
		memset(sfbuff, 0, 10000);
	    }
	}
	else {
	    fread(sfbuff, 1, len, lf);
	    write(sock_fd, sfbuff, len);
	}
	shutdown(sock_fd, SHUT_WR);
	int bytes_read = 0;
	char buff2[1024];
	memset(buff2, 0, 1024);
	char* it = buff2;
	while ((bytes_read = read(sock_fd, it, 1)) > 0) {
	    it = it + bytes_read;
	}
	if (strstr(buff2, "OK") == buff2) {
	    print_success();
	}
	else if (strstr(buff2, "ERROR") == buff2) {
	    char* t1 = strstr(buff2, "\n");
	    t1 = strstr((t1+1), "\n");
	    char t2 = *t1;
	    *t1 = 0;
	    print_error_message(buff2+6);
	    *t1 = t2;
	}
	else {
	    print_invalid_response();
	}
    }
    
    shutdown(sock_fd, 0);
    free(command);
    
    return 0;
}




/**
 * Given commandline argc and argv, parses argv.
 *
 * argc argc from main()
 * argv argv from main()
 *
 * Returns char* array in form of {host, port, method, remote, local, NULL}
 * where `method` is ALL CAPS
 */
char **parse_args(int argc, char **argv) {
    if (argc < 3) {
        return NULL;
    }

    char *host = strtok(argv[1], ":");
    char *port = strtok(NULL, ":");
    if (port == NULL) {
        return NULL;
    }

    char **args = calloc(1, 6 * sizeof(char *));
    args[0] = host;
    args[1] = port;
    args[2] = argv[2];
    char *temp = args[2];
    while (*temp) {
        *temp = toupper((unsigned char)*temp);
        temp++;
    }
    if (argc > 3) {
        args[3] = argv[3];
    }
    if (argc > 4) {
        args[4] = argv[4];
    }

    return args;
}

/**
 * Validates args to program.  If `args` are not valid, help information for the
 * program is printed.
 *
 * args     arguments to parse
 *
 * Returns a verb which corresponds to the request method
 */
verb check_args(char **args) {
    if (args == NULL) {
        print_client_usage();
        exit(1);
    }

    char *command = args[2];

    if (strcmp(command, "LIST") == 0) {
        return LIST;
    }

    if (strcmp(command, "GET") == 0) {
        if (args[3] != NULL && args[4] != NULL) {
            return GET;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "DELETE") == 0) {
        if (args[3] != NULL) {
            return DELETE;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "PUT") == 0) {
        if (args[3] == NULL || args[4] == NULL) {
            print_client_help();
            exit(1);
        }
        return PUT;
    }

    // Not a valid Method
    print_client_help();
    exit(1);
}

