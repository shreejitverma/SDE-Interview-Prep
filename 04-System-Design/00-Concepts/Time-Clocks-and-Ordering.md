---
type: concept
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources:
  - "Time, Clocks, and the Ordering of Events in a Distributed System, Leslie Lamport, CACM 1978"
  - "Spanner: Google's Globally Distributed Database, Corbett et al., OSDI 2012"
---

# Time, Clocks, and Ordering

## TL;DR

A wall clock on two machines is not a single clock.
If you sort events by those timestamps, you will drop a write that actually happened later, or you will invent an order that never existed.
Use a clock that the protocol maintains, and use the wall clock only when the protocol also bounds its error.

## What each clock can say

| Clock | Safe use | Unsafe use |
|---|---|---|
| Wall clock (`CLOCK_REALTIME`) | Human display, certificates, loose TTLs | Deciding which of two writes wins |
| Monotonic clock | Durations and timeouts on one machine | Comparing two machines |
| Lamport clock | "If A caused B, then L(A) is less than L(B)" | Detecting that two events were concurrent |
| Vector clock | Detecting concurrency and causality | Clusters with a huge, changing replica set |
| Hybrid logical clock | A physical timestamp that still respects causality | A proof of linearizability by itself |
| TrueTime | External consistency, after a commit wait | A normal NTP deployment |

Lamport's condition is one direction.
If event A happened before event B in the causal order, the Lamport timestamp of A is smaller.
The converse is false.
A smaller timestamp does not mean A caused B.

A vector clock keeps one counter per replica.
On send, the sender increments its own slot.
On receive, each slot becomes the max of the local and incoming values, and then the receiver increments its own slot.
Event A happened before B when every slot of A is less than or equal to B and at least one slot is strictly smaller.
Otherwise the events are concurrent.
Concurrent events are exactly the ones last-write-wins will destroy if you collapse them with a wall clock.

## Worked Lamport step

```python
def on_send(clock: int) -> int:
    return clock + 1

def on_receive(local: int, incoming: int) -> int:
    return max(local, incoming) + 1
```

Start both nodes at 0.
Node A sends at 1.
Node B, which has already done local work up to 5, receives that message and steps to 6.
The receive happens after the send in causal order, and the clock says so.
Two nodes that never exchange a message can both sit at timestamp 4.
Nothing in the number 4 says which write the database should keep.

## TrueTime and commit wait

Spanner's TrueTime API returns an interval `[earliest, latest]` that is guaranteed to contain the true current time.
The width is the uncertainty ε.
A commit waits until the leader's current earliest time is past the timestamp it assigned.
After that wait, every other replica agrees the timestamp is in the past.
That is what external consistency costs: about one uncertainty interval on the write, which is why the clock infrastructure matters.
[[CockroachDB-Distributed-SQL]] uses a hybrid logical clock instead, and its consistency story is not Spanner's story.
Do not describe them as the same mechanism.

## Pitfalls

- NTP can step a wall clock backward.
  A Snowflake generator that trusts that clock will reuse ids or stall.
  [[04-Distributed-ID-Generator/design|The id study]] treats this as a first-class failure.
- Last-write-wins with client-supplied timestamps lets a client with a fast clock erase everyone else's writes.
- Vector clocks grow with replicas and with conflicts you forgot to garbage-collect.
- A lease that uses a wall clock on the holder and a wall clock on the storage node can overlap if either clock jumps.
  [[18-Distributed-Lock/design|The lock study]] requires a fencing token for that reason.

## Interview questions

1. Draw two concurrent writes that a Lamport clock orders anyway.
2. Why does Spanner wait after assigning a timestamp.
3. When is a hybrid logical clock enough, and when do you still need a consensus log.
4. A replica's NTP offset is 200 ms. What breaks in last-write-wins.

## Further reading

- Lamport, CACM 1978.
- Spanner, OSDI 2012, sections on TrueTime and commit wait.
- [[Consensus-and-Failure-Detection]] for the log that gives you a total order when clocks cannot.
