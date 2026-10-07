---
type: concept
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources:
  - "Hadoop: The Definitive Guide (4th Edition) by Tom White"
  - "The Hadoop Distributed File System (Konstantin Shvachko et al., IEEE MSST 2010)"
  - "Designing Data-Intensive Applications by Martin Kleppmann"
---

# Hadoop and HDFS Architecture

## TL;DR

Apache Hadoop is a foundational framework for distributed storage and large-scale batch processing across commodity hardware clusters.
Storage is governed by the Hadoop Distributed File System (HDFS), a distributed, write-once, append-only file system optimized for streaming reads over multi-gigabyte files split into large blocks (default 128MB).
HDFS utilizes a master-worker architecture where an in-memory NameNode manages the namespace metadata and block mapping, while distributed DataNodes store raw block replicas.
High availability is achieved through Active-Standby NameNode pairs synchronized via a Quorum Journal Manager (QJM) and Apache ZooKeeper.
Compute resource management is decoupled via YARN (Yet Another Resource Negotiator), arbitrating cluster resources through a central ResourceManager, per-node NodeManagers, and per-job ApplicationMasters executing MapReduce and Spark workloads.

## Mental Model

HDFS pairs a centralized in-memory NameNode metadata directory with a distributed fleet of DataNodes streaming raw data blocks across rack topologies.

```mermaid
graph TD
    Client["HDFS Client Application"]
    
    subgraph HDFSControlPlane["HDFS Control Plane (Active / Standby NameNode)"]
        ActiveNN["Active NameNode (Namespace in RAM, edits log, fsimage)"]
        StandbyNN["Standby NameNode (Applies edits log continuously)"]
        QJM["Quorum Journal Manager (QJM: JournalNodes Paxos Quorum)"]
        ZK["Apache ZooKeeper (ZKFC: Health Monitor & Failover)"]
        
        ActiveNN <--> QJM
        StandbyNN <--> QJM
        ZK -.-> ActiveNN
        ZK -.-> StandbyNN
    end
    
    Client -->|1. Request File Block Locations| ActiveNN
    
    subgraph DataNodeFleet["HDFS Storage Fleet (Rack-Aware Block Placement)"]
        subgraph Rack1["Rack 1 (Local Rack)"]
            DN1["DataNode 1 (Stores Block A - Replica 1)"]
            DN2["DataNode 2 (Stores Block A - Replica 2)"]
        end
        subgraph Rack2["Rack 2 (Remote Rack)"]
            DN3["DataNode 3 (Stores Block A - Replica 3)"]
        end
    end
    
    Client -->|2. Stream 64KB Packets via Pipeline| DN1
    DN1 -->|Pipeline Stream| DN2
    DN2 -->|Pipeline Stream| DN3
    
    DN1 -.->|Block Reports & Heartbeats (3s)| ActiveNN
    DN2 -.->|Block Reports & Heartbeats (3s)| ActiveNN
    DN3 -.->|Block Reports & Heartbeats (3s)| ActiveNN
```

## Architectural Internals and Deep Dive

### 1. HDFS Architecture: NameNode and DataNodes
HDFS abstracts storage across thousands of servers using a master-worker topology:
- **NameNode (Master)**:
  - Maintains the entire filesystem directory tree and block mapping in physical RAM for ultra-fast metadata resolution.
  - Does not store actual file data.
    It stores only file attributes, permissions, and an array of Block IDs representing the file's data.
  - A 1GB file is broken into eight 128MB blocks; the NameNode maps `file.csv -> [blk_101, blk_102, ... blk_108]`.
- **DataNodes (Workers)**:
  - Store and retrieve raw data blocks on local Linux ext4/xfs filesystems.
  - Do not know which high-level files their blocks belong to.
    Each block is simply stored as a raw binary file (`blk_101`) alongside a metadata checksum file (`blk_101.meta`).
  - Send periodic Heartbeats (every 3 seconds) and Block Reports (every 6 hours) to the NameNode, verifying block liveness and integrity.

### 2. NameNode Metadata Persistence: `fsimage` and `edits` Log
Because the NameNode holds all metadata in volatile RAM, system crashes would cause total data loss without persistence:
- **`fsimage`**: A point-in-time serialized binary snapshot of the entire filesystem namespace and inode metadata.
- **`edits` Log**: An append-only write-ahead log recording every transaction that modifies metadata (creates, renames, deletes, block allocations).
- **Startup Sequence**: On boot, the NameNode reads `fsimage` into memory, replays all un-applied transactions from the `edits` log, writes a fresh `fsimage`, empties the `edits` log, and enters SafeMode until DataNodes report their active blocks.
- **Standby NameNode & Checkpointing**: In HA deployments, the Standby NameNode acts as the checkpointer.
  It continuously downloads edits logs from the Quorum Journal Manager (QJM), merges them into its local `fsimage`, and uploads the compacted `fsimage` back to the Active NameNode, preventing the active edits log from growing uncontrollably.

