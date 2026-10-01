CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread -std=gnu11

.PHONY: all test run clean

all: test_rvm

test_rvm: src/test_rvm.c src/rvm.c src/rvm.h
	$(CC) $(CFLAGS) -o test_rvm src/test_rvm.c src/rvm.c

test: test_rvm
	./test_rvm
	bash fio-fsync.sh
	@echo "PASS: all tests completed successfully"

run: test

clean:
	rm -f test_rvm *.log *.data src/*.o
