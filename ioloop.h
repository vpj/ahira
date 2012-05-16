#ifndef _IOLOOP_H
#define _IOLOOP_H

#include "headers.h"

typedef int (*handler_function )(struct epoll_event e, void *context);

void ioloop_init();
void ioloop_start();
void ioloop_add_handler(
  int fd,
  handler_function hf,
  void *context,
  unsigned int events);

void ioloop_modify_handler(int fd, unsigned int events);
void ioloop_remove_handler(int fd);
int ioloop_set_nonblocking(int fd);
#endif

