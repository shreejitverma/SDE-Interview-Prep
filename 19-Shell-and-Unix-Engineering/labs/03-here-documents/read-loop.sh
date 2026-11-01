#!/bin/sh
# read-loop: a while-read loop fed by a here-document, with and without a command inside the
# loop that also reads standard input. /bin/cat is external, so it really is a second reader.
# Usage: read-loop.sh eat|guarded
# Prints one "got LINE" per iteration and a final "n=COUNT" taken after the loop, which shows
# that a loop fed by a here-document runs in the current shell.
mode=${1:?usage: read-loop.sh eat|guarded}
n=0
case $mode in
  eat)
    while read -r line; do
      /bin/cat >/dev/null
      n=$((n + 1))
      echo "got $line"
    done <<EOF
a
b
c
EOF
    ;;
  guarded)
    while read -r line; do
      /bin/cat >/dev/null </dev/null
      n=$((n + 1))
      echo "got $line"
    done <<EOF
a
b
c
EOF
    ;;
  *)
    echo "read-loop.sh: unknown mode: $mode" >&2
    exit 2
    ;;
esac
echo "n=$n"
