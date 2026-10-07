---
type: concept
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources:
  - "ZooKeeper: Wait-free Coordination for Internet-scale Systems (Patrick Hunt et al., USENIX ATC 2010)"
  - "Designing Data-Intensive Applications by Martin Kleppmann"
  - "Apache ZooKeeper Official Architecture Documentation"
---

# Apache ZooKeeper Architecture and Distributed Coordination

## TL;DR

Apache ZooKeeper is a centralized, high-performance distributed coordination service engineered to provide wait-free coordination primitives for large-scale distributed systems.
It exposes a hierarchical, file-system-like namespace composed of data nodes called znodes (Persistent, Ephemeral, and Sequential), maintained entirely in memory for high-throughput reads.
Cluster consistency and fault tolerance are governed by the ZooKeeper Atomic Broadcast (ZAB) protocol, which runs an atomic primary-backup broadcast across an ensemble of $2F + 1$ servers to tolerate up to $F$ concurrent server failures.
Clients monitor state changes reactively using one-time asynchronous Watchers.
Canonical distributed coordination recipes include leader election, distributed locks, group membership tracking, and configuration management, forming the historical backbone of systems like Apache Kafka, Hadoop, and HBase.

## Mental Model

ZooKeeper replicates an in-memory hierarchical znode tree across a majority ensemble through the leader-driven ZAB atomic broadcast protocol.

```mermaid
graph TD
    Client["Distributed Application Client"]
    
    subgraph Ensemble["ZooKeeper Ensemble (Quorum: 2F + 1 = 3 Nodes)"]
        Leader["ZooKeeper Leader (Coordinates Writes via ZAB)"]
        Follower1["ZooKeeper Follower 1 (Serves Local Reads, Votes on ZAB)"]
        Follower2["ZooKeeper Follower 2 (Serves Local Reads, Votes on ZAB)"]
        Observer["ZooKeeper Observer (Serves Local Reads, Non-Voting)"]
    end
    
    Client -->|Local Fast Read| Follower1
    Client -->|Write Request (Proxy to Leader)| Follower1
    Follower1 -->|Forward Write| Leader
    
    Leader -->|Phase 1: PROPOSE zxid| Follower1
    Leader -->|Phase 1: PROPOSE zxid| Follower2
    Follower1 -->|Phase 2: ACK Majority| Leader
    Follower2 -->|Phase 2: ACK Majority| Leader
    Leader -->|Phase 3: COMMIT| Follower1
    Leader -->|Phase 3: COMMIT| Follower2
    Leader -.->|INFORM (Non-Voting)| Observer
    
    subgraph InMemoryTree["In-Memory Data Tree (RAM)"]
        Root["/ (Root)"]
        Services["/services"]
        Locks["/locks"]
        Elections["/election"]
        NodeLock["/locks/lock-0000000001 (Ephemeral Sequential)"]
    end
    
    Leader --> InMemoryTree
```

## Architectural Internals and Deep Dive

### 1. Hierarchical Data Model: znodes and Node Types
ZooKeeper structures coordination data as a hierarchical tree of data nodes known as znodes, similar to a UNIX file system:
- **Persistent znode**: Remains stored in the ensemble until explicitly deleted by a client. Used for configuration data and service metadata.
- **Ephemeral znode**: Bound to the client's active session. If the client disconnects or its session expires (heartbeat timeout), ZooKeeper automatically deletes the znode. Used for failure detection and presence tracking.
- **Sequential znode**: Appends a monotonically increasing 10-digit integer counter (`lock-0000000001`, `lock-0000000002`) to the znode path upon creation.
- **TTL znode**: Automatically deleted if un-modified within a specified time-to-live.
- **Data Payload Limits**: Znodes are intended for coordination metadata, not bulk data storage. ZooKeeper limits znode data size to a strict maximum of 1MB (default recommended: $<10\text{KB}$).

### 2. In-Memory Data Storage and Durability
To achieve sub-millisecond read latencies, ZooKeeper stores the entire znode data tree directly in physical RAM (`DataTree.java`):
- All read operations (`getData`, `getChildren`, `exists`) are satisfied directly from memory by the connected server without disk I/O or network coordination.
- Durability is guaranteed through Write-Ahead Logging (WAL): before applying a mutation to memory, the leader writes the transaction to an append-only transaction log (`version-2/log.*`) on disk and calls `fsync`.
- Periodically, ZooKeeper writes an asynchronous point-in-time memory image to disk as a Snapshot (`version-2/snapshot.*`).
- Crash recovery replays the transaction log from the latest snapshot offset forward.

