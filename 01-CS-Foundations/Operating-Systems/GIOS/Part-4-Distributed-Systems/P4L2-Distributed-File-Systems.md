---
type: concept
track: [sde]
level:
status: solid
last_reviewed:
sources:
  - "Georgia Tech CS 6200 P4L2"
  - "The Andrew File System (AFS), Morris et al. (1986)"
  - "The Google File System, Ghemawat et al. (SOSP 2003)"
  - "NFS Version 4 Protocol, RFC 7530"
  - "Hadoop Distributed File System (HDFS) Architecture Guide"
---

# P4L2: Distributed File Systems

## Table of Contents
1. [DFS Concepts and Goals](#1-dfs-concepts-and-goals)
2. [DFS Models](#2-dfs-models)
3. [Caching and Consistency](#3-caching-and-consistency)
4. [NFS (Network File System)](#4-nfs-network-file-system)
5. [AFS (Andrew File System)](#5-afs-andrew-file-system)
6. [GFS (Google File System)](#6-gfs-google-file-system)
7. [HDFS (Hadoop Distributed File System)](#7-hdfs-hadoop-distributed-file-system)
8. [Modern DFS: Ceph](#8-modern-dfs-ceph)
9. [Windows Distributed File Systems](#9-windows-distributed-file-systems)
10. [Replication and Fault Tolerance](#10-replication-and-fault-tolerance)
11. [Performance Analysis](#11-performance-analysis)
12. [Quizzes and Exercises](#12-quizzes-and-exercises)
13. [Cross-Platform Distributed File Systems: Linux, macOS, and Windows](#13-cross-platform-distributed-file-systems-linux-macos-and-windows)

---

## 1. DFS Concepts and Goals

A **Distributed File System (DFS)** presents a single, unified filesystem namespace to clients while the actual data is spread across multiple servers (and possibly multiple data centers).

```
Without DFS:                         With DFS:
Client A sees:  /home/userA          All clients see:  /
Client B sees:  /home/userB                            ├── home/
(no sharing!)                                          │   ├── userA/  (on server1)
                                                       │   └── userB/  (on server2)
                                                       ├── data/        (on server3)
                                                       └── logs/        (on server4)
```

### Design Goals

| Goal | Description | Trade-off |
|------|-------------|-----------|
| **Transparency** | Clients unaware of distribution | Performance cost |
| **Concurrency** | Multiple clients access same files | Consistency complexity |
| **Replication** | Data on multiple servers | Consistency cost |
| **Heterogeneity** | Different OSes/hardware | Interop cost |
| **Fault tolerance** | Server failures invisible to clients | State management |
| **Consistency** | All clients see same view | Latency cost |
| **Security** | Access control, encryption | Management cost |
| **Scalability** | Add servers to handle more load | Complexity |

### File Access Patterns (What DFS Must Optimize)

SAIC study on DFS workloads:
```
File size distribution:
  < 1 KB:     64% of files (but only 0.5% of data)
  1–10 KB:    18% of files
  10–100 KB:  11% of files
  > 1 MB:      3% of files (but 90% of data)

Access patterns:
  Sequential reads/writes: 80%+ of all I/O
  Random access:           < 20%
  Read-mostly:             ~ 70% of all operations

Implication: Optimize for sequential large-file reads (streaming)
             Keep metadata operations fast (many small files)
```

---

## 2. DFS Models

### Upload/Download Model

Client downloads entire file, modifies locally, uploads back:

```
Client          Server
  │   ──GET──►  │  Server sends complete file
  │   ◄──FILE─  │
  │   [modify]  │  Client modifies locally
  │   ──PUT──►  │  Client uploads complete file back
  │   ◄──OK───  │

Pros: Simple, works offline, good for reads
Cons: Bandwidth waste (download whole file for small edit)
      Consistency issues (two clients download, both modify)
```

### Remote Access Model

Operations executed on server, client sees changes immediately:

```
Client              Server
  │  ──open()──►   │
  │  ◄──fd─────   │
  │  ──read(fd)──► │  Server reads from disk
  │  ◄──data────   │
  │  ──write()──►  │  Server writes to disk immediately
  │  ◄──ok──────   │

Pros: Consistent (single source of truth)
      Minimal bandwidth (only data needed)
Cons: Every op goes over network (high latency)
      Server is bottleneck
```

### Hybrid Model (NFS/AFS style)

Cache blocks locally, write-through or write-back with consistency protocol:

```
Client                       Server
  │                          │
  │  [cache miss: block 5]   │
  │  ──read(block 5)──────►  │
  │  ◄──block 5 + token───   │
  │  [cache hit: block 5]    │  Next read served from cache
  │                          │
  │  [write to block 5]      │
  │  ──write(block 5)──────► │  Write-through: server updated
  │  ◄──ok──────────────────  │
  │  [invalidate tokens]────────► Other clients notified
```

---

## 3. Caching and Consistency

### Cache Coherence Problem

```
Time → 

Client A:  read(x)→5   write(x,10)          read(x)→?
                        │
Client B:  read(x)→5              read(x)→?
                                  ▲
                                  │
                             Inconsistency window!
                             B sees stale 5 or fresh 10?
```

### Consistency Models

| Model | Definition | Example |
|-------|------------|---------|
| **Strict** | Read always returns most recent write | Impossible without global clock |
| **Sequential** | All see writes in same order | Costly, requires coordination |
| **Causal** | Causally-related writes seen in order | Lamport clocks |
| **Session** | Client sees its own writes | NFS default |
| **Eventual** | Will converge if no new writes | DNS, many NoSQL DBs |

### NFS Consistency (Close-to-Open)

NFS v3 uses **close-to-open consistency**: when you open a file, you see the server's current version. When you close, your changes are flushed to server.

```c
/* nfs_consistency_demo.c - Illustrate NFS close-to-open semantics */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

/*
 * On NFS-mounted filesystem:
 *
 * Client A (writer):           Client B (reader):
 * 
 *   fd = open("f", O_WRONLY)
 *   write(fd, "hello", 5)
 *                               fd = open("f", O_RDONLY)
 *                               read(fd, buf, 5)  ← may see old data!
 *   close(fd)  ← flush to server
 *                               close(fd)
 *                               fd = open("f", O_RDONLY)
 *                               read(fd, buf, 5)  ← NOW sees "hello"
 */

void demonstrate_nfs_semantics(const char *nfs_path) {
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/test_consistency.txt", nfs_path);

    /* Writer */
    int fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    write(fd, "updated content", 15);
    close(fd);  /* This flush guarantees server has latest version */
    printf("Writer: closed file (data now on server)\n");

    /* Reader - after open() will see latest version */
    fd = open(filepath, O_RDONLY);
    char buf[64] = {0};
    read(fd, buf, 63);
    printf("Reader after open: '%s'\n", buf);
    close(fd);
}
```

```bash
# Check NFS mount cache settings
mount | grep nfs
# server:/share on /mnt/nfs type nfs4 (rw,relatime,vers=4.1,
#    rsize=1048576,wsize=1048576,namlen=255,
#    hard,proto=tcp,timeo=600,retrans=2,
#    sec=sys,clientaddr=192.168.1.10,
#    local_lock=none,addr=192.168.1.1)

# NFS mount options that affect caching:
#   actimeo=N    - attribute cache timeout (default: 3-60s for files, 30-60s for dirs)
#   noac         - disable attribute caching (strict consistency, slow!)
#   sync         - synchronous writes (vs async, default)

# Mount with strict consistency (no attribute caching):
sudo mount -t nfs4 -o noac server:/share /mnt/strict

# Mount with aggressive caching (high performance, weaker consistency):
sudo mount -t nfs4 -o actimeo=60 server:/share /mnt/cached

# Show NFS client statistics
nfsstat -c
cat /proc/net/rpc/nfs
# Shows: reads, writes, cache hits/misses, errors

# Watch NFS I/O in real time
iostat -x 1 | grep nfs
nfsiostat 1  # (part of nfs-utils)
```

---

## 4. NFS (Network File System)

NFS was developed by Sun Microsystems in 1984. Now at version 4.2 (RFC 7862).

### NFS Architecture

```
┌──────────────────────────────────────────────────┐
│                CLIENT                            │
│  ┌────────────┐   ┌──────────┐  ┌─────────────┐ │
│  │ User App   │   │  VFS     │  │ Page Cache  │ │
│  │ open/read/ │──►│ (Virtual │──►│ (block      │ │
│  │ write/stat │   │  File    │  │  cache)     │ │
│  └────────────┘   │  System) │  └─────────────┘ │
│                   └────┬─────┘         │         │
│                        │               │         │
│               ┌────────▼──────────┐    │         │
│               │   NFS Client      │    │         │
│               │   (nfs.ko kernel  │◄───┘         │
│               │    module)        │              │
│               └────────┬──────────┘              │
└────────────────────────┼───────────────────────── ┘
                         │  RPC (Sun RPC / RDMA)
                         │
┌────────────────────────┼────────────────────────── ┐
│                SERVER  ▼                           │
│               ┌────────────────┐                  │
│               │  NFS Server    │                  │
│               │  (nfsd kernel  │                  │
│               │   threads)     │                  │
│               └────────┬───────┘                  │
│                        │                          │
│               ┌────────▼───────┐                  │
│               │  Local FS      │                  │
│               │  (ext4/xfs/zfs)│                  │
│               └────────────────┘                  │
└────────────────────────────────────────────────── ┘
```

### NFS File Handles

NFS uses **file handles** (opaque identifiers) instead of file descriptors. File handles survive server restarts.

```
NFS v3 file handle (up to 64 bytes):
  [fsid (8 bytes)][inode number (4 bytes)][generation number (4 bytes)]

NFS v4 file handle: variable length, opaque to client
```

### NFS Server Setup (Linux)

```bash
# ── SERVER SETUP ──────────────────────────────────────────────────

# Install NFS server
sudo apt-get install nfs-kernel-server

# Create export directory
sudo mkdir -p /srv/nfs/shared
sudo chown nobody:nogroup /srv/nfs/shared
sudo chmod 777 /srv/nfs/shared

# Configure exports
sudo tee /etc/exports << 'EOF'
# Syntax: directory  client(options)
#
# Export /srv/nfs/shared to 192.168.1.0/24 network
/srv/nfs/shared  192.168.1.0/24(rw,sync,no_subtree_check,no_root_squash)

# Read-only export with root squash (security best practice)
/srv/data/public  *(ro,sync,no_subtree_check,root_squash)

# Export to specific hosts
/srv/private  192.168.1.10(rw,sync) 192.168.1.11(ro,sync)
EOF

# Export options explained:
#   rw              - read/write access
#   ro              - read-only
#   sync            - writes to disk before ACK (safe, slower)
#   async           - buffer writes (faster, data loss on crash)
#   no_subtree_check - don't verify file is in exported subtree (performance)
#   no_root_squash  - root on client = root on server (dangerous!)
#   root_squash     - root on client → nobody:nogroup on server (safe)
#   all_squash      - ALL users mapped to nobody:nogroup
#   anonuid=1000    - UID for squashed users
#   anongid=1000    - GID for squashed users
#   fsid=0          - root of NFS v4 namespace

# Apply export changes (without restart)
sudo exportfs -ra

# Show current exports
sudo exportfs -v
# /srv/nfs/shared  192.168.1.0/24(sync,wdelay,hide,no_subtree_check,sec=sys,
#   rw,secure,no_root_squash,no_all_squash)

# Start and enable NFS server
sudo systemctl enable --now nfs-kernel-server
sudo systemctl status nfs-kernel-server

# Check NFS server is listening
ss -tlnp | grep :2049  # NFS
ss -tlnp | grep :111   # portmapper/rpcbind

# View NFS server stats
cat /proc/net/rpc/nfsd
nfsstat -s  # server stats

# Set number of NFS server threads (tune for load)
echo 16 > /proc/fs/nfsd/threads
# Or set permanently in /etc/default/nfs-kernel-server:
# RPCNFSDCOUNT=16
```

### NFS Client Setup (Linux)

```bash
# ── CLIENT SETUP ──────────────────────────────────────────────────

# Install NFS client
sudo apt-get install nfs-common

# Show available NFS exports from server
showmount -e 192.168.1.1
# Export list for 192.168.1.1:
# /srv/nfs/shared 192.168.1.0/24

# Mount NFS share
sudo mkdir -p /mnt/nfs
sudo mount -t nfs4 192.168.1.1:/srv/nfs/shared /mnt/nfs

# Mount with options
sudo mount -t nfs4 \
    -o rw,hard,intr,rsize=1048576,wsize=1048576,timeo=14,retrans=3 \
    192.168.1.1:/srv/nfs/shared /mnt/nfs

# Mount options:
#   hard    - retries forever on failure (vs soft = return error)
#   intr    - allow SIGINT to interrupt hung NFS call
#   rsize   - read buffer size (max: 1MB, tune to MTU)
#   wsize   - write buffer size
#   timeo   - timeout in 0.1s units (14 = 1.4s)
#   retrans - number of retries before error
#   vers=4.1 - force NFS version 4.1 (with pNFS support)
#   proto=rdma - use RDMA transport (requires RDMA hardware)

# Persistent mount via /etc/fstab
echo "192.168.1.1:/srv/nfs/shared /mnt/nfs nfs4 rw,hard,intr,_netdev 0 0" \
    | sudo tee -a /etc/fstab

# Verify mount
df -h /mnt/nfs
stat /mnt/nfs  # Shows NFS file attributes

# Unmount
sudo umount /mnt/nfs

# ── MONITORING ──────────────────────────────────────────────────

# Client-side NFS statistics
nfsstat -c
cat /proc/net/rpc/nfs

# Watch NFS performance
nfsiostat -h 1  # human-readable, every 1 second

# strace an NFS operation to see what happens
strace -e trace=network,read,write cat /mnt/nfs/bigfile 2>&1 | head -50

# Capture NFS traffic
sudo tcpdump -i eth0 port 2049 -w nfs_capture.pcap
# Analyze with Wireshark: nfs_capture.pcap → follow TCP stream
```

### NFS v4 vs v3

```
NFS v3:
  - Stateless server (server doesn't track open files)
  - Client responsible for lock management (NLM protocol separate)
  - UDP or TCP transport
  - Uses separate portmapper, mountd, lockd daemons

NFS v4:
  - Stateful (server tracks opens, locks, leases)
  - Integrated locking (no separate NLM)
  - TCP only (more reliable)
  - Single port 2049 (firewall-friendly)
  - Compound RPCs (multiple ops in one network roundtrip)
  - Strong security (RPCSEC_GSS / Kerberos)
  - ACLs (NFSv4 ACLs similar to Windows ACLs)

NFS v4.1:
  - pNFS (parallel NFS): clients access data servers directly
  - Sessions with exactly-once semantics
  - Better HA/failover

NFS v4.2 (RFC 7862):
  - Server-side copy (no data on wire for cp)
  - Sparse file support
  - Application Data Block (ADB)
```

```bash
# Use NFS v4.1 with pNFS for parallel data access
sudo mount -t nfs4 -o vers=4.1 server:/share /mnt/nfs

# Check NFS version negotiated
cat /proc/mounts | grep nfs
# Verify: vers=4.1 or vers=4.2 in options

# NFS v4 ACLs
nfs4_getfacl /mnt/nfs/myfile
nfs4_setfacl -a A::user@domain:rw /mnt/nfs/myfile

# Kerberos-secured NFS
# Client must have valid Kerberos ticket
kinit username@REALM.COM
# Mount with Kerberos
sudo mount -t nfs4 -o sec=krb5 server:/share /mnt/secure
#   sec=krb5      - Kerberos auth only
#   sec=krb5i     - Kerberos + data integrity
#   sec=krb5p     - Kerberos + encryption (slowest, most secure)
```

---

## 5. AFS (Andrew File System)

AFS was developed at CMU (1980s) and designed specifically to scale to 5,000+ clients, far beyond NFS's capabilities.

### AFS Key Innovations

```
NFS (stateless):
  Client polls server every ~3 seconds to check if cached file is stale
  → O(clients × files) server load

AFS (callback-based):
  Server promises to NOTIFY client if file changes ("callback")
  → Server only contacts clients when data changes
  → O(modifying clients) server load - much more scalable!
```

```
AFS Callback Mechanism:
                                                           
  Client A opens /afs/cmu/file.txt                        
  ┌────────┐   ──fetch_and_give_me_callback(file.txt)──►  ┌────────┐
  │Client A│   ◄──file_data + callback_promise──────────  │ Server │
  │        │   [caches file locally]                      │        │
  └────────┘                                              └────────┘
                                                               │
  Client B writes to file.txt                                  │
  ┌────────┐   ──store(file.txt, new_data)──────────────────►  │
  │Client B│   ◄──ok───────────────────────────────────────   │
  └────────┘                                              ┌────▼───┐
                                                          │ Server │
  ──callback_break(file.txt)──────────────────────────────►        │
  [Client A's cache is now invalid!]                     └────────┘
  ┌────────┐
  │Client A│ [next access: must re-fetch from server]
  └────────┘
```

### OpenAFS Setup (Linux)

```bash
# Install OpenAFS client
sudo apt-get install openafs-client openafs-krb5

# Configure AFS cell
echo "cs.cmu.edu" | sudo tee /etc/openafs/ThisCell
echo "128.2.10.2 cs.cmu.edu" | sudo tee /etc/openafs/CellServDB

# Start AFS client
sudo systemctl start openafs-client

# Access AFS namespace
ls /afs/cs.cmu.edu/  # Browse CMU's AFS

# AFS authentication
kinit user@CS.CMU.EDU
aklog  # Get AFS token

# List tokens
tokens

# Check cache statistics
/usr/sbin/afsd -stat 300 -dcache 100 -volumes 50 -daemons 3

# AFS-specific commands
fs whereis /afs/cs.cmu.edu/user/mydir  # Which server stores this?
fs checkvolumes              # Refresh volume location cache
fs getcellstatus cs.cmu.edu  # Show cell connection status
vos listvol server1          # List volumes on a server
```

---

## 6. GFS (Google File System)

GFS (2003) was designed for Google's workload: massive files, append-heavy writes, commodity hardware with frequent failures.

### GFS Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                        GFS CLUSTER                             │
│                                                                │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │                    MASTER (single)                       │  │
│  │  - Namespace tree                                        │  │
│  │  - File → chunk ID mapping                               │  │
│  │  - Chunk → chunkserver location mapping                  │  │
│  │  - Chunk lease management                                │  │
│  │  - Garbage collection                                    │  │
│  │  - Chunk migration (load balancing)                      │  │
│  └───────────┬──────────────────────────┬────────────────── ┘  │
│              │ metadata ops             │ heartbeat/report     │
│  ┌───────────▼──┐  ┌───────────────┐  ┌▼──────────────────┐   │
│  │ Chunkserver  │  │ Chunkserver   │  │  Chunkserver      │   │
│  │     A        │  │      B        │  │       C           │   │
│  │  chunk 1 ✓   │  │  chunk 1 ✓   │  │   chunk 1 ✓       │   │
│  │  chunk 2 ✓   │  │  chunk 3 ✓   │  │   chunk 2 ✓       │   │
│  └──────────────┘  └───────────────┘  └───────────────────┘   │
│       ▲                  ▲                    ▲                 │
│       └─────────── direct data reads/writes ──┘                 │
│                          ▲                                      │
└──────────────────────────┼──────────────────────────────────── ┘
                           │
                    ┌──────┴──────┐
                    │   CLIENT    │
                    │  GFS lib    │
                    └─────────────┘
```

### GFS Design Decisions

| Decision | Why | Trade-off |
|----------|-----|-----------|
| Large chunk size (64 MB) | Fewer master ops, efficient streaming | Wasted space for small files |
| Single master | Simple consistency, no distributed lock | SPOF (mitigated by shadow masters) |
| Relaxed consistency | Better performance for appends | Apps must handle duplicates |
| Append-optimized | Google's workload is mostly appends | Random writes are slow |
| No client-side caching | Simplicity, large streaming reads | Latency for repeated reads |

### GFS Read Operation

```python
# gfs_client_simulation.py - Simulate GFS client read logic
import random
import time
from dataclasses import dataclass, field
from typing import List, Dict, Tuple

CHUNK_SIZE = 64 * 1024 * 1024  # 64 MB

@dataclass
class ChunkLocation:
    chunk_id: int
    chunkservers: List[str]   # 3 replicas
    version: int

@dataclass
class GFSMaster:
    """Simplified GFS master - holds metadata only."""
    file_chunks: Dict[str, List[int]] = field(default_factory=dict)
    chunk_locations: Dict[int, ChunkLocation] = field(default_factory=dict)
    next_chunk_id: int = 1000

    def get_chunk_location(self, filename: str, chunk_index: int
                           ) -> ChunkLocation:
        """Client calls this to find which chunkserver holds a chunk."""
        chunk_ids = self.file_chunks.get(filename, [])
        if chunk_index >= len(chunk_ids):
            raise FileNotFoundError(f"{filename}[chunk {chunk_index}]")
        return self.chunk_locations[chunk_ids[chunk_index]]

    def create_file(self, filename: str, num_chunks: int):
        chunk_ids = []
        servers = ["server-A", "server-B", "server-C", "server-D"]
        for i in range(num_chunks):
            cid = self.next_chunk_id
            self.next_chunk_id += 1
            # Pick 3 random servers for replication
            replicas = random.sample(servers, 3)
            self.chunk_locations[cid] = ChunkLocation(
                chunk_id=cid, chunkservers=replicas, version=1
            )
            chunk_ids.append(cid)
        self.file_chunks[filename] = chunk_ids
        return chunk_ids


class GFSClient:
    def __init__(self, master: GFSMaster):
        self.master = master
        self.chunk_cache: Dict[str, Tuple[ChunkLocation, float]] = {}
        self.CACHE_TTL = 60.0  # seconds

    def read(self, filename: str, offset: int, length: int) -> bytes:
        """
        GFS read algorithm:
        1. Calculate which chunks are needed
        2. Contact master for chunk locations (cached!)
        3. Read from nearest chunkserver
        """
        result = b""
        chunk_index = offset // CHUNK_SIZE
        chunk_offset = offset % CHUNK_SIZE
        remaining = length

        while remaining > 0:
            # Step 1: Get chunk location from master (or cache)
            cache_key = f"{filename}:{chunk_index}"
            cached = self.chunk_cache.get(cache_key)
            if cached and (time.time() - cached[1]) < self.CACHE_TTL:
                location = cached[0]
                print(f"  Cache hit for {cache_key}")
            else:
                location = self.master.get_chunk_location(filename, chunk_index)
                self.chunk_cache[cache_key] = (location, time.time())
                print(f"  Cache miss → asked master for {cache_key}")

            # Step 2: Read from closest chunkserver (simplified: pick first)
            server = location.chunkservers[0]
            read_size = min(remaining, CHUNK_SIZE - chunk_offset)
            print(f"  Reading {read_size} bytes from chunk {location.chunk_id} "
                  f"on {server} (offset={chunk_offset})")

            # In reality: TCP connection to chunkserver, read data
            result += bytes(read_size)  # Simulated data

            remaining -= read_size
            chunk_offset = 0
            chunk_index += 1

        return result


# Demo
master = GFSMaster()
# Create a 200MB file (4 chunks of 64MB minus last partial)
master.create_file("/bigdata/logs/2024-01-01.log", 4)

client = GFSClient(master)
print("=== Reading 100MB starting at byte 0 ===")
data = client.read("/bigdata/logs/2024-01-01.log", 0, 100*1024*1024)
print(f"Got {len(data)} bytes")

print("\n=== Reading again (should hit cache) ===")
data = client.read("/bigdata/logs/2024-01-01.log", 0, 1024)
```

### GFS Append Operation (Atomic Record Append)

```
GFS Atomic Record Append (key feature):

Client sends: append "record" to /logs/stream

Master grants lease to PRIMARY chunkserver for chunk X

Client sends data to ALL replicas (pipeline):
  Client ──data──► Server-A ──data──► Server-B ──data──► Server-C
  (All servers buffer in memory)

Client sends WRITE command to PRIMARY:
  Primary picks offset, validates size fits in chunk
  Primary sends APPLY(offset) to all secondaries
  All secondaries apply at SAME offset
  Secondaries reply to primary: OK
  Primary replies to client: SUCCESS (offset)

Result: Record atomically appended, all replicas at same offset
```

---

## 7. HDFS (Hadoop Distributed File System)

HDFS is an open-source implementation inspired by GFS, optimized for MapReduce workloads.

### HDFS Setup and Usage

```bash
# ── INSTALLATION ──────────────────────────────────────────────────

# Download Hadoop
wget https://downloads.apache.org/hadoop/common/hadoop-3.3.6/hadoop-3.3.6.tar.gz
tar -xzf hadoop-3.3.6.tar.gz -C /opt/
sudo ln -s /opt/hadoop-3.3.6 /opt/hadoop

# Set environment variables
export HADOOP_HOME=/opt/hadoop
export PATH=$PATH:$HADOOP_HOME/bin:$HADOOP_HOME/sbin
export JAVA_HOME=/usr/lib/jvm/java-11-openjdk-amd64

# Configure HDFS (core-site.xml)
cat > $HADOOP_HOME/etc/hadoop/core-site.xml << 'EOF'
<configuration>
  <property>
    <name>fs.defaultFS</name>
    <value>hdfs://namenode:9000</value>
  </property>
  <property>
    <name>hadoop.tmp.dir</name>
    <value>/data/hadoop/tmp</value>
  </property>
</configuration>
EOF

# Configure HDFS (hdfs-site.xml)
cat > $HADOOP_HOME/etc/hadoop/hdfs-site.xml << 'EOF'
<configuration>
  <!-- Replication factor (default 3) -->
  <property>
    <name>dfs.replication</name>
    <value>3</value>
  </property>
  <!-- Block size: 128MB (vs GFS's 64MB) -->
  <property>
    <name>dfs.blocksize</name>
    <value>134217728</value>
  </property>
  <!-- NameNode storage -->
  <property>
    <name>dfs.namenode.name.dir</name>
    <value>/data/hdfs/namenode</value>
  </property>
  <!-- DataNode storage -->
  <property>
    <name>dfs.datanode.data.dir</name>
    <value>/data/hdfs/datanode</value>
  </property>
</configuration>
EOF

# Format NameNode (first time only!)
hdfs namenode -format

# Start HDFS
start-dfs.sh
# Or individually:
# hdfs --daemon start namenode
# hdfs --daemon start datanode

# Verify cluster is up
hdfs dfsadmin -report
# Shows: Live datanodes, capacity, used space, etc.

# ── HDFS OPERATIONS ───────────────────────────────────────────────

# List files (like ls -la)
hdfs dfs -ls /
hdfs dfs -ls -R /user/

# Create directory
hdfs dfs -mkdir -p /user/myuser/data

# Upload local file to HDFS
hdfs dfs -put /local/path/large_file.csv /user/myuser/data/
# Or copy preserving attributes:
hdfs dfs -copyFromLocal /local/file.txt /hdfs/path/

# Download from HDFS
hdfs dfs -get /hdfs/path/file.txt /local/path/
hdfs dfs -copyToLocal /hdfs/file.txt ./

# Read file content
hdfs dfs -cat /user/myuser/data/file.txt
hdfs dfs -tail /user/myuser/data/logfile.txt  # last 1KB

# Move/rename
hdfs dfs -mv /hdfs/old_path /hdfs/new_path

# Delete
hdfs dfs -rm /hdfs/file.txt
hdfs dfs -rm -r /hdfs/directory/     # recursive
hdfs dfs -rm -r -skipTrash /hdfs/dir/  # bypass trash

# Check disk usage
hdfs dfs -du -h /user/myuser/
hdfs dfs -df -h /  # filesystem summary

# ── BLOCK INSPECTION ──────────────────────────────────────────────

# Show block locations for a file
hdfs fsck /user/myuser/data/large_file.csv -files -blocks -locations
# Output:
# /user/myuser/data/large_file.csv:
#   Under-replicated blocks: 0
#   Corrupt blocks: 0
#   Missing blocks: 0
# Total blocks: 8 (8 replicated)
# Block 1: 134217728 bytes, 3 replicas
#   datanode1.local:50010, datanode2.local:50010, datanode3.local:50010

# Full filesystem check
hdfs fsck / -files -blocks

# ── SNAPSHOTS (for backups) ────────────────────────────────────────

# Enable snapshots on directory
hdfs dfsadmin -allowSnapshot /user/myuser/data

# Create snapshot
hdfs dfs -createSnapshot /user/myuser/data backup-2024-01-01

# List snapshots
hdfs dfs -ls /user/myuser/data/.snapshot/

# Compare snapshots
hdfs snapshotDiff /user/myuser/data backup-2024-01-01 .  # vs current

# Restore from snapshot
hdfs dfs -cp /user/myuser/data/.snapshot/backup-2024-01-01/file.txt \
             /user/myuser/data/file.txt

# ── HDFS JAVA API ─────────────────────────────────────────────────
```

```java
// HDFSDemo.java - Java API for HDFS
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.*;
import java.io.*;
import java.net.URI;

public class HDFSDemo {
    public static void main(String[] args) throws Exception {
        Configuration conf = new Configuration();
        conf.set("fs.defaultFS", "hdfs://namenode:9000");

        FileSystem fs = FileSystem.get(new URI("hdfs://namenode:9000"), conf, "hdfs");

        // ── Write to HDFS ────────────────────────────────────────
        Path outputPath = new Path("/user/demo/output.txt");
        try (FSDataOutputStream out = fs.create(outputPath,
                                                 true,    // overwrite
                                                 128*1024, // buffer size
                                                 (short)3, // replication
                                                 128*1024*1024L)) { // block size
            for (int i = 0; i < 1_000_000; i++) {
                out.writeBytes("Line " + i + ": Hello HDFS!\n");
            }
        }
        System.out.println("Wrote to HDFS: " + outputPath);

        // ── Read from HDFS ───────────────────────────────────────
        try (FSDataInputStream in = fs.open(outputPath)) {
            BufferedReader br = new BufferedReader(new InputStreamReader(in));
            String line;
            int count = 0;
            while ((line = br.readLine()) != null && count++ < 5) {
                System.out.println(line);
            }
        }

        // ── File metadata ────────────────────────────────────────
        FileStatus status = fs.getFileStatus(outputPath);
        System.out.printf("Size: %d bytes, Replication: %d, Block size: %d%n",
            status.getLen(), status.getReplication(), status.getBlockSize());

        // ── Block locations ──────────────────────────────────────
        BlockLocation[] locations = fs.getFileBlockLocations(outputPath, 0, status.getLen());
        for (BlockLocation loc : locations) {
            System.out.printf("Block [%d-%d] on: %s%n",
                loc.getOffset(), loc.getOffset() + loc.getLength(),
                String.join(", ", loc.getHosts()));
        }

        fs.close();
    }
}
```

```bash
# Compile and run HDFS demo
javac -classpath $(hadoop classpath) HDFSDemo.java
java -classpath $(hadoop classpath):. HDFSDemo

# Python API for HDFS (via WebHDFS REST or pyarrow)
pip install pyarrow fsspec

python3 << 'EOF'
import pyarrow.hdfs as hdfs

# Connect to HDFS
fs = hdfs.connect(host='namenode', port=8020, user='hdfs')

# Write
with fs.open('/user/demo/pytest.txt', 'wb') as f:
    f.write(b"Hello from Python!\n" * 1000)

# Read
with fs.open('/user/demo/pytest.txt', 'rb') as f:
    data = f.read(100)
    print(data)

# List
files = fs.ls('/user/demo/')
for f in files:
    print(f['name'], f['size'])
EOF
```

---

## 8. Modern DFS: Ceph

Ceph is a modern, production-grade DFS that provides object, block, and file storage.

```bash
# ── CEPH SETUP (minimal test cluster) ─────────────────────────────

# Install ceph tools
sudo apt-get install ceph-common ceph-fuse

# Deploy with cephadm (modern approach)
sudo apt-get install python3 cephadm
sudo cephadm bootstrap --mon-ip 192.168.1.1

# Check cluster health
ceph status
# cluster:
#   id:     abc123
#   health: HEALTH_OK
# services:
#   mon: 3 daemons, quorum mon1,mon2,mon3
#   mgr: mon1(active)
#   osd: 6 osds: 6 up, 6 in
# data:
#   pools:   3 pools, 96 PGs
#   objects: 21 objects, 2.1 KiB
#   usage:   6.0 GiB used, 294 GiB / 300 GiB avail

# ── CEPH FILE SYSTEM ──────────────────────────────────────────────

# Create CephFS metadata and data pools
ceph osd pool create cephfs_data 32
ceph osd pool create cephfs_metadata 32
ceph fs new myfs cephfs_metadata cephfs_data

# Check fs status
ceph fs status

# Mount CephFS (kernel driver - fastest)
sudo mkdir /mnt/cephfs
sudo mount -t ceph mon1:6789,mon2:6789:/ /mnt/cephfs \
    -o name=admin,secret=$(ceph auth get-key client.admin)

# Mount via FUSE (user-space, more portable)
sudo ceph-fuse /mnt/cephfs -m mon1:6789

# Use it like a regular filesystem
ls /mnt/cephfs
cp /local/file.txt /mnt/cephfs/
cat /mnt/cephfs/file.txt

# ── RADOS OBJECT STORAGE ──────────────────────────────────────────

# Create a pool
ceph osd pool create mypool 32
ceph osd pool application enable mypool rgw

# Store/retrieve objects with rados
rados -p mypool put myobject /local/file.txt
rados -p mypool get myobject /tmp/retrieved.txt
rados -p mypool ls  # list objects

# ── CEPH PERFORMANCE TUNING ───────────────────────────────────────

# Benchmark raw OSD throughput
rados bench -p mypool 60 write --no-cleanup
rados bench -p mypool 60 seq  # sequential read
rados bench -p mypool 60 rand # random read

# Benchmark CephFS
fio --name=cephfs-test --ioengine=libaio --direct=1 \
    --rw=read --bs=1M --size=4G --numjobs=4 \
    --filename=/mnt/cephfs/testfile
```

---

## 9. Windows Distributed File Systems

### SMB/CIFS (Server Message Block)

```powershell
# ── WINDOWS FILE SHARING (SMB) ────────────────────────────────────

# Share a folder
New-SmbShare -Name "SharedDocs" -Path "C:\SharedDocuments" `
    -FullAccess "DOMAIN\AdminGroup" `
    -ReadAccess "DOMAIN\AllUsers" `
    -Description "Shared Documents"

# List shares
Get-SmbShare
# Name         ScopeName  Path                   Description
# ----         ---------  ----                   -----------
# SharedDocs   *          C:\SharedDocuments      Shared Documents
# IPC$         *                                  Remote IPC
# C$           *          C:\                     Default share

# Show share permissions
Get-SmbShareAccess -Name "SharedDocs"

# Set share permissions
Grant-SmbShareAccess -Name "SharedDocs" -AccountName "DOMAIN\JohnDoe" `
    -AccessRight Change -Force

# Connect to a share
New-PSDrive -Name Z -PSProvider FileSystem -Root \\server\SharedDocs -Persist
# Or with credentials:
$cred = Get-Credential
New-PSDrive -Name Z -PSProvider FileSystem -Root \\server\SharedDocs `
    -Credential $cred -Persist

# Mount from command line
net use Z: \\server\SharedDocs /user:DOMAIN\username password

# Check connection
Get-SmbConnection
net use

# SMB performance tuning
Set-SmbClientConfiguration -DirectoryCacheLifetime 15 -FileInfoCacheLifetime 10
Set-SmbServerConfiguration -EnableMultiChannel $true  # Multi-channel SMB3

# ── DFS NAMESPACE ─────────────────────────────────────────────────

# Install DFS features
Install-WindowsFeature FS-DFS-Namespace, FS-DFS-Replication, RSAT-DFS-Mgmt-Con

# Create DFS namespace
New-DfsnRoot -Path \\domain\dfsroot -Type DomainV2 `
    -Description "Company DFS Root"

# Add DFS folder (links UNC path into namespace)
New-DfsnFolder -Path \\domain\dfsroot\Documents `
    -TargetPath \\fileserver1\SharedDocs `
    -Description "Company Documents"

# Add failover target
New-DfsnFolderTarget -Path \\domain\dfsroot\Documents `
    -TargetPath \\fileserver2\SharedDocsMirror

# View DFS structure
Get-DfsnRoot -Path \\domain\dfsroot
Get-DfsnFolder -Path \\domain\dfsroot\*
Get-DfsnFolderTarget -Path \\domain\dfsroot\Documents

# ── DFS REPLICATION ───────────────────────────────────────────────

# Create replication group
New-DfsReplicationGroup -GroupName "DocReplication" `
    -Description "Replicate SharedDocs between servers"

# Add members (servers)
Add-DfsrMember -GroupName "DocReplication" `
    -ComputerName "FileServer1", "FileServer2"

# Create replicated folder
New-DfsReplicatedFolder -GroupName "DocReplication" `
    -FolderName "SharedDocs"

# Set content paths on each member
Set-DfsrMembership -GroupName "DocReplication" `
    -FolderName "SharedDocs" `
    -ComputerName "FileServer1" `
    -ContentPath "C:\SharedDocuments" `
    -PrimaryMember $true

Set-DfsrMembership -GroupName "DocReplication" `
    -FolderName "SharedDocs" `
    -ComputerName "FileServer2" `
    -ContentPath "D:\SharedDocuments"

# Create connections (bidirectional)
Add-DfsrConnection -GroupName "DocReplication" `
    -SourceComputerName "FileServer1" `
    -DestinationComputerName "FileServer2"

# Start initial sync
Start-DfsrPropagation -GroupName "DocReplication" `
    -FolderName "SharedDocs"

# Check replication status
Get-DfsrState -ComputerName "FileServer1" -Verbose
Get-DfsrBacklog -GroupName "DocReplication" `
    -FolderName "SharedDocs" `
    -SourceComputerName "FileServer1" `
    -DestinationComputerName "FileServer2"
```

```bash
# ── LINUX ACCESS TO SMB SHARES ────────────────────────────────────

# Install Samba client
sudo apt-get install smbclient cifs-utils

# List shares on Windows server
smbclient -L //windowsserver -U "DOMAIN\\username"
# Sharename       Type  Comment
# ---------       ----  -------
# SharedDocs      Disk  Shared Documents
# IPC$            IPC   Remote IPC

# Mount SMB share on Linux
sudo mkdir /mnt/windows
sudo mount -t cifs //windowsserver/SharedDocs /mnt/windows \
    -o username=myuser,password=mypass,domain=DOMAIN,vers=3.0

# Persistent mount in /etc/fstab
echo "//windowsserver/SharedDocs /mnt/windows cifs \
    credentials=/etc/samba/credentials,vers=3.0,_netdev 0 0" \
    | sudo tee -a /etc/fstab

# Credentials file (more secure than inline password)
sudo tee /etc/samba/credentials << 'EOF'
username=myuser
password=mypass
domain=DOMAIN
EOF
sudo chmod 600 /etc/samba/credentials
```

---

## 10. Replication and Fault Tolerance

### Replication Strategies

```
Strategy           | Writes | Reads  | Failure tolerance
──────────────────────────────────────────────────────────────
Primary-backup     |   1    | 1/N    | Survives N-1 backups failing
Chain replication  |   N    | 1 (tail)| Strong consistency
Quorum (N=3,Q=2)  |   2    | 2      | Survives 1 failure
Erasure coding    |   k+m  | k      | Survives m failures, k<N storage
```

### Quorum-Based Replication

```python
# quorum_demo.py - Simulate quorum reads/writes
import random
from typing import List, Dict, Optional

class Replica:
    def __init__(self, name: str):
        self.name = name
        self.store: Dict[str, tuple] = {}  # key → (value, version)
        self.available = True

    def write(self, key: str, value: str, version: int) -> bool:
        if not self.available:
            return False
        self.store[key] = (value, version)
        return True

    def read(self, key: str) -> Optional[tuple]:
        if not self.available:
            return None
        return self.store.get(key)


class QuorumSystem:
    """
    Read quorum (R) + Write quorum (W) > N total replicas
    Guarantees: at least one replica in any read quorum was in latest write quorum
    Common: N=3, W=2, R=2
    """
    def __init__(self, replicas: List[Replica], W: int, R: int):
        self.replicas = replicas
        self.N = len(replicas)
        self.W = W  # Write quorum size
        self.R = R  # Read quorum size
        assert W + R > self.N, "Must satisfy W + R > N for consistency"
        self.version_counter = 0

    def write(self, key: str, value: str) -> bool:
        self.version_counter += 1
        version = self.version_counter
        successes = 0
        for r in self.replicas:
            if r.write(key, value, version):
                successes += 1
        if successes >= self.W:
            print(f"Write OK: key={key} val={value} ver={version} "
                  f"({successes}/{self.N} replicas)")
            return True
        print(f"Write FAILED: only {successes}/{self.N} replicas acked")
        return False

    def read(self, key: str) -> Optional[str]:
        """Read from R replicas, return value with highest version."""
        responses = []
        for r in self.replicas:
            result = r.read(key)
            if result is not None:
                responses.append(result)

        if len(responses) < self.R:
            print(f"Read FAILED: only {len(responses)}/{self.R} replicas responded")
            return None

        # Return value with highest version (latest write)
        best = max(responses, key=lambda x: x[1])
        print(f"Read OK: key={key} val={best[0]} ver={best[1]} "
              f"(from {len(responses)} replicas)")
        return best[0]


# Demo: N=3, W=2, R=2
replicas = [Replica("R1"), Replica("R2"), Replica("R3")]
q = QuorumSystem(replicas, W=2, R=2)

q.write("x", "hello")
q.read("x")

# Simulate replica failure
replicas[2].available = False
print("\n--- Replica R3 is down ---")
q.write("x", "world")  # Still OK (2/3 replicas)
q.read("x")             # Still OK (2/3 replicas)

# Two replicas down - system should fail
replicas[1].available = False
print("\n--- R2 and R3 are down ---")
q.write("x", "failed")  # Should fail (only 1/3)
q.read("x")             # Should fail
```

---

## 11. Performance Analysis

```bash
# ── BENCHMARK NFS vs LOCAL ─────────────────────────────────────────

# Sequential write speed
fio --name=seqwrite --ioengine=libaio --iodepth=16 \
    --rw=write --bs=1M --size=4G --numjobs=1 \
    --filename=/mnt/nfs/fio_test --direct=1
# Compare with:
fio --name=seqwrite --ioengine=libaio --iodepth=16 \
    --rw=write --bs=1M --size=4G --numjobs=1 \
    --filename=/local/fio_test --direct=1

# IOPS benchmark (small random)
fio --name=randread --ioengine=libaio --iodepth=32 \
    --rw=randread --bs=4k --size=2G --numjobs=4 \
    --filename=/mnt/nfs/fio_test --direct=1

# Metadata throughput (small file creates)
time for i in $(seq 1 10000); do touch /mnt/nfs/tmp_$i; done
time rm -f /mnt/nfs/tmp_*

# Network bandwidth that NFS is consuming
iftop -i eth0 -f "port 2049"
# Or:
nethogs eth0

# ── LATENCY ANALYSIS ─────────────────────────────────────────────

# Measure stat() latency on NFS vs local
python3 << 'EOF'
import os, time, statistics

def bench_stat(path, n=1000):
    latencies = []
    for _ in range(n):
        t0 = time.perf_counter()
        os.stat(path)
        latencies.append((time.perf_counter() - t0) * 1e6)  # µs
    return latencies

local_l = bench_stat('/local/testfile')
nfs_l   = bench_stat('/mnt/nfs/testfile')

for name, lats in [("Local", local_l), ("NFS", nfs_l)]:
    s = sorted(lats)
    print(f"{name}: mean={statistics.mean(lats):.1f}µs "
          f"p50={s[500]:.1f}µs p99={s[990]:.1f}µs")
EOF
```

---

## 12. Quizzes and Exercises

> [!question]
> **Quiz 1: File Caching Locations in Distributed File Systems (Clips 479-480)**
> In a distributed file system architecture connecting multiple client nodes across a network to storage servers, at which physical and logical layers can file data and metadata be cached?
> Select all valid caching locations in the DFS storage hierarchy:
> 1. Client buffer cache in host RAM
> 2. Client local persistent disk / solid state drive
> 3. Server buffer cache in server RAM
> 4. Network switch packet buffers

> [!success]- Answer
> **Valid caching locations:** Options 1, 2, and 3.
> 
> **Architectural Breakdown:**
> - **Client Host RAM (Option 1):** The primary and lowest-latency cache.
> File read and write operations hit the client Operating System buffer cache / page cache, avoiding all network round-trip latency.
> - **Client Persistent Disk (Option 2):** Used in architectures such as the Andrew File System (AFS).
> AFS downloads entire files or large chunks to local client disk partitions (the client local cache).
> This allows files to remain cached across client reboots and frees client volatile RAM for application processes.
> - **Server RAM Buffer Cache (Option 3):** When a client cache miss occurs, an RPC is sent over the network to the file server.
> The server inspects its own memory page cache before initiating costly block device reads from physical disk spindles or NVMe drives.
> - **Network Switch Packet Buffers (Option 4):** Switches buffer transit Ethernet or IP frames during network congestion.
> They do not cache application-level filesystem blocks, file inodes, or directory trees.

---

> [!question]
> **Quiz 2: Server-Driven DFS with Session Semantics (Clips 482-483)**
> Consider a distributed file system implementing server-driven session semantics (changes are flushed to the server on `close()` and validated on `open()`).
> What state must the server maintain in its per-file data structure to coordinate client access correctly?
> 1. Current list of active readers
> 2. Current list of concurrent active writers
> 3. Current file version number / modification generation counter
> 4. Complete client stack traces of all open file descriptors

> [!success]- Answer
> **Required state items:** Options 1, 2, and 3.
> 
> **Mechanism Explanation:**
> Under session semantics, a file's contents are frozen into a local session snapshot upon `open()`.
> All reads and writes target the local cache during the session.
> Upon `close()`, the client flushes modified blocks to the server, creating a new authoritative file version.
> To prevent lost updates and coordinate invalidations, the server per-file table must record:
> - **Active Readers:** To determine which clients must be notified of invalidations or to track active read leases.
> - **Active Writers:** To detect concurrent write sessions.
> If multiple clients open the file for writing simultaneously, the server must determine conflict resolution (for example, last-close-wins, branching, or cache disablement).
> - **Version Number:** Incremented whenever a writer closes the file and commits changes.
> When another client invokes `open()`, the client passes its cached version number; if the server version is newer, the client invalidates its local cache and fetches the new file state.
> Client internal stack traces (Option 4) are private process state inside the client OS kernel and have no meaning to the storage server.

---

> [!question]
> **Quiz 3: Replication vs. Partitioning: Capacity and Fault Tolerance (Clips 486-487)**
> A distributed storage cluster consists of 3 identical server nodes.
> Each individual server has the physical storage capacity to hold exactly 100 unique files (providing 300 total physical file storage slots across the entire cluster).
> Calculate the total unique file storage capacity of the cluster and the percentage of unique files lost/unavailable if exactly 1 server crashes under two architectures:
> - **Architecture A (3-way Full Replication):** Every file stored in the cluster is mirrored across all 3 nodes.
> - **Architecture B (Pure Partitioning / Sharding):** Files are partitioned uniformly across the 3 nodes with zero replication (each file exists on exactly one node).

> [!success]- Answer
> **Quantitative Comparison:**
> 
> | Metric | Architecture A (3-Way Replication) | Architecture B (Partitioning / Sharding) |
> | :--- | :--- | :--- |
> | **Total Unique Capacity** | **100 files** | **300 files** |
> | **Capacity Calculation** | $\min(C_1, C_2, C_3) = 100$ files | $C_1 + C_2 + C_3 = 100 + 100 + 100 = 300$ files |
> | **Lost Files on 1 Node Crash** | **0 files (0% data lost)** | **100 files (33.3% data lost)** |
> | **Availability Calculation** | 2 replica nodes remain online ($100\%$ available) | $1 / 3$ of the partition space is unreachable ($33.3\%$ unavailable) |
> 
> **Key Takeaway:**
> Replication maximizes fault tolerance and read throughput at the expense of usable storage efficiency ($33.3\%$ storage efficiency for 3-way replication).
> Partitioning maximizes usable storage capacity and aggregate write bandwidth across separate disks, but any single node failure immediately causes permanent or temporary data loss for its assigned partition.

---

> [!question]
> **Quiz 4: Stale NFS File Handle ESTALE (Clips 489-490)**
> A software engineer runs a distributed build job on an NFS client mount point.
> Suddenly, a system call targeting a file returns an error with errno 116 (`ESTALE`: Stale file handle).
> What is the architectural root cause of `ESTALE` in the Network File System protocol?

> [!success]- Answer
> **Root Cause:**
> In NFS (particularly NFS v2 and v3), the server is stateless.
> The server does not maintain client open file tables.
> Instead, files are identified across RPC calls using an opaque **file handle** generated by the server.
> The file handle contains three essential fields:
> 1. The filesystem identifier (`fsid`).
> 2. The internal inode number of the target file (`fileid`).
> 3. An inode generation count (`generation`).
> 
> If Client A opens a file and receives a file handle, but another client (or a local administrative process on the server) deletes or unlinks the file, the server frees the inode.
> When the underlying filesystem reallocates that inode to a completely new file, the server increments the inode generation count.
> When Client A subsequently submits an RPC request containing the old file handle, the server inspects the inode on disk.
> The server detects that the stored generation count on disk does not match the generation count inside the client's file handle, or that the inode is completely unallocated.
> Because the file referenced by the handle no longer exists in that exact identity, the server returns the error `ESTALE`.
> The client OS cannot transparently recover and raises `ESTALE` to the calling application.

---

> [!question]
> **Quiz 5: NFS Cache Consistency Semantics (Clips 492-493)**
> Does the standard Network File System (NFS) protocol provide strict session semantics, strict POSIX consistency, or periodic consistency?
> Explain how NFS handles file modifications across multiple clients in practice.

> [!success]- Answer
> **Answer: Neither strict session semantics nor strict POSIX semantics.**
> NFS implements a pragmatic, opportunistic consistency model often described as **close-to-open consistency** coupled with **periodic attribute polling**.
> 
> **How It Operates:**
> 1. **Flush on Close:** When a client application closes a file (`close()`), the client kernel synchronously flushes all modified and dirty buffer cache pages back to the NFS server before returning from the `close()` system call.
> 2. **Check on Open:** When another client opens the same file (`open()`), the client kernel issues an RPC (`GETATTR`) to query the server's last modification timestamp (`mtime`).
> If the server timestamp is newer than the client cached copy, the client purges its local buffer cache and refetches data from the server.
> 3. **The Periodic Polling Gap:** While a file remains continuously open across multiple clients, the clients do not query the server on every read or write.
> Instead, each client caches file attributes for a configurable time window governed by mount options:
> - `acregmin` (minimum attribute cache timeout for regular files, default 3 seconds).
> - `acregmax` (maximum attribute cache timeout for regular files, default 60 seconds).
> 
> If Client 1 and Client 2 both have the file open simultaneously, writes performed by Client 1 will not be observed by Client 2 until Client 1 closes the file or flushes blocks, and Client 2's attribute timer expires.
> This violates POSIX consistency (which requires that a write be immediately visible to all concurrent readers).

---

> [!question]
> **Quiz 6: Sprite DFS Empirical Findings and Cache-Disable Policy (Clips 494-497)**
> In the classic empirical study of the Sprite distributed file system (Ousterhout et al.):
> 1. What key behavioral characteristics of file system access patterns were uncovered?
> 2. How did Sprite handle cache consistency when concurrent write-sharing actually occurred?

> [!success]- Answer
> **Empirical Findings:**
> The researchers instrumented unix workstations in an academic and research environment and discovered:
> - **Write Frequency:** Approximately $33\%$ of all file opens were for writing, while $67\%$ were read-only.
> - **Short Lifespans:** Roughly $75\%$ of files were open for less than $0.5$ seconds, and most temporary files were deleted within minutes of creation.
> - **Small File Sizes:** Over $90\%$ of all accessed files were smaller than 10 KB.
> - **Rare Concurrent Write-Sharing:** True concurrent write-sharing (where one process writes to a file while another process simultaneously reads or writes the same file) accounted for less than $1\%$ of all file accesses.
> 
> **Sprite's Cache-Disable-on-Write-Sharing Policy:**
> Because concurrent write sharing was so rare, Sprite allowed clients to cache file blocks in local RAM without executing continuous validation checks.
> However, the central Sprite file server tracked all active opens for every file.
> When a client opened a file that was already opened by another client on a different machine, and at least one of the clients requested write access:
> 1. The server recognized the onset of concurrent write-sharing.
> 2. The server contacted the writing client and ordered it to flush its dirty blocks to the server.
> 3. The server signaled all participating clients to **disable local caching** for that specific file.
> 4. For the entire duration of the concurrent access session, all reads and writes bypassed the client local buffer caches and traveled synchronously over the network to the server buffer cache.
> 5. Once the file was closed and only a single client remained, caching was re-enabled.

---

> [!question]
> **Quiz 7: Stateless vs. Stateful Distributed File Servers**
> Contrast stateless servers (e.g., NFS v3) with stateful servers (e.g., AFS, NFS v4) across crash recovery, file locking, and cache invalidation.

> [!success]- Answer
> **Comprehensive Architectural Comparison:**
> 
> | Dimension | Stateless Server (NFS v3) | Stateful Server (AFS, NFS v4) |
> | :--- | :--- | :--- |
> | **Server State Kept** | None in kernel memory regarding client sessions. Requests contain complete authorization and position context. | Maintains client identifiers, open file instances, byte-range locks, and cache delegations/callbacks. |
> | **Crash Recovery** | Trivial and immediate. The server reboots and immediately processes incoming client RPCs; clients simply retry timed-out requests. | Complex. Requires a **grace period** upon reboot during which clients reclaim existing locks and open state before new requests are accepted. |
> | **File Locking** | Impossible natively within the base protocol. Requires an out-of-band auxiliary daemon (Network Lock Manager - NLM), introducing fragile pseudo-state. | Native and robust. Byte-range locks are integrated directly into the protocol state machine. |
> | **Cache Invalidation** | Client-driven. Clients must periodically poll the server (`GETATTR`) to verify file freshness, creating server scalability bottlenecks. | Server-driven. The server grants **callbacks** (AFS) or **delegations** (NFS v4). When a file changes, the server pushes invalidation messages to clients. |

---

## 13. Cross-Platform Distributed File Systems: Linux, macOS, and Windows

### Linux: Kernel NFS Server and Client Operations

#### 1. Configuring and Exporting NFS Shares on Linux
The Linux kernel implements high-performance in-kernel NFS servicing (`nfsd`).
Exports are defined in `/etc/exports`.

```bash
# Install kernel NFS server package (Debian/Ubuntu)
sudo apt-get update && sudo apt-get install -y nfs-kernel-server

# Create export directory with shared permissions
sudo mkdir -p /srv/nfs/shared_data
sudo chown -R nobody:nogroup /srv/nfs/shared_data
sudo chmod 777 /srv/nfs/shared_data

# Configure /etc/exports
# rw: read-write access
# sync: commit changes to disk before replying to RPCs (prevents data loss on crash)
# no_subtree_check: disables subtree checking for improved transfer speed and reliability
# insecure: allows clients using source ports > 1024 (essential for macOS clients)
cat << 'EOF' | sudo tee -a /etc/exports
/srv/nfs/shared_data  192.168.1.0/24(rw,sync,no_subtree_check,insecure)
EOF

# Export all declared file systems and verify active exports
sudo exportfs -arv
sudo exportfs -s

# Enable and start NFS server systemd service
sudo systemctl enable --now nfs-server
```

#### 2. Mounting NFS on Linux Clients
Modern Linux distributions default to NFS v4.2.

```bash
# Install client utilities
sudo apt-get install -y nfs-common

# Create mount target
sudo mkdir -p /mnt/nfs_client

# Mount NFS share with production performance options:
# hard: client retries indefinitely if server crashes, avoiding application IO errors
# intr: allows user to interrupt hung operations via SIGINT/SIGQUIT
# rsize/wsize: max block transfer size (1048576 = 1MB)
# timeo: RPC timeout in tenths of a second (600 = 60s)
# retrans: number of minor timeouts before a major timeout
sudo mount -t nfs -o vers=4.2,hard,intr,rsize=1048576,wsize=1048576,timeo=600,retrans=2 192.168.1.50:/srv/nfs/shared_data /mnt/nfs_client

# Inspect mounted NFS details and active RPC parameters
cat /proc/mounts | grep nfs
```

#### 3. Linux NFS Performance and Diagnostic Tooling
```bash
# Display client-side NFS RPC statistics and cache hit rates
nfsstat -c

# Display server-side NFS statistics (v3 vs v4 operations, read/write distributions)
nfsstat -s

# Monitor continuous per-mount I/O throughput and latency (1 second intervals)
nfsiostat 1

# Inspect raw network RPC socket buffers
cat /proc/net/rpc/nfs
```

---

### macOS: Darwin NFS Client, Built-in nfsd, and SMB

#### 1. Mounting NFS on macOS (The `resvport` Requirement)
By default, the macOS Darwin kernel uses unprivileged ephemeral network ports ($>1024$) for outbound NFS client connections.
Standard UNIX/Linux NFS servers reject requests from unprivileged ports with `Permission denied` unless the server explicitly specifies `insecure` in `/etc/exports` or the client uses the `resvport` mount option.

```zsh
# Create local mount point on macOS
sudo mkdir -p /System/Volumes/Data/mnt/nfs_share

# Mount remote NFS export using resvport (forces port < 1024)
sudo mount -t nfs -o resvport,vers=4,hard,intr,rsize=1048576,wsize=1048576 192.168.1.50:/srv/nfs/shared_data /System/Volumes/Data/mnt/nfs_share

# Verify active mount details using mount command
mount -v | grep nfs
```

#### 2. Running the Native Darwin NFS Server on macOS
macOS includes a native BSD-derived NFS daemon (`nfsd`).

```zsh
# Configure /etc/exports on macOS
# -ro: read only, or -maproot=root for administrative access
cat << 'EOF' | sudo tee -a /etc/exports
/Users/Shared/Public -network 192.168.1.0 -mask 255.255.255.0 -alldirs
EOF

# Verify syntax of /etc/exports without starting daemon
sudo nfsd checkexports

# Enable and start the macOS nfsd daemon
sudo nfsd enable
sudo nfsd restart

# Query operational status of macOS nfsd
sudo nfsd status
```

#### 3. macOS Native SMB Client and Real-Time Filesystem Tracing
```zsh
# Mount an SMB share via Darwin command line
mkdir -p /Volumes/RemoteShare
mount -t smbfs //username:password@192.168.1.50/shared_data /Volumes/RemoteShare

# Trace all active filesystem calls (VFS layer) in real time
# Filters specifically for filesystem system calls and displays file paths
sudo fs_usage -w -f filesys
```

---

### Windows: Services for NFS and PowerShell SMB Architecture

#### 1. Enabling and Mounting NFS on Windows
Windows provides the "Client for NFS" feature on Pro and Enterprise editions.

```powershell
# Enable NFS Client Feature in Windows via Administrator PowerShell
Enable-WindowsOptionalFeature -Online -FeatureName ServicesForNFS-ClientOnly,ClientForNFS-Infrastructure -NoRestart

# Mount NFS share to drive letter Z:
# anon: use anonymous user mapping
# casesensitive=yes: force UNIX case-sensitivity compliance
# fileaccess=777: set default permissions
mount.exe -o anon,fileaccess=777,casesensitive=yes \\192.168.1.50\srv\nfs\shared_data Z:

# Verify active NFS network mounts
net use
nfsstat.exe
```

#### 2. Native Windows SMB 3.1.1 and DFS-N Operations
In Windows enterprise networks, the Distributed File System Namespace (DFS-N) provides a logical tree aggregating distinct SMB shares across multiple physical file servers.

```powershell
# Inspect all active SMB client connections and negotiated dialect (e.g., 3.1.1)
Get-SmbConnection

# Verify SMB Multichannel network adapter bindings for high throughput
Get-SmbMultichannelConnection

# Create an enterprise SMB share on Windows Server
New-SmbShare -Name "CompanyData" -Path "C:\Data\Company" -FullAccess "Domain Admins" -ReadAccess "Everyone"

# Query SMB server configuration parameters
Get-SmbServerConfiguration | Select-Object -Property EnableSMB1Protocol, EnableSMB2Protocol, EncryptData
```

---

*Cross-links: [[P4L1-Remote-Procedure-Calls]] (NFS uses Sun RPC) | [[P3L3-Inter-Process-Communication]] (local file sharing) | [[P3L5-IO-Management]] (VFS layer)*

