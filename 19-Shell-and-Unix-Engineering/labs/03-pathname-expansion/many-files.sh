#!/bin/sh
# many-files: create N empty files with 200-character names in an existing directory.
# Usage: many-files.sh DIR N
# 32000 names of 200 bytes are about 6.4 MB of argument strings, above the 6 MiB that Linux
# allows at any stack limit (execve(2)) and above the 1 MiB ARG_MAX of macOS.
set -eu
cd "$1"
awk -v n="$2" 'BEGIN { for (i = 1; i <= n; i++) printf "%0200d\n", i }' | xargs touch --