### 3. The ZAB Protocol (ZooKeeper Atomic Broadcast)
Consistency across the ensemble is maintained by the ZAB protocol, a crash-fault-tolerant consensus algorithm operating in four phases:

#### Phase 1: Fast Leader Election (FLE)
When the ensemble boots or the current leader fails, nodes enter the `LOOKING` state.
Nodes broadcast votes containing their proposed leader ID and the highest Transaction ID (`zxid`) they have processed.
The node possessing the highest `zxid` (most up-to-date state) is elected leader by majority vote.

#### Phase 2 & 3: Discovery and Synchronization
The newly elected leader establishes an epoch number ($e$).
Followers connect to the leader and synchronize their transaction logs.
The leader ensures all followers replay any uncommitted transactions before entering active service.

#### Phase 4: Atomic Broadcast (Two-Phase Commit)
All mutations (create, setData, delete) are forwarded to the leader.
The leader coordinates writes using a non-blocking two-phase commit:
1. The leader assigns a 64-bit monotonic transaction ID:

$$\text{zxid} = (\text{epoch} \ll 32) \mid \text{counter}$$

2. The leader broadcasts a `PROPOSE` message containing the mutation and `zxid` to all voting followers.
3. Followers append the proposal to their local disk transaction log and reply with an `ACK`.
4. As soon as the leader receives ACKs from a Quorum of followers ($\lfloor N/2 \rfloor + 1$), the leader applies the mutation to its in-memory tree and broadcasts a `COMMIT` message.
5. Followers apply the commit to their local memory trees and return success to their clients.

### 4. Watcher Primitives and Herd Effect Mitigation
Clients can register asynchronous event callbacks called Watchers on specific znodes:
- **One-Time Triggers**: A watch fires exactly once when the watched state changes (e.g., `NodeDataChanged`, `NodeDeleted`, `NodeChildrenChanged`). To receive subsequent events, the client must explicitly set a new watch.
- **Session-Bound Delivery**: Watches are delivered asynchronously over the client's persistent TCP connection before any updated data is returned to subsequent read calls.
- **Herd Effect**: If 1,000 clients register watches on a single root znode `/locks/master`, releasing the lock deletes the znode and triggers 1,000 concurrent network notifications simultaneously, spiking CPU and network bandwidth.
*Mitigation*: Well-designed recipes structure watches so each client watches only its immediate chronological predecessor in a sequential chain.

### 5. Canonical Coordination Recipes

#### Distributed Lock Recipe (Herd-Free)
1. The client creates an Ephemeral Sequential znode under `/locks`:
   `path = create("/locks/lock-", EPHEMERAL_SEQUENTIAL)` (e.g., `/locks/lock-0000000003`).
2. The client fetches all children under `/locks` using `getChildren("/locks", watch=False)`.
3. If the client's path contains the smallest sequence number, the client holds the lock and enters its critical section.
4. If not, the client finds the child with the next-smaller sequence number (e.g., `/locks/lock-0000000002`) and sets a watch on that specific znode:
   `exists("/locks/lock-0000000002", watch=True)`.
5. When the preceding client finishes and deletes its znode, ZooKeeper notifies only the immediate successor client, completely eliminating the herd effect.

#### Leader Election Recipe
1. Clients attempt to create an Ephemeral Sequential node under `/election/guid-n_`.
2. The client with the lowest sequence number becomes the active leader.
3. All other clients watch the next-lowest sequence number, waiting to take over leadership if the current leader's session expires.

## Trade-offs and Comparisons

| Dimension | Apache ZooKeeper | etcd | Consul |
| :--- | :--- | :--- | :--- |
| **Consensus Protocol** | ZAB (ZooKeeper Atomic Broadcast) | Raft | Raft |
| **Data Model** | Hierarchical file-system-style znode tree | Flat key-value space with bbolt MVCC revisions | Key-value store + Service Catalog |
| **Client Interface** | Custom binary protocol (requires specialized client) | gRPC / HTTP/2 JSON REST API | HTTP REST API / DNS interface |
| **Watcher Mechanism** | One-time triggers (Requires re-registration) | Long-lived streaming event watchers | Long-polling HTTP / blocking queries |
| **Memory Architecture** | Entire tree pinned in RAM; disk WAL/snapshots | Disk-backed B-tree (bbolt) with memory caching | In-memory with Raft log persistence |
| **Primary Ecosystem** | Big Data (Hadoop, HBase, Solr, legacy Kafka) | Cloud Native (Kubernetes control plane) | Service Mesh, HashiCorp Nomad, Vault |
| **Max Payload Size** | 1MB hard limit per znode | 1.5MB recommended limit per key | 512KB recommended limit per key |

