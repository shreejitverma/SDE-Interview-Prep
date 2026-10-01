#!/bin/bash
set -e

echo "--- fio fsync latency test ---"
echo "Motivating RioVista: standard RVM pays high fsync overhead."

fio --name=fsync_test \
    --ioengine=sync \
    --rw=randwrite \
    --bs=1k \
    --numjobs=1 \
    --size=1M \
    --fdatasync=1 \
    --time_based \
    --runtime=2 \
    | grep -E "fsync/fdatasync|lat.*usec|lat.*msec"

echo "PASS: fio_fsync"
