CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread
PYTHON = /opt/aos-venv/bin/python

.PHONY: all run test clean

all: wal_shadow quicksilver_2pc.py

wal_shadow: wal_shadow.c
	$(CC) $(CFLAGS) -o $@ $<

run: all
	@echo "--- Write-Ahead Logging vs Shadow Paging ---"
	./wal_shadow
	@echo "\n--- Quicksilver Two-Phase Commit Simulation ---"
	$(PYTHON) quicksilver_2pc.py run

test: all
	@./wal_shadow > /dev/null
	@$(PYTHON) quicksilver_2pc.py run > /dev/null
	@echo "PASS: wal_shadow executed successfully."
	@echo "PASS: quicksilver_2pc.py executed successfully."

clean:
	rm -f wal_shadow wal_log.bin wal_data.bin shadow_data.bin
	rm -rf __pycache__ *.pyc
