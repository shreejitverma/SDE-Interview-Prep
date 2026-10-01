CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread

TARGETS = page_faults pagemap cache_tlb prod_cons

.PHONY: all clean run test

all: $(TARGETS)

page_faults: page_faults.c
	$(CC) $(CFLAGS) -o $@ $<

pagemap: pagemap.c
	$(CC) $(CFLAGS) -o $@ $<

cache_tlb: cache_tlb.c
	$(CC) $(CFLAGS) -o $@ $<

prod_cons: prod_cons.c
	$(CC) $(CFLAGS) -o $@ $<

run: all
	./page_faults
	./pagemap
	./cache_tlb
	./prod_cons

drop_caches:
	sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches'

test: all
	@set -e; \
	echo "Running page_faults..."; \
	./page_faults; \
	echo "PASS: page_faults completed."; \
	echo "Running pagemap..."; \
	./pagemap; \
	echo "PASS: pagemap completed."; \
	echo "Running cache_tlb..."; \
	./cache_tlb; \
	echo "PASS: cache_tlb completed."; \
	echo "Running prod_cons..."; \
	./prod_cons > /dev/null; \
	echo "PASS: prod_cons completed."; \
	echo "PASS: All tests passed."

clean:
	rm -f $(TARGETS) fault_test.dat
