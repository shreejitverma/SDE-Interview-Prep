#!/bin/sh
# pipe_probe.sh - Probe SIGPIPE generation, pipeline exit status, and EPIPE error handling.
# Part of 19-Shell-and-Unix-Engineering/01-Unix-Process-Model/SIGPIPE-and-EPIPE
set -u

# Step 1: Default pipeline termination.
# Upstream generates infinite lines; head consumes 2 and terminates.
# Upstream receives SIGPIPE and dies silently; pipeline exit status reflects head (0).
printf '=== Step 1: Default pipeline termination ===\n'
yes 2>/dev/null | head -n 2
printf 'pipeline exit status: %d\n' "$?"

# Step 2: Signal exit status inspection.
# When a process terminates due to SIGPIPE (signal 13), the shell wait status reflects it.
printf '=== Step 2: Direct SIGPIPE exit status ===\n'
exit_code=0
sh -c 'kill -13 $$' 2>/dev/null || exit_code="$?"
printf 'kill -13 exit status: %d\n' "${exit_code:-0}"

# Step 3: Ignored SIGPIPE exposing EPIPE.
# When SIGPIPE is set to SIG_IGN, the kernel does not kill the writer.
# The write() system call fails with EPIPE (errno 32).
printf '=== Step 3: Ignored SIGPIPE exposing EPIPE ===\n'
(
  trap '' PIPE
  yes | head -n 2
)
printf 'pipeline exit status: %d\n' "$?"

# Step 4: Pipeline exit status under pipefail (where supported).
printf '=== Step 4: Pipeline exit status under pipefail ===\n'
# shellcheck disable=SC3040 # feature detection of pipefail support in subshell
if (set -o pipefail 2>/dev/null); then
  pf_status=0
  # Measure the shell under test itself, in a subshell, not whatever sh is on PATH.
  # shellcheck disable=SC2312,SC3040 # the pipeline status is the value being measured; pipefail is guarded above
  pf_status=$(
    set -o pipefail
    yes 2>/dev/null | head -n 2 >/dev/null
    echo "$?"
  ) || pf_status="$?"
  printf 'pipefail exit status: %s\n' "${pf_status:-0}"
else
  printf 'pipefail not supported by this shell\n'
fi
