CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread -std=c11

.PHONY: all run test clean

all: gms_sim

gms_sim: gms_sim.c
	$(CC) $(CFLAGS) -o $@ $<

run: gms_sim
	./gms_sim

test: gms_sim
	./gms_sim > test_out.txt
	@grep -q "PASS: Cost measurement completed." test_out.txt && echo "PASS: gms_sim completed successfully" || (echo "FAIL: test failed"; exit 1)

clean:
	rm -f gms_sim test_out.txt