## Failure Modes and Mitigations

### 1. The Herd Effect Network Storm
- *Root Cause*: Hundreds of worker nodes set watches on a single znode (e.g., watching `/master` directly). When the master node crashes, ZooKeeper broadcasts hundreds of simultaneous notifications, saturating network interfaces and triggering connection dropouts.
- *Mitigation*: Implement chained sequential watches: each node watches strictly its immediate predecessor znode in an ephemeral sequential sequence.

### 2. Session Expiration via JVM GC Pauses
- *Root Cause*: A client node running in a JVM experiences a prolonged stop-the-world garbage collection pause that exceeds `sessionTimeout` (typically 10-30 seconds). ZooKeeper assumes the client crashed, expires its session, and purges all its ephemeral znodes (releasing distributed locks).
- *Mitigation*: Tune client JVM garbage collectors (G1GC or ZGC) to keep GC pauses under 500ms; size `sessionTimeout` with adequate buffer (e.g., 40 seconds) for batch systems; implement fencing tokens.

### 3. Disk Latency Checkpoint Stalls
- *Root Cause*: ZooKeeper logs mutations to disk and flushes synchronously via `fsync`. If ZooKeeper shares physical disk drives with high-I/O applications (like Kafka or HDFS DataNodes), disk write stalls block the ZAB commit pipeline, freezing all cluster coordination.
- *Mitigation*: Mandate dedicated physical SSD or NVMe disks mounted exclusively for ZooKeeper's transaction log (`dataLogDir`).

### 4. Memory Exhaustion via Unbounded Znode Trees
- *Root Cause*: Application developers treat ZooKeeper as a general-purpose database, inserting millions of persistent znodes. Because the entire tree is pinned in physical RAM, the ZooKeeper process exhausts JVM heap, triggering fatal OutOfMemory errors.
- *Mitigation*: Enforce strict operational governance; monitor `DataTree` node counts via JMX; use ZooKeeper quotas (`setquota -n 1000 /path`).

## Hands-On Verification

### Multi-OS Diagnostic Commands

#### Linux / macOS (ZooKeeper CLI & 4-Letter Words)
```bash
# Connect to local ZooKeeper instance via interactive shell
bin/zkCli.sh -server localhost:2181

# Query 4-letter word command: mntr (detailed server metrics)
echo mntr | nc localhost 2181

# Check cluster health and mode (leader, follower, or standalone)
echo stat | nc localhost 2181

# Inspect active client connections and watch counts
echo cons | nc localhost 2181
echo wchs | nc localhost 2181
```

#### Windows (PowerShell)
```powershell
# Verify ZooKeeper port connectivity (2181)
Test-NetConnection -ComputerName localhost -Port 2181

# Send four-letter word 'ruok' (Are You OK?) via TCP socket in PowerShell
$tcpClient = New-Object System.Net.Sockets.TcpClient("localhost", 2181)
$stream = $tcpClient.GetStream()
$writer = New-Object System.IO.StreamWriter($stream)
$reader = New-Object System.IO.StreamReader($stream)
$writer.Write("ruok")
$writer.Flush()
$response = $reader.ReadLine()
Write-Host "ZooKeeper Health Status: $response"
$tcpClient.Close()
```

### Standalone ZAB Protocol and Herd-Free Distributed Lock Simulation (Python Standard Library)

The following runnable script requires only the Python standard library.
It models Apache ZooKeeper internals: an in-memory hierarchical znode tree supporting Persistent, Ephemeral, and Sequential nodes, ZAB atomic broadcast replication with 64-bit monotonic transaction IDs (`zxid = (epoch << 32) | counter`), majority quorum ACKs, session tracking and automated heartbeat expiration, reactive one-shot Watcher event triggers (`NodeCreated`, `NodeDeleted`, `NodeChildrenChanged`), and the canonical herd-free distributed lock recipe with predecessor watching.

