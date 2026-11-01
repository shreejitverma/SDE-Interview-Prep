#!/bin/sh
# many-files: create N empty files with 200-character names in an existing directory.
# Usage: many-files.sh DIR N
# 13000 names of 200 bytes are about 2.6 MB of argument strings, above the 2 MiB that Linux
# allows with an 8 MiB stack and above the 1 MiB ARG_MAX of macOS.
set -eu
cd "$1"
awk -v n="$2" 'BEGIN { for (i = 1; i <= n; i++) printf "%0200d\n", i }' | xargs touch --
