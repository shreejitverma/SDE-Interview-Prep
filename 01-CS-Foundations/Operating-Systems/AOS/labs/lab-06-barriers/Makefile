CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread -fopenmp
MPICC = mpicc

all: measure_threads measure_mpi

measure_threads: measure_threads.c
	$(CC) $(CFLAGS) -o measure_threads measure_threads.c

measure_mpi: measure_mpi.c
	$(MPICC) $(CFLAGS) -o measure_mpi measure_mpi.c

run: all
	@echo "--- Simulator ---"
	/opt/aos-venv/bin/python simulator.py --plot
	@echo "--- Pthread and OpenMP Barriers ---"
	./measure_threads 4
	@echo "--- MPI Barrier ---"
	mpirun --allow-run-as-root -np 4 ./measure_mpi

test: run
	@echo "PASS: lab-06-barriers"

clean:
	rm -f measure_threads measure_mpi barrier_plot.png
