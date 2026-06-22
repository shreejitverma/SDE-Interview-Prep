# Chapter 119: Robotics and ROS2 Development

Robotics is where multiple timescales of real-time meet in one system: a navigation planner that may take hundreds of milliseconds runs alongside a motor-control loop that must execute at a kilohertz with hard deadlines, and a perception pipeline moving gigabytes of sensor data per second sits between them. C++ is the language of serious robotics — and **ROS2** (Robot Operating System 2) is its dominant framework. This chapter covers the ROS2 architecture, the zero-copy transport that makes high-bandwidth sensor data feasible, and the real-time disciplines (executors, allocators) that protect the control loop.

## Chapter Roadmap

- 119.1 The Many Timescales of Robotics
- 119.2 ROS2 Architecture and DDS
- 119.3 Zero-Copy Transport
- 119.4 Real-Time Executors and Priority
- 119.5 Allocation Discipline in the Control Loop
- 119.6 The Robotics Discipline

---

## 119.1 The Many Timescales of Robotics

A robot is not one real-time system but several, coupled. **Hard real-time** subsystems (motor control, safety interlocks) must hit microsecond-to-millisecond deadlines or the robot becomes unstable or unsafe. **Soft real-time** subsystems (navigation, planning) should be fast but can tolerate occasional lateness. **Throughput** subsystems (perception, mapping) move enormous sensor streams (LiDAR point clouds, camera frames) where bandwidth dominates.

> **Why this matters.** These timescales have *conflicting* requirements that a single naive design cannot satisfy: the control loop needs determinism (Chapter 106), the perception pipeline needs bandwidth (Chapter 99), and the planner needs compute — and they must coexist without the bandwidth-heavy perception jittering the deadline-bound control loop. The engineering of a robotics system is largely about *isolating* these timescales so each gets what it needs: the control loop runs allocation-free and lock-free on a dedicated, possibly isolated core (Chapter 96), while perception runs on other cores with zero-copy transport for its big data. C++ is chosen because it can serve *all three* — the determinism of the control loop and the throughput of perception — in one coherent system.

---

## 119.2 ROS2 Architecture and DDS

**ROS2** structures a robot as a graph of **nodes** (independent processes or components) communicating through:

- **Topics** — publish/subscribe channels for streaming data (sensor readings, commands).
- **Services** — request/response RPC for occasional queries.
- **Actions** — long-running goals with feedback (navigate-to-point).

Underneath, ROS2 runs on **DDS** (Data Distribution Service), an industrial pub/sub middleware that handles discovery, serialization, and quality-of-service (reliability, durability, deadline) policies.

