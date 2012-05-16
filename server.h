#ifndef SERVER_H
#define SERVER_H
#include "headers.h"

#define MAX_PENDING 100

typedef struct server_handler_context_s {
 struct sockaddr_in addr;
 int fd;
} server_handler_context;

server_handler_context *
server_handler_init(int port);

int
handle_server_events(struct epoll_event e, void *context);

#endif
