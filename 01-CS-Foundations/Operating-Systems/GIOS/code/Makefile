# GIOS Code Examples - Makefile
#
# Builds all C examples; Python scripts run directly.
#
# Usage:
#   make              - Build all C programs
#   make test         - Build and run all demos
#   make bench        - Run performance benchmarks
#   make clean        - Remove build artifacts
#   make <target>     - Build single target (e.g., make fork_exec_demo)

CC      := gcc
CFLAGS  := -Wall -Wextra -O2 -march=native -std=gnu11
LDFLAGS := -lpthread

# macOS doesn't have -lrt; Linux does
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    LDFLAGS += -lrt
endif

# ── C Targets ──────────────────────────────────────────────────────────
C_TARGETS := \
    fork_exec_demo        \
    producer_consumer_posix \
    spinlock_implementations \
    mmap_shared_memory    \
    pthreads_lifecycle    \
    mutex_condvar         \
    reader_writer

.PHONY: all clean test bench demo python-test

all: $(C_TARGETS)
	@echo ""
	@echo "Built $(words $(C_TARGETS)) C programs successfully."
	@echo "Python scripts run directly: python3 page_replacement_sim.py --demo"
	@echo "                             python3 disk_scheduler_sim.py --demo"

# ── Generic rule: foo.c → foo ──────────────────────────────────────────
%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "Built: $@"

# ── Individual targets with specific flags ─────────────────────────────
fork_exec_demo: fork_exec_demo.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

producer_consumer_posix: producer_consumer_posix.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

spinlock_implementations: spinlock_implementations.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

mmap_shared_memory: mmap_shared_memory.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

pthreads_lifecycle: pthreads_lifecycle.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

mutex_condvar: mutex_condvar.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

reader_writer: reader_writer.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# ── Test: run each demo briefly ────────────────────────────────────────
test: all python-test
	@echo "\n=== Running C Demos ==="

	@echo "\n--- fork_exec_demo (demos 1,2,3) ---"
	./fork_exec_demo 1
	./fork_exec_demo 2
	./fork_exec_demo 3

	@echo "\n--- producer_consumer (2 producers, 2 consumers, 5 items each) ---"
	./producer_consumer_posix 4 2 2 5

	@echo "\n--- pthreads_lifecycle (basic) ---"
	./pthreads_lifecycle 1

	@echo "\n--- mutex_condvar (condvar demo) ---"
	./mutex_condvar 4

	@echo "\n--- reader_writer (implementations) ---"
	./reader_writer 1

	@echo "\n=== All tests passed ==="

python-test:
	@echo "\n=== Running Python Demos ==="
	python3 page_replacement_sim.py --demo 2>&1 | head -60
	python3 disk_scheduler_sim.py   --demo 2>&1 | head -60
	@echo "Python demos complete."

# ── Benchmarks ────────────────────────────────────────────────────────
bench: all
	@echo "\n=== Spinlock Benchmark (4 threads, 1M iterations) ==="
	./spinlock_implementations 4 1000000

	@echo "\n=== Producer-Consumer Throughput ==="
	./producer_consumer_posix 16 4 4 50000

	@echo "\n=== Reader-Writer Benchmark ==="
	./reader_writer 2

	@echo "\n=== Page Replacement Benchmark ==="
	python3 page_replacement_sim.py --benchmark 2>&1 | tail -30

	@echo "\n=== Disk Scheduling Benchmark ==="
	python3 disk_scheduler_sim.py --benchmark 2>&1 | tail -30

# ── Clean ─────────────────────────────────────────────────────────────
clean:
	rm -f $(C_TARGETS)
	rm -f /tmp/gios_mmap_demo /tmp/gios_bench_file /tmp/cleanup_test.txt
	@echo "Cleaned up build artifacts."

# ── Help ──────────────────────────────────────────────────────────────
help:
	@echo "GIOS Code Examples - Available targets:"
	@echo "  make              - Build all C programs"
	@echo "  make test         - Build and run all demos"
	@echo "  make bench        - Run performance benchmarks"
	@echo "  make clean        - Remove build artifacts"
	@echo ""
	@echo "Individual C targets:"
	@for t in $(C_TARGETS); do echo "  make $$t"; done
	@echo ""
	@echo "Python scripts (no build needed):"
	@echo "  python3 page_replacement_sim.py --demo"
	@echo "  python3 page_replacement_sim.py --benchmark"
	@echo "  python3 disk_scheduler_sim.py   --demo"
	@echo "  python3 disk_scheduler_sim.py   --benchmark"
	@echo "  python3 disk_scheduler_sim.py   --ssd"
