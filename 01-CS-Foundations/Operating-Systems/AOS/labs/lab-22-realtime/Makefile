CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread

all: timers

timers: timers.c
	$(CC) $(CFLAGS) -o $@ $<

run: all
	./run-cyclictest.sh
	./test.sh

test: all
	./test.sh

clean:
	rm -f timers expected-output.txt *.png *.json *.out *.log
