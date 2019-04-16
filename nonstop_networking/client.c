/**
 * Nonstop Networking
 * CS 241 - Spring 2019
 */
#include "common.h"
#include "format.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netdb.h>
#include <fcntl.h>
char **parse_args(int argc, char **argv);
verb check_args(char **args);
int connect_to_server(char*host, char*port);

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


int main(int argc, char **argv) {
    // Good luck!
    int should_exit = 1;
    int mode1 = O_CREAT | O_TRUNC | O_RDWR;
    mode_t mode2 = S_IRWXU | S_IRWXG | S_IRWXO;
    char** info = parse_args(argc, argv);
    char* remote = info[3];
    char* local = info[4];
    verb count = check_args(info); 
    int sockfd = connect_to_server(info[0], info[1]);
	

    if (count == 4) { //UNKNOWN
    
    }


    if (count == 3) { //LIST
        dprintf(sockfd, "LIST\n");
        shutdown(sockfd, SHUT_WR);
        char data[102400]; //100x size
        char* data_p = data;
        int byte = read(sockfd, data_p, 1);
        while(byte) {
            data_p = data_p + byte;
            byte = read(sockfd, data_p, 1);
        }
        
        size_t size = 0;
        size_t max_size = 8;
        for (int i = max_size - 1; i >= 0; i--) {
            int amt = i * 4; //shifting byte
            size += data[i + 3] << amt;
        }
        char* str_ok = strstr(data, "OK");
        char* str_err = strstr(data, "ERROR");
        if (str_ok == data) {
            char* data_ptr = data;
            char* data_ptr2 = data_ptr + max_size + 3;
            write(1, data_ptr2, size);
        } else if (str_err == data) {
            char* msg = strstr(data, "\n");
            char* msg2 = strstr(msg + 1, "\n");
            *msg2 = '\0';
            char* final_msg = data + 6;
            print_error_message(final_msg);
        } else {
            print_invalid_response();
        }
        should_exit = 0;
    }


    if (count == 2) { //DELETE
        dprintf(sockfd,"DELETE %s\n", remote);
        shutdown(sockfd, SHUT_WR);
        char server[1024];
        memset(server, 0, 1024);
        char* server_p = server;
        int byte = read(sockfd, server_p, 1);
        
        while(byte) {
            server_p = server_p + byte;
            byte = read(sockfd, server_p, 1);
        }
        
        char* str_ok = strstr(server, "OK");
        char* str_err = strstr(server, "ERROR");
        if (str_ok == server) { //okay mesg
            print_success();//successful response
        } else if (str_err == server) { //error in response
            char* msg = strstr(server, "\n");
            char* msg2 = strstr(msg + 1, "\n");
            *msg2 = '\0';
            char* final_msg = server + 6;
            print_error_message(final_msg);
        } else {
            print_invalid_response();
        }
        
        should_exit = 0;
    } 


    if (count == 1) { 
        FILE* local_f = fopen(local, "r");
    
 	 if (!local_f) {
            exit(1);
         } //no file exists
        
        struct stat f_stat;
        stat(local, &f_stat);
        
        size_t stat_size = f_stat.st_size;
        //set data to be 8 bytes
        char binary_data[8];
        
	for (int i = 0; i < 8; i++) {
            binary_data[i] = stat_size & 0xFF;
            stat_size = stat_size >> 8;
        }
        dprintf(sockfd, "PUT %s\n", remote);

        for (int i = 0; i < 8; i++) {
            dprintf(sockfd, "%c", binary_data[i]);
        }
        
        char file_arr[10240];
        memset(file_arr, 0, 10240); // initialize to 0
        
        if (f_stat.st_size >= 1024) {
            size_t size = fread(file_arr, sizeof(char), 1024, local_f);
            while(size && !feof(local_f)) {
                write(sockfd, file_arr, size);
                memset(file_arr, 0, 10240);
                size = fread(file_arr, sizeof(char), 1024, local_f);
                
            }
            
        } 

	else {
            fread(file_arr, sizeof(char), f_stat.st_size, local_f);
            write(sockfd, file_arr, f_stat.st_size);
        }
        
        shutdown(sockfd, SHUT_WR);
        char server[1024];
        memset(server, 0, 1024);
        char* server_p = server;
        int byte = read(sockfd, server_p, 1);
        
	while(byte) {
            server_p = server_p + byte;
            byte = read(sockfd, server_p, 1);
        }
        
        char* str_ok = strstr(server, "OK");
        char* str_err = strstr(server, "ERROR");
        
        if (str_ok == server) { //okay mesg
            print_success();//successful response
        }
	 else if (str_err == server) {
            char* msg = strstr(server, "\n");
            char* msg2 = strstr(msg + 1, "\n");
            *msg2 = '\0';
            char* final_msg = server + 6;
            print_error_message(final_msg);
        }
	
	 else {
            print_invalid_response();
        }
        should_exit = 0;
    } 
    


   if (count == 0) { //GET
        int open_f = open(local, mode1, mode2); 
        dprintf(sockfd, "GET %s\n", remote);
        shutdown(sockfd, SHUT_WR);
        char header[1024]; //limit header length to 1024 bytes
        memset(header, 0, 1024);
        int error = errno;
        int result = 0;
        
        while(true) {
            result = read(sockfd, header, 1024);
            if (result == -1 && errno == EAGAIN) {
                errno = error;
                continue;
            } else if (result >= 0) {
                break;
            } else {
                exit(1); //failed
            }
            errno = error;
        }
        
        char* buff = header;
        char* str_ok = strstr(buff, "OK");
        char* str_err = strstr(buff, "ERROR");
  
      if (str_ok == buff) {
            int size = 0;
	    for (int i = 7; i >= 0; i--) {
                int amt = i * 4;
                size += (int) (header[i + 3] << amt);
            }
            int new_size = result - 11;
            write(open_f, buff + 11, new_size); 
            char buff_arr[4096]; 
            memset(buff_arr, 0, 4096);
	while(true) {
                error = errno;
                int read_val = read(sockfd, buff_arr, 4096);
                if (read_val > 0) {
                    write(open_f, buff_arr, read_val); //write to local
                    new_size += read_val;
                } else if (!read_val) {
                    break;
                } else {
                    if (errno == EAGAIN && result == -1) {
                        errno = error;
                        continue;
                    }
                }
                errno = error;
            }
        } 
	else if (str_err == buff) { 
            char* mesg = strstr(header, "\n"); // newline
            char* mesg2 = strstr(mesg + 1, "\n");
            *mesg2 = '\0'; // terminate with null
            char* final_msg = header + 6;
            print_error_message(final_msg);
        }
	
	 else { //neither ok or error
            print_invalid_response();
        }

        should_exit = 0;
    }



    shutdown(sockfd, 0);
    if (should_exit) {
        exit(0);
    }
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

int connect_to_server(char* host, char* port) {
    int s_domain = AF_INET;
    int s_type = SOCK_STREAM;
    int sfd = socket(s_domain, s_type, 0);
   
    struct addrinfo current, *result;
    memset(&current, 0, sizeof(struct addrinfo));
    
    current.ai_family = AF_INET;
    current.ai_socktype = SOCK_STREAM;
    
   
    int tempo = getaddrinfo(host, port, &current, &result);
    if (tempo != 0) {
        fprintf(stderr, "getaddrinfo : %s\n", gai_strerror(tempo));
        exit(1);
    }
    
    int temp = connect(sfd, result->ai_addr, result->ai_addrlen);
    freeaddrinfo(result);
    if (temp == -1) {
        perror("connection error");
        exit(1);
    }

    return sfd;
}
