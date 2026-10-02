CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread
PYTHON = /opt/aos-venv/bin/python

all: seccomp_filter

seccomp_filter: seccomp_filter.c
	$(CC) $(CFLAGS) -o $@ $^

run: all
	@echo "--- Testing Namespaces & Capabilities ---"
	@./caps_namespaces.sh
	@echo "\n--- Testing Seccomp Filter ---"
	@bash -c './seccomp_filter 2>&1 || { if [ $$? -eq 159 ]; then echo "seccomp_filter successfully killed by SIGSYS (exit 159)"; else echo "seccomp_filter failed with unexpected exit code $$?"; exit 1; fi; }'
	@echo "\n--- Testing Andrew Handshake ---"
	@$(PYTHON) andrew_handshake.py

test: run
	@echo "PASS: lab-24-security"

clean:
	rm -f seccomp_filter
