CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread -std=c11

BINS = ipc_bench sendfile_bench

all: $(BINS)

ipc_bench: ipc_bench.c
	$(CC) $(CFLAGS) -o $@ $^

sendfile_bench: sendfile_bench.c
	$(CC) $(CFLAGS) -o $@ $^

run: all
	chmod +x test.sh
	./test.sh

test: all
	chmod +x test.sh
	./test.sh

clean:
	rm -f $(BINS) testdata.bin