> **Why this matters.** The node/topic architecture is the actor model (Chapter 78) applied to robotics: independent nodes with no shared state, communicating by messages — which gives modularity (swap a perception node without touching control) and fault isolation (a crashed node doesn't take down the system). DDS provides the transport with *quality-of-service* knobs that matter for the timescales of §119.1: a control command topic uses *reliable, low-latency* QoS, while a camera-feed topic might use *best-effort* QoS (drop frames rather than buffer and add latency). The pub/sub design is the distributed-systems pattern (Chapter 84) at robot scale — and shares its hazard: the message-passing overhead (serialization + transport) is the cost, which §119.3 attacks for high-bandwidth data.

---

## 119.3 Zero-Copy Transport

Standard ROS2 message passing *serializes* each message and copies it through the transport — fine for small commands, ruinous for large sensor data (a single LiDAR point cloud or 4K frame is megabytes, published many times per second). **Zero-copy transport** (via **Iceoryx** shared memory) eliminates the copies for intra-host communication.

```text
Standard:   Publisher --serialize--> copy --transport--> copy --deserialize--> Subscriber
Zero-copy (Iceoryx, same host):
  1. Publisher requests a chunk from a shared-memory segment.
  2. Publisher writes the data directly into that chunk.
  3. Publisher sends only the OFFSET (a pointer) to the subscriber.
  4. Subscriber reads the chunk directly.  -> ZERO copies, zero serialization.
```
*Listing 119.1 — Zero-copy transport via shared memory: publish a pointer, not the data.*

> **Why this matters / cost model.** For a megabyte-scale message published at 30–100 Hz, the serialize-and-copy cost dominates and adds latency the control and perception loops cannot afford. Zero-copy shared-memory transport reduces the per-message cost to passing an *offset* — the publisher writes the data once into shared memory and hands the subscriber a pointer, exactly the kernel-bypass/zero-copy principle of Chapters 99–100 applied to inter-process communication. This is what makes high-bandwidth perception pipelines feasible: the data is produced once and consumed in place, never copied. The constraints mirror those of any shared-memory design — the data must be a standard-layout type (Chapter 111), lifetime must be managed (the chunk is owned until all subscribers release it), and it only works *intra-host* (cross-host still needs the network). For large sensor data on one machine, zero-copy is the difference between a real-time pipeline and a backed-up one.

---

## 119.4 Real-Time Executors and Priority

A ROS2 node's callbacks (topic handlers, timers) are run by an **executor**. The default executor processes callbacks in a way that can suffer **priority inversion** — a low-priority "log the camera frame" callback delaying a high-priority "emergency stop" callback. Real-time robotics uses prioritised executors that respect callback importance.

> **Why this matters.** Priority inversion (Chapter 95) is the cardinal sin of a real-time robot: if a safety-critical callback (the emergency-stop topic) waits behind a bulk-data callback (camera logging) in a single-threaded executor, the robot's stop is delayed — a safety failure. The fix is the same as the threading discipline of Chapter 96: assign callbacks to **callback groups** with priorities, run safety-critical callbacks on a dedicated high-priority executor (and ideally a dedicated, isolated core), and never let low-priority bulk work share the path with the control loop. This is the hot/cold split (Chapter 106) expressed in ROS2's execution model: the deadline-bound callbacks get an isolated, prioritised path; everything else runs elsewhere. The executor configuration *is* the real-time guarantee.

---

## 119.5 Allocation Discipline in the Control Loop

In a kilohertz-plus control loop, **heap allocation is fatal** — the same rule as the audio callback (Chapter 118) and the trading hot path (Chapter 106), because allocation can hit a slow path (lock, `mmap`, page fault) and blow the deadline, and over time heap *fragmentation* makes allocation latency unpredictable.

```cpp
// Min standard: C++17. A real-time control callback using a stack arena — no heap.
#include <memory_resource>
void control_callback() {
    std::array<std::byte, 4096> buf;
    std::pmr::monotonic_buffer_resource arena{buf.data(), buf.size()};  // stack-backed, no malloc
    std::pmr::vector<float> message(&arena);    // allocates from the stack buffer (Chapter 79)
    // ... build the control message with NO heap allocation ...
}
```
*Listing 119.2 — A control-loop callback uses a stack-backed `monotonic_buffer_resource` to avoid the heap (Chapter 79).*

> **Why this matters / cost model.** The control loop is a hard-real-time hot path, so the allocation-free discipline of Chapters 79 and 97 applies in full: pre-allocate everything, use object pools and fixed-capacity containers, and for transient per-message scratch use a stack-backed `std::pmr::monotonic_buffer_resource` so standard containers work without touching the heap (Chapter 79). ROS2 supports custom allocators precisely so the message-building path can be made allocation-free. The deeper reason is *determinism over time*: even if `malloc` is fast on average, repeated allocation/deallocation fragments the heap, and a fragmented heap's allocation latency drifts upward and becomes unpredictable — so a robot that runs for hours degrades. Allocation-free control loops stay deterministic indefinitely. This is the determinism discipline (Chapter 106) as a *longevity* requirement, not just a latency one.

---

## 119.6 The Robotics Discipline

| Timescale | Requirement | Technique |
|---|---|---|
| Hard real-time (motor/safety) | Bounded latency, determinism | Allocation-free, prioritised executor, isolated core |
| Soft real-time (navigation) | Fast, occasional lateness OK | Standard executor, separate threads |
| Throughput (perception) | High bandwidth, low copy cost | Zero-copy shared-memory transport |
| Inter-node communication | Modularity + fault isolation | ROS2 topics/DDS with appropriate QoS |

> **The discipline.** Robotics is a *composition* of the disciplines in this book, each applied to the subsystem that needs it: the control loop is a hard-real-time hot path (allocation-free, lock-free, isolated — Chapters 96, 106, 118); perception is a throughput pipeline (zero-copy transport — Chapters 99–100); the architecture is the actor model (independent nodes, message passing — Chapter 78); and the whole runs on DDS pub/sub with QoS tuned per timescale. The unifying skill is *isolation*: keep the timescales from interfering — the bulk perception data must never jitter the safety loop, enforced through prioritised executors, isolated cores, and zero-copy transport. C++ earns its place in robotics by serving every one of these requirements in a single system. The next chapter turns to another high-throughput domain — the storage engines underpinning modern databases.