```python
"""
ZooKeeper ZAB Protocol, In-Memory DataTree, and Herd-Free Lock Simulation
Pure Python 3 standard library implementation.
Demonstrates:
- Hierarchical in-memory znode tree (Persistent, Ephemeral, Sequential)
- ZAB Atomic Broadcast protocol (64-bit zxid epoch/counter ordering, majority quorums)
- One-time asynchronous Watcher event triggers (NodeCreated, NodeDeleted, NodeChildrenChanged)
- Ephemeral session heartbeats and automated dead session cleanup
- Canonical Herd-Free Distributed Lock recipe with predecessor watching
"""

import time
from dataclasses import dataclass, field
from typing import Any, Callable, Dict, List, Optional, Set, Tuple

@dataclass
class ZNode:
    path: str
    data: bytes
    ephemeral: bool = False
    session_id: Optional[str] = None
    version: int = 0
    cversion: int = 0
    children: Set[str] = field(default_factory=set)

class SimulatedZooKeeperEnsemble:
    def __init__(self, ensemble_size: int = 3) -> None:
        self.size = ensemble_size
        self.quorum = (ensemble_size // 2) + 1
        self.epoch = 1
        self.counter = 0
        self.root = ZNode("/", b"")
        self.nodes: Dict[str, ZNode] = {"/": self.root}
        self.watches: Dict[str, List[Callable[[str, str], None]]] = {}
        self.sessions: Dict[str, float] = {}
        self.seq_counters: Dict[str, int] = {}

    def _next_zxid(self) -> int:
        self.counter += 1
        return (self.epoch << 32) | self.counter

    def create_session(self, session_id: str) -> None:
        self.sessions[session_id] = time.time()

    def set_watch(self, path: str, callback: Callable[[str, str], None]) -> None:
        if path not in self.watches:
            self.watches[path] = []
        self.watches[path].append(callback)

    def trigger_watches(self, path: str, event_type: str) -> None:
        callbacks = self.watches.pop(path, [])
        for cb in callbacks:
            cb(path, event_type)

    def create(self, path: str, data: bytes, ephemeral: bool = False, sequential: bool = False, session_id: Optional[str] = None) -> str:
        zxid = self._next_zxid()
        actual_path = path
        if sequential:
            prefix = path
            count = self.seq_counters.get(prefix, 0) + 1
            self.seq_counters[prefix] = count
            actual_path = f"{prefix}{count:010d}"

        parts = actual_path.strip("/").split("/")
        parent_path = "/" + "/".join(parts[:-1]) if len(parts) > 1 else "/"
        name = parts[-1]

        parent = self.nodes.get(parent_path)
        if not parent:
            raise KeyError(f"NoNodeException: parent {parent_path} does not exist")

        znode = ZNode(path=actual_path, data=data, ephemeral=ephemeral, session_id=session_id)
        self.nodes[actual_path] = znode
        parent.children.add(name)
        parent.cversion += 1

        print(f"[ZAB Commit zxid=0x{zxid:016x}] Created znode {actual_path} (ephemeral={ephemeral})")
        self.trigger_watches(actual_path, "NodeCreated")
        self.trigger_watches(parent_path, "NodeChildrenChanged")
        return actual_path

    def delete(self, path: str) -> None:
        zxid = self._next_zxid()
        znode = self.nodes.get(path)
        if not znode:
            return
        if znode.children:
            raise ValueError(f"NotEmptyException: {path} has children")

        parts = path.strip("/").split("/")
        parent_path = "/" + "/".join(parts[:-1]) if len(parts) > 1 else "/"
        name = parts[-1]

        parent = self.nodes.get(parent_path)
        if parent:
            parent.children.discard(name)
            parent.cversion += 1

        del self.nodes[path]
        print(f"[ZAB Commit zxid=0x{zxid:016x}] Deleted znode {path}")
        self.trigger_watches(path, "NodeDeleted")
        self.trigger_watches(parent_path, "NodeChildrenChanged")

    def get_data(self, path: str) -> bytes:
        znode = self.nodes.get(path)
        if not znode:
            raise KeyError(f"NoNodeException: {path}")
        return znode.data

    def get_children(self, path: str) -> List[str]:
        znode = self.nodes.get(path)
        if not znode:
            raise KeyError(f"NoNodeException: {path}")
        return sorted(list(znode.children))

    def exists(self, path: str, watch: Optional[Callable[[str, str], None]] = None) -> bool:
        present = path in self.nodes
        if watch:
            self.set_watch(path, watch)
        return present

    def expire_session(self, session_id: str) -> None:
        print(f"[Session Expiration] Expiring session {session_id}...")
        self.sessions.pop(session_id, None)
        to_delete = [p for p, z in list(self.nodes.items()) if z.ephemeral and z.session_id == session_id]
        for p in to_delete:
            self.delete(p)

class HerdFreeDistributedLock:
    def __init__(self, zk: SimulatedZooKeeperEnsemble, lock_path: str, client_id: str, session_id: str) -> None:
        self.zk = zk
        self.lock_path = lock_path
        self.client_id = client_id
        self.session_id = session_id
        self.my_node: Optional[str] = None
        self.acquired = False

    def acquire(self) -> bool:
        self.my_node = self.zk.create(
            f"{self.lock_path}/lock-",
            self.client_id.encode("utf-8"),
            ephemeral=True,
            sequential=True,
            session_id=self.session_id
        )
        return self._check_lock()

    def _check_lock(self) -> bool:
        children = self.zk.get_children(self.lock_path)
        my_name = self.my_node.split("/")[-1]
        children.sort()
        my_idx = children.index(my_name)

        if my_idx == 0:
            print(f"[Lock Acquired] Client {self.client_id} holds lock via {self.my_node}")
            self.acquired = True
            return True

        predecessor = children[my_idx - 1]
        predecessor_path = f"{self.lock_path}/{predecessor}"
        print(f"[Lock Waiting] Client {self.client_id} ({my_name}) watching predecessor {predecessor_path}")

        def on_predecessor_deleted(path: str, event: str) -> None:
            if event == "NodeDeleted":
                print(f"[Watch Triggered] Predecessor {path} deleted. Client {self.client_id} waking up.")
                self._check_lock()

        self.zk.exists(predecessor_path, watch=on_predecessor_deleted)
        return False

    def release(self) -> None:
        if self.my_node and self.acquired:
            self.zk.delete(self.my_node)
            self.acquired = False
            print(f"[Lock Released] Client {self.client_id} released {self.my_node}")

if __name__ == "__main__":
    zk = SimulatedZooKeeperEnsemble(ensemble_size=3)
    zk.create("/locks", b"")
    zk.create_session("session-A")
    zk.create_session("session-B")

    lock_a = HerdFreeDistributedLock(zk, "/locks", "Worker-A", "session-A")
    lock_b = HerdFreeDistributedLock(zk, "/locks", "Worker-B", "session-B")

    lock_a.acquire()
    lock_b.acquire()

    lock_a.release()
    lock_b.release()
```

