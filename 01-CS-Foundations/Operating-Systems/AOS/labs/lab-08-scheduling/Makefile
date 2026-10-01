CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread

all: affinity_workload

affinity_workload: affinity_workload.c
	$(CC) $(CFLAGS) -o $@ $<

run: all
	./test.sh

test: all
	./test.sh

clean:
	rm -f affinity_workload perf.data perf.data.old

.PHONY: all run test clean
