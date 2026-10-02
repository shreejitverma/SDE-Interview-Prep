.PHONY: all run test clean

all:

run:
	@echo "--- Unix Pipeline ---"
	@./pipeline.sh README.md | head -n 5
	@echo ""
	@echo "--- Python MapReduce ---"
	@./mr_python.py README.md | head -n 5
	@echo ""
	@echo "--- Scheduler Simulator ---"
	@./scheduler.py

test:
	@./pipeline.sh README.md > /dev/null
	@echo "PASS: pipeline.sh"
	@./mr_python.py README.md > /dev/null
	@echo "PASS: mr_python.py"
	@./scheduler.py > /dev/null
	@echo "PASS: scheduler.py"

clean:
	@rm -f *.pyc
	@rm -rf __pycache__
