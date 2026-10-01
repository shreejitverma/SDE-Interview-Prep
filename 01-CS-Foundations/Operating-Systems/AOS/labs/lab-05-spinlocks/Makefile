CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread -std=c11 -D_POSIX_C_SOURCE=200809L

.PHONY: all run test clean

all: spinlocks

spinlocks: spinlocks.c main.c
	$(CC) $(CFLAGS) -o spinlocks spinlocks.c main.c

run: spinlocks
	./spinlocks benchmark

test: spinlocks
	./spinlocks test

clean:
	rm -f spinlocks *.o
