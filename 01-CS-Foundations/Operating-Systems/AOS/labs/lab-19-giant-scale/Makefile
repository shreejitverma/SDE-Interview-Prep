.PHONY: all run test clean

all:

run: test

test:
	@echo "--- Starting 2 nodes (capacity 5000 items/sec each) ---"
	@python3 server.py 8000 5000 > /dev/null 2>&1 & echo $$! > server1.pid
	@python3 server.py 8001 5000 > /dev/null 2>&1 & echo $$! > server2.pid
	@sleep 1
	@echo "--- Baseline: 10 QPS * 1000 items/query = 10,000 items/sec load ---"
	@python3 client.py replicated 10 2 8000,8001 > out_rep1.txt
	@python3 client.py partitioned 10 2 8000,8001 > out_part1.txt
	@cat out_rep1.txt
	@cat out_part1.txt
	@echo "--- Node 1 failed! ---"
	@kill -9 `cat server2.pid`
	@sleep 1
	@echo "--- Degraded Replicated: Load balancer sends all 10 QPS to remaining node ---"
	@python3 client.py replicated 10 2 8000 > out_rep2.txt
	@cat out_rep2.txt
	@echo "--- Degraded Partitioned: Scatter-gather to both, Node 1 times out ---"
	@python3 client.py partitioned 10 2 8000,8001 > out_part2.txt
	@cat out_part2.txt
	@echo "--- Checking results ---"
	@grep -q "Yield: 100.0%" out_part2.txt || (echo "FAIL: Degraded partitioned yield should be 100%" && exit 1)
	@grep -q "Harvest: 50.0%" out_part2.txt || (echo "FAIL: Degraded partitioned harvest should be 50%" && exit 1)
	@echo "PASS: All tests passed."
	@kill -9 `cat server1.pid`
	@rm -f server1.pid server2.pid out_*.txt

clean:
	@kill -9 `cat server1.pid 2>/dev/null` 2>/dev/null || true
	@kill -9 `cat server2.pid 2>/dev/null` 2>/dev/null || true
	@rm -f *.pid out_*.txt