### 3. The HDFS Write Pipeline and Rack Awareness
Writing a file to HDFS involves a multi-node streaming pipeline:
1. **Metadata Allocation**: Client calls `create()`; the NameNode verifies permissions, allocates a new Block ID, and returns a prioritized list of three DataNodes based on Rack Awareness.
2. **Rack-Aware Replica Placement Policy**:
   - **Replica 1**: Placed on the local node (if running on a cluster node) or a randomly selected node on a random rack.
   - **Replica 2**: Placed on a different node within the exact same rack as Replica 1.
   - **Replica 3**: Placed on a node in a completely different physical rack.
   - *Rationale*: Storing two replicas on the same rack minimizes inter-switch network bandwidth during pipelining, while the third replica on a separate rack guarantees survivability if an entire rack switch or power distribution unit (PDU) fails.
3. **Pipelined Data Streaming**:
   - The client buffers data into 64KB packets.
   - The client streams Packet 1 to DataNode 1 over TCP.
   - As DataNode 1 receives Packet 1, it writes it to local disk while simultaneously streaming it across the local rack switch to DataNode 2.
   - DataNode 2 writes to disk and streams across the core switch to DataNode 3.
   - Once all three nodes acknowledge Packet 1, the pipeline advances to Packet 2.

```
Client ----(Packet 1)----> DataNode 1 ----(Packet 1)----> DataNode 2 ----(Packet 1)----> DataNode 3
      <---(Ack Packet 1)--            <---(Ack Packet 1)--            <---(Ack Packet 1)--
```

### 4. Short-Circuit Local Reads
In standard HDFS, a client reading data contacts a DataNode over a local TCP socket, even if the client process is running on the exact same physical machine as the data block.
This incurs operating system TCP stack overhead, checksum calculations, and context switches.
Short-Circuit Local Reads bypass the DataNode daemon entirely:
- The client queries the NameNode for block locations and detects that a block replica resides on its local machine.
- The client requests a file descriptor directly from the local DataNode via a UNIX Domain Socket.
- The client reads the raw block file directly from the local filesystem using the kernel page cache, achieving native local disk read bandwidth (gigabytes per second) and eliminating networking overhead.

### 5. YARN: Yet Another Resource Negotiator
In Hadoop 1.x, the MapReduce engine was tightly coupled with cluster resource management via JobTracker and TaskTrackers, bottlenecking clusters at ~4,000 nodes and restricting computing strictly to MapReduce batch jobs.
Hadoop 2.0 introduced YARN, decoupling resource arbitration from compute engines:
- **ResourceManager (RM)**: The global cluster master. Consists of:
  - *Scheduler*: Allocates resources (CPU, Memory) to applications using pluggable policies (CapacityScheduler, FairScheduler) without monitoring application status.
  - *ApplicationsManager (ASM)*: Accepts job submissions, negotiates the first Container to execute the application-specific coordinator, and restarts failed coordinators.
- **NodeManager (NM)**: The per-machine agent.
  Monitors resource utilization (CPU, memory cgroups), tracks node health, and executes task Containers.
- **ApplicationMaster (AM)**: A per-application coordinator instance (one per running Spark job or MapReduce job).
  It negotiates compute containers from the ResourceManager Scheduler and instructs NodeManagers to launch tasks, reporting progress directly to the client.

### 6. MapReduce Engine Internals
The classic batch processing execution flow proceeds in discrete phases:
1. **InputSplit**: Large files are divided into logical chunks (typically matching HDFS 128MB block boundaries).
   A dedicated Map task is spawned for each InputSplit, scheduled close to the data (Data Locality).
2. **Map Phase**: The `RecordReader` parses raw bytes into `(key, value)` pairs passed to user-defined `map()` functions.
3. **Spill and Partition**: Mapper output buffers in RAM (`mapreduce.task.io.sort.mb`, default 100MB).
   When the buffer reaches 80% capacity, a background thread sorts the records by key, applies the Partitioner (`hash(key) % num_reducers`), runs an optional Combiner (mini-reducer), and spills to an indexed file on local disk.
4. **Shuffle and Sort Phase**: Reducers pull their assigned partition partitions across the network via HTTP from all mappers (the Shuffle).
   The Reducer merges and sorts incoming keys into a single sorted stream.
5. **Reduce Phase**: User-defined `reduce()` functions process values for each unique key and stream results to HDFS via an `OutputFormat`.

## Trade-offs and Comparisons

| Dimension | Apache Hadoop (HDFS + YARN) | Apache Spark | Amazon S3 / Cloud Object Storage |
| :--- | :--- | :--- | :--- |
| **Storage Model** | Distributed append-only block filesystem | In-memory RDD / DataFrame caching engine | Flat namespace object store (REST API) |
| **I/O Bottleneck** | Disk-bound (Spills intermediate MapReduce to disk)| Memory-bound (In-memory DAG pipeline) | Network-bound (HTTP/S REST API transfers) |
| **Batch Latency** | Minutes to Hours (High startup and disk latency)| Seconds to Minutes (10x-100x faster than MR) | Milliseconds to Seconds per object query |
| **Compute Engine** | MapReduce batch engine (Coarse-grained) | Multi-workload (SQL, Streaming, ML, Graph) | Decoupled; queried via Trino, Spark, Athena |
| **Small File Performance** | Pathological (Exhausts NameNode RAM) | High (Partitions data in memory) | Native (Millions of small objects supported) |
| **Data Locality** | Native compute-storage co-location | Native co-location on HDFS; decoupled on S3 | Disaggregated compute and storage (Zero locality) |

