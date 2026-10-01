---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lessons: [L05e]
environment: "Lima VM aos (Ubuntu 24.04 arm64)"
tags: [cs6210, cs6210/lab]
---

# lab-12-components: Micro-protocol stacks from components and common-path optimization

> [!info] Goal
> Make L05e concrete with real commands and measurements by constructing a multi-layer micro-protocol stack and synthesizing a common-case bypass fast-path to reduce layering overhead.

See [setup](../setup/README.md) for the VM.

## What you should see

Run the benchmark to measure the cost of micro-protocol layering and the benefit of a synthesized bypass path:

```sh
make run
```

```text
--- Ensemble Micro-Protocol Bypass Benchmark ---
Unoptimized layered stack: 1.89 us / msg
Optimized CCP fast-path:   1.13 us / msg
Savings:                   0.75 us (40.0%)
```

You should see a substantial reduction in per-message processing latency, demonstrating why monolithic performance can be achieved without losing component-based maintainability.

## How it works

The lab implements a simplified multi-layer network stack (`ProtocolStack`) consisting of:
1. **Application Layer**: Generates and consumes payloads.
2. **Fragmentation Layer**: Splits payloads exceeding the MTU (Maximum Transmission Unit) into chunks.
3. **Ordering Layer**: Ensures packets are delivered in sequence by buffering out-of-order packets.
4. **Dummy Layers**: 20 empty layers simulating the deep stacks (often 20+ micro-protocols) used in flexible architectures like Ensemble.
5. **Checksum Layer**: Computes and verifies MD5 hashes of packet payloads.
6. **Bottom Layer**: Emulates the network medium by passing packets to the peer stack.

**The Common Case Predicate (CCP)**
The Ensemble system identifies the "common path"—the conditions under which a packet requires minimal processing (e.g., unfragmented, in-order, correct checksum). In our stack, the CCP requires that:
- The packet is not a fragment.
- The packet's sequence number matches the expected sequence number (in-order delivery).
- The ordering buffer is empty (no previously out-of-order packets waiting).
- The checksum is valid.

**The Optimization**
When the `FastPathBypass` is enabled, the sender creates the packet, increments the sequence, computes the checksum, and pushes directly to the `BottomLayer`—bypassing 23 layer boundaries.
On the receiver side, a hook inside the `BottomLayer` evaluates the CCP. If true, the packet is delivered directly to the `ApplicationLayer`, skipping the intermediate layers and avoiding multiple function calls and state lookups.

## Experiments

1. **Varying the Payload Size**
   Modify the payload size in `messages` to be larger than 64 bytes.
   - *Prediction:* What happens to the fast-path time when the message size exceeds the MTU?
2. **Deepening the Stack**
   Modify `stack.py` to add 50 `DummyLayer` instances instead of 20.
   - *Prediction:* How will this change affect the unoptimized latency, and how much will the bypass savings percentage grow?
3. **Out-of-Order Delivery**
   Introduce a random shuffle in `NetworkLayer.send` to deliver packets out of order.
   - *Prediction:* Will the CCP condition hold? How will the system perform compared to the fully unoptimized stack?

## Questions

<details>
<summary>Why does the CCP check if the ordering buffer is empty?</summary>

If a prior packet was lost or delayed, subsequent packets are buffered by the `OrderingLayer`. If the delayed packet finally arrives and satisfies `packet.seq_num == expected_seq`, delivering it might unblock other packets in the buffer. The fast path only delivers the current packet; if it bypassed the `OrderingLayer` while the buffer had data, the buffered packets would never be automatically checked and delivered. Falling back to the slow path ensures the buffer is drained correctly.
</details>

<details>
<summary>Why didn't we just write a monolithic stack to begin with?</summary>

A monolithic stack is difficult to modify or extend. If an application suddenly needs encryption or a different flow-control algorithm, a monolithic design requires rewriting and retesting the entire stack. A component-based design allows us to easily snap micro-protocols together, verify their individual correctness using tools like IOA, and then use a system like NuPrl to automatically synthesize the monolithic-like fast path.
</details>

<details>
<summary>Why are function calls across layer boundaries expensive?</summary>

In a heavily layered protocol stack, every boundary crossing requires a function call, which incurs stack frame setup and teardown. Moreover, accessing independent states across numerous layers degrades data and instruction cache locality, leading to costly cache misses.
</details>
