# Chapter 84: Distributed C++

A distributed system is one where a message between two components can be lost, delayed, duplicated, or reordered, and where any participant can fail independently while the others keep running. Moving C++ from a single process into this world changes the cost model by orders of magnitude — a function call is nanoseconds, a network round trip is tens of microseconds to milliseconds — and introduces failure modes (partial failure, split brain, inconsistency) that have no analogue in single-process code. This chapter covers the three pillars every distributed C++ system needs: efficient serialization, remote procedure calls, and consensus, each with the cost model and correctness hazards that govern its use.

## Chapter Roadmap

- 84.1 The Distributed Cost Model and Failure Model
- 84.2 Serialization: Packing Data for the Wire
- 84.3 Remote Procedure Calls
- 84.4 RPC Delivery Semantics and Idempotency
- 84.5 Consensus: Agreeing Under Failure (Raft)
- 84.6 Hazards: Partial Failure, Split Brain, and the Fallacies

---

## 84.1 The Distributed Cost Model and Failure Model

The first thing to internalise is the latency gap. A local function call is ~1 ns; a main-memory access ~100 ns; a same-datacenter network round trip ~50–500 μs; a cross-continent round trip ~100+ ms. The network is **5–8 orders of magnitude** slower than a function call.

The second is the failure model. In a single process, a function either returns or the whole process crashes. In a distributed system there is a third outcome: **partial failure** — the call was sent, but you never learn whether it arrived, executed, or replied, because the network or the remote node failed somewhere in between.

> **Why this matters.** These two facts drive every design decision in distributed C++. The latency gap means you **batch** (amortise the round trip over many operations), **pipeline** (issue many requests without waiting), and **co-locate** (keep chatty components together). The partial-failure model means every remote operation must define what happens on timeout, retry, and duplicate — questions that simply do not exist for a local call. A distributed system that treats remote calls like local calls (the first of the "fallacies of distributed computing": *the network is reliable*) is a system that corrupts data the first time a packet drops.

---

## 84.2 Serialization: Packing Data for the Wire

To send a C++ object over a network you must **serialize** it — convert in-memory representation into a flat byte sequence — and **deserialize** it on the other side. In-memory pointers, padding, and endianness do not survive the wire, so they must be encoded explicitly.

```cpp
// Min standard: C++17. Portable. Illustrative length-prefixed binary serializer.
#include <vector>
#include <cstdint>
#include <cstring>
#include <string>
#include <type_traits>

class Buffer {
    std::vector<uint8_t> data_;
public:
    template <typename T>
    void write(const T& val) {
        static_assert(std::is_trivially_copyable_v<T>);          // safe to memcpy
        const auto* p = reinterpret_cast<const uint8_t*>(&val);
        data_.insert(data_.end(), p, p + sizeof(T));
    }
    void write_string(const std::string& s) {
        write<uint32_t>(static_cast<uint32_t>(s.size()));        // length prefix
        const auto* p = reinterpret_cast<const uint8_t*>(s.data());
        data_.insert(data_.end(), p, p + s.size());
    }
    const uint8_t* begin() const { return data_.data(); }
    size_t size() const { return data_.size(); }
};
```
*Listing 84.1 — A length-prefixed binary serializer. The `is_trivially_copyable` check gates the `memcpy`-style write.*

> **Why this matters / cost model.** The `static_assert(is_trivially_copyable_v<T>)` is the correctness lynchpin: you may bit-copy an `int` or a POD struct, but *not* a `std::string` or anything with pointers — those must be serialized field by field (hence `write_string` sends length + bytes, not the `std::string` object's pointer). Real systems also handle **endianness** (network byte order) and **schema evolution** (adding a field without breaking old peers), which is why production code uses Protocol Buffers, FlatBuffers, Cap'n Proto, or SBE rather than raw `memcpy`. The format choice is a cost-model decision: Protobuf is compact and schema-evolvable but requires a parse step; FlatBuffers/Cap'n Proto allow *zero-copy* access (read fields directly from the received buffer, no deserialization pass) — the right choice for low-latency paths where the parse cost dominates.

---

## 84.3 Remote Procedure Calls

A **remote procedure call (RPC)** makes calling a function on another machine look like a local call. The client invokes a **stub** that serializes the arguments and a function identifier, sends them, and blocks (or returns a future) until the reply arrives; the server deserializes, dispatches to the real function, and sends the result back.

```cpp
// Min standard: C++11. Conceptual stub. Real frameworks generate this from an IDL.
// User writes:   auto result = service.Add(5, 3);
// Generated stub:
int Add(int a, int b) {
    Buffer buf;
    buf.write<uint32_t>(101);          // function ID for 'Add'
    buf.write(a);
    buf.write(b);
    return network.send_and_wait(buf); // serialize, send, block for reply, deserialize
}
```
*Listing 84.2 — An RPC client stub. The function ID selects the remote handler; arguments are serialized.*

> **Why this matters.** RPC's value is that it lets you structure a distributed system as ordinary function calls — but its danger is the same: the abstraction *hides* the network, tempting you to forget the 100-μs latency and the partial-failure mode. A local `Add(5,3)` cannot time out, be retried, or execute twice; the remote one can do all three. Production frameworks (gRPC, Thrift, Cap'n Proto RPC) generate stubs from an interface definition language (IDL), handle connection management and flow control, and — crucially — surface timeouts and errors the local-call illusion would hide. Treat every RPC signature as returning "result *or* failure," never just "result."

