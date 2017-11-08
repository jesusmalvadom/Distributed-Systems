CC = gcc
CFLAGS = -g -lpthread
DEPS = semaphore.h
EXEC = middleware server client

COPT = -Wall

all: clean ${EXEC}

semaphore.o: semaphore.c semaphore.h
	gcc -c semaphore.c

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

middleware: middleware.o semaphore.o
	gcc -o $@ $^ $(CFLAGS)

server: server.o semaphore.o
	gcc -o $@ $^ $(CFLAGS)

client: client.o semaphore.o
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OBJ) $(EXEC)
