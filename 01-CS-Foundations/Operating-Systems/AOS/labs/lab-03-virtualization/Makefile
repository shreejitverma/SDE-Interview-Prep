all: ksm_test

fetch:
	if [ ! -f cirros.img ]; then \
		wget -qO cirros.img https://download.cirros-cloud.net/0.6.3/cirros-0.6.3-aarch64-disk.img || true; \
	fi

ksm_test: ksm_test.c
	$(CC) -Wall -Wextra -O2 -pthread -o $@ $<

run: fetch ksm_test
	./test.sh

test: run
	@echo "PASS"

clean:
	rm -f ksm_test cirros.img expected-output.txt domain.xml

.PHONY: all fetch run test clean
