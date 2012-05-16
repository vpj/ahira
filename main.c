#include "ioloop.h"
#include "server.h"

int main(int argc, char *argv[]) {
 if (argc != 2) {
  fprintf(stderr, "USAGE: simple_server <port>\n");
  exit(1);
 }

 ioloop_init();

 server_handler_init(atoi(argv[1]));

 ioloop_start();

 return 0;
}
