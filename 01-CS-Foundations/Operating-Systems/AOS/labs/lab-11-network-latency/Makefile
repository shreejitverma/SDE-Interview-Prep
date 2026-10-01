PYTHON = /opt/aos-venv/bin/python
PROTOC = protoc

.PHONY: all run test clean

all: capsule_pb2.py

capsule_pb2.py: capsule.proto
	$(PROTOC) --python_out=. capsule.proto

run: all
	@echo "--- Network Latency ---"
	@bash latency_test.sh
	@echo ""
	@echo "--- Marshaling Cost ---"
	@$(PYTHON) marshal_test.py
	@echo ""
	@echo "--- Capsule Simulator ---"
	@$(PYTHON) capsule_sim.py

test: all
	@echo "Checking protobuf compilation..."
	@test -f capsule_pb2.py && echo "PASS: Protobuf generated" || (echo "FAIL: Protobuf not generated"; exit 1)
	@echo "Checking Python imports..."
	@$(PYTHON) -c "import capsule_pb2; import json; import struct; import time" && echo "PASS: Python dependencies present" || (echo "FAIL: Python dependencies missing"; exit 1)
	@echo "Testing latency_test.sh (dry run / basic check)..."
	@bash -n latency_test.sh && echo "PASS: latency_test.sh syntax OK" || (echo "FAIL: latency_test.sh syntax error"; exit 1)
	@echo "Testing marshal_test.py..."
	@$(PYTHON) marshal_test.py > /dev/null && echo "PASS: marshal_test.py runs" || (echo "FAIL: marshal_test.py failed"; exit 1)
	@echo "Testing capsule_sim.py..."
	@$(PYTHON) capsule_sim.py > /dev/null && echo "PASS: capsule_sim.py runs" || (echo "FAIL: capsule_sim.py failed"; exit 1)
	@echo "ALL TESTS PASSED"

clean:
	rm -f capsule_pb2.py
	rm -rf __pycache__
