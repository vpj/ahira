#include "ioloop.h"
#include "echo.h"
#include "server.h"
#include "debug.h"

server_handler_context *
server_handler_init(int port) {
 server_handler_context *c;

 c = malloc(sizeof(server_handler_context));
 memset(c, 0, sizeof(server_handler_context));

 if ((c->fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
  log_error("Failed to create server socket", FATAL_ERROR);
 }

 c->addr.sin_family = AF_INET;
 c->addr.sin_addr.s_addr = htonl(INADDR_ANY);
 c->addr.sin_port = htons(port);

 if (bind(c->fd, (struct sockaddr *) &(c->addr),
                              sizeof(c->addr)) < 0) {
  log_error("Failed to bing server socket", FATAL_ERROR);
 }

 if (listen(c->fd, MAX_PENDING) < 0) {
  log_error("Failed to listen on server socket", FATAL_ERROR);
 }

 ioloop_set_nonblocking(c->fd);

 ioloop_add_handler(c->fd, handle_server_events, c, EPOLLIN);

 return c;
}

static void
free_context(server_handler_context *c) {
 free(c);
}

int 
handle_server_events(struct epoll_event e, void *context) {
 server_handler_context *c = (server_handler_context *)context;
 struct sockaddr_in client_addr;
 socklen_t ca_len = sizeof(client_addr);

 log_epoll_event(e);

 int client = accept(c->fd, (struct sockaddr *) &client_addr,
                 &ca_len);

 if(client < 0){
  log_error("Error accepting", ERROR_MESSAGE);
  return -1;
 }

 printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));
 /* asdfasdf */
 echo_handler_init(client, client_addr);
 return 0;
}

