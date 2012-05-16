#ifndef ECHO_H
#define ECHO_H
#include "headers.h"

#define BUFFER_SIZE 1000

typedef struct { 
 struct sockaddr_in addr;
 int fd;
 char buffer[BUFFER_SIZE];
 int received;
} echo_handler_context;

echo_handler_context *
echo_handler_init(int fd, struct sockaddr_in addr);

int 
handle_echo_events(struct epoll_event e, void *context);

#endif 

