CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread

all: syscall_cost context_switch

syscall_cost: syscall_cost.c
	$(CC) $(CFLAGS) -o $@ $^

context_switch: context_switch.c
	$(CC) $(CFLAGS) -o $@ $^

run: all
	@echo "--- Syscall Cost ---"
	@./syscall_cost
	@echo ""
	@echo "--- Context Switch Cost ---"
	@./context_switch
	@echo ""
	@echo "--- perf bench sched pipe ---"
	@perf bench sched pipe || true

test: run
	@echo "PASS: lab-01 tests"

clean:
	rm -f syscall_cost context_switch