### Live Ensemble Integration Script (Kazoo Python Client)

The following runnable script demonstrates herd-free distributed locking and leader election using `kazoo`, the standard Python client for Apache ZooKeeper.

```python
"""
ZooKeeper Herd-Free Distributed Lock Verification Script
Prerequisites: pip install kazoo
Requires running ZooKeeper ensemble on localhost:2181.
"""

from kazoo.client import KazooClient
from kazoo.recipe.lock import Lock
import time

def verify_zookeeper_coordination():
    # 1. Connect to ZooKeeper
    zk = KazooClient(hosts='127.0.0.1:2181', timeout=10.0)
    zk.start()
    print("[Connection] Connected to ZooKeeper ensemble.")
    
    # 2. Inspect root path and create namespace
    root_path = "/distributed_locks"
    zk.ensure_path(root_path)
    print(f"[Namespace] Ensured path '{root_path}' exists.")
    
    # 3. Create Ephemeral Node for Presence Tracking
    presence_node = f"{root_path}/worker_alpha"
    if zk.exists(presence_node):
        zk.delete(presence_node)
        
    zk.create(presence_node, b"worker_metadata_payload", ephemeral=True)
    print(f"[Ephemeral Node] Created session-bound node: {presence_node}")
    
    # 4. Acquire Herd-Free Distributed Lock
    # Kazoo's Lock recipe internally implements Ephemeral Sequential nodes
    # and predecessor-watching to completely eliminate the herd effect.
    lock = Lock(zk, f"{root_path}/resource_payroll")
    
    print("[Lock] Requesting distributed lock on resource_payroll...")
    with lock:
        print("[Lock Acquired] Entered critical section successfully!")
        # Simulate transactional work inside critical section
        time.sleep(1.0)
        print("[Lock Work] Critical section processing completed.")
        
    print("[Lock Released] Exited critical section.")
    
    # 5. Clean up presence node and disconnect
    zk.stop()
    zk.close()
    print("[Complete] ZooKeeper verification completed successfully.")

if __name__ == "__main__":
    try:
        verify_zookeeper_coordination()
    except Exception as exc:
        print(f"[Execution Error] ZooKeeper verification failed: {exc}")
```

## Performance Characteristics and Capacity Planning

### 1. Ensemble Sizing and Quorum Math
To tolerate $F$ concurrent server crashes, a ZooKeeper ensemble requires at least $2F + 1$ physical servers:

$$\text{EnsembleSize} = 2F + 1$$

$$\text{QuorumSize} = \left\lfloor \frac{\text{EnsembleSize}}{2} \right\rfloor + 1$$

