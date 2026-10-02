#!/usr/bin/env bash
set -euo pipefail

if [ $# -eq 0 ]; then
    echo "Usage: $0 <file>"
    exit 1
fi

# map: split words and lowercase
# shuffle: sort
# reduce: count unique
cat "$1" | tr -cs 'A-Za-z' '\n' | tr 'A-Z' 'a-z' | sort | uniq -c | sort -nr
