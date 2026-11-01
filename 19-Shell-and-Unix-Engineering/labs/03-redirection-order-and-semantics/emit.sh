#!/bin/sh
# emit.sh [ARG...]: write "out N" to standard output (N is the argument count) and "err" to standard error.
# It is a separate script so the shell under test must fork and exec it, with the file descriptors
# exactly as the redirections left them.
echo "out $#"
echo err >&2
