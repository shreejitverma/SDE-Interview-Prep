CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread

all: clocks mutex

clocks: clocks.c
	$(CC) $(CFLAGS) -o $@ $^

mutex: mutex.c
	$(CC) $(CFLAGS) -o $@ $^

run: all
	@echo "--- Running Clocks ---"
	./clocks
	@echo "--- Running Mutex ---"
	./mutex

test: all
	@echo "--- Testing Clocks ---"
	./clocks > test_clocks.out
	@grep -q "Node 2 RECV.*Lamport:  7" test_clocks.out || (echo "FAIL: Clocks did not reach expected state"; exit 1)
	@echo "PASS: Clocks Simulation"
	@echo "--- Testing Mutex ---"
	./mutex > test_mutex.out
	@grep -q "Total Messages EXCHANGED: 36" test_mutex.out || (echo "FAIL: Mutex messages count incorrect"; exit 1)
	@echo "PASS: Mutex Simulation"

clean:
	rm -f clocks mutex test_*.out