## Failure Modes and Mitigations

### 1. The Small Files Catastrophe (NameNode Memory Exhaustion)
- *Root Cause*: Generating millions of files smaller than 1MB (e.g., streaming logs writing 10KB files).
  Because the NameNode allocates approximately 150 bytes of physical RAM per file inode and per block regardless of file size, 100 million tiny files consume 30GB+ of JVM heap memory purely for metadata, while storing only 1TB of actual data.
- *Mitigation*: Merge small files into Apache Parquet or ORC columnar files; run Hadoop Archive (`HAR`) tools; use sequence files; offload small file storage to Amazon S3.

### 2. NameNode Single Point of Failure (SPOF)
- *Root Cause*: In non-HA clusters, NameNode hardware failure freezes all cluster read and write operations.
  Reconstructing metadata from `fsimage` and `edits` takes hours on massive clusters.
- *Mitigation*: Deploy HDFS NameNode High Availability with Quorum Journal Manager (QJM) and ZooKeeper Failover Controller (ZKFC) for automated sub-30-second active-standby failover.

### 3. Straggler Task Latency Spikes
- *Root Cause*: A single worker node experiences hardware degradation (failing disk drive, thermal CPU throttling).
  Map or Reduce tasks scheduled on that node run $10\times$ slower than normal, delaying completion of the entire batch job.
- *Mitigation*: Enable Speculative Execution (`mapreduce.map.speculative = true`): YARN detects tasks running significantly slower than the median, launches a duplicate speculative task on a different node, and terminates whichever finishes second.

### 4. DataNode Disk Imbalance and Hotspotting
- *Root Cause*: Adding new DataNodes to an existing cluster leaves existing nodes at 95% capacity while new nodes sit at 5% capacity, causing new writes and map tasks to concentrate unevenly.
- *Mitigation*: Run the HDFS Disk Balancer (`hdfs balancer -threshold 5`) in the background to rebalance blocks across nodes and rack topologies non-disruptively.

## Hands-On Verification

### Multi-OS Diagnostic Commands

#### Linux / macOS (HDFS and YARN CLI)
```bash
# Check HDFS cluster storage capacity, live nodes, and missing blocks
hdfs dfsadmin -report

# Inspect file blocks, replica locations, and rack assignments
hdfs fsck /data/sample_dataset.csv -files -blocks -racks

# Upload a file to HDFS with an explicit replication factor of 2
hdfs dfs -D dfs.replication=2 -put local_data.csv /data/local_data.csv

# Query running YARN applications and cluster resource allocations
yarn application -list
yarn node -list -all
```

#### Windows (PowerShell via Hadoop Binary Wrappers)
```powershell
# Verify HDFS connectivity via PowerShell
hdfs.cmd dfs -ls /

# Check active YARN cluster memory metrics
yarn.cmd top
```

### Pure Python 3 Standard-Library HDFS Cluster Simulation

The following self-contained script simulates an HDFS cluster without external dependencies:
- **Block and Checksums**: Computes SHA256 checksums per block to detect bit-rot and corruption.
- **Rack-Aware Replica Placement**: Places Replica 1 on the primary rack, Replica 2 on a distinct node in the same rack, and Replica 3 on a separate physical rack.
- **Pipelined Write Streaming**: Streams blocks through a multi-node pipeline with backward ACK propagation.
- **NameNode Journaling and Checkpointing**: Appends mutations to an in-memory `edits` log with monotonic transaction IDs and merges them into `fsimage`.
- **Fault-Tolerance and Self-Healing**: Simulates DataNode hardware failure and triggers automatic re-replication to restore full $3\times$ redundancy.

