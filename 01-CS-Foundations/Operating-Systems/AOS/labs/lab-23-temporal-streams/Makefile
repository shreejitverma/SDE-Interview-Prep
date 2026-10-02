.PHONY: all run test clean fetch

PYTHON = /opt/aos-venv/bin/python
CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread

all: run

measure_clock: measure_clock.c
	$(CC) $(CFLAGS) -o $@ $^

run: measure_clock
	@echo "Running Temporal Stream Demo"
	@$(PYTHON) stream_store.py
	@echo "\nRunning Clock Sync Simulation"
	@$(PYTHON) clock_sync.py
	@echo "\nRunning Clock Overhead Measurement"
	@./measure_clock

test: measure_clock
	@$(PYTHON) stream_store.py test
	@$(PYTHON) clock_sync.py test
	@./measure_clock > /dev/null
	@echo "PASS: lab-23-temporal-streams tests passed"

clean:
	rm -f *.json measure_clock

fetch:
	@echo "No external assets to fetch"
