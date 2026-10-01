---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lessons: [L05d]
environment: "Lima VM aos (Ubuntu 24.04 arm64)"
tags: [cs6210, cs6210/lab]
---

# lab-11-network-latency: Network latency budgets: ping, iperf3, tc netem, and capsule routing

> [!info] Goal
> Make L05d (Active Networks) concrete by examining network latencies, marshaling costs for programmable packets, and a simulation of demand-loaded capsule routing.

See [setup](../setup/README.md) for the VM.

## Prerequisites

- Lima VM configured via `make vm` in the `setup` directory.
- `perf` and network tools (`iproute2`, `iperf3`, `tc`).
- Python 3.12 with `protobuf` inside the AOS venv.

## Run Commands

Run inside the VM:
```bash
# Setup the VM (if not already done)
cd 01-CS-Foundations/Operating-Systems/AOS/labs/setup
make shell

# Run the lab (creates veth pairs, requires sudo inside the VM)
cd ../lab-11-network-latency
make test
make run
```

## What you should see

The output of `make run` consists of three parts.
First, a baseline network latency check using Linux network namespaces and virtual ethernet (`veth`) devices:

```text
[+] Testing loopback baseline...
rtt min/avg/max/mdev = 0.015/0.078/0.122/0.045 ms

[+] Testing veth baseline latency...
rtt min/avg/max/mdev = 0.059/0.174/0.335/0.117 ms

[+] Testing veth baseline throughput (iperf3)...
[  5]   0.00-2.00   sec  13.2 GBytes  56595 Mbits/sec    0             sender
```

After injecting a 10ms delay and 5% packet loss using `tc netem`:

```text
[+] Testing delayed veth latency...
rtt min/avg/max/mdev = 10.096/12.436/16.035/1.802 ms

[+] Testing delayed veth throughput (iperf3)...
[  5]   0.00-2.00   sec  1.88 MBytes  7.87 Mbits/sec   32             sender
```
*(Throughput drops precipitously from ~56 Gbps to ~7 Mbps due to TCP congestion control reacting to the 5% packet loss and 10ms artificial delay).*

Second, the marshaling cost (serialization overhead) of building an Active Network "capsule":

```text
Marshaling Cost Comparison (Medians implicitly represented over multiple iterations):
Struct   - Serialize:   68.4 ns | Deserialize:   89.2 ns | Size: 96 bytes
JSON     - Serialize: 1025.2 ns | Deserialize:  808.8 ns | Size: 134 bytes
Protobuf - Serialize:  166.4 ns | Deserialize:  266.8 ns | Size: 87 bytes
```

Third, a trace of the Capsule Simulator demonstrating demand-loading of executable logic:

```text
=== Simulation 1: Unknown Capsule Arrives at NodeB ===
[NodeB] Received capsule with hash: 99bb56c3... from NodeA
[NodeB] Unknown capsule type. Suspending and requesting code from NodeA
[NodeB] -> [NodeA]: Requesting code for 99bb56c3...
[NodeA] Received code request for 99bb56c3...
[NodeB] Received and verified code. Storing and resuming...
[NodeB] Executing logic: FORWARD_TO_DESTINATION | Payload: Hello Active Network!

=== Simulation 2: Subsequent Capsule Arrives at NodeB ===
[NodeB] Received capsule with hash: 99bb56c3... from NodeA
[NodeB] Code found in soft store. Executing...
[NodeB] Executing logic: FORWARD_TO_DESTINATION | Payload: Second message
```

## How it works

- **Network Namespaces and `veth`:** The script `latency_test.sh` uses `ip netns add` to create isolated networking contexts. It bridges them with a virtual ethernet pair (`veth`). This allows us to safely benchmark and disrupt traffic without breaking the VM's SSH connection.
- **Traffic Control (`tc netem`):** The Linux Traffic Control utility `tc` adds queuing disciplines. The `netem` (Network Emulator) module intercepts outgoing packets on `veth1` and holds them to simulate latency, or drops them randomly to simulate loss. This demonstrates the harsh conditions Active Networks (like ANTS) face when demand-loading code across wide-area networks.
- **Marshaling Costs:** Active Networks require packets to carry structured metadata (code hashes, previous node IDs). `marshal_test.py` contrasts raw C-style binary packing (`struct.pack`) against text-based (JSON) and schema-based (Protobuf) serialization. Because measurements lack hardware PMU counters in the VM, `time.perf_counter()` (which maps to `clock_gettime(CLOCK_MONOTONIC)`) measures median execution time over multiple iterations.
- **Capsule Simulator:** The `capsule_sim.py` script mimics the ANTS toolkit's core mechanism. When a node receives a capsule with an unrecognized MD5 type hash, it cannot process it immediately. It pauses, requests the payload logic from the upstream node, verifies the hash upon receipt, caches the logic in a "soft store", and resumes execution.

## Experiments to try

1. **Increase packet loss:** In `latency_test.sh`, change `loss 5%` to `loss 20%`.
   - *Prediction:* Latency measurements will show dropped sequences, and iperf3 throughput will plummet to near zero as TCP struggles to maintain an open window.
2. **Measure Protobuf vs JSON with larger payloads:** In `marshal_test.py`, increase the `payload` string to 100 KB.
   - *Prediction:* The serialization gap between JSON and Protobuf will widen significantly. JSON string encoding overhead will scale poorly compared to Protobuf's binary handling.
3. **Simulate a malicious capsule:** Modify `capsule_sim.py` so the upstream node returns a modified code block whose MD5 hash does not match the capsule's `type_hash`.
   - *Prediction:* The receiving node will compute the hash, detect the mismatch, and drop the capsule to prevent executing spoofed logic, just as ANTS did for security.

## Questions

<details>
<summary>Why does the iperf3 throughput drop so dramatically with only 5% packet loss and a 10ms delay?</summary>
TCP guarantees reliable delivery and uses packet loss as the primary signal for network congestion. Even a small 5% loss causes the TCP congestion control algorithm (like CUBIC or Reno) to repeatedly halve its sending window. Combined with the 10ms round-trip delay, the sender spends most of its time waiting for acknowledgments or retransmitting, rather than utilizing the 56 Gbps link capacity.
</details>

<details>
<summary>In the Active Networks model (ANTS), why does the capsule only carry a hash instead of the actual code block?</summary>
Carrying the full code block in every packet would introduce massive network overhead and serialization latency for every transmission. By carrying a lightweight MD5 hash, the code is only transmitted once during the initial demand-loading phase. Subsequent packets incur zero transmission penalty since the code is already cached in the node's soft store.
</details>

<details>
<summary>How does the `struct.pack` approach achieve such low latency compared to JSON and Protobuf?</summary>
`struct.pack` translates directly into simple memory copies, laying out bytes exactly as specified by the format string without any dynamic parsing, key allocation, or metadata generation. JSON requires string parsing, character escaping, and dictionary building. Protobuf is faster than JSON but still computes variable-length integer bounds and field tags.
</details>

<details>
<summary>Why must `tc netem` be executed with `sudo`?</summary>
Manipulating network queuing disciplines directly affects the kernel's network stack and routing behavior. If an unprivileged user could alter queuing disciplines, they could launch local denial-of-service attacks by silently dropping or delaying all traffic on an interface.
</details>
