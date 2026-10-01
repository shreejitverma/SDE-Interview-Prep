CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread -std=c11 -D_GNU_SOURCE
TARGETS = scalable_counters false_sharing read_mostly

all: $(TARGETS)

scalable_counters: scalable_counters.c
	$(CC) $(CFLAGS) -o $@ $<

false_sharing: false_sharing.c
	$(CC) $(CFLAGS) -o $@ $<

read_mostly: read_mostly.c
	$(CC) $(CFLAGS) -o $@ $<

run: all
	./scalable_counters
	./false_sharing
	./read_mostly

test: all
	./scalable_counters > /dev/null && echo "PASS: scalable_counters" || exit 1
	./false_sharing > /dev/null && echo "PASS: false_sharing" || exit 1
	./read_mostly > /dev/null && echo "PASS: read_mostly" || exit 1

clean:
	rm -f $(TARGETS)
