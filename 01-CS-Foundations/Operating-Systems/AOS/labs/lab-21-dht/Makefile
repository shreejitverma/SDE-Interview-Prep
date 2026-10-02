.PHONY: all run test clean fetch

all:
	@echo "All simulator scripts are ready."

run:
	@echo "\n--- Running Consistent Hashing ---"
	@./consistent_hashing.py
	@echo "\n--- Running Chord Routing ---"
	@./chord_routing.py
	@echo "\n--- Running Coral Sloppy DHT ---"
	@./coral_sloppy.py
	@echo "\n--- Running Dynamo Quorums ---"
	@./dynamo_quorums.py

test: run
	@echo "PASS: All tests completed successfully."

clean:
	rm -rf __pycache__

fetch:
	@echo "No external images to fetch."
