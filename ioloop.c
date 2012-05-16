#include "ioloop.h"
#include "debug.h"

#define EPOLL_EVENTS 100
#define MAX_EVENTS 100
#define HASHTABLE_SIZE 97

typedef struct {
 handler_function h;
 void * context;
} handler;

typedef struct hash_entry_s hash_entry_t;

typedef hash_entry_t * hash_entry;

struct hash_entry_s {
 int fd;
 handler *h;
 hash_entry next;
};

typedef struct {
 hash_entry *table;
 int size;
} hash_table;

typedef struct {
 struct epoll_event *events;
 hash_table *handlers;
 int epfd;
} ioloop_context;

ioloop_context *ioloop = NULL;
static hash_table * 
hash_table_init(int s) {
 hash_table *t = malloc(sizeof(hash_table));
 t->size = s;
 t->table = malloc(sizeof(hash_entry) * s);
 memset(t->table, 0, sizeof(hash_entry) * s);

 return t;
}

static int
hash_value(hash_table *ht, int fd) {
 return fd % ht->size;
}

static handler *
hash_get(hash_table *ht, int fd) {
 int hv;
 hash_entry he;

 hv = hash_value(ht, fd);
 he = ht->table[hv];

 while(he) {
  if(he->fd == fd) {
   return he->h;
  }
  he = he->next;
 }

 return NULL;
}

static int
hash_set(hash_table *ht, int fd, handler *h) {
 int hv;
 hash_entry *he;
 hash_entry n;

 hv = hash_value(ht, fd);
 he = &(ht->table[hv]);

 while(*he) {
  if((*he)->fd == fd) {
   (*he)->h = h;
   return 1;
  }

  he = &((*he)->next);
 }

 *he = malloc(sizeof(hash_entry_t));
 memset(*he, 0, sizeof(hash_entry_t));
 (*he)->h = h;
 (*he)->fd = fd;

 return 1;
}

static int
hash_remove(hash_table *ht, int fd) {
 int hv;
 hash_entry *he;
 hash_entry n;

 hv = hash_value(ht, fd);
 he = &(ht->table[hv]);

 while(*he) {
  if((*he)->fd == fd) {
   *he = (*he)->next;
   return 1;
  }

  he = &((*he)->next);
 }

 return 0;
}

int ioloop_set_nonblocking(int fd) {
 int flags = 0;

 if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
  flags = 0;
 return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void
ioloop_init() {
 ioloop = malloc(sizeof(ioloop_context));
 ioloop->epfd = epoll_create(EPOLL_EVENTS);

 if(ioloop->epfd < 0)
  log_error("Failed to create epoll", FATAL_ERROR);

 ioloop->handlers = hash_table_init(HASHTABLE_SIZE);
 ioloop->events = malloc(sizeof(struct epoll_event) * MAX_EVENTS);
}

void
ioloop_start() {
 int nfds;
 int i;
 int fd;
 handler *h;

 for(;;) {
  nfds = epoll_wait(ioloop->epfd, ioloop->events, MAX_EVENTS, -1 /* Timeout */);

  for(i = 0; i < nfds; ++i) {
   fd = ioloop->events[i].data.fd;

   if(!(h = hash_get(ioloop->handlers, fd))) {
    log_error("Event handler not present", ERROR_MESSAGE);
    continue;
   }

   h->h(ioloop->events[i], h->context);
  }
 }
}

/* Will replace exisitng handler if present ?  */
void
ioloop_add_handler(int fd, handler_function hf, void *context, unsigned int events) {
 handler *h;
 struct epoll_event e;

 h = malloc(sizeof(handler));
 h->h = hf;
 h->context = context;
 hash_set(ioloop->handlers, fd, h);

 e.data.fd = fd;
 e.events = events;

 if(epoll_ctl(ioloop->epfd, EPOLL_CTL_ADD, fd, &e) < 0) {
  log_error("Failed to insert handler to epoll", ERROR_MESSAGE);
  log_epoll_event(e);
 }
}

void
ioloop_modify_handler(int fd, unsigned int events) {
 struct epoll_event e;

 if(!hash_get(ioloop->handlers, fd)) {
  log_error("Changing handler of a non registered file descriptor", ERROR_MESSAGE);
  return;
 }

 e.data.fd = fd;
 e.events = events;

 if(epoll_ctl(ioloop->epfd, EPOLL_CTL_MOD, fd, &e) < 0) {
  log_error("Failed to modify epoll events", ERROR_MESSAGE);
  log_epoll_event(e);
 }
}

void
ioloop_remove_handler(int fd) {
 struct epoll_event e;

 if(!hash_remove(ioloop->handlers, fd)) {
  log_error("Removing handler of a non registered file descriptor", ERROR_MESSAGE);
  return;
 }

 e.data.fd = fd;

 if(epoll_ctl(ioloop->epfd, EPOLL_CTL_DEL, fd, &e) < 0) {
  log_error("Failed to modify epoll events", ERROR_MESSAGE);
  log_epoll_event(e);
 }
}

