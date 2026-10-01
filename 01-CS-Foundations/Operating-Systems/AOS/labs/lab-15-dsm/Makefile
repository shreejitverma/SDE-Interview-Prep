CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2 -pthread

all: dsm

dsm: dsm.c
	$(CC) $(CFLAGS) -o $@ $^

run: dsm
	./dsm

test: dsm
	@./dsm > /dev/null && echo "PASS" || (echo "FAIL"; exit 1)

clean:
	rm -f dsm