```python
"""
Simulated Apache Hadoop HDFS Cluster and Storage Engine
Pure Python 3 standard library simulation demonstrating:
1. Rack-Aware Replica Placement Policy (local rack pair + remote rack)
2. Pipelined block write streaming with backward acknowledgment
3. NameNode in-memory namespace, edits write-ahead log, and fsimage checkpointing
4. DataNode failure detection and self-healing under-replication recovery
"""

import hashlib
from typing import List, Dict, Tuple, Optional


class Block:
    def __init__(self, block_id: str, data: bytes):
        self.block_id = block_id
        self.data = data
        self.checksum = hashlib.sha256(data).hexdigest()
        self.size = len(data)


class DataNode:
    def __init__(self, node_id: str, rack_id: str):
        self.node_id = node_id
        self.rack_id = rack_id
        self.storage: Dict[str, Block] = {}
        self.alive = True

    def write_block_pipeline(self, block: Block, pipeline: List["DataNode"]) -> bool:
        """Pipelined block streaming simulation."""
        if not self.alive:
            return False
        self.storage[block.block_id] = block
        if pipeline:
            next_node = pipeline[0]
            remaining_pipeline = pipeline[1:]
            ack = next_node.write_block_pipeline(block, remaining_pipeline)
            if not ack:
                return False
        return True

    def read_block(self, block_id: str) -> Optional[bytes]:
        if not self.alive or block_id not in self.storage:
            return None
        blk = self.storage[block_id]
        if hashlib.sha256(blk.data).hexdigest() != blk.checksum:
            raise ValueError(f"Bitrot detected on node {self.node_id} for block {block_id}")
        return blk.data


class NameNode:
    def __init__(self):
        self.inodes: Dict[str, Dict] = {}
        self.block_locations: Dict[str, List[str]] = {}
        self.edits_log: List[Tuple[int, str, Dict]] = []
        self.txid_counter = 0
        self.fsimage: Dict = {}
        self.last_checkpoint_txid = 0
        self.block_id_seq = 1000

    def log_edit(self, op: str, details: Dict):
        self.txid_counter += 1
        self.edits_log.append((self.txid_counter, op, details))

    def choose_datanodes_rack_aware(self, datanodes: List[DataNode], replication: int = 3) -> List[DataNode]:
        """
        Rack-Aware Replica Placement Policy:
        Replica 1: First node in primary rack.
        Replica 2: Second node in the exact same rack as Replica 1.
        Replica 3: Node in a different physical rack.
        """
        live_nodes = [dn for dn in datanodes if dn.alive]
        if len(live_nodes) < replication:
            raise RuntimeError("Insufficient live DataNodes for requested replication")

        racks: Dict[str, List[DataNode]] = {}
        for dn in live_nodes:
            racks.setdefault(dn.rack_id, []).append(dn)

        rack_list = list(racks.keys())
        if len(rack_list) < 2 and replication > 1:
            return live_nodes[:replication]

        r1_rack = rack_list[0]
        dn1 = racks[r1_rack][0]
        chosen = [dn1]

        if len(racks[r1_rack]) > 1:
            chosen.append(racks[r1_rack][1])
        else:
            chosen.append(racks[rack_list[1]][0])

        r2_rack = rack_list[1] if rack_list[1] != r1_rack else rack_list[0]
        for dn in racks[r2_rack]:
            if dn not in chosen:
                chosen.append(dn)
                break

        return chosen[:replication]

    def allocate_block(self, path: str, size: int) -> Tuple[str, int]:
        self.block_id_seq += 1
        blk_id = f"blk_{self.block_id_seq}"
        self.log_edit("ALLOCATE_BLOCK", {"path": path, "block_id": blk_id, "size": size})
        return blk_id, self.txid_counter

    def commit_file(self, path: str, block_ids: List[str], locations: Dict[str, List[str]]):
        self.inodes[path] = {"blocks": block_ids, "replication": 3}
        for bid, locs in locations.items():
            self.block_locations[bid] = locs
        self.log_edit("COMMIT_FILE", {"path": path, "blocks": block_ids})

    def checkpoint(self):
        """Simulate Standby NameNode checkpointing: merges edits into fsimage."""
        print(f"[Checkpoint] Merging {len(self.edits_log)} edits into fsimage (up to txid {self.txid_counter})...")
        self.fsimage = {
            "inodes": dict(self.inodes),
            "last_txid": self.txid_counter
        }
        self.last_checkpoint_txid = self.txid_counter
        self.edits_log.clear()
        print("[Checkpoint] fsimage updated. edits log truncated cleanly.")


class SimulatedHDFSCluster:
    def __init__(self):
        self.namenode = NameNode()
        self.datanodes: List[DataNode] = [
            DataNode("dn-1-rackA", "rack-A"),
            DataNode("dn-2-rackA", "rack-A"),
            DataNode("dn-3-rackB", "rack-B"),
            DataNode("dn-4-rackB", "rack-B"),
            DataNode("dn-5-rackC", "rack-C"),
        ]

    def write_file(self, path: str, content: bytes, block_size: int = 64) -> bool:
        print(f"\n--- Writing '{path}' ({len(content)} bytes) to HDFS ---")
        blocks_data = [content[i:i + block_size] for i in range(0, len(content), block_size)]
        committed_bids = []
        locations = {}

        for chunk in blocks_data:
            blk_id, _ = self.namenode.allocate_block(path, len(chunk))
            block = Block(blk_id, chunk)
            pipeline = self.namenode.choose_datanodes_rack_aware(self.datanodes, replication=3)
            print(f"[Write Pipeline] {blk_id}: Client -> {pipeline[0].node_id} ({pipeline[0].rack_id}) -> {pipeline[1].node_id} ({pipeline[1].rack_id}) -> {pipeline[2].node_id} ({pipeline[2].rack_id})")
            ack = pipeline[0].write_block_pipeline(block, pipeline[1:])
            if not ack:
                raise IOError(f"Pipeline write failed for block {blk_id}")
            committed_bids.append(blk_id)
            locations[blk_id] = [dn.node_id for dn in pipeline]

        self.namenode.commit_file(path, committed_bids, locations)
        print(f"[NameNode] Committed file '{path}' with {len(committed_bids)} block(s).")
        return True

    def read_file(self, path: str) -> bytes:
        if path not in self.namenode.inodes:
            raise FileNotFoundError(path)
        blocks = self.namenode.inodes[path]["blocks"]
        assembled = bytearray()
        dn_map = {dn.node_id: dn for dn in self.datanodes}

        for bid in blocks:
            locs = self.namenode.block_locations.get(bid, [])
            read_success = False
            for node_id in locs:
                dn = dn_map.get(node_id)
                if dn and dn.alive:
                    data = dn.read_block(bid)
                    if data is not None:
                        assembled.extend(data)
                        read_success = True
                        break
            if not read_success:
                raise IOError(f"Missing block {bid}: all replicas unavailable")
        return bytes(assembled)

    def trigger_heartbeat_and_rebalance(self):
        """Simulate NameNode detecting under-replicated blocks after node death."""
        print("\n--- Running NameNode Health Check & Under-Replication Scan ---")
        dn_map = {dn.node_id: dn for dn in self.datanodes}
        for bid, locs in list(self.namenode.block_locations.items()):
            live_locs = [nid for nid in locs if dn_map[nid].alive]
            self.namenode.block_locations[bid] = live_locs
            if len(live_locs) < 3:
                print(f"[Alert] Block {bid} under-replicated ({len(live_locs)}/3 replicas). Scheduling repair...")
                available = [dn for dn in self.datanodes if dn.alive and dn.node_id not in live_locs]
                if available:
                    source_dn = dn_map[live_locs[0]]
                    target_dn = available[0]
                    blk_data = source_dn.storage[bid]
                    target_dn.storage[bid] = blk_data
                    live_locs.append(target_dn.node_id)
                    self.namenode.block_locations[bid] = live_locs
                    print(f"[Self-Healing] Replicated {bid} from {source_dn.node_id} -> {target_dn.node_id}. Healthy replicas: {len(live_locs)}/3")


def run_hdfs_simulation():
    cluster = SimulatedHDFSCluster()
    data = b"Apache Hadoop HDFS Architecture: streaming reads, append-only files, rack-aware replica distribution, and NameNode metadata management."
    cluster.write_file("/analytics/user_logs.txt", data, block_size=64)

    read_back = cluster.read_file("/analytics/user_logs.txt")
    print(f"\n[Read Verification] Successfully read {len(read_back)} bytes: {read_back.decode('utf-8')[:50]}...")
    assert read_back == data

    cluster.namenode.checkpoint()

    print("\n[Fault Injection] Simulating hard drive failure on DataNode 'dn-1-rackA'...")
    cluster.datanodes[0].alive = False

    read_survived = cluster.read_file("/analytics/user_logs.txt")
    print(f"[Fault Tolerance] Successfully read data with degraded node: {len(read_survived)} bytes intact.")

    cluster.trigger_heartbeat_and_rebalance()


if __name__ == "__main__":
    run_hdfs_simulation()
```