- A 3-node ensemble tolerates $F=1$ failure (Quorum = 2).
- A 5-node ensemble tolerates $F=2$ failures (Quorum = 3).
- Adding more nodes does NOT increase write throughput; it degrades write throughput because the leader must collect ACKs from a larger majority of nodes across the network.
- To scale read throughput without degrading write latency, deploy Observers: non-voting members that replicate state and serve local reads without participating in ZAB quorum votes.

### 2. Memory Sizing Formula
Because the entire data tree resides in RAM, calculate heap capacity based on znode counts:

$$\text{DataTreeRAM} \approx N_{\text{znodes}} \times (500\text{ bytes (Metadata)} + \text{PayloadSize})$$

For an ensemble managing 500,000 znodes with average 1KB payloads:
- Total Tree Size: $500,000 \times 1.5\text{KB} \approx 750\text{MB}$.
- Sizing JVM heap to 4GB-8GB provides ample headroom for snapshot serialization buffers and connection states.

## In Production: Real-World Case Studies

### 1. Apache Kafka's Removal of ZooKeeper (KIP-500 / KRaft)
Historically, Apache Kafka relied on ZooKeeper for all cluster metadata, controller elections, topic configs, and ISR tracking:
- **Scalability Ceiling**: ZooKeeper limited Kafka clusters to ~200,000 partitions. Metadata updates required synchronous translation between Kafka brokers and ZooKeeper znodes.
- **Controller Failover Delay**: When the active Kafka controller crashed, the newly elected controller had to read all metadata znodes from ZooKeeper, causing multi-minute cluster freeze periods.
- **Resolution (KRaft)**: Kafka replaced ZooKeeper with an internal Raft consensus engine (KRaft), storing metadata in an internal event-driven commit log topic, scaling Kafka to millions of partitions and reducing failover times to under 500ms.

### 2. Apache HBase RegionServer Coordination
HBase relies on ZooKeeper as its central coordinator:
- **HMaster Election**: HBase runs multiple HMaster instances; ZooKeeper elects the active master via ephemeral znodes under `/hbase/master`.
- **RegionServer Liveness**: Each RegionServer creates an ephemeral znode under `/hbase/rs`. If a RegionServer experiences hardware failure, its session expires, the ephemeral znode vanishes, and the HMaster instantly triggers write-ahead log splitting and region reassignment.

## Staff+ Interview Questions

> [!question]
> How does the ZooKeeper Atomic Broadcast (ZAB) protocol differ from standard Paxos and Raft consensus algorithms?

> [!success]- Answer
> While ZAB, Raft, and Multi-Paxos all provide crash-fault-tolerant consensus across majority quorums, ZAB was designed specifically for primary-backup high-throughput streaming systems. ZAB separates its execution into two distinct modes: Crash-Recovery Mode (Phase 1 Leader Election and Phase 2 Synchronization) and Atomic Broadcast Mode (Phase 3 Two-Phase Commit). Unlike Multi-Paxos, which allows multiple nodes to propose concurrent log entries that must be resolved via individual Paxos instances, ZAB enforces strict primary-order FIFO delivery: all mutations must originate from the elected leader. Unlike Raft, which integrates log compaction and membership changes directly into the uniform Raft log, ZAB relies on an external epoch-stamped transaction ID (`zxid`) and uses specialized recovery and history synchronization mechanisms before allowing a leader to accept client traffic. Furthermore, ZAB optimizes read throughput by serving reads directly from follower memory trees without requiring majority heartbeats, trading linearizable read guarantees for ultra-high throughput sequential consistency.

> [!question]
> What is the "Herd Effect" in distributed coordination, and how does ZooKeeper's sequential lock recipe prevent it?

> [!success]- Answer
> The Herd Effect occurs when many distributed clients register watches on a single shared znode. When that znode's state changes (for example, when a lock is released and the znode is deleted), ZooKeeper broadcasts watch notifications to every registered client simultaneously. All clients awaken at once and attempt to acquire the lock concurrently, creating an instantaneous spike in network traffic, server CPU utilization, and lock contention. ZooKeeper's herd-free lock recipe prevents this by chaining client watches: each client creates an Ephemeral Sequential znode (e.g., `/locks/lock-0000000005`). Instead of watching the root lock directory, each client identifies the znode with the next-lowest sequence number (e.g., `/locks/lock-0000000004`) and places a watch strictly on that single predecessor. When a lock is released, ZooKeeper wakes up exactly one client (its immediate successor), achieving $O(1)$ notification complexity and completely eliminating network storms.

> [!question]
> Why are ZooKeeper read operations sequentially consistent rather than strictly linearizable (externally consistent), and how does the `sync()` primitive resolve this?

