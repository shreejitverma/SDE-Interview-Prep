CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread -std=c11
BINS = false_sharing ping_pong litmus

.PHONY: all run test clean

all: $(BINS)

false_sharing: false_sharing.c
	$(CC) $(CFLAGS) $< -o $@

ping_pong: ping_pong.c
	$(CC) $(CFLAGS) $< -o $@

litmus: litmus.c
	$(CC) $(CFLAGS) $< -o $@

run: all
	@echo "--- Running False Sharing ---"
	@./false_sharing
	@echo "--- Running Ping Pong ---"
	@./ping_pong
	@echo "--- Running Litmus Tests ---"
	@./litmus

test: all
	@echo "Running tests..."
	@./false_sharing | grep -q "PASS" || (echo "FAIL: false_sharing" && exit 1)
	@echo "PASS: false_sharing test completed"
	@./ping_pong | grep -q "PASS" || (echo "FAIL: ping_pong" && exit 1)
	@echo "PASS: ping_pong test completed"
	@./litmus | grep -q "PASS" || (echo "FAIL: litmus" && exit 1)
	@echo "PASS: litmus test completed"

clean:
	rm -f $(BINS) *.o