### Educational MapReduce Paradigm Simulator (Python)

The following runnable script demonstrates the fundamental distributed MapReduce computational model: generating input splits, executing parallel mapper functions, running an in-memory shuffle and partition sort, and reducing aggregated values.

```python
"""
Educational MapReduce Paradigm Simulator (Hadoop In-Memory Simulation)
Demonstrates InputSplits, Map phase, Partition/Shuffle/Sort, and Reduce aggregation.
"""

from collections import defaultdict
import itertools

def map_function(document_id, text_chunk):
    """Mapper: Tokenizes text and emits (word, 1) pairs."""
    for word in text_chunk.lower().strip().split():
        # Clean basic punctuation
        clean_word = "".join(char for char in word if char.isalnum())
        if clean_word:
            yield (clean_word, 1)

def partitioner(key, num_reducers):
    """Partitioner: Maps keys to specific reducer IDs using consistent hashing."""
    return hash(key) % num_reducers

def reduce_function(word, counts):
    """Reducer: Sums counts for a given word."""
    return (word, sum(counts))

def run_mapreduce_pipeline():
    # 1. Raw Dataset (Representing HDFS Input Data)
    raw_documents = [
        "Distributed systems rely on distributed file systems like HDFS",
        "HDFS splits large files into blocks across DataNodes",
        "MapReduce processes data where data resides achieving data locality",
        "Data locality eliminates network transfer bottlenecks across racks"
    ]
    
    print("[Input] Ingesting 4 input documents across distributed splits...")
    NUM_REDUCERS = 2
    
    # 2. Map Phase (Parallel Task Execution)
    intermediate_pairs = []
    for doc_id, doc_text in enumerate(raw_documents):
        mapped_records = list(map_function(doc_id, doc_text))
        intermediate_pairs.extend(mapped_records)
    print(f"[Map Phase] Emitted {len(intermediate_pairs)} intermediate (key, value) pairs.")

    # 3. Shuffle and Sort Phase
    # Partition records across Reducer buckets
    reducer_buckets = defaultdict(list)
    for key, value in intermediate_pairs:
        r_id = partitioner(key, NUM_REDUCERS)
        reducer_buckets[r_id].append((key, value))

    # Sort records within each reducer bucket
    sorted_reducer_inputs = {}
    for r_id, pairs in reducer_buckets.items():
        # Sort by key to prepare for grouped reduction
        pairs.sort(key=lambda x: x[0])
        # Group identical keys together
        grouped = {}
        for key, group in itertools.groupby(pairs, key=lambda x: x[0]):
            grouped[key] = [val for _, val in group]
        sorted_reducer_inputs[r_id] = grouped
    print(f"[Shuffle/Sort] Distributed intermediate keys across {NUM_REDUCERS} Reducer partitions.")

    # 4. Reduce Phase (Final Aggregation)
    final_output = {}
    for r_id, grouped_keys in sorted_reducer_inputs.items():
        print(f"  [Reducer {r_id}] Processing {len(grouped_keys)} unique keys...")
        for word, counts in grouped_keys.items():
            out_word, total = reduce_function(word, counts)
            final_output[out_word] = total

    # 5. Display Top Aggregated Words
    print("\n--- Final Aggregated Word Counts (Top 5) ---")
    sorted_words = sorted(final_output.items(), key=lambda x: x[1], reverse=True)[:5]
    for word, count in sorted_words:
        print(f"  {word}: {count}")

if __name__ == "__main__":
    run_mapreduce_pipeline()
```