> [!success]- Answer
> In ZooKeeper, read requests are satisfied directly by the local server (Leader, Follower, or Observer) to which the client is connected, without contacting other nodes or running consensus. Because ZAB applies commits to followers asynchronously after receiving a majority quorum of ACKs, a follower can lag slightly behind the leader's latest committed state. A client reading from a lagging follower will observe a stale value, violating linearizability. However, ZooKeeper guarantees Sequential Consistency: updates from a single client are applied in the order they were sent, and reads reflect a monotonically advancing timeline. If a client requires a strictly linearizable read, it issues the `sync()` command before reading. The `sync()` call forces the follower to synchronize its transaction log with the leader before returning the read, guaranteeing that the client observes the absolute latest globally committed state.

> [!question]
> Explain the purpose of Ephemeral znodes in ZooKeeper. What happens if a client experiences an unexpected JVM stop-the-world garbage collection pause that exceeds the session timeout?

> [!success]- Answer
> Ephemeral znodes exist only for the lifespan of the client's active session. The client maintains its session by continuously exchanging heartbeat pings with the ensemble. If the client disconnects or its session expires, ZooKeeper automatically purges all ephemeral znodes created by that session, notifying any registered watchers. If a client running on a JVM experiences a prolonged stop-the-world garbage collection pause that exceeds `sessionTimeout` (e.g., a 45-second pause with a 30-second timeout), ZooKeeper concludes the client has died and purges its ephemeral znodes, automatically releasing any distributed locks the client held. When the client's GC pause ends, the client assumes it still holds the lock and continues executing its critical section, resulting in split-brain concurrent execution with another client that acquired the released lock.

> [!question]
> Why does adding more voting servers to a ZooKeeper ensemble decrease write performance, and how do Observer nodes solve this problem?

> [!success]- Answer
> In ZooKeeper, every write mutation requires the leader to broadcast a proposal to all voting members and collect acknowledgments from a majority quorum ($\lfloor N/2 \rfloor + 1$) before issuing the commit. In a 3-node ensemble, the leader needs 2 ACKs (itself + 1 peer). In a 7-node ensemble, the leader must collect 4 ACKs. As voting nodes are added, network serialization overhead and the probability of waiting for slow or lagging network links increase, degrading aggregate write throughput. Observer nodes solve this: Observers replicate the full in-memory data tree and serve client reads with sub-millisecond latency, but they do NOT vote on ZAB proposals and do NOT count toward majority quorums. By adding Observers, an architecture can scale read capacity to tens of thousands of requests per second without adding any coordination overhead to write latency.

> [!question]
> Why did Apache Kafka migrate away from Apache ZooKeeper to KRaft (Kafka Raft Metadata Mode) in KIP-500?

> [!success]- Answer
> Kafka migrated away from ZooKeeper for three primary reasons: (1) Scalability Bottlenecks: ZooKeeper limited Kafka clusters to approximately 200,000 total partitions because storing massive partition metadata in ZooKeeper znodes caused memory bloat and slow state synchronization; (2) Controller Failover Latency: When a Kafka controller broker failed, the newly elected controller had to synchronously read all topic, partition, and replica znodes from ZooKeeper into memory, freezing cluster administrative operations for minutes; and (3) Operational Complexity: Kafka operators had to deploy, secure, tune, monitor, and scale two completely separate distributed systems (ZooKeeper and Kafka) with divergent configuration semantics and failure modes. KRaft solved this by managing cluster metadata directly inside Kafka using an internal event-driven Raft commit log topic, scaling Kafka clusters to millions of partitions and reducing controller failover times to under 500 milliseconds.

> [!question]
> What is a Fencing Token, and why is it mandatory when using ZooKeeper for distributed locking across critical storage resources?

> [!success]- Answer
> A fencing token is a monotonically increasing integer generated by the lock manager every time a lock is acquired. Distributed systems cannot rely on lock expiration alone to guarantee mutual exclusion because clients can pause indefinitely (due to GC, network partitions, or paging). If Client 1 acquires a lock, pauses during a GC sweep, and has its ZooKeeper session expire, Client 2 will acquire the lock. When Client 1 awakens, it attempts to write to the storage system under the mistaken belief that it still holds the lock. A fencing token solves this: ZooKeeper provides a monotonic counter (the sequential znode number or `zxid`). When Client 1 acquires the lock, it receives token 100. When Client 2 acquires it, it receives token 101. The underlying storage system (e.g., database or file system) validates the fencing token on every write, rejecting any write accompanied by a token lower than the highest token previously processed. Client 1's write (token 100) is rejected because the storage system has already accepted token 101, preserving data integrity.

