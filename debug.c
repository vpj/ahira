#include "debug.h"

void log_epoll_event(struct epoll_event e) {
 printf("Event fd: %d events: ", e.data.fd);
 if(e.events & EPOLLERR)
  printf("EPOLLERR ");
 if(e.events & EPOLLET)
  printf("EPOLLET ");
 if(e.events & EPOLLHUP)
  printf("EPOLLHUP ");
 if(e.events & EPOLLIN)
  printf("EPOLLIN ");
 if(e.events & EPOLLONESHOT)
  printf("EPOLLONESHOT ");
 if(e.events & EPOLLOUT)
  printf("EPOLLOUT ");
 if(e.events & EPOLLPRI)
  printf("EPOLLPRI ");

 printf("\n");
}

void log_error(char *m, int level) {
 if(level & FATAL_ERROR)
  perror("FATAL ERROR: ");
 else if(level & ERROR_MESSAGE)
  perror("ERROR: ");

 perror(m);

 if(level & FATAL_ERROR)
  exit(1);
}