## Performance Characteristics and Capacity Planning

### 1. HDFS Block Sizing Math
Why is the default HDFS block size 128MB or 256MB, compared to a standard OS file system 4KB block size?
- Disk Seek Time: $\approx 10\text{ms}$ on traditional hard disk drives.
- Disk Transfer Rate: $\approx 100\text{MB/s}$.
- Goal: Bound disk seek time to $<1\%$ of total transfer time to maximize sequential read efficiency:

$$\frac{\text{SeekTime}}{\text{TransferTime}} = 0.01 \implies \text{TransferTime} = 100 \times 10\text{ms} = 1\text{ second}$$

$$\text{OptimalBlockSize} = 1\text{ second} \times 100\text{MB/s} = 100\text{MB} \implies \text{Standardized to 128MB}$$

### 2. Cluster Storage Overhead Sizing Formula
To store $U$ terabytes of uncompressed raw user data with default replication factor $R=3$:

$$\text{RawStorageRequired} = U \times 3 \times 1.30\text{ (Intermediate YARN Spills \& OS Headroom)}$$

For $U = 500\text{TB}$ of user data:

$$\text{RawStorageRequired} = 500\text{TB} \times 3 \times 1.30 \approx 1,950\text{TB} \approx 1.95\text{ Petabytes of physical disk}$$

## In Production: Real-World Case Studies

### 1. Yahoo's 40,000-Node Hadoop Footprint
Yahoo was the original birthplace and primary corporate sponsor of Apache Hadoop:
- **Web Crawling and Inverted Indexing**: Processed trillions of web pages to generate web search indices using multi-thousand-node MapReduce clusters.
- **Pioneering YARN**: Designed and contributed YARN to the open-source community to break the 4,000-node JobTracker architectural ceiling, scaling single Hadoop clusters past 10,000 nodes.

### 2. Facebook's Multi-Petabyte Data Warehouse
Facebook historically operated one of the largest Hadoop HDFS footprints in the world:
- **Hive Creation**: Engineered Apache Hive on top of Hadoop to allow SQL queries to be translated into distributed MapReduce jobs across HDFS data.
- **Cold Storage Optimization**: Developed Reed-Solomon HDFS Erasure Coding (HDFS-RAID) to reduce replication overhead from $3\times$ ($200\%$ overhead) down to $1.4\times$ ($40\%$ overhead) across exabytes of historical data.

## Staff+ Interview Questions

> [!question]
> Why does HDFS use an exceptionally large default block size (128MB or 256MB), and what happens to the system if an application stores millions of 10KB files?

> [!success]- Answer
> HDFS uses large 128MB blocks to minimize the cost of disk seeks relative to sequential data transfer.
> On mechanical hard drives, a seek takes ~10ms while sequential read transfers at ~100MB/s; transferring a 128MB block takes over 1 second, ensuring disk seek time represents $<1\%$ of total I/O time, maximizing disk throughput for large streaming reads.
> Furthermore, large blocks minimize NameNode memory consumption: the NameNode maintains an in-memory index of every block in the cluster.
> If an application stores millions of tiny 10KB files, each file still consumes a discrete block metadata entry in NameNode RAM (~150 bytes per inode and block).
> Storing 100 million 10KB files requires only 1TB of disk storage, but consumes over 30GB of NameNode heap memory purely for metadata tracking, starving the NameNode of memory and saturating it with heartbeat block reports.

> [!question]
> How does the HDFS Write Pipeline ensure data durability across rack topologies, and what is the Rack-Aware Replica Placement Policy?

