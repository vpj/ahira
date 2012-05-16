#include "ioloop.h"
#include "echo.h"
#include "debug.h"

echo_handler_context *
echo_handler_init(int fd, struct sockaddr_in addr) {
 echo_handler_context *c;

 c = malloc(sizeof(echo_handler_context));
 memset(c, 0, sizeof(echo_handler_context));

 ioloop_set_nonblocking(fd);

 c->fd = fd;
 c->addr = addr;
 c->received = 0;

 ioloop_add_handler(fd, handle_echo_events, c, EPOLLIN);

 return c;
}

static void
free_context(echo_handler_context *c) {
 free(c);
}

int 
handle_echo_events(struct epoll_event e, void *context) {
 echo_handler_context *c = (echo_handler_context *)context;

 printf("echo event\n");
 log_epoll_event(e);

 if(e.events & EPOLLHUP) {
  close(c->fd);

  ioloop_remove_handler(c->fd);

  free_context(c);
  return -1;
 }

 if(e.events & EPOLLERR) {
  return -1;
 }

 if(e.events & EPOLLOUT) {
  if(c->received > 0) {
   printf("Writing: %s\n", c->buffer);

   if (send(c->fd, c->buffer, c->received, 0) != c->received) {
    log_error("Could not write to stream", ERROR_MESSAGE);
   }
  }

  c->received = 0;

  ioloop_modify_handler(c->fd, EPOLLIN);
 }

 if(e.events & EPOLLIN) {
  if ((c->received = recv(c->fd, c->buffer, BUFFER_SIZE, 0)) < 0) {
   log_error("Error reading from socket", ERROR_MESSAGE);
  } else {
   c->buffer[c->received] = 0;
   printf("Reading %d bytes: %s\n", c->received, c->buffer);
  }

  if(c->received > 0) {
   ioloop_modify_handler(c->fd, EPOLLOUT);
  } else {
   close(c->fd);
   ioloop_remove_handler(c->fd);

   free_context(c);
  }
 }

 return 0;
}


