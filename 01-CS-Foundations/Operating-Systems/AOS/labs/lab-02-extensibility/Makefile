CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread

all: userfaultfd_demo

userfaultfd_demo: userfaultfd_demo.c
	$(CC) $(CFLAGS) -o $@ $<

run: all
	@echo "--- Running userfaultfd_demo (Exokernel analogy) ---"
	@sudo ./userfaultfd_demo
	@echo ""
	@echo "--- Running bpftrace exec trace (SPIN analogy) ---"
	@sudo ./trace_exec.sh

test: all run
	@echo "PASS: lab-02-extensibility"

clean:
	rm -f userfaultfd_demo