> [!success]- Answer
> When a client writes a block, it streams 64KB packets sequentially through a multi-node pipeline: Client -> DataNode 1 -> DataNode 2 -> DataNode 3.
> Acknowledgments travel backwards along the pipeline.
> To balance network performance with catastrophic rack failure tolerance, HDFS implements the Rack-Aware Replica Placement Policy.
> Replica 1 is placed on the local node (or a randomly selected node on a random rack if the client is external).
> Replica 2 is placed on a different node within the exact same rack as Replica 1.
> Replica 3 is placed on a node in a completely different physical rack.
> This guarantees that copying data between Replica 1 and Replica 2 stays within the local top-of-rack switch (saving core datacenter switch bandwidth), while the third replica protects the data if the entire primary rack loses power, networking, or cooling.

> [!question]
> What is the function of the `fsimage` and `edits` log in the HDFS NameNode, and how does the Standby NameNode prevent the edits log from growing infinitely?

> [!success]- Answer
> The NameNode maintains the filesystem namespace in physical RAM for sub-millisecond lookups.
> For persistence, it uses Write-Ahead Logging: the `fsimage` is a point-in-time binary snapshot of the entire filesystem structure, and the `edits` log records every subsequent metadata mutation (file creation, block allocation, deletion).
> Replaying a massive edits log during reboot takes hours.
> In high-availability configurations, the Standby NameNode acts as a background checkpointer: it continuously reads new transactions from the Quorum Journal Manager (QJM), merges them into its local in-memory image, writes a consolidated new `fsimage` to disk, and pushes the new `fsimage` back to the Active NameNode via HTTP.
> The Active NameNode then truncates its edits log, bounding recovery time and memory footprint.

> [!question]
> How does YARN decouple cluster resource management from compute execution, and what are the specific responsibilities of the ResourceManager, NodeManager, and ApplicationMaster?

> [!success]- Answer
> In legacy Hadoop 1.x, resource management and MapReduce job scheduling were tightly coupled inside the JobTracker, creating a scalability bottleneck at ~4,000 nodes.
> YARN decoupled these roles into three distinct components.
> First, ResourceManager (RM): the global cluster arbiter whose Scheduler allocates generic compute Containers (CPU, RAM) across the cluster without monitoring task progress, while its ApplicationsManager handles job submissions and restarts failed coordinators.
> Second, NodeManager (NM): the per-node daemon that manages physical compute resources, enforces cgroups resource limits, and reports container health.
> Third, ApplicationMaster (AM): a transient, per-application coordinator running inside a container.
> The AM negotiates resource containers from the ResourceManager, coordinates execution across NodeManagers, handles task failures, and reports final status, allowing non-MapReduce frameworks (Apache Spark, Flink, Tez) to run natively on the shared cluster.

> [!question]
> Explain the Shuffle and Sort phase in Apache MapReduce. Why is this phase considered the primary performance bottleneck of batch MapReduce jobs?

> [!success]- Answer
> The Shuffle and Sort phase is the data redistribution stage that bridges mappers and reducers.
> As mappers run, their output is buffered in RAM; when the buffer reaches capacity, it is sorted by key, partitioned by reducer ID, and spilled to local disk.
> The Reducer must fetch its assigned partition from the local disks of every mapper across the entire cluster over HTTP (the Shuffle), merge the incoming partitions, and sort them into a single ordered stream before passing them to the `reduce()` function.
> This phase is the primary performance bottleneck because: (1) it forces all intermediate data to be written to local physical disks twice (once on mapper spill, once on reducer merge); (2) it causes massive all-to-all cross-rack network traffic saturation as every mapper transfers data to every reducer; and (3) it acts as a barrier synchronization point where reducers cannot finish execution until the slowest mapper has completed.

> [!question]
> What are "Short-Circuit Local Reads" in HDFS, and how do they optimize read throughput for compute tasks co-located on DataNodes?

> [!success]- Answer
> In standard HDFS, when a client application reads data, it opens a TCP socket to the DataNode daemon hosting the target block, even if the client process is running on the exact same physical server.
> The DataNode reads the block from local disk and transmits it across the local loopback TCP stack, incurring context switches, socket buffer copies, and checksum overhead.
> Short-Circuit Local Reads allow the client to read the raw block file directly from the local disk filesystem (ext4/xfs).
> The client sends a request to the local DataNode over a UNIX domain socket; the DataNode validates permissions and passes the open file descriptor directly to the client process via `SCM_RIGHTS`.
> The client reads data directly from the kernel page cache at raw storage speeds (gigabytes per second), bypassing the DataNode daemon and networking stacks entirely.

> [!question]
> What is "Speculative Execution" in Hadoop YARN, and how does it mitigate the straggler problem in distributed batch processing?

> [!success]- Answer
> In a large distributed batch job running thousands of parallel tasks, total job completion time is governed by the slowest running task (a "straggler").
> Stragglers are typically caused by hardware degradation (failing disk drives with bad sectors, CPU thermal throttling, or bad network cables).
> Speculative Execution is a fault-mitigation feature: YARN's ApplicationMaster monitors task completion rates across the cluster.
> If it detects that a specific task is progressing significantly slower than the cluster median, it does not kill the slow task; instead, it launches an identical duplicate "speculative" copy of that task on a different, healthy node.
> Whichever task finishes first commits its output to HDFS, and the slower copy is terminated.
> This prevents single hardware degradation events from stalling multi-hour batch pipelines.

