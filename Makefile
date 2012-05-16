CC = gcc -fPIC
LDFLAGS = -lm

# set DEBUG options
ifdef DEBUG
CFLAGS = -Wall -Wextra -ggdb -pg -DDEBUG
else
CFLAGS = -Wall -O2
endif

#name all the object files
OBJS = main.o debug.o echo.o ioloop.o server.o 

all : server

server: $(OBJS)
	$(CC) $(LDFLAGS) -o main $^

debug :
	make all DEBUG=1

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $^

doxy :
	doxygen Doxyfile

clean :
	rm -rf $(OBJS) main