---

## 84.4 RPC Delivery Semantics and Idempotency

Because of partial failure, every RPC has a **delivery semantic**:

| Semantic | Behaviour on failure | Cost |
|---|---|---|
| **At-most-once** | Never retry; the call may not happen | Simple; may lose operations |
| **At-least-once** | Retry until acknowledged; may execute *more than once* | Needs idempotency or duplication |
| **Exactly-once** | Appears to happen exactly once | Expensive; needs dedup + state |

> **Why this matters.** This is the single most consequential distributed-systems decision and the one most often gotten wrong. If a client sends "transfer \$100," times out, and retries, did the first attempt execute? You cannot know. With **at-least-once** delivery, the retry may double the transfer. The fix is **idempotency**: design the operation so that executing it twice has the same effect as once (e.g. "set balance to X" rather than "add \$100," or attach a unique request ID the server deduplicates). True **exactly-once** is not achievable at the network layer — it is *constructed* from at-least-once delivery plus idempotent operations or server-side deduplication. Every retry path in distributed C++ must answer: *is this operation safe to execute twice?*

---

## 84.5 Consensus: Agreeing Under Failure (Raft)

When multiple nodes must agree on a single value or an ordered log of operations *despite* node failures, they run a **consensus** algorithm. **Raft** is the most widely-implemented: nodes elect a **leader** that orders all operations; followers replicate the leader's log; a value is **committed** once a majority (quorum) has stored it.

```cpp
// Min standard: C++11. Raft node state sketch.
enum class State { Follower, Candidate, Leader };

struct Node {
    State state = State::Follower;
    int   current_term = 0;     // monotonically increasing election term
    int   voted_for = -1;       // who this node voted for this term
    int   my_id;

    void on_election_timeout() {
        if (state == State::Follower) {
            state = State::Candidate;
            ++current_term;          // start a new term
            voted_for = my_id;       // vote for self
            request_votes();         // ask peers; become Leader on majority
        }
    }
    void request_votes();            // RPC to all peers
};
```
*Listing 84.3 — Raft leader-election state. A node that hears no leader becomes a candidate and stands for election.*

> **Why this matters / cost model.** Consensus is how distributed systems get a *single source of truth* — a replicated state machine that survives a minority of nodes failing. The cost is latency and availability trade-offs governed by **quorums**: committing requires a round trip to a majority, so a 5-node cluster tolerates 2 failures but pays a majority round trip per commit. The **term** number is the mechanism that prevents two leaders from both acting on stale information: a higher term always wins, so a partitioned old leader is harmlessly superseded. Raft (and Paxos) underpin etcd, ZooKeeper, CockroachDB, and Kafka's metadata — anywhere a distributed system needs reliable agreement. The lesson: consensus is *expensive* (a majority round trip), so use it for the small amount of critical metadata that must be consistent, not for the bulk data path.

---

## 84.6 Hazards: Partial Failure, Split Brain, and the Fallacies

The "fallacies of distributed computing" are the false assumptions that sink naive distributed C++:

- *The network is reliable* — packets drop; every send can fail.
- *Latency is zero* — round trips are 5–8 orders of magnitude slower than calls.
- *Bandwidth is infinite* — large payloads serialize slowly and saturate links.
- *The network is secure* — assume eavesdropping and tampering.
- *Topology doesn't change* — nodes come and go; addresses change.

Two specific hazards dominate:

- **Partial failure.** A node may have received and executed your request even though you got a timeout. Always design for "don't know" as a third outcome (§84.4).
- **Split brain.** A network partition can leave two halves each believing it is in charge. Quorum-based consensus (§84.5) prevents this: a partition without a majority cannot commit, so it cannot diverge.

> **The discipline.** Distributed C++ is single-process C++ plus an adversary — the network — that can delay, drop, duplicate, and reorder, and components that fail independently. The single-process performance disciplines of this volume still apply (serialization is on the hot path; zero-copy formats matter; allocation-free parsing matters), but they sit beneath a layer of *failure-aware* design: idempotent operations, explicit timeouts, quorum consensus for the truth, and the assumption that anything that can fail will. Build the fast local node using the rest of this volume; build the distributed layer assuming every message is a gamble.
