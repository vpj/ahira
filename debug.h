#ifndef DEBUG_H
#define DEBUG_H
#include "headers.h"

#define FATAL_ERROR 2
#define ERROR_MESSAGE 1

void log_error(char *m, int level);

void log_epoll_event(struct epoll_event e);
#endif
