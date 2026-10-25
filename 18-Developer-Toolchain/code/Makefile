# Verification entry points for the Developer Toolchain labs.
#
# Every output block in an 18-Developer-Toolchain note is copied from a lab's
# expected/<platform>.txt, and `make verify` re-runs the labs and diffs them,
# so a tool upgrade that changes behavior fails here instead of in a reader's terminal.

PYTHON ?= python3
LAB    ?=

.PHONY: help verify record list

help:
	@echo "make verify            - run every lab whose requirements are met and diff against expected output"
	@echo "make record            - (re)write expected output for this platform (review the diff before committing)"
	@echo "make list              - list labs and their requirements"
	@echo "Restrict to labs whose path contains LAB, e.g. 'make verify LAB=02-Version-Control/mini_git/'"
	@echo "Opt in to gated labs with TOOLCHAIN_LABS_PMU=1 or TOOLCHAIN_LABS_NETWORK=1"

verify:
	@$(PYTHON) -m _harness.verify $(LAB)

record:
	@$(PYTHON) -m _harness.verify --record $(LAB)

list:
	@find . -name run.sh -not -path './_harness/*' | sort | while read -r f; do \
		printf '%-50s %s\n' "$$(dirname "$$f")" "$$(grep -m1 -E '^# *requires:' "$$f" || echo '# requires: -')"; \
	done
