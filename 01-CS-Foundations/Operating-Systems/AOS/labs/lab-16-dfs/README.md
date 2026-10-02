---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lessons: [L07c]
environment: "Lima VM aos (Ubuntu 24.04 arm64)"
tags: [cs6210, cs6210/lab]
---

# lab-16-dfs: Distributed file system ideas: RAID parity, log-structured writes, and a FUSE cache

> [!info] Goal
> Make L07c concrete with real commands and measurements.

See [setup](../setup/README.md) for the VM.

## Prerequisites
- A Linux environment with Python 3.12, FUSE 3, and a C compiler (`gcc`).
- `libfuse3-dev` installed for compiling the FUSE daemon.

## Run commands

Inside the VM, from the `AOS/labs/lab-16-dfs` directory:

```sh
make run
make test
```

## What you should see

The output demonstrates three major concepts from distributed file systems: RAID-5 parity, a log-structured store, and a FUSE write-back cache.

For RAID-5, you should see the input written as stripes over 5 disks with rotating parity, taking well under 1 ms:
```
[RAID] Wrote 19 bytes as 2 stripes over 5 disks, parity rotating.
[RAID] Parity computation took 0.01 ms
...
[RAID] Reconstructed disk 1 from the other 4 disks.
[RAID] Read 19 bytes from 4 data blocks per stripe into raid_output.txt.
Hello RAID-5 World
```

For the log-structured store (LFS), notice how overwriting an object merely appends a new entry, updating the index but leaving garbage behind until `clean` runs:
```
[LFS] Appended obj 1 (len 26) at offset 62
[LFS] Cleaned log. Size reduced from 100 to 63 bytes.
```

For the FUSE write-back cache, a process writes to the mount, but the data is held in user-space memory until the file is closed or `fusermount3 -u mnt` is called:
```
Checking backing store directly (should be empty or not exist):
Not flushed yet
File closed. Checking backing store again:
AAAAAAAAAA
```

## How it works

1. **RAID-5 Simulator (`raid5.py`)**: Data is split into stripes of 4 data blocks plus 1 parity block (the XOR of the 4).
   The parity block rotates across the 5 disks from stripe to stripe, which is what distinguishes RAID-5 from RAID-4's dedicated parity disk and spreads small-write parity updates over all disks.
   Any one lost disk is rebuilt by XORing the same stripe on the other four; an 8-byte length header on every disk lets reads drop the stripe padding.
2. **Log-Structured Store (`lfs.py`)**: Instead of updating files in place (which incurs seek penalties), all writes are appended continuously to a log. An in-memory index maps object IDs to their current byte offset and length in the log file. Old versions become stale space. The `clean` process acts as a garbage collector, copying only the live data (referenced by the index) into a new, compacted log.
3. **FUSE Write-Back Cache (`fuse_cache.c`)**: Filesystem operations are intercepted by `libfuse3`. The write operation buffers data in an internal memory array and returns success immediately without touching the backing disk. The data is only flushed to the backing storage file when `flush` or `release` is called, simulating how a client in a distributed system might batch operations before communicating with the central server.

## Experiments

1. **Increase RAID data size**:
   - **Prediction**: If you write a 10 MB file to the RAID simulator, how long will parity computation take using pure Python loops?
   - **Action**: Modify the Makefile to generate a large input file (`head -c 10M </dev/urandom > raid_input.txt`) and run `make run`. Observe the `[RAID] Parity computation took ...` line.
2. **Observe LFS cleaning performance**:
   - **Prediction**: If you append 1000 small updates to the same object ID, how much space will the `clean` command reclaim?
   - **Action**: Use a shell loop to run `./lfs.py store.log write 1 "data"` multiple times, then execute `./lfs.py store.log clean`. Compare the log size reduction in the output.
3. **Interrupting a write-back cache**:
   - **Prediction**: What happens to your data if the FUSE daemon process (`fuse_cache`) is killed with `SIGKILL` (using `kill -9`) before the writer closes the file?
   - **Action**: Modify the `Makefile` to find the FUSE PID and `kill -9` it during the `sleep 2` window while the python writer has the file open. Check if the backing store contains the data.

## Questions

<details>
<summary>Why does the xFS distributed file system use stripe groups instead of striping across all servers (like Zebra)?</summary>

Striping across all nodes in a massive cluster results in extremely small data fragments for each block, leading to high metadata overhead and network inefficiencies. Stripe groups limit parity operations to a subset of nodes, ensuring fragment sizes remain large enough to utilize network bandwidth efficiently while maintaining high availability.
</details>

<details>
<summary>In the FUSE write-back cache, what is the risk of deferring the flush to the backing store?</summary>

Deferring the flush improves write latency for the client, but it introduces a window of vulnerability. If the client machine crashes or the FUSE daemon terminates unexpectedly before the flush occurs, any data in the write-back cache is permanently lost, violating strict consistency guarantees.
</details>

<details>
<summary>How does a log-structured store improve write performance, and what is its primary trade-off?</summary>

It improves write performance by transforming all scattered random writes into sequential appends, maximizing disk throughput. The primary trade-off is the necessity of a garbage collection (cleaning) process, which must periodically read the log to consolidate live data and reclaim space from obsolete entries, consuming CPU and I/O bandwidth.
</details>