> [!question]
> Why have modern cloud architectures largely replaced on-premises HDFS with cloud object stores like Amazon S3 and Google Cloud Storage?

> [!success]- Answer
> Modern architectures have migrated from HDFS to cloud object stores primarily due to the Decoupling of Compute and Storage.
> In HDFS, storage and compute are co-located on the same physical servers: scaling storage capacity requires purchasing and running more compute nodes, leading to massive underutilization and high operational costs.
> Cloud object stores (S3, GCS) decouple storage from compute: petabytes of data sit in low-cost, durable object storage, while compute clusters (Spark, Trino, EMR) are spun up ephemerally on-demand, scaled dynamically during queries, and terminated immediately when jobs complete.
> Furthermore, cloud object stores eliminate NameNode memory scaling limits, eliminate complex multi-rack HDFS maintenance, provide built-in 11 9's durability via multi-AZ erasure coding, and deliver lower total cost of ownership.

> [!question]
> What is HDFS NameNode SafeMode, what specific conditions govern entering and leaving SafeMode during cluster startup, and why is write activity prohibited during this state?

> [!success]- Answer
> SafeMode is an administrative and startup state during which the NameNode operates in strict read-only mode, prohibiting any modifications to the filesystem directory tree and blocking block replication.
> When the NameNode boots, it loads the directory structure from `fsimage` and replays the `edits` log into memory, but it does not persistently store block-to-DataNode mappings.
> Instead, it must wait for all DataNodes to connect and transmit their initial Block Reports detailing which blocks reside on their local disks.
> The NameNode automatically exits SafeMode only after three thresholds are met: (1) the percentage of safely replicated blocks meets or exceeds `dfs.namenode.safemode.threshold-pct` (default 99.9%); (2) the number of live DataNodes meets `dfs.namenode.safemode.min.datanodes` (default 0); and (3) an extension period (`dfs.namenode.safemode.extension`, default 30 seconds) has elapsed to allow late block reports to arrive.
> Writes and deletes are strictly prohibited because modifying the namespace while block locations are incomplete would lead to incorrect replication decisions, spurious block duplication, or accidental data loss.

> [!question]
> How does HDFS NameNode High Availability prevent the "Split-Brain" scenario where two NameNodes both believe they are Active, and what role do ZooKeeper and fencing scripts play?

> [!success]- Answer
> Split-Brain occurs when a network partition isolates the Active NameNode from the cluster, causing the Standby NameNode to promote itself to Active while the former Active continues accepting write requests, resulting in catastrophic metadata divergence and filesystem corruption.
> HDFS HA prevents split-brain using a combination of Quorum Journal Manager (QJM) consensus, ZooKeeper Failover Controllers (ZKFC), and Node Fencing.
> For metadata writes, the Active NameNode must write transactions to a quorum of JournalNodes using a Paxos-style protocol; an epoch number is assigned upon election, and JournalNodes automatically reject writes from any NameNode bearing an older epoch.
> ZooKeeper manages automated failover via ZKFC processes using ephemeral znodes (`ActiveBreadCrumb`); the active lock can only be held by one node at a time.
> Before promoting the Standby NameNode to Active, ZKFC triggers Fencing: it executes a configured fencing script (such as `sshfence` to kill the remote NameNode process, or power management fencing via IPMI/STONITH to physically power off the machine), guaranteeing that the previous Active is dead before the new Active begins servicing traffic.

## Related Concepts and Wikilinks

- [[MapReduce-Architecture]] - Foundational computational model powering Hadoop batch execution.
- [[Apache-Spark]] - Next-generation in-memory computing engine superseding MapReduce.
- [[Amazon-S3-and-Object-Storage]] - Cloud-native object storage replacing local HDFS filesystems.
- [[Apache-ZooKeeper]] - Coordination service powering HDFS NameNode automatic failover.
- [[Apache-Mesos]] - Datacenter cluster manager competing with Hadoop YARN.
- [[Partitioning-and-Sharding]] - Block partitioning and distributed storage architectures.

## Further Reading and References

- White, Tom. *Hadoop: The Definitive Guide* (4th Edition). O'Reilly Media, 2015.
- Shvachko, Konstantin, et al. "The Hadoop Distributed File System." *IEEE 26th Symposium on Mass Storage Systems and Technologies (MSST)*, 2010.
- Vavilapalli, Vinod Kumar, et al. "Apache Hadoop YARN: Yet Another Resource Negotiator." *ACM Symposium on Cloud Computing (SoCC)*, 2013.
- Dean, Jeffrey, and Sanjay Ghemawat. "MapReduce: Simplified Data Processing on Large Clusters." *Communications of the ACM*, 2008.
- Ghemawat, Sanjay, Howard Gobioff, and Shun-Tak Leung. "The Google File System." *ACM SOSP*, 2003.