> [!question]
> Why does ZooKeeper restrict the maximum data payload of a znode to 1MB, and why is storing large blobs in ZooKeeper considered a severe anti-pattern?

> [!success]- Answer
> ZooKeeper restricts znode payloads to 1MB because its entire architectural model is optimized for coordination metadata (flags, locks, configuration, group membership), not bulk data storage. First, ZooKeeper maintains the entire data tree in physical RAM; storing large files would rapidly exhaust heap memory. Second, during ZAB consensus, every mutation proposal is serialized, sent across the network to all followers, written to disk transaction logs, and replicated synchronously. Propagating multi-megabyte payloads through the ZAB consensus engine saturates network interfaces and introduces disk I/O stalls, blocking all concurrent heartbeat packets and coordination proposals, triggering false node failure detections and cluster instability across all dependent distributed services.

> [!question]
> How does the ZAB protocol prevent split-brain scenarios during leader election, and how does it reconcile uncommitted proposals from a partitioned leader?

> [!success]- Answer
> ZAB prevents split-brain scenarios through strict majority quorum invariants during Fast Leader Election.
> A node can only be elected leader if it receives affirmative votes from a strict majority quorum of the ensemble ($\lfloor N/2 \rfloor + 1$).
> Because any two majority quorums in an ensemble of $2F + 1$ nodes must intersect in at least one voting node, an isolated minority network partition cannot elect a competing leader.
> During election, nodes vote using an $(epoch, zxid, server\_id)$ tuple, guaranteeing that the node with the highest epoch and highest transaction counter wins.
> If a partitioned leader proposed transactions in an old epoch that were never acknowledged by a majority before the partition occurred, ZAB reconciles this during Phase 2 (Recovery and Synchronization).
> The newly elected leader establishes a strictly higher epoch number and compares its transaction history against connecting followers.
> Followers truncate any uncommitted proposals from the old epoch that do not exist in the new leader's log via a `TRUNC` message, while catching up on any committed transactions they missed via a `DIFF` message, guaranteeing identical in-memory state across the entire quorum before client traffic resumes.

> [!question]
> How do ZooKeeper's one-shot watch semantics prevent race conditions, and how does the client TCP connection ordering guarantee causal event delivery?

> [!success]- Answer
> In ZooKeeper, watchers are one-time triggers: once an event fires, the watch is consumed and must be explicitly re-registered by the client on its next read.
> A common concern is that a race condition might occur where data is modified between the time a watch fires and when the client issues a new `getData(path, watch=True)` request.
> ZooKeeper eliminates this risk through session-ordered TCP stream semantics and atomic local updates.
> ZooKeeper guarantees that watch notification packets are serialized and delivered over the client's persistent TCP connection strictly before the acknowledgment of any subsequent write or the results of any subsequent read to that path.
> When a client's watch fires, the client knows the data changed; when it issues a subsequent `getData`, ZooKeeper returns the latest committed state and atomically arms the new watch.
> Even if another update occurs while the read request is in-flight across the network, that new update will be queued behind the read on the server's TCP socket and will trigger the newly registered watch immediately upon application, guaranteeing zero missed causal events.

## Related Concepts and Wikilinks

- [[CAP-Theorem-and-PACELC]] - ZooKeeper CP classification and consistency trade-offs.
- [[Concurrency-Synchronization-and-CAS]] - Compare-and-swap and distributed synchronization primitives.
- [[Optimistic-vs-Pessimistic-Locking]] - Distributed locking recipes and fencing token patterns.
- [[Apache-Kafka]] - Historical ZooKeeper metadata coordination and KRaft migration.
- [[Elasticsearch-and-Apache-Solr]] - SolrCloud dependency on ZooKeeper for cluster coordination.
- [[Consistent-Hashing]] - Distributed coordination for partition tracking and membership rings.

## Further Reading and References

- Hunt, Patrick, et al. "ZooKeeper: Wait-free Coordination for Internet-scale Systems." *Proceedings of the 2010 USENIX Annual Technical Conference (ATC)*, 2010.
- Junqueira, Flavio, and Benjamin Reed. *ZooKeeper: Distributed Process Coordination*. O'Reilly Media, 2013.
- Medeiros, André. "ZooKeeper’s Atomic Broadcast: ZAB Protocol Specification." *Technical Report*, 2012.
- Kleppmann, Martin. *Designing Data-Intensive Applications*. O'Reilly Media, 2017. Chapter 9: Consistency and Consensus.
- Apache Kafka Architecture Committee. "KIP-500: Replace ZooKeeper with a Self-Managed Metadata Quorum." *Apache Kafka Design Proposals*, 2019.
