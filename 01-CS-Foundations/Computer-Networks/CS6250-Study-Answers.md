# CS 6250 Computer Networks - Study Guide with Detailed Answers

## Lesson 1: Introduction, History, and Internet Architecture

### Advantages and Disadvantages of Layered Architecture

**Advantages:**
- **Modularity**: Each layer handles specific tasks independently, making the system easier to understand and develop
- **Interoperability**: Different implementations can exist at each layer as long as they follow the interface specification
- **Maintenance and Updates**: Changes to one layer don't necessarily require changes to other layers
- **Reusability**: Protocols and implementations can be reused across different systems
- **Scalability**: New layers or components can be added without disrupting existing functionality
- **Debugging**: Issues can be isolated to specific layers, simplifying troubleshooting

**Disadvantages:**
- **Overhead**: Each layer adds processing overhead (headers, processing time)
- **Duplication**: Some functionality may be implemented at multiple layers (e.g., error checking)
- **Reduced Efficiency**: Layered communication may not be the most efficient for all use cases
- **Coupling**: Although designed to be independent, layers often have dependencies
- **Performance Trade-offs**: Strict adherence to layering can negatively impact performance in time-critical applications
- **Complexity**: The abstraction can hide important details necessary for optimization

### OSI Model vs. Five-Layered Internet Model

**OSI Model (7 Layers):**
1. Physical Layer
2. Data Link Layer
3. Network Layer
4. Transport Layer
5. Session Layer
6. Presentation Layer
7. Application Layer

**Five-Layered Internet Model:**
1. Physical Layer
2. Link Layer (combines Data Link)
3. Network Layer
4. Transport Layer
5. Application Layer

**Differences:**
- The Internet model combines Session, Presentation, and Application concerns into a single Application Layer
- The OSI model is more theoretically complete but the Internet model is simpler and more practical
- Session and Presentation layer functionality is handled by applications in the Internet model
- The five-layer model better reflects how the modern Internet actually works

**Similarities:**
- Both use layering as the fundamental organizing principle
- Both separate concerns by layer
- Both follow a hierarchical model with each layer providing services to the layer above

### What Are Sockets?

**Definition**: A socket is an endpoint for network communication that acts as an interface between an application and the operating system's network protocol stack.

**Key Characteristics:**
- **Abstraction**: Sockets abstract away the complexity of network programming
- **Connection Point**: Serves as a connection point for sending/receiving data
- **Unique Identity**: Identified by IP address, port number, and protocol type
- **Types**: Stream sockets (TCP) and datagram sockets (UDP)

**Common Socket Operations:**
```bash
# Python example for TCP client socket
import socket
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # Create TCP socket
s.connect(('192.168.1.1', 80))  # Connect to host
s.send(b'GET / HTTP/1.1\r\n')   # Send data
data = s.recv(1024)              # Receive data
s.close()                         # Close socket
```

### OSI Model Layers - Detailed Description

**Layer 1: Physical Layer**
- Deals with actual physical transmission of raw bits
- Defines electrical, mechanical, and procedural specifications
- Examples: cables, connectors, signal encoding
- Protocols: Ethernet, WiFi physical standard (802.11), DSL

**Layer 2: Data Link Layer**
- Organizes bits into frames and provides error detection/correction
- Handles MAC addressing and switching
- Examples: Ethernet frames, PPP, WiFi MAC
- Protocols: Ethernet, PPP, Wi-Fi (MAC), Frame Relay

**Layer 3: Network Layer**
- Routes packets across networks using logical IP addresses
- Handles routing decisions
- Examples: IP routing, ICMP
- Protocols: IPv4, IPv6, ICMP, IGMP

**Layer 4: Transport Layer**
- Provides end-to-end communication and reliability
- Manages connection establishment and termination
- Examples: TCP, UDP
- Protocols: TCP, UDP, SCTP

**Layer 5: Session Layer**
- Manages sessions and dialogue control
- Establishes, maintains, and terminates sessions
- Examples: RPC, NetBIOS, Session initiation
- Protocols: SSL/TLS, NFS

**Layer 6: Presentation Layer**
- Handles data formatting, encryption, and compression
- Ensures data is in usable format for applications
- Examples: Encryption, compression, character encoding
- Protocols: JPEG, MPEG, TLS encryption

**Layer 7: Application Layer**
- Provides services directly to user applications
- User-facing protocols and services
- Examples: Email, web browsing, file transfer
- Protocols: HTTP, HTTPS, FTP, SMTP, DNS, SSH, Telnet

### Protocols at Each Layer of Five-Layered Model

| Layer | Name | Protocols |
|-------|------|-----------|
| 5 | Application | HTTP/HTTPS, FTP, SMTP, POP3, DNS, SSH, Telnet, DHCP, NTP |
| 4 | Transport | TCP, UDP, SCTP |
| 3 | Network | IP (IPv4/IPv6), ICMP, IGMP, ARP |
| 2 | Link | Ethernet, PPP, WiFi (802.11), Frame Relay, HDLC |
| 1 | Physical | Twisted pair, Fiber optic, Wireless (RF) |

### Encapsulation and Its Role in Layered Model

**Encapsulation**: The process of wrapping data with protocol information as it moves down through layers.

**Process:**
1. Application layer creates a message (Application Data)
2. Transport layer adds a header (TCP/UDP header) → called a **Segment**
3. Network layer adds a header (IP header) → called a **Packet** or **Datagram**
4. Link layer adds a header and trailer (Frame header/trailer) → called a **Frame**
5. Physical layer transmits the bits

**Example:**
```
Application Data: "Hello"
↓ Transport (TCP): [TCP Header | Hello]
↓ Network (IP): [IP Header | TCP Header | Hello]
↓ Link (Ethernet): [Frame Header | IP Header | TCP Header | Hello | Frame Trailer]
↓ Physical: 010101011... (bit transmission)
```

**Decapsulation**: The reverse process when data moves up the stack, with each layer removing its header.

### End-to-End (E2E) Principle

**Definition**: The principle that certain functions are better performed at the "ends" of a system (at endpoints) rather than in intermediary nodes.

**Core Idea**: Complex reliability mechanisms should be implemented at endpoints, not intermediaries, because intermediaries cannot fully understand application semantics.

**Benefits:**
- Simpler core network (intelligence at edges)
- Better suited for application-specific needs
- Reduced complexity in network infrastructure
- Endpoints have complete information about application requirements

### Violations of E2E Principle

1. **Firewalls**: Intermediary nodes filter traffic based on content/patterns
2. **Network Address Translation (NAT)**: Intermediary changes addresses
3. **Deep Packet Inspection (DPI)**: Core network inspects application-layer content
4. **Quality of Service (QoS)**: Core network manages traffic prioritization
5. **Caching Proxies**: Intermediaries cache content from applications
6. **Load Balancers**: Intermediaries distribute traffic

**Practical Command to Inspect Packets (Violation Example):**
```bash
# Using tcpdump to inspect packet content at network intermediate point
sudo tcpdump -i eth0 -n port 80 -A
# This shows inspecting HTTP traffic at the link layer (middlebox/DPI)
```

### EvoArch Model

**Definition**: Evolutionary model of Internet architecture that explains how the Internet structure naturally evolved.

**Key Components:**
- **Hourglass Concept**: The model is shaped like an hourglass
- **Network Layer (IP)**: Forms the "narrow waist" - minimal standardization point
- **Upper Layers**: Multiple protocols compete and evolve above IP
- **Lower Layers**: Multiple technologies coexist below IP
- **Generality vs. Specificity**: Movement from specific to general moving up, and vice versa moving down

**The Hourglass Model:**
```
Multiple Applications (HTTP, SMTP, FTP, etc.)
         ↓
Transport Protocols (TCP, UDP)
         ↓
    IP (Narrow Waist)
         ↓
    Link Technologies (Ethernet, WiFi, PPP)
         ↓
Physical Media (Fiber, Twisted Pair, Wireless)
```

### A Round in the EvoArch Model

**Definition**: An evolutionary cycle describing how technologies naturally propagate through the Internet architecture.

**Phases in a Round:**
1. **Creation**: A new protocol/technology is created at some layer
2. **Selection**: Competition occurs; successful protocols survive
3. **Replication**: Successful protocols are adopted widely
4. **Mutation**: Variations and improvements emerge
5. **Oscillation**: The cycle repeats as new technologies emerge

**Example Round**: 
- DNS was created at application layer
- Competition with other naming systems (BIND vs. others)
- DNS "won" and replicated globally
- Mutations: DNSSEC, DNS over HTTPS emerged
- New oscillations continue

### Ramifications of the Hourglass Shape

1. **IP as Internet Foundation**: IPv4 and IPv6 become the focal point
2. **Difficulty in Innovation Below IP**: Hard to introduce new network layer protocols (would require coordinating globally)
3. **Flexibility Above IP**: Easy to innovate at transport and application layers
4. **Stagnation of Network Layer**: IPv4 has persisted for 40+ years due to installed base
5. **Consequences**:
   - Network layer changes require massive coordination
   - Application layer innovation is rapid
   - Lock-in effect at the waist level
   - Path dependency in architecture evolution

### Network Devices and Their Operating Layers

| Device | Layer(s) | Description |
|--------|---------|-------------|
| **Repeater** | Layer 1 (Physical) | Amplifies signals; extends network range |
| **Hub** | Layer 1 (Physical) | Multi-port repeater; broadcasts to all ports |
| **Bridge** | Layer 2 (Data Link) | Connects segments using MAC addresses |
| **Router** | Layer 3 (Network) | Routes packets using IP addresses |
| **Switch** | Layer 2 (Data Link) | Modern bridge with multiple interfaces |

### What Is a Bridge and How Does It Learn?

**Bridge Definition**: A network device that operates at Layer 2 (Data Link Layer) to connect multiple network segments while learning MAC addresses.

**Bridge Operations:**

1. **Learning Phase**:
   - When a frame arrives, the bridge reads the source MAC address
   - Associates the MAC address with the incoming port
   - Stores this in a MAC address table (CAM table)
   - Future frames destined for that MAC are forwarded only to that port

2. **Forwarding Decisions**:
   - **If destination MAC is known**: Forward to specific port
   - **If destination MAC is unknown**: Flood to all ports except incoming port
   - **If source = destination MAC**: Drop frame (loop prevention)

**Example Learning Scenario:**
```
Initial State: Bridge MAC table is empty

Frame 1 arrives: Source: A1:B2:C3:D4:E5:F6, Destination: FF:FF:FF:FF:FF:FF (broadcast)
  → Bridge learns: A1:B2:C3:D4:E5:F6 is on Port 1
  → Floods frame to all ports

Frame 2 arrives: Source: A1:B2:C3:D4:E5:F7, Destination: A1:B2:C3:D4:E5:F6
  → Bridge learns: A1:B2:C3:D4:E5:F7 is on Port 2
  → Forwards to Port 1 (known destination)
```

**Practical Command - View MAC Table:**
```bash
# On Linux/macOS, view ARP table (similar concept)
arp -a

# On Windows
arp -a

# View MAC address table on Cisco switch (if you have access)
show mac-address-table
```

### Distributed Algorithm

**Definition**: An algorithm executed by multiple autonomous entities (nodes) in a distributed system to collectively solve a problem without central coordination.

**Characteristics:**
- **No Central Coordinator**: Each node makes local decisions
- **Asynchronous**: Nodes don't need to operate in lockstep
- **Local Information**: Nodes use only local/neighbor information typically
- **Convergence**: Eventually reaches a consistent global state
- **Message Passing**: Nodes communicate via messages

**Examples:**
- Spanning Tree Algorithm
- Distance Vector Routing
- Consensus algorithms
- Byzantine Fault Tolerance

### Spanning Tree Algorithm

**Purpose**: Prevents loops in redundant network topologies by removing redundant links.

**Problem It Solves**: In bridged networks with loops, frames can circulate infinitely, causing network collapse.

**Algorithm Overview:**

1. **Election Phase**: Elect a root bridge (lowest bridge ID)
2. **Path Calculation**: Each non-root bridge calculates shortest path to root
3. **Port Role Assignment**:
   - Root ports: towards the root bridge
   - Designated ports: forwards traffic towards leaves
   - Blocked ports: redundant links disabled
4. **Convergence**: Network reaches stable tree state

**States for Bridge Ports:**
- **Disabled**: No data processing
- **Blocking**: Receives BPDUs, discards data
- **Listening**: Processes BPDUs, discards data
- **Learning**: Learns MAC addresses, discards data
- **Forwarding**: Normal data processing

**Example Spanning Tree:**
```
Initial topology with loop:
    Switch A ←→ Switch B
      ↓        ↓
    Switch C ←→ Switch D

After STA:
    Switch A (Root) ← (designated port)
      ↓
    Switch C
      ↓
    Switch D ← (blocked port)
```

**Commands to View Spanning Tree:**
```bash
# Cisco switch command
show spanning-tree

# Linux bridge command
brctl show

# View detailed STP info
brctl showstp <bridge_name>
```

---

## Lesson 2: Transport and Application Layers

### What Does the Transport Layer Provide?

The Transport Layer provides:

1. **Process-to-Process Communication**: Identifies applications using port numbers
2. **Reliable Delivery**: TCP ensures all data arrives without loss or duplication
3. **Multiplexing/Demultiplexing**: Multiple applications can share one network connection
4. **Connection Management**: Establishment and termination of connections (TCP)
5. **Flow Control**: Prevents sender from overwhelming receiver
6. **Congestion Control**: Prevents network from becoming overwhelmed
7. **Error Detection**: Detects corrupted segments
8. **Ordering**: TCP maintains sequence numbers for proper ordering

### Transport Layer Packet - Terminology

**Segment**: A packet at the transport layer. Consists of:
- Source port number
- Destination port number
- Control flags (SYN, ACK, FIN, RST, etc.)
- Sequence number
- Acknowledgment number
- Data payload
- Checksums

```
TCP Segment Structure:
┌─────────────────────────────────────┐
│    Source Port (16 bits)            │
├─────────────────────────────────────┤
│    Destination Port (16 bits)       │
├─────────────────────────────────────┤
│    Sequence Number (32 bits)        │
├─────────────────────────────────────┤
│    Acknowledgment Number (32 bits)  │
├─────────────────────────────────────┤
│    Flags (SYN, ACK, FIN, etc.)     │
├─────────────────────────────────────┤
│    Window Size (16 bits)            │
├─────────────────────────────────────┤
│    Checksum (16 bits)               │
├─────────────────────────────────────┤
│    Payload (Application Data)       │
└─────────────────────────────────────┘
```

### Two Main Transport Layer Protocols

1. **TCP (Transmission Control Protocol)**
   - Connection-oriented
   - Reliable delivery
   - Ordered delivery
   - Flow control and congestion control
   - Slower but guaranteed delivery

2. **UDP (User Datagram Protocol)**
   - Connectionless
   - Unreliable (best-effort)
   - Unordered delivery
   - No flow or congestion control
   - Fast and lightweight

### Multiplexing and Demultiplexing

**Multiplexing**: Combining multiple data streams into a single transmission stream

**Demultiplexing**: Separating a single transmission stream into multiple data streams

**Why Necessary**:
- Multiple applications on same host need to communicate
- Without multiplexing, each application would need separate network interface
- Allows efficient use of network resources
- Enables concurrent network applications

**How It Works**:
- Sender: Multiple applications → Transport layer adds port numbers → Single network connection
- Receiver: Network data arrives → Transport layer reads port numbers → Routes to correct application

**Example**:
```
Application 1 (Port 8080) ─┐
Application 2 (Port 3306) ─┤ Transport Layer Multiplexing
Application 3 (Port 443)  ─┘
                             ↓
                      Network Interface
                             ↓
Demultiplexing at Receiver:
                      Network Interface
                             ↓
Application 1 ← Port 8080
Application 2 ← Port 3306
Application 3 ← Port 443
```

### Two Types of Multiplexing/Demultiplexing

1. **Connectionless Multiplexing/Demultiplexing (UDP)**
   - Uses destination IP and port to deliver datagrams
   - No connection state maintained
   - Each datagram is independent
   - Multiple datagrams from same source can arrive out of order

2. **Connection-Oriented Multiplexing/Demultiplexing (TCP)**
   - Uses 4-tuple: (source IP, source port, destination IP, destination port)
   - Maintains connection state
   - Ensures ordered delivery
   - More overhead but reliable

**Practical Command - View Network Connections:**
```bash
# Show all listening ports and established connections
netstat -tulpn

# More modern approach using ss (socket statistics)
ss -tulpn

# View specific process using port
lsof -i :8080

# Example output interpretation:
# Proto Recv-Q Send-Q Local Address       Foreign Address     State
# tcp   0      0      127.0.0.1:8080      0.0.0.0:*           LISTEN
#       ↑ local port indicates multiplexed application
```

### Differences Between UDP and TCP

| Feature | UDP | TCP |
|---------|-----|-----|
| **Connection** | Connectionless | Connection-oriented |
| **Reliability** | Unreliable | Reliable |
| **Ordering** | Unordered | Ordered |
| **Speed** | Fast | Slower |
| **Overhead** | Low | High |
| **Error Detection** | Checksum only | Checksum + retransmission |
| **Flow Control** | No | Yes |
| **Congestion Control** | No | Yes |
| **Use Cases** | Streaming, Gaming, DNS | Web, Email, FTP |
| **Header Size** | 8 bytes | 20-60 bytes |

**Header Comparison:**
```
UDP Header (8 bytes):
┌──────────────┐
│ Source Port  │ (2 bytes)
├──────────────┤
│ Dest Port    │ (2 bytes)
├──────────────┤
│ Length       │ (2 bytes)
├──────────────┤
│ Checksum     │ (2 bytes)
└──────────────┘

TCP Header (20 bytes minimum):
┌──────────────────────────────────────┐
│ Source Port | Dest Port              │ (4 bytes)
├──────────────────────────────────────┤
│ Sequence Number                      │ (4 bytes)
├──────────────────────────────────────┤
│ Acknowledgment Number                │ (4 bytes)
├──────────────────────────────────────┤
│ Data Offset | Flags | Window         │ (4 bytes)
├──────────────────────────────────────┤
│ Checksum | Urgent Pointer            │ (4 bytes)
└──────────────────────────────────────┘
```

### When Would Applications Choose UDP Over TCP?

1. **Real-Time Communication**
   - VoIP (Skype, WhatsApp calls)
   - Video conferencing (Zoom)
   - Online gaming (Valorant, CS:GO)
   - Reason: Low latency > guaranteed delivery

2. **Live Streaming**
   - YouTube Live
   - Twitch
   - Reason: Missing few frames acceptable, latency matters

3. **DNS Queries**
   - DNS lookups
   - Reason: Simple request-response, fast

4. **IoT Sensors**
   - Sensor data collection
   - Reason: Occasional lost data acceptable, overhead unacceptable

5. **Multicast/Broadcast**
   - Network-wide announcements
   - Reason: TCP can't multicast, overhead too high

6. **Network Management**
   - SNMP
   - DHCP
   - Reason: Lightweight, periodic nature

**Decision Criteria:**
- Delay-sensitive → UDP
- Loss-tolerant → UDP
- Low overhead critical → UDP
- Guaranteed delivery critical → TCP
- Ordered delivery critical → TCP
- Transaction-based → TCP

### TCP Three-Way Handshake

**Purpose**: Establish a TCP connection reliably.

**Steps**:

1. **SYN (Synchronization)**
   - Client sends a segment with SYN flag set (seq=x, usually random)
   - Example: Client initiates: `SYN, seq=1000`

2. **SYN-ACK (Synchronization-Acknowledgment)**
   - Server responds with SYN flag set and ACK flag set
   - Server sends back: `SYN, ACK, seq=y (random), ack=x+1`
   - Server acknowledges client's sequence number

3. **ACK (Acknowledgment)**
   - Client sends segment with ACK flag set
   - Client sends: `ACK, seq=x+1, ack=y+1`
   - Client acknowledges server's sequence number

**Sequence Diagram:**
```
Client                                Server
  |                                     |
  |-------- SYN (seq=1000) ----------->|
  |                                     |
  |<-- SYN-ACK (seq=2000, ack=1001) ---|
  |                                     |
  |------- ACK (seq=1001, ack=2001) -->|
  |                                     |
  |<--- Connection Established ------->|
```

**Practical Commands to Monitor Handshake:**
```bash
# Using tcpdump to capture TCP handshake
sudo tcpdump -i eth0 'tcp[tcpflags] & tcp-syn != 0' -n

# Using tcpdump to show three-way handshake for specific connection
sudo tcpdump -i eth0 host 192.168.1.100 and port 80 -S -n

# Using netstat to see connections in various states
netstat -tn | grep ESTABLISHED
netstat -tn | grep SYN_RECV  # Server waiting for ACK
netstat -tn | grep SYN_SENT  # Client waiting for SYN-ACK
```

**Example Output:**
```
192.168.1.100.54321 > 93.184.216.34.80: Flags [S] (SYN)
93.184.216.34.80 > 192.168.1.100.54321: Flags [S.] (SYN-ACK)
192.168.1.100.54321 > 93.184.216.34.80: Flags [.] (ACK)
```

### TCP Connection Teardown

**Purpose**: Gracefully close a TCP connection.

**Normal Teardown (4-Way Handshake)**:

1. **FIN from Client**
   - Client sends: `FIN, seq=x, ack=y`
   - Signals: "I have no more data to send"

2. **ACK from Server**
   - Server responds: `ACK, seq=y, ack=x+1`
   - Acknowledges receipt of FIN

3. **FIN from Server**
   - Server sends: `FIN, seq=y, ack=x+1`
   - Signals: "I'm done too"

4. **ACK from Client**
   - Client sends: `ACK, seq=x+1, ack=y+1`
   - Confirms server's FIN

**Sequence Diagram:**
```
Client                                Server
  |                                     |
  |-------- FIN (seq=1000) ----------->|
  |                                     |
  |<------ ACK (seq=2000, ack=1001) ---|
  |                                     |
  |<------ FIN (seq=2000, ack=1001) ---|
  |                                     |
  |------- ACK (seq=1001, ack=2001) -->|
  |                                     |
  |<--- Connection Closed ------------>|
```

**Half-Open Connections**:
- After client sends FIN, it enters `FIN_WAIT_1` state
- Can still receive data from server
- Once server FINs, client enters `TIME_WAIT` state for 2×MSL (Maximum Segment Lifetime)

**TIME_WAIT Purpose**:
- Ensures delayed packets don't confuse new connections
- Allows graceful completion of connection
- Typically 30-120 seconds

**Abrupt Teardown (RST - Reset)**:
- Either side can send RST flag to forcefully close
- Used when connection is broken or unresponsive
- No graceful shutdown

**TCP Connection States:**
```
CLOSED → SYN_SENT → ESTABLISHED ← SYN_RECV ← LISTEN
           ↓            ↓ ↑        ↓
        CLOSED       FIN_WAIT   CLOSE_WAIT
                     TIME_WAIT  LAST_ACK
                        ↓          ↓
                      CLOSED ← CLOSED
```

### Automatic Repeat Request (ARQ)

**Purpose**: Ensure reliable data transmission over unreliable channels.

**Core Mechanism**:
- Sender transmits frame
- Receiver acknowledges receipt
- If no ACK received within timeout, sender retransmits
- Continues until successful

**Key Concepts**:
- **Positive Acknowledgment**: Receiver confirms successful receipt
- **Timeout**: Sender waits for ACK before resending
- **Sequence Numbers**: Identify frames and avoid duplicates

### Stop-and-Wait ARQ

**Definition**: Simplest ARQ mechanism where sender waits for ACK before sending next frame.

**Process**:
1. Sender transmits frame with sequence number
2. Sender waits for ACK
3. If ACK received: mark complete, go to next frame
4. If timeout occurs: retransmit same frame
5. Repeat until all frames sent

**Timing Diagram**:
```
Sender                              Receiver
  |                                   |
  |-------- Frame 1 (seq=0) -------->|
  |                                   | Processing...
  |<------- ACK (ack=1) -------------|
  |                                   |
  |-------- Frame 2 (seq=1) -------->|
  |                                   | Processing...
  |<------- ACK (ack=2) -------------|
```

**Problems with Stop-and-Wait**:
- Very inefficient (sender idle while waiting for ACK)
- Throughput = Frame Size / (RTT + Processing Time)
- Utilization = 1 / (1 + 2×Delay×Bandwidth)

**Throughput Calculation**:
```
If RTT = 100ms, Frame Size = 1000 bits, Bandwidth = 1 Mbps:
Transmission time = 1000 bits / 1 Mbps = 1 ms
Efficiency = 1ms / (1ms + 100ms) = 1%
Very poor efficiency!
```

### Go-Back-N ARQ

**Definition**: Sender can transmit multiple frames before waiting for acknowledgment. If error detected, goes back N frames and retransmits.

**Key Features**:
- **Sender Window**: Can send up to N frames without ACK
- **Cumulative ACK**: ACK(k) acknowledges all frames up to k
- **On Error**: Retransmit all N frames starting from error

**Example (N=4)**:
```
Sender sends:           Frame 0, Frame 1, Frame 2, Frame 3
Receiver gets:          Frame 0, Frame 1, ❌ Frame 2 (error), Frame 3
Receiver sends:         ACK(1) [acknowledges up to Frame 1]
Sender receives ACK(1): Discards 0,1 but must retransmit 2,3
Sender sends:           Frame 2, Frame 3, Frame 4, ...
```

**Advantages**:
- Better utilization than Stop-and-Wait
- Simple implementation at receiver

**Disadvantages**:
- Retransmits all frames after error (wasteful)
- Receiver must buffer out-of-order frames

### Selective Acknowledgment (SACK)

**Definition**: Enhancement to ARQ where receiver acknowledges specific frames received, not just the highest in-order frame.

**How It Works**:
- Receiver sends: "I got Frame 2, 3, 5 (Frame 4 is missing)"
- Sender only retransmits Frame 4
- More efficient than Go-Back-N

**Example**:
```
Sender sends:       Frame 0, 1, 2, 3, 4, 5
Receiver gets:      Frame 0, 1, ❌ 2, 3, 4, 5
Receiver sends:     SACK: ack_range=(3-5) [acknowledges 3,4,5 missing only 2]
Sender retransmits: Frame 2
```

**TCP SACK Implementation**:
```
# Check if SACK is enabled (usually default in modern TCP)
cat /proc/net/tcp

# TCP header includes SACK permitted option during handshake
# SACK option in ACK segments specifies non-contiguous blocks received
```

### Fast Retransmit

**Purpose**: Avoid waiting for timeout; retransmit on receipt of duplicate ACKs.

**Mechanism**:
- If receiver gets out-of-order segment, it immediately sends duplicate ACK
- If sender receives 3 duplicate ACKs for same segment, assume loss
- Retransmit immediately without waiting for timeout

**Example**:
```
Sender sends:           Seg 1, 2, 3, 4, 5
Receiver gets:          Seg 1, ❌ 2, 3, 4, 5
Receiver sends ACKs:    ACK(2), ACK(2), ACK(2) [3 duplicates]
Sender sees 3 dup ACKs: Immediately retransmit Seg 2
No need to wait for timeout!
```

**Performance Impact**:
- Reduces retransmission latency by ~RTT
- Critical for high-speed networks where timeout is too long

### Transmission Control

**Definition**: Mechanisms that ensure data transmission doesn't overwhelm network or receiver.

**Two Main Types**:
1. **Flow Control**: Protects receiver from being overwhelmed
2. **Congestion Control**: Protects network from being overwhelmed

**Why Necessary**:
- Receiver has limited buffer space
- Network has limited capacity
- Uncontrolled transmission causes packet loss and retransmissions
- Can lead to network collapse (congestion collapse)

### Flow Control - Detailed

**Definition**: Mechanism to prevent sender from overwhelming receiver buffer.

**Why Needed**:
- Receiver has limited buffer capacity (e.g., 64KB)
- Sender might transmit faster than receiver can process
- Without flow control: buffer overflow → packet loss → retransmissions → worse situation

**TCP Flow Control Implementation**:

1. **Window Size Advertisement**:
   - Receiver advertises how much data it can accept (rwnd = receiver window)
   - Communicated in TCP header (16-bit field)

2. **Sender Constraint**:
   - Sender's window (cwnd) = min(cwnd, rwnd)
   - Never sends more data than receiver window

**Example**:
```
Receiver buffer = 4000 bytes
Receiver processes at 1000 bytes/RTT
Sender transmits at 2000 bytes/RTT (if unchecked)

With Flow Control:
- Receiver advertises rwnd=4000
- Sender transmits 4000 bytes
- Receiver processes 1000 bytes, frees buffer
- Receiver advertises rwnd=5000 (processed 1000, had 4000)
- Sender transmits only what's advertised
```

**TCP Window Scaling**:
```bash
# TCP window field is only 16 bits = 65535 bytes max
# For high-bandwidth, high-latency links, need larger windows
# TCP Window Scaling option allows windows up to 1GB
# Negotiated in SYN segment

# Check window scaling on Linux
cat /proc/sys/net/ipv4/tcp_window_scaling
```

### Congestion Control

**Definition**: Mechanism to prevent network overload by limiting sending rate based on network capacity.

**Why Needed**:
- Multiple flows compete for network resources
- Uncontrolled transmission causes congestion collapse
- Packets drop, causing retransmissions, worsening congestion
- Network becomes unusable (efficiency → 0)

### Goals of Congestion Control

1. **Efficiency**: Use network capacity fully without exceeding it
2. **Fairness**: Allocate resources fairly among competing flows
3. **Low Latency**: Minimize queuing delays
4. **Responsiveness**: Quickly adapt to network changes
5. **Stability**: Avoid oscillations and instability
6. **Fast Recovery**: Quickly utilize available bandwidth after congestion

### Network-Assisted Congestion Control

**Definition**: Congestion control where network explicitly signals congestion to endpoints.

**Mechanisms**:
1. **Explicit Congestion Notification (ECN)**
   - Router marks packets when queue reaches threshold
   - Receiver relays mark to sender via ACK
   - Sender reduces rate on ECN flag

2. **Choke Packets**
   - Router sends "source quench" message to sender
   - Tells sender to reduce transmission rate

3. **Load Indication**
   - Network tells sender current congestion level
   - Sender adjusts rate accordingly

**Advantages**:
- Explicit information > guessing from timeouts
- Faster convergence to equilibrium
- Avoids unnecessary retransmissions

**Disadvantages**:
- Requires router support (not always available)
- Backward compatibility issues

### End-to-End Congestion Control

**Definition**: Congestion control where endpoints infer congestion from network behavior (packet loss, delays) without explicit signals.

**How It Works**:
- No router involvement
- Endpoints infer congestion from:
  - Packet loss (timeout or duplicate ACK)
  - Round-trip time increase
  - Delay increase

**Advantages**:
- Works with any router (no modification needed)
- Fully deployable end-host solution

**Disadvantages**:
- Less precise (guessing based on effects)
- Slower convergence
- Can cause retransmissions before actual congestion

**TCP Uses End-to-End Model**

### How a Host Infers Congestion

Endpoints detect congestion through:

1. **Packet Loss**
   - Timeout: No ACK received within RTO
   - Duplicate ACKs: 3 or more duplicate ACKs indicate loss
   - Indicates: Network is congested, dropping packets

2. **Round-Trip Time (RTT) Increase**
   - RTT growing = queues building up
   - Indicates: Network approaching congestion

3. **Delay Increase**
   - Explicit delay increase signals congestion
   - Less common but available in some protocols

**Practical Detection**:
```bash
# Monitor packet loss
ping -c 100 8.8.8.8 | grep "% packet loss"

# Monitor RTT changes
ping -D 8.8.8.8 | head -20

# Use mtr to show real-time packet loss and latency
mtr -r -c 100 8.8.8.8
```

### How TCP Sender Limits Sending Rate

**TCP Congestion Window (cwnd)**:
- Controls maximum transmission window
- `Sending Rate = cwnd / RTT`
- Maintained by sender based on congestion signals

**Window Constraint**:
```
Maximum Segment Window = min(cwnd, rwnd)
Where:
  cwnd = congestion window (congestion control)
  rwnd = receiver window (flow control)
```

**Rate Limiting Mechanism**:
```
On packet loss: cwnd ← cwnd / 2 (reduce rate)
On successful transmission: cwnd ← cwnd + 1 (increase rate)
Result: Rate oscillates around network capacity
```

### Additive Increase/Multiplicative Decrease (AIMD)

**Definition**: Congestion control strategy used by TCP Tahoe/Reno.

**Algorithm**:
- **Additive Increase**: When no congestion, increase cwnd linearly (by 1 per RTT)
- **Multiplicative Decrease**: On congestion event, decrease cwnd by half (×0.5)

**Behavior**:
```
No Congestion:  cwnd = cwnd + 1            [slow growth]
Congestion:     cwnd = cwnd × 0.5          [rapid decrease]
```

**Visualization**:
```
cwnd
 |     /\         /\         /\
 |    /  \       /  \       /  \
 |   /    \     /    \     /    \
 |  /      \   /      \   /      \
 |_/_______\_/_______\_/_______\_ time
     Additive   Multiplicative
     Increase   Decrease
```

**Fairness Property**:
- Flows with same RTT converge to equal share
- Good fairness for similar paths

**Why AIMD**:
- Additive increase ensures fair sharing
- Multiplicative decrease ensures quick response to congestion
- Together: stable convergence to network capacity

### TCP Slow Start

**Definition**: Initial phase of TCP where window grows exponentially rather than linearly.

**Purpose**:
- TCP starts conservatively (cwnd=1 or 2)
- Gradually probes for available bandwidth
- Exponential growth finds available bandwidth quickly

**Mechanism**:
- Start: cwnd = 1 (or 2 with IW10)
- Each RTT: cwnd doubles (or increases by 1 per ACK received)
- Growth: 1 → 2 → 4 → 8 → 16 → ...
- Until: Congestion loss or reaching slow start threshold (ssthresh)

**Example Timeline**:
```
RTT 0: cwnd = 1, sends 1 segment
RTT 1: cwnd = 2, sends 2 segments
RTT 2: cwnd = 4, sends 4 segments
RTT 3: cwnd = 8, sends 8 segments
RTT 4: cwnd = 16, loss detected, ssthresh = 8
RTT 5: cwnd = 1, slow start again
...
cwnd grows: 1, 2, 4, 8, (loss) → cwnd = 8 (congestion avoidance)
```

**Congestion Avoidance Phase**:
- Once cwnd ≥ ssthresh: Switch from slow start to congestion avoidance
- cwnd increases by 1 per RTT (linear growth)
- More conservative than exponential growth

**TCP Phases Summary**:
```
Slow Start:         cwnd exponentially increases until loss
Congestion Avoidance: cwnd linearly increases
Loss Detected:      cwnd drops to ssthresh/2, restart slow start
```

### TCP Fairness with Same RTT

**Scenario**: Two TCP flows with identical RTT and network conditions.

**Answer**: Yes, TCP is fair in this case.

**Why**:
- Both flows experience same network conditions
- AIMD algorithm is symmetric
- Both increase/decrease cwnd proportionally
- Converge to equal bandwidth share

**Example**:
```
Flow 1: cwnd = 4, Loss → cwnd = 2
Flow 2: cwnd = 4, Loss → cwnd = 2
Both grow additively to 3, 4, 5, ...
Eventually: cwnd1 ≈ cwnd2 ≈ C/2 (where C = link capacity)
```

**Convergence to Fairness**:
```
Bandwidth Allocation
 | Flow 1 ═══╗
 |           ║ Converges
 | Flow 2 ═══╝
 |___________ time

Each gets ~50% of link capacity
```

### TCP Fairness with Different RTTs

**Scenario**: Two TCP flows with different RTT values.

**Answer**: No, TCP is NOT fair when RTTs differ significantly.

**Why**:
- Flow with smaller RTT can increase cwnd faster
- Experiences more ACKs per unit time
- Additive increase happens more frequently
- Multiplicative decrease still same magnitude (×0.5)

**Example**:
```
Flow A: RTT = 10 ms
Flow B: RTT = 100 ms

In 100ms time period:
Flow A: 10 ACKs, increases cwnd 10 times
Flow B: 1 ACK, increases cwnd 1 time
Flow A grows much faster!

At congestion, both reduce cwnd by ×0.5:
Flow A: 16 → 8
Flow B: 2 → 1
Absolute reduction same, but impact different!
```

**Result**: Low-RTT flows get more bandwidth

**Bandwidth Ratio**:
```
Bandwidth_A / Bandwidth_B ≈ RTT_B / RTT_A

If RTT_B = 10 × RTT_A:
Flow A gets ~10× bandwidth of Flow B
```

**Issues**:
- Satellite links (500ms RTT) starved by local flows (1ms RTT)
- Unfair and inefficient
- Still an open problem in networking

### TCP CUBIC

**Definition**: Modern TCP congestion control algorithm replacing TCP Reno for high-speed networks.

**Problem with AIMD/Reno**:
- Linear increase too slow for high-speed links
- Recovery after loss very slow (increases by 1 per RTT)
- Not efficient for high BDP (Bandwidth-Delay Product) networks

**CUBIC Solution**:
- Uses **cubic function** for window growth
- Window = (t - K)³ + W_max / 2
- Where t = time, K = inflection point, W_max = window before loss

**Key Features**:

1. **Concave Growth Phase**:
   - Initial fast growth after congestion
   - Gradually slows down approaching W_max

2. **Convex Growth Phase**:
   - After reaching W_max, accelerates growth
   - Aggressive exploration for available bandwidth

3. **Fast Recovery**:
   - Quickly ramps up to previous window size
   - Then continues growing

**Behavior**:
```
Cubic Growth:
Window
  |       Convex phase
  |      (accelerating)
  |    /
  |   /
  |__/_________ W_max (concave phase, decelerating)
  |
  |___________
    Congestion    Time
```

**Benefits**:
- Better for high-speed, long-distance links
- Maintains TCP-friendliness (coexists with standard TCP)
- Used in Linux, Windows, Mac by default

### TCP Throughput Calculation

**Basic Formula**:
```
Throughput ≈ C / RTT
Where:
  C = constant depending on loss probability
  RTT = round trip time
```

**With Loss Probability p**:
```
Throughput ≈ (1.22 × MSS) / (RTT × √p)

Where:
  MSS = Maximum Segment Size (usually 1460 bytes)
  p = probability of packet loss
  RTT = round trip time
```

**Derivation Intuition**:
- TCP increase by 1 per RTT → takes 1/p RTTs to hit loss
- Before loss, cwnd goes from W/2 to W
- Average window ≈ 3W/4
- Time to lose one packet = (W/2)/(3W/4) × 1/p × RTT = (2/3) × RTT/p
- Throughput = MSS / Time = (3/2) × (1/RTT) × MSS × p

**Example Calculation**:
```
Given:
  MSS = 1460 bytes
  RTT = 50 ms
  Loss probability = 0.01 (1%)

Throughput = (1.22 × 1460) / (0.05 × √0.01)
           = 1780.2 / (0.05 × 0.1)
           = 1780.2 / 0.005
           = 356,040 bytes/sec
           ≈ 2.85 Mbps

Interpretation: With 1% loss and 50ms RTT, expect ~2.85 Mbps throughput
```

**Practical Measurement**:
```bash
# Measure TCP throughput using iperf
# Terminal 1 (Server):
iperf -s

# Terminal 2 (Client):
iperf -c <server_ip> -t 60

# Output shows throughput, loss percentage
# Bandwidth = X Mbits/sec
```

---

## Lesson 3: Intradomain Routing

### Forwarding vs. Routing

**Forwarding**:
- **Layer**: Data Plane (layer 2-3)
- **Function**: Moving packets between input and output ports
- **Scope**: Individual router, local decision
- **Timeframe**: Per-packet basis
- **Process**: Consult routing table, forward packet to appropriate port
- **Complexity**: Simple table lookup

**Routing**:
- **Layer**: Control Plane
- **Function**: Computing forwarding tables (determining paths through network)
- **Scope**: Entire network, global coordination
- **Timeframe**: Periodic, event-driven
- **Process**: Calculate shortest paths, distribute routing information
- **Complexity**: Significant computation and communication

**Analogy**:
- **Routing** = Planning the route (Google Maps)
- **Forwarding** = Actually driving (executing the plan)

**Relationship**:
```
Routing Algorithm → Forwarding Table → Forwarding Action
    (control)          (data)          (per packet)
```

### Link-State Routing Algorithm

**Core Idea**: Each router knows the complete network topology. All routers have identical topology knowledge.

**Key Features**:
- **Global Knowledge**: Every router knows entire network topology
- **Dijkstra's Algorithm**: Each router independently calculates shortest path to all destinations
- **Local Computation**: No need for routing messages (topology is known)
- **Convergence**: All routers independently compute identical paths

**Process**:
1. Each router floods its link state to all other routers
2. Every router receives LSAs (Link State Advertisements) from all other routers
3. Each router builds complete topology map
4. Each router runs Dijkstra independently
5. All routers compute identical shortest paths

### Link-State Routing - Example

**Example Network**:
```
        1
    A ------- B
    |         |
  2 |       3 | 2
    |         |
    D ------- C
        4
```

**Topology Knowledge** (all routers know):
- A-B: cost 1
- B-C: cost 2
- C-D: cost 4
- D-A: cost 2

**Dijkstra Execution from A** (finding shortest paths from A):

| Iteration | Visited | Current | Distances |
|-----------|---------|---------|-----------|
| 0 | A | A | A:0, B:∞, C:∞, D:∞ |
| 1 | A,B | B | A:0, B:1, C:∞, D:2 |
| 2 | A,B,D | D | A:0, B:1, C:6, D:2 |
| 3 | A,B,C,D | C | A:0, B:1, C:3, D:2 |

**Shortest Paths from A**:
- To A: Direct (cost 0)
- To B: Via B (cost 1)
- To D: Via D (cost 2)
- To C: Via B→C (cost 1+2=3) or D→C (cost 2+4=6), choose B→C

**Routing Table at A**:

| Destination | Next Hop | Cost |
|-------------|----------|------|
| B | B | 1 |
| C | B | 3 |
| D | D | 2 |

**Computational Complexity**:
```
Dijkstra: O(n log n) where n = number of routers
With n routers: O(n² log n) with naive implementation
With binary heap: O((n + m) log n) where m = edges
```

### Distance Vector Routing Algorithm

**Core Idea**: Routing by rumor. Routers only know their neighbors and exchange distance vectors periodically.

**Key Features**:
- **Local Knowledge**: Routers only know direct neighbors
- **Iterative**: Routers exchange info with neighbors repeatedly
- **Decentralized**: No central computation point
- **Bellman-Ford**: Based on finding shortest path through neighbors

**Key Equation** (Bellman-Ford):
```
D_x(y) = min_v { c(x,v) + D_v(y) }

Meaning: Distance from x to y = minimum over all neighbors v of:
         (cost to neighbor v) + (v's distance to y)
```

**Process**:
1. Each router starts with direct neighbor costs
2. Each router exchanges distance vectors with neighbors
3. Each router updates distances using Bellman-Ford equation
4. Repeat until convergence

### Distance Vector - Example

**Example Network**:
```
    A ----1---- B ----1---- C
         2         2
         └─────────┘
```

**Initial State** (direct neighbors only):
- A: {A:0, B:1}
- B: {A:1, B:0, C:1}
- C: {B:1, C:0}

**Round 1 - Routers exchange vectors**:

Router A receives from B:
- B's vector: {A:1, B:0, C:1}
- Update via B: A→C = 1 (A→B) + 1 (B→C) = 2

Router C receives from B:
- B's vector: {A:1, B:0, C:1}
- Update via B: C→A = 1 (C→B) + 1 (B→A) = 2

After Round 1:
- A: {A:0, B:1, C:2}
- B: {A:1, B:0, C:1}
- C: {A:2, B:1, C:0}

**Routing Tables After Convergence**:

| Router A | Destination | Distance | Next Hop |
|----------|-------------|----------|----------|
| | B | 1 | B |
| | C | 2 | B |

| Router B | Destination | Distance | Next Hop |
|----------|-------------|----------|----------|
| | A | 1 | A |
| | C | 1 | C |

| Router C | Destination | Distance | Next Hop |
|----------|-------------|----------|----------|
| | A | 2 | B |
| | B | 1 | B |

### Count-to-Infinity Problem

**Problem**: When a link fails, distance vector routers can fall into a loop where distance estimates grow infinitely.

**Scenario**:
```
Original: A ----1---- B ----1---- C

Link B-C fails!
```

**Bad Scenario**:
```
Round 1:
- C tries to reach B, distance grows to ∞
- B still thinks C is reachable via A: B→A(1) + A→C(∞) = ∞
- Actually B to C would be: B→A(1) + A→? C is via B originally

Round 2:
- A receives from B: "I can reach C at distance 2"
- A updates: A→C = 2 (via B)

Round 3:
- C still down the link B-C
- But A/B keep increasing distances
- A says: distance to C is 2
- B asks A: "Can you reach C?" A: "Yes, 2"
- B updates: distance to C is 3 (via A)

This continues: 2 → 3 → 4 → 5 → ... until reaching "infinity"
```

**Why It Happens**:
- Routers have inconsistent view of topology
- Outdated information creates loops
- Packets bounce in loop, distance grows each hop
- Takes time proportional to max distance to detect failure

### Poison Reverse Solution

**Solution**: If router X learns about destination D via router Y, router X tells Y that X's distance to D is ∞ (poisoned).

**Mechanism**:
- Break immediate loops by making loop path invalid
- Route back through next-hop becomes unreachable
- Prevents ping-ponging of packets

**Example Prevention**:
```
Original link B-C fails:

Round 1:
- C can't reach B, distance = ∞
- B sends DV to A with C at ∞ (poison reverse)
- A receives from B: "You can't reach C" → A doesn't use B for C
- A still can't reach C either

Prevents: Loop where A→B→A→B...
```

**Limitations**:
- Solves immediate loops (2-hop cycles)
- Doesn't solve larger loops (A→B→C→A)
- Maximum hop count (like TTL) still needed as fallback

### Routing Information Protocol (RIP)

**Definition**: Distance vector routing protocol for intradomain routing.

**Characteristics**:
- **Algorithm**: Bellman-Ford algorithm
- **Metric**: Hop count (each link = 1)
- **Max Distance**: 15 hops (16 = infinity)
- **Update Interval**: Every 30 seconds
- **Convergence**: Slow (several minutes)
- **Overhead**: Exchanges full routing tables

**RIP Versions**:
- **RIPv1**: No subnet mask, classful addressing
- **RIPv2**: Includes subnet mask, more flexible

**RIP Entry Format**:
```
- Address Family Identifier (AFI): 2 (IPv4)
- IP Address
- Subnet Mask
- Next Hop
- Metric (1-15)
```

**RIP Limitations**:
- Hop count metric doesn't consider link bandwidth or delay
- Slow convergence (routing loops possible during failure)
- High overhead for large networks
- Rarely used in modern networks (replaced by OSPF)

**Practical Command** (if RIP were running):
```bash
# View RIP routing table
show ip rip database

# Enable RIP on router interface
router rip
  version 2
  network 192.168.0.0
  passive-interface GigabitEthernet0/0  # Don't send RIP on this interface
```

### Open Shortest Path First (OSPF)

**Definition**: Link-state routing protocol for intradomain routing.

**Characteristics**:
- **Algorithm**: Dijkstra (link-state)
- **Metric**: Cost (inverse of bandwidth: 100,000,000 / bandwidth)
- **Max Distance**: Unlimited
- **Update Interval**: Event-driven (not periodic)
- **Convergence**: Fast
- **Overhead**: LSAs only when topology changes

**Advantages over RIP**:
- Uses link bandwidth, not just hop count
- Fast convergence (seconds vs. minutes)
- Supports larger networks
- Event-driven updates (less overhead)
- Can load balance across equal-cost paths

**OSPF Areas**:
- Networks divided into areas
- Reduces computation complexity
- Area 0 (backbone) connects all areas
- Routers send LSAs only within area

**OSPF Router Types**:
- **Internal Routers**: Within single area
- **Area Border Routers (ABR)**: Connect multiple areas
- **Backbone Routers**: Attach to area 0
- **Autonomous System Border Routers (ASBR)**: Connect to other ASes

**OSPF Packet Types**:
1. **Hello**: Discover neighbors, maintain relationships
2. **Database Description (DBD)**: Summarize LSA contents
3. **Link State Request**: Request specific LSAs
4. **Link State Update**: Flood LSAs
5. **Link State Acknowledgment**: Confirm LSA receipt

**Practical Commands**:
```bash
# Enable OSPF on router
router ospf 1
  network 192.168.0.0 0.0.0.255 area 0
  network 10.0.0.0 0.0.0.255 area 1

# View OSPF database
show ip ospf database

# View OSPF neighbors
show ip ospf neighbor

# View OSPF routing table
show ip route ospf

# Debug OSPF
debug ip ospf adj  # Debug adjacency formation
debug ip ospf events  # Debug OSPF events
```

### How Router Processes Advertisements

**LSA Processing Steps**:

1. **Receive LSA**:
   - Check sequence number (discard old updates)
   - Check checksum (validate)
   - Check age (too old = discard)

2. **Compare with Database**:
   - If newer version exists in database: Update database
   - If same version: May retransmit if needed
   - If older version: Flood current version back to sender

3. **Update Routing Table**:
   - If LSA changes topology, recalculate routes
   - Run Dijkstra algorithm

4. **Flood Advertisement**:
   - Send LSA to all neighbors except sender
   - Ensures network-wide propagation

**Sequence Number Management**:
- LSAs include sequence number
- New version = higher sequence number
- Prevents routing loops from old advertisements
- Wraps around using modulo arithmetic

### Hot Potato Routing

**Definition**: Routers send traffic to nearest (lowest cost) exit as quickly as possible.

**Principle**: "Send packets out ASAP; minimize time in your network"

**Mechanism**:
- Routing decision driven by "hot potato" metric
- Choose path with minimum internal cost
- Get packets out of your AS as quickly as possible

**Example**:
```
AS 1 receives traffic destined for AS 2
  │
  ├─ Exit 1 (cost 5 internal)
  ├─ Exit 2 (cost 3 internal) ← Choose this!
  └─ Exit 3 (cost 10 internal)

Even if Exit 1 has better external path,
AS 1 chooses Exit 2 because it exits faster
```

**Business Perspective**:
- Internet exchange points (IXPs) serve as hot potatoes
- AS wants to offload traffic quickly
- Reduces operational cost (less bandwidth through own network)

**Practical Impact**:
- Traffic between two ASes might take indirect paths
- Not globally optimal but individually rational for each AS
- Can lead to inefficient routing from end-user perspective

---

## Lesson 4: AS Relationships and Interdomain Routing

### Relationships Between ISPs, IXPs, and CDNs

**ISP (Internet Service Provider)**:
- Provides connectivity to end users and businesses
- Owns infrastructure (fiber, routers, data centers)
- Charges customers for bandwidth
- Interconnects with other ISPs through:
  - Peering (bilateral direct connection)
  - Transit (paying for upstream connectivity)
  - IXPs (Internet Exchange Points)

**IXP (Internet Exchange Point)**:
- Neutral meeting point where ISPs/networks interconnect
- Provides switching infrastructure
- Facilitates traffic exchange between parties
- Reduces cost vs. direct bilateral links
- Often cheaper than paying transit provider

**CDN (Content Delivery Network)**:
- Distributes content geographically to end users
- Caches popular content closer to users
- Reduces bandwidth cost for content providers
- Interconnects with ISPs at IXPs
- Reduces congestion on Internet backbone

**Relationships**:
```
Content Provider → CDN ┐
                       ├─ IXP ─┬─ ISP A
User (ISP) ───────────┤        ├─ ISP B
                       ├─ ISP C
                       └─ Other CDNs
```

**Economic Dynamics**:
- ISPs pay CDNs for content delivery
- Or negotiate free peering at IXPs
- CDNs benefit from lower cost, better performance
- ISPs benefit from reduced peering costs

### What Is an AS (Autonomous System)?

**Definition**: A collection of IP networks and routers under the control of a single entity (organization) that uses a single routing policy internally.

**Key Characteristics**:
- **Single Administrative Control**: One organization manages all routers
- **Unique AS Number (ASN)**: Identifier for the AS (e.g., AS 15169 = Google)
- **Internal Routing**: Uses single IGP (RIP, OSPF, IS-IS)
- **External Routing**: Uses BGP to communicate with other ASes

**Examples**:
- AS 15169: Google
- AS 8075: Microsoft
- AS 16509: Amazon (AWS)
- AS 3561: Savvis (hosting provider)
- AS 209: Qwest (ISP)

**AS Structure**:
```
AS 65000
┌─────────────────────────┐
│ Router 1 ─── Router 2   │
│    ↓          ↓         │
│ Router 3 ─── Router 4   │
│                         │
│ (Internal routing)      │
└─────────────────────────┘
         │
    (BGP routing to other ASes)
         │
    ┌────┴────┐
    ↓         ↓
  AS 2     AS 3
```

### AS Relationships with Other Parties

**Types of AS Relationships**:

1. **Peer-to-Peer (Peering)**
   - Two equal ISPs exchange traffic directly
   - Typically free (no money changes hands)
   - Both benefit from reduced backbone traffic
   - Example: AT&T and Verizon peering at IXP

2. **Provider-Customer (Transit)**
   - Customer ISP pays provider for connectivity
   - Customer receives routes to all providers' customers
   - Provider typically larger (upstream)
   - Example: Small ISP buys Internet access from large ISP

3. **Hybrid**
   - Complex relationships, especially for large ISPs
   - Peering with some, customer to others
   - May vary by region or service

**Economic Incentives**:
```
Large ISP perspective:
  - Peering reduces backbone cost ✓
  - But reduces revenue (transit fees) ✗
  - Only peers with similar-size networks

Small ISP perspective:
  - Peering beneficial if can reach customers ✓
  - But hard to negotiate with large ISPs ✗
  - Usually buys transit from upstream
```

### What Is BGP (Border Gateway Protocol)?

**Definition**: Interdomain routing protocol used to exchange routing information between Autonomous Systems.

**Purpose**: Enable routing across multiple ASes (the entire Internet)

**Key Characteristics**:
- **Protocol Type**: Path-vector routing (enhancement of distance vector)
- **Port**: TCP port 179
- **Convergence**: Slow (minutes)
- **Scalability**: Can handle Internet-scale (100,000+ prefixes)
- **Policy-Based**: Supports complex routing policies

**BGP Operation**:
- Routers establish BGP sessions (TCP connection)
- Exchange NLRIs (Network Layer Reachability Information) = IP prefixes
- Exchange AS_PATH = list of ASes the route traversed
- Make routing decisions based on policy and path attributes

**Why BGP, Not OSPF Across Internet**:
- OSPF requires global topology knowledge → doesn't scale
- BGP is policy-based → each AS can implement own policy
- BGP separates concerns → no need to trust other ASes' internals

### How an AS Determines Import/Export Rules

**Policy Decision Process**:

1. **Business Relationships**:
   - Which ASes are providers, peers, customers?
   - What traffic relationships exist?

2. **Import Rules** (incoming routes from neighbors):
   - **From Provider/Peer**: Accept and use
   - **From Customer**: Accept and use (and re-export to provider/peer)
   - **From Peer**: Don't re-export to other peers (peering is bilateral)
   - **Prepend AS_PATH**: Make routes less attractive if needed

3. **Export Rules** (sending routes to neighbors):
   - **To Provider**: Send only own routes (customer routes too)
   - **To Peer**: Send only own routes (not customer routes)
   - **To Customer**: Send everything (maximizing customer's reachability)

**Example Policy**:
```
AS A = ISP with customers and peers

Export policy:
  - To Provider: Export A's routes + customer routes
  - To Peer B: Export only A's routes (not customer routes)
  - To Customer C: Export all routes

Import policy:
  - From Provider: Accept all
  - From Peer: Accept routes originating from peer's customers
  - From Customer: Accept all
```

**Practical BGP Configuration** (Cisco):
```
router bgp 65000
  neighbor 10.0.0.1 remote-as 65001    ! Peer connection
  
  address-family ipv4
    neighbor 10.0.0.1 activate
    
    ! Export policy
    neighbor 10.0.0.1 route-map EXPORT_TO_PEER out
    
    ! Import policy
    neighbor 10.0.0.1 route-map IMPORT_FROM_PEER in

! Define route maps
route-map EXPORT_TO_PEER permit 10
  match ip address prefix-list OWN_ROUTES
  
route-map IMPORT_FROM_PEER permit 10
  set as-path prepend 65000
```

### Original Design Goals of BGP

**Original Goals**:
1. **Reachability**: Ensure routes exist from all ASes to all others
2. **Policy**: Allow each AS to implement own routing policy
3. **Decentralization**: No central authority needed
4. **Scalability**: Handle growing Internet with many ASes

**Later Considerations**:
1. **Security**: BGP has no origin validation (can hijack prefixes)
2. **Convergence Time**: Improved convergence speed
3. **Memory/CPU**: Reduced resource consumption
4. **Route Optimization**: Better path selection (not just reachability)
5. **Traffic Engineering**: Fine-grained control over traffic flows

### BGP Basics

**BGP Messages**:

1. **OPEN**: Establish BGP session
   - Declares AS number
   - Router ID
   - Hold timer

2. **UPDATE**: Advertise reachability
   - NLRI (Network Layer Reachability Information) = IP prefixes
   - AS_PATH = AS sequence to reach prefix
   - Other path attributes

3. **KEEPALIVE**: Maintain session
   - Sent periodically (heartbeat)
   - Keeps TCP connection alive

4. **NOTIFICATION**: Error notification
   - Session terminated after NOTIFICATION

**BGP Session States**:
```
IDLE
  ↓ (connect)
CONNECT
  ↓ (send OPEN)
OPEN_SENT
  ↓ (receive OPEN)
OPEN_CONFIRM
  ↓ (receive KEEPALIVE)
ESTABLISHED (ready to exchange UPDATE)
```

**Path Attributes** (in UPDATE messages):
- **AS_PATH**: Sequence of ASes from origin to destination (e.g., 65001 65002 65003)
- **NEXT_HOP**: IP address of router advertising route
- **LOCAL_PREF**: Local preference (higher = preferred, AS 1 to choose routes internally)
- **MED (Multi-Exit Discriminator)**: Suggests external AS which entry point to use (lower = preferred)
- **ORIGIN**: How route was learned (IGP < EGP < Incomplete)

### iBGP vs. eBGP

**eBGP (External BGP)**:
- BGP session between routers in different ASes
- Crosses AS boundary
- Exchanges external route information
- More critical (must be reliable)

**iBGP (Internal BGP)**:
- BGP session between routers in same AS
- Used to distribute external routes internally
- Doesn't need to be as fast (internal redundancy)
- Reduces load on IGP

**Comparison**:

| Aspect | iBGP | eBGP |
|--------|------|------|
| **Location** | Within AS | Between ASes |
| **TTL** | No check | TTL = 1 typically |
| **AS_PATH** | Unchanged | Prepended with own AS |
| **LOCAL_PREF** | Propagated | Not sent externally |
| **Purpose** | Internal distribution | External connectivity |
| **Routes** | External routes | External routes |

**Example Topology**:
```
AS 65000
  R1 ─ (iBGP) ─ R2  ← Both internal BGP
  │               │
(eBGP)          (eBGP)
  │               │
AS 1            AS 2
```

### iBGP vs. IGP-like Protocols (RIP, OSPF)

| Feature | iBGP | IGP (OSPF/RIP) |
|---------|------|---|
| **Scope** | External AS routes | Internal network routing |
| **Information** | IP prefixes + AS_PATH | Network topology |
| **Algorithm** | Path vector | Distance vector / Link state |
| **Convergence** | Slow (minutes) | Fast (seconds) |
| **Overhead** | Low (policy-based) | Medium-High (topology updates) |
| **Scalability** | High (100,000+ prefixes) | Limited (100s of routes) |
| **Policy** | Fine-grained | Limited |

**Relationship**:
- IGP routes within AS (used by iBGP for next-hop)
- iBGP uses IGP routes to reach eBGP neighbors
- iBGP distributes external routes (learned via eBGP)

**Practical Setup**:
```
Router R1 connects to external AS via eBGP:
  ├─ Learns external routes via eBGP
  ├─ Distributes via iBGP to internal routers
  └─ Uses OSPF to find path to eBGP neighbor

Router R2 (internal):
  ├─ Learns external routes via iBGP
  ├─ Combines with IGP routes (via OSPF)
  └─ Makes forwarding decisions
```

### BGP Decision Process

**When multiple routes exist for same prefix, BGP chooses based on priority**:

1. **AS_PATH Length**: Prefer shorter AS_PATH (closer to destination)
2. **LOCAL_PREF**: Prefer higher LOCAL_PREF
3. **Origin**: IGP > EGP > Incomplete
4. **MED**: Prefer lower MED (if from same AS)
5. **eBGP vs. iBGP**: Prefer eBGP learned routes
6. **Closest IGP Neighbor**: Prefer route to closer BGP speaker
7. **Oldest Route**: Stability (prefer older routes)
8. **Router ID**: Lowest router ID wins tiebreaker

**Example Decision**:
```
Route 1: AS_PATH = 65001 65002, LOCAL_PREF = 100
Route 2: AS_PATH = 65003, LOCAL_PREF = 100
Route 3: AS_PATH = 65001 65002, LOCAL_PREF = 50

Decision:
  Route 3 eliminated (LOCAL_PREF = 50 < 100)
  Route 1 vs Route 2: Route 2 has shorter AS_PATH → Choose Route 2
```

### Two Main Challenges with BGP

**Challenge 1: Convergence Time**
- **Problem**: BGP converges very slowly (minutes) after topology change
- **Example**: If link fails, takes several minutes for all routers to re-converge
- **Cause**: Limited information sharing (only changed routes), path vector nature
- **Impact**: Traffic loss during convergence window
- **Partial Solution**: RFCs specify fast convergence, but still not as fast as IGP

**Challenge 2: Route Stability**
- **Problem**: BGP can oscillate between multiple equally good paths
- **Cause**: Multiple ASes implementing different policies
- **Example**:
  ```
  Path A preferred by AS 1 but not AS 2
  Path B preferred by AS 2 but not AS 1
  Routes keep switching between Path A and Path B
  ```
- **Impact**: Frequent path changes cause:
  - Increased router CPU (recalculating routes)
  - Route flapping (alternating announcements/withdrawals)
  - Increased latency variability
  - Network instability
- **Solution**: Route damping (suppress flapping routes temporarily)

### What Is an IXP (Internet Exchange Point)?

**Definition**: Physical facility where Internet carriers interconnect and exchange traffic.

**Components**:
- **Switching Infrastructure**: Layer 2 switches connecting members
- **Route Server**: BGP speakers that facilitate route exchange
- **Policy Enforcement**: Typically open peering (no restrictions)
- **Physical Infrastructure**: Rack space, fiber connections, power
- **Colocation Services**: Hosting of BGP speakers

**How It Works**:
```
ISP A                  ISP B
  │                      │
  └──────┬────────┬──────┘
         │        │
      Switch     Route Server
         │        │
  ┌──────┴────────┴──────┐
  │   IXP Peering LAN    │
  └──────┬────────┬──────┘
         │        │
  CDN   │        │  Content Provider
```

### Four Reasons for IXPs' Increased Popularity

1. **Cost Reduction**
   - Peering through IXP cheaper than bilateral peering with all parties
   - No need for dedicated lines to each peer
   - Shared switching infrastructure amortized across participants

2. **Local Content Delivery**
   - CDNs use IXPs to locate edge servers
   - Traffic stays local, improves user experience
   - Reduces backbone congestion

3. **Business Benefits for ISPs**
   - Improves customer experience (faster speeds)
   - Reduces upstream bandwidth costs
   - Increases competitive advantage

4. **Technology Advances**
   - Ethernet technology improvements made IXPs economical
   - Route server BGP automation simplified administration
   - Cost/performance improvements made IXP model scalable

### Services Provided by IXPs

1. **Public Peering**
   - Any member can peer with any other member
   - Open peering fabric
   - Route server facilitates

2. **Private Peering**
   - Direct bilateral connections between specific members
   - Bypasses switch fabric
   - Better SLA and performance control

3. **Colocation Services**
   - Hosting rack space for equipment
   - Power, cooling, security
   - Connect to switching fabric

4. **Management Services**
   - IP address allocation
   - Monitoring and statistics
   - Member directory

5. **Additional Services**
   - DDoS mitigation (scrubbing)
   - DNS services
   - NTP (network time protocol)

### How a Route Server Works

**Purpose**: Simplify BGP peering for many members at IXP.

**Problem Without Route Server**:
- N members need N(N-1)/2 BGP sessions (full mesh)
- For 100 members: ~5,000 BGP sessions!
- Impractical

**Route Server Solution**:
- All members establish one BGP session with route server
- Route server connects to all members
- Members don't need to peer with each other directly

**Architecture**:
```
Without Route Server:        With Route Server:
ISP 1 ←→ ISP 2  ←→ ISP 3   ISP 1    ISP 2
  ↑        ↓        ↑          \    /
  └────────┼────────┘         Route Server
          (O(n²) sessions)         ↑
                                ISP 3
                           (O(n) sessions)
```

**Route Server Function**:
1. Receives BGP sessions from all members
2. Accepts route announcements from each member
3. Re-announces routes to other members (with modifications)
4. Applies policies:
   - No modification of AS_PATH (key aspect)
   - No LOCAL_PREF injection (members control preferences)
   - Separate view for each member (not visible to others)

**BGP Policy at Route Server**:
```
Member A announces: 192.0.2.0/24 via AS 65001
Route Server sends to Member B: 192.0.2.0/24 AS_PATH: 65001
(NOT 65001 65002 as it would be with regular BGP)

Result: Routes appear directly reachable (no ASN prepended)
Benefits: Simpler routing, better path visibility
```

**Practical Advantages**:
- Scalability: O(n) vs O(n²)
- Simplification: Members don't configure peers
- Democratic: Any member can participate
- Isolation: Failed member doesn't affect others

---

## Lesson 5: Router Design and Algorithms (Part 1)

### Basic Components of a Router

**Main Components**:

1. **Input Ports**
   - Receives incoming packets
   - Physical interface (Ethernet, Serial, etc.)
   - Performs line termination
   - Data link protocol processing
   - Lookup forwarding table entry

2. **Switching Fabric**
   - Transfers packets from input to output ports
   - Core of router performance
   - Determines maximum throughput

3. **Output Ports**
   - Receives packets from switching fabric
   - Performs queuing/buffering
   - Link layer framing
   - Transmits on outgoing link

4. **Routing Processor (Control Plane)**
   - Runs routing protocols (BGP, OSPF)
   - Computes routing tables
   - Manages configuration
   - Typically runs on separate CPU

**Router Architecture Diagram**:
```
Input Ports        Switching Fabric        Output Ports
┌──────────┐      ┌─────────────────┐     ┌──────────┐
│ Eth0     ├─────→│                 ├────→│ Eth1     │
├──────────┤      │   Switching     │     ├──────────┤
│ Eth1     ├─────→│   Fabric        ├────→│ Eth2     │
├──────────┤      │  (Crossbar,     │     ├──────────┤
│ Eth2     ├─────→│   Bus, etc.)    ├────→│ Eth3     │
├──────────┤      │                 │     ├──────────┤
│ Eth3     ├─────→│                 ├────→│ Eth4     │
└──────────┘      └────────┬────────┘     └──────────┘
                           │
                    Routing Processor
                    (Control Plane)
```

### Forwarding (Switching) Function

**Purpose**: Move packets from input ports to output ports.

**Process**:

1. **Look Up Forwarding Table**
   - Input port receives packet
   - Extract destination IP address
   - Search forwarding table
   - Find output port

2. **Forward to Output Port**
   - Queue packet to output port
   - Switch fabric routes packet physically
   - Output port prepares transmission

3. **Handle Exceptions**
   - Checksum failed → discard
   - TTL expired → send ICMP
   - Unknown destination → send ICMP unreachable

**Decision Logic** (Per Packet):
```
for each arriving packet:
  1. Read destination IP address
  2. Find longest matching prefix in routing table
  3. Output port = routing table entry
  4. Queue packet to output port
  5. Perform error checking
```

**Performance Metric**: Forwarding throughput (e.g., 100 Gbps = can forward 100 Gbps of packets)

### Input and Output Port Functionalities

**Input Port Functions**:
1. **Physical Interface**: Receive bits from cable
2. **Line Termination**: Decapsulate physical layer framing
3. **Data Link Processing**:
   - Remove Ethernet header/trailer
   - Check CRC checksum
   - Manage MAC addresses
4. **IP Lookup**:
   - Extract destination IP
   - Search routing table (prefix matching)
   - Determine output port
5. **Packet Queuing**:
   - Buffer packet if switching fabric not ready
   - Forward to switching fabric

**Output Port Functions**:
1. **Packet Queuing**:
   - Buffer packets awaiting transmission
   - Manage queue discipline (FIFO, priority, weighted fair)
   - Drop packets if queue full (tail drop)
2. **Link Layer Processing**:
   - Add MAC header/trailer
   - Encapsulate in frame format
3. **Line Transmission**:
   - Transmit packet on outgoing link
   - Perform bit-level transmission

**Bottleneck Analysis**:
- **Bandwidth Constraint**: Output link speed (e.g., 1 Gbps limits transmission)
- **Queue Management**: Queuing discipline affects latency and loss
- **Scheduling**: Order of packet transmission affects fairness

### Purpose of Router's Control Plane

**Control Plane** (Routing Processor):
- Runs routing protocols (BGP, OSPF, RIP)
- Computes/updates routing tables
- Manages configuration
- Handles administrative functions

**Relationship to Data Plane**:
```
Control Plane (Slow, Complex)
  ├─ Runs routing protocols
  ├─ Computes routing decisions
  └─ Updates routing table ↓

Data Plane (Fast, Simple)
  ├─ Uses routing table
  ├─ Forwards packets quickly
  └─ Per-packet decisions
```

**Example Flow**:
```
BGP Update → Routing Processor computes new route
           → Updates routing table
           → Data plane forwards using new table
           → (milliseconds to seconds for update)
           → (nanoseconds per packet forwarding)
```

### Tasks Occurring in a Router

**Control Plane Tasks**:
1. **Routing Protocol Processing**: BGP, OSPF message handling
2. **Route Computation**: Dijkstra, Bellman-Ford algorithms
3. **Routing Table Management**: Insert, delete, update routes
4. **Configuration**: Interface configuration, policy management
5. **Monitoring**: Collect statistics, MIBs for SNMP
6. **Logging**: Record significant events

**Data Plane Tasks**:
1. **Packet Reception**: Receive bits from link
2. **Decapsulation**: Remove headers
3. **Lookup**: Find routing table entry
4. **Switching**: Route to output port
5. **Queuing**: Buffer management
6. **Encapsulation**: Add headers for output
7. **Transmission**: Send bits on link

**Time-Critical Path** (Per-Packet):
```
Receive packet → Lookup → Switch → Queue → Transmit
  (nanoseconds level)
```

**Non-Time-Critical Path** (Per-Update):
```
Receive routing update → Compute routes → Update table
  (milliseconds level)
```

### Types of Switching

**Three Main Types**:

#### 1. Switching via Memory
```
Architecture:
┌─────────────┐
│   Memory    │ (traditional approach)
│             │
│  Routing    │
│  Table      │
└─────────────┘
     ↑ ↓
 Input  Output
 Bus    Bus
```

**Process**:
1. Input port reads packet from link
2. Packet transferred to memory (system bus)
3. CPU copies packet to memory location
4. CPU looks up routing table
5. Packet copied from memory to output port buffer
6. Output port sends packet on link

**Characteristics**:
- All packets pass through CPU → memory bottleneck
- Limited by memory bandwidth (e.g., 10 Gbps buses)
- Bandwidth = 2 × Memory Bus (send + receive)
- Very limited scale

**Throughput Limitation**: ~4-5 Gbps on traditional systems

**Can send multiple packets in parallel**: No (single CPU)

#### 2. Switching via Bus
```
Architecture:
          Bus (shared)
          ↓ ↑
    ┌─────┴─┴─────┐
    │             │
┌─Input  ┌─Input  ┌─Output
│        │        │
```

**Process**:
1. Input port receives packet
2. Tags packet with destination output port
3. Packet placed on shared bus
4. Packet travels to destination output port
5. Output port extracts packet from bus

**Characteristics**:
- All packets share single bus → bus bandwidth limit
- No CPU involvement (except tagging)
- One packet at a time on bus
- Used in older Ethernet switches

**Throughput Limitation**: Limited by bus speed (e.g., 30-40 Gbps)

**Can send multiple packets in parallel**: No (single shared bus)

#### 3. Switching via Crossbar (Interconnect)
```
Architecture:
        Crossbar Switch
         (N×N matrix)
    ┌─────────────────────┐
    │  ┌─┐ ┌─┐ ┌─┐ ┌─┐   │
In1 ├──┤ ├─┤ ├─┤ ├─┤ ├──┤ Out1
    │  └─┘ └─┘ └─┘ └─┘   │
    │  ┌─┐ ┌─┐ ┌─┐ ┌─┐   │
In2 ├──┤ ├─┤ ├─┤ ├─┤ ├──┤ Out2
    │  └─┘ └─┘ └─┘ └─┘   │
    │  ┌─┐ ┌─┐ ┌─┐ ┌─┐   │
In3 ├──┤ ├─┤ ├─┤ ├─┤ ├──┤ Out3
    │  └─┘ └─┘ └─┘ └─┘   │
    │  ┌─┐ ┌─┐ ┌─┐ ┌─┐   │
In4 ├──┤ ├─┤ ├─┤ ├─┤ ├──┤ Out4
    │  └─┘ └─┘ └─┘ └─┘   │
    └─────────────────────┘
```

**Process**:
1. Each input port independently routes packet
2. Crossbar switch allows any input-output connection
3. Multiple inputs can send to different outputs simultaneously
4. Non-blocking (if scheduling configured correctly)

**Characteristics**:
- Parallel switching possible
- Multiple packets can transfer simultaneously
- Requires sophisticated scheduling to avoid conflicts
- Highest performance

**Throughput**: Limited only by link speeds (sum of all input/output links)

**Can send multiple packets in parallel**: Yes! (multiple independent paths)

**Scheduling Challenge**:
- What if two inputs want same output?
- Need arbitration/scheduling algorithm
- Some designs block (packets wait), others have parallel paths

**Modern routers use Crossbar** (or faster variants like optical switches)

### Two Fundamental Problems with Routers

**Problem 1: Switching Fabric Congestion**
- **Issue**: Packets arrive on multiple input ports destined to same output
- **Result**: Output port becomes bottleneck
- **Consequence**: Queue builds at output, eventually overflows (tail drop)
- **Example**:
  ```
  Input 1 → Packet A (to Output 1)
  Input 2 → Packet B (to Output 1)
  Input 3 → Packet C (to Output 1)
  
  Output 1 can only transmit one at a time:
  Packets B and C must queue, then drop if queue full
  ```

**Problem 2: Head-of-Line (HOL) Blocking**
- **Issue**: Packet at front of input queue blocks subsequent packets
- **Scenario**: 
  ```
  Input Queue:
  1. Packet to Output 2 (blocked - Output 2 busy)
  2. Packet to Output 1 (ready - Output 1 free)
  3. Packet to Output 1 (ready - Output 1 free)
  ```
- **Result**: Packets 2,3 can't send despite Output 1 being free
- **Consequence**: Reduced throughput, wasted capacity
- **HOL Blocking Throughput Limit**: ~59% utilization with uniform traffic

**Causes**:
- FIFO queue at input (processes in order)
- Can't see beyond head-of-queue packet
- Must wait for head packet's output to be free

### Router Bottlenecks and Their Causes

**Bottleneck 1: Memory Bandwidth** (Switching via Memory)
- **Cause**: All packets pass through single shared memory and bus
- **Limitation**: Memory bandwidth (e.g., 10-20 GBps)
- **Per-packet cost**: Time to write to memory + read from memory
- **Example**: 1 Gbps link can transmit 125 MB/s, multiple ports exceed memory bandwidth

**Bottleneck 2: Bus Contention** (Switching via Bus)
- **Cause**: All packets share single bus
- **Limitation**: Bus bandwidth (e.g., 100 Gbps buses max)
- **Serialization**: Packets sent one-at-a-time on bus
- **Latency**: Increased queuing as packets wait for bus access

**Bottleneck 3: Switching Fabric Non-Blocking Properties** (Crossbar)
- **Cause**: Improper scheduling can block packets
- **Limitation**: Scheduling overhead, complexity
- **Issue**: If scheduling not optimized, blocking still occurs

**Bottleneck 4: Output Link Capacity**
- **Cause**: Each output link has fixed capacity
- **Limitation**: Link bandwidth (e.g., 1 Gbps, 10 Gbps)
- **Consequence**: Multiple inputs sending to same output cause queuing

**Bottleneck 5: Lookup/Forwarding Latency**
- **Cause**: Prefix matching in routing table takes time
- **Limitation**: Memory latency, complex algorithms
- **Challenge**: Must be done per-packet (nanosecond timescale)

### Prefix Notation Conversion

**Three Prefix Notation Formats**:

#### 1. Dot-Decimal (with netmask)
```
Example: 192.168.1.0 with netmask 255.255.255.0
Meaning: First 24 bits are network, last 8 bits are host
Range: 192.168.1.0 through 192.168.1.255
```

#### 2. CIDR Slash Notation
```
Example: 192.168.1.0/24
Meaning: /24 = first 24 bits are network
Equivalent to: 192.168.1.0 with netmask 255.255.255.0
```

#### 3. Bit Masking
```
192.168.1.0 in binary: 11000000.10101000.00000001.00000000
Netmask 255.255.255.0: 11111111.11111111.11111111.00000000
AND operation masks last octet to 0
Result: Network address is 192.168.1.0
```

**Conversion Examples**:

| Dot-Decimal | CIDR | Bits |
|-------------|------|------|
| 192.168.0.0 / 255.255.255.0 | 192.168.0.0/24 | First 24 bits |
| 10.0.0.0 / 255.255.0.0 | 10.0.0.0/16 | First 16 bits |
| 172.16.0.0 / 255.255.240.0 | 172.16.0.0/20 | First 20 bits |

**Practical Command to Convert**:
```bash
# Using ipcalc to show conversions
ipcalc 192.168.1.0/24

# Output:
# Address:   192.168.1.0
# Netmask:   255.255.255.0
# Wildcard:  0.0.0.255
# Network:   192.168.1.0
# Broadcast: 192.168.1.255
# HostMin:   192.168.1.1
# HostMax:   192.168.1.254
```

### What Is CIDR and Why Was It Introduced?

**CIDR (Classless Inter-Domain Routing)**:
- Replaces classful IP addressing
- Allows arbitrary prefix lengths (/1 to /32)
- More efficient IP space allocation

**Classful Addressing (Pre-CIDR)**:
```
Class A: 1.0.0.0 - 126.255.255.255  (/8)  → 16 million addresses per block
Class B: 128.0.0.0 - 191.255.255.255 (/16) → 65,536 addresses per block
Class C: 192.0.0.0 - 223.255.255.255 (/24) → 256 addresses per block
```

**Problems with Classful**:
- No flexibility: Class B too big for some organizations, Class C too small
- Wasteful allocation: Company needing 500 addresses must take Class B (65,536)
- Route table explosion: Each Class C = one routing table entry

**CIDR Solution**:
- Any prefix length allowed (/8, /16, /22, /24, /27, etc.)
- Allocate exactly what organization needs
- Fewer routing table entries through aggregation

**Example Benefits**:
```
Before CIDR (needs ~256 routes):
200.1.0.0/24
200.2.0.0/24
200.3.0.0/24
... (many more)

After CIDR (aggregates to 1 route):
200.0.0.0/8 (covers all 256 /24 blocks)
```

**CIDR Aggregation (Supernetting)**:
```
Multiple small prefixes → Single large prefix
200.1.0.0/24
200.2.0.0/24
200.3.0.0/24
200.4.0.0/24
Can aggregate to: 200.0.0.0/21 (if contiguous)
Reduces routing table entries dramatically
```

### Network Traffic Characteristics - Four Key Observations

**Observation 1: Traffic is Bursty (Not Smooth)**
- **Fact**: Network traffic arrives in bursts, not uniform rate
- **Cause**: Applications send in bursts (TCP slow start, user clicks)
- **Consequence**:
  - Need large buffers to smooth bursts
  - Queuing delays increase
  - Tail drop more likely

**Observation 2: Many Flows, Dominated by Few**
- **Fact**: Many flows exist, but few dominate bandwidth
- **Pattern**: 80% traffic from 20% of flows (Pareto distribution)
- **Consequence**:
  - Large flows cause congestion
  - Many small flows matter less
  - Scheduling should prioritize fairness across flows

**Observation 3: Traffic Follows Spatial Locality**
- **Fact**: Most traffic destined for nearby networks, not random
- **Cause**: People access local content (web, email)
- **Consequence**:
  - Prefix table shows locality patterns
  - Can optimize prefix lookup for common destinations
  - Hot potato routing exploits this

**Observation 4: TCP Timescale Properties**
- **Fact**: RTT varies (milliseconds to hundreds of ms)
- **Consequence**:
  - Packet losses detected at different times
  - Timeout-based retransmission varies
  - Congestion control convergence depends on RTT

### Why We Need Multibit Tries

**Problem with Unibit Tries** (checking 1 bit at a time):
- **Lookup Steps**: 32 steps for IPv4 (one per bit)
- **Memory Access**: 32 memory accesses per lookup
- **Latency**: Very high (each access ~10-100 ns, total ~300+ ns)
- **Throughput**: Limited by lookup latency

**Example Unibit Trie** (inefficient):
```
Prefix lookup for 192.168.1.5:
Bit 1 (1) → Node 1
Bit 2 (1) → Node 2
Bit 3 (0) → Node 3
Bit 4 (0) → Node 4
... (28 more iterations)
Total: 32 memory accesses!
```

**Multibit Tries Solution**:
- Process multiple bits at once (stride = 2, 4, 8, 16 bits)
- Reduce lookup depth: 32 bits ÷ 8-bit stride = 4 steps instead of 32
- Trade-off: More memory, faster lookup

**Memory vs. Speed Trade-off**:
```
Unibit Trie:
  Memory: Low (only used prefixes)
  Latency: High (32 accesses)

Multibit Trie (8-bit stride):
  Memory: Higher (256 entries per node)
  Latency: Low (4 accesses)

8-bit stride typically optimal balance
```

### Prefix Expansion and Why It's Needed

**Prefix Expansion**: Converting prefixes to fixed lengths for use in multibit trie.

**Why Needed**:
- Multibit tries expect fixed-stride nodes
- Prefixes can be any length (e.g., 21, 23, 26 bits)
- Must expand prefixes to align with stride boundaries

**Example - 8-bit Stride**:
```
Original prefixes:
  192.168.1.0/24 (24 bits)
  10.0.0.0/8 (8 bits)
  172.16.0.0/12 (12 bits)

After expansion to 8-bit boundaries:
  192.168.1.0/24 → {
    192.168.1.0/24
    ... (entire /24 network)
  }
  10.0.0.0/8 → {
    10.0.0.0/8
    10.1.0.0/8
    ... (entire /8 network)
  }
  172.16.0.0/12 → {
    172.16.0.0/12
    ... (16 entries for each /8)
  }

Result: All prefixes aligned to 8-bit boundaries
```

**Storage:**
- **Old prefixes**: Original count (few)
- **New prefixes**: Expanded count (much larger)

**Example Explosion**:
```
1 prefix of /1 expands to 128 entries of /8
1 prefix of /8 expands to 1 entry of /8
1 prefix of /16 expands to 256 entries of /16 (if using 8-bit)

Memory usage can 10-100x increase depending on prefix distribution
```

### Performing Prefix Lookups with Different Trie Types

**Unibit Trie Example**:
```
Lookup 192.168.1.5:
Binary: 11000000.10101000.00000001.00000101

Trie:
Level 0 (bit 1=1):
    ├─ 0: [Subtrie]
    └─ 1: [Subtrie] ← Take this
Level 1 (bit 2=1):
    ├─ 0: [Subtrie]
    └─ 1: [Subtrie] ← Take this
...
Result: 32 levels, at each level follow one of two paths based on bit
```

**Fixed-Length Multibit Trie (8-bit stride)**:
```
Lookup 192.168.1.5:
IP: 192.168.1.5
Split to octets: [192, 168, 1, 5]

Trie Level 0 (first octet = 192):
  Array[0..255]: Array[192] points to next trie

Trie Level 1 (second octet = 168):
  Array[0..255]: Array[168] points to next trie

Trie Level 2 (third octet = 1):
  Array[0..255]: Array[1] points to next trie

Trie Level 3 (fourth octet = 5):
  Array[0..255]: Array[5] contains result

Result: 4 array accesses!
```

**Variable-Length Multibit Trie (variable stride)**:
```
Stride 1: Depth 1 (2 entries)
Stride 2: Depth 2 (4 entries)
Stride 8: Depth 3 (256 entries)

Dynamically choose stride based on prefix distribution
Typically uses 8-16 bit strides
```

### Prefix Expansion Details

**How Many Prefix Lengths Do Prefixes Have After Expansion?**

**Old Prefixes**: Arbitrary lengths (1-32)
```
Example: 192.168.1.0/24, 10.0.0.0/8, 172.16.0.0/12
Lengths: 24, 8, 12
```

**New Prefixes**: All multiples of stride length
```
Example (8-bit stride): All prefixes become /8, /16, /24, /32
Or (16-bit stride): All prefixes become /16, /32

So new prefixes only at boundary multiples
```

### Benefits of Variable-Stride vs. Fixed-Stride Multibit Tries

**Fixed-Stride Multibit Trie**:
```
Example: 8-bit stride throughout
┌─────────┬─────────┬─────────┬─────────┐
│  Stride │  Stride │  Stride │  Stride │
│    8    │    8    │    8    │    8    │
└─────────┴─────────┴─────────┴─────────┘
Depth: 4 (fixed)
Memory per node: 256 entries (fixed)
```

**Benefits**:
- Simple, uniform structure
- Predictable memory usage
- Fast lookup (consistent depth)

**Drawbacks**:
- May waste memory (empty nodes)
- Doesn't adapt to prefix distribution
- Fixed 4 levels even if could do in 2-3 with better distribution

**Variable-Stride Multibit Trie**:
```
Example: Adapts to distribution
┌─────────────┐
│  Stride 16  │ [65,536 entries]
└──┬────┬────┘
   ├─ Stride 8 ─ Stride 8
   └─ Stride 8 ─ Stride 8

Depth: 2-3 depending on path
Memory per node: Variable (8-65,536 entries)
```

**Benefits**:
- Adapts to actual prefix distribution
- Can achieve shallower trees if prefixes concentrated
- Memory-efficient (don't allocate unused strides)
- Faster lookup for common cases

**Drawbacks**:
- More complex implementation
- Variable depth lookup
- Memory allocation complexity

**Practical Decision**:
- Modern routers typically use 16-bit or 32-bit strides (2 levels for IPv4)
- Variable stride for specialized cases
- Modern hardware (TCAM - Ternary CAM) often does exact prefix match in constant time

---

## Lesson 6: Router Design and Algorithms (Part 2)

### Why Packet Classification Is Needed

**Purpose**: Differentiate packets for different treatment beyond simple destination-based forwarding.

**Use Cases**:

1. **Quality of Service (QoS)**
   - Prioritize video traffic over email
   - Reserve bandwidth for VoIP
   - Different delay/loss tolerances

2. **Firewall Rules**
   - Allow/block based on source, destination, port, protocol
   - Security policy enforcement

3. **Policy-Based Routing**
   - Route based on source/destination, not just destination
   - Load balancing across multiple paths

4. **Traffic Engineering**
   - Shape traffic for specific applications
   - Charge different rates based on classification

5. **Access Control**
   - Differentiate between customers/VIP vs. regular
   - Per-user policies

**Classification Criteria**:
- Source/Destination IP addresses
- Source/Destination ports
- Protocol type (TCP/UDP/ICMP)
- IP version, flags, ToS bits
- Packet type

### Three Established Variants of Packet Classification

**Variant 1: Linear Search**
- **Method**: Check each rule sequentially until match
- **Complexity**: O(rules) - must check all rules in worst case
- **Lookup Time**: Slow (100-1000+ rules possible)
- **Implementation**: Simple

**Variant 2: Caching**
- **Method**: Cache recently matched rules
- **Complexity**: O(1) cache hit, O(rules) cache miss
- **Benefit**: Locality of reference (packets often have same source)
- **Implementation**: Simple

**Variant 3: Trie-Based**
- **Method**: Build trie structures for multi-dimensional matching
- **Complexity**: O(k × log(n)) where k = number of fields, n = rules
- **Benefit**: Much faster than linear search
- **Implementation**: Complex, memory-intensive

### Simple Solutions to Packet Classification Problem

**Solution 1: Largest Matching Prefix**
```
For destination-based routing:
Match longest prefix in routing table
Example:
  Rule 1: 192.168.0.0/16 → Forward to Port 1
  Rule 2: 192.168.1.0/24 → Forward to Port 2
  Packet to 192.168.1.5 matches both, choose Rule 2 (longer prefix)
```

**Solution 2: Priority-Based Rules**
```
Rules checked in priority order:
  Rule 1 (Priority 100): If port 22 → Deny
  Rule 2 (Priority 50): If source 10.0.0.0/8 → Allow
  Rule 3 (Priority 1): Default → Deny

Packet arrives: Pick first matching rule
```

**Solution 3: Ternary Content Addressable Memory (TCAM)**
```
Hardware-based solution:
- Matches 0, 1, or X (wildcard) in constant time
- Used in modern routers
- Very fast but expensive and power-hungry
- Typical capacity: 1 million entries
```

### Fast Searching Using Set-Pruning Tries

**Basic Idea**: Build separate tries for each field, prune cross-matching results.

**Architecture**:
```
Source IP Trie ┐
               ├─ Find matches in each trie
Dest IP Trie   ├─ Intersect results
Protocol Trie  ┤
Port Trie      │
...            ┘
```

**Example**:
```
Rule 1: Source 10.0.0.0/16, Dest 192.168.0.0/16, TCP → Action A
Rule 2: Source 10.0.0.0/16, Dest 172.16.0.0/16, UDP → Action B

Packet: Source 10.0.0.1, Dest 192.168.1.1, TCP

Lookup process:
1. Source IP Trie: 10.0.0.1 matches 10.0.0.0/16 → {Rule 1, Rule 2}
2. Dest IP Trie: 192.168.1.1 matches 192.168.0.0/16 → {Rule 1}
3. Protocol Trie: TCP matches → {Rule 1}
4. Intersect results: Rule 1 is in all three → Match!
```

### Main Problem with Set-Pruning Tries

**Problem**: Cross-product explosion in memory

**Example**:
```
Source trie has 1000 prefixes
Dest trie has 1000 prefixes
For each source prefix, must store list of destination prefixes

Memory = 1000 × 1000 = 1,000,000 entries
Becomes impractical with more fields or larger tries
```

**Memory Complexity**: O(∏(number of rules for each field))

**When it explodes**:
- Many overlapping rules
- Multiple fields with similar distribution
- Large number of rules

### Difference Between Pruning and Backtracking Approaches

**Pruning Approach**:
```
Build tries for each field independently
For each source prefix, store list of destination prefixes

Advantages:
  - Fast lookup (direct access to candidates)

Disadvantages:
  - Memory explosion with many rules
  - Redundancy (same dest prefix stored multiple times)

Memory: High
Lookup: Fast
```

**Backtracking Approach**:
```
Traverse trie for first field
For each match, recursively search trie for second field
Only follow branches that could match

Advantages:
  - Better memory efficiency (no redundancy)
  - Adaptive to rule distribution

Disadvantages:
  - Slower lookup (more steps)
  - Complex implementation

Memory: Low
Lookup: Moderate
```

### Benefit of Grid of Tries Approach

**Grid of Tries**: Multi-dimensional structure organizing rules in grid

**Architecture**:
```
2D Grid (Source IP × Dest IP):
        Dest IP
          1  2  3 ...
Source 1: [R1][ ][ ]
IP     2:  [ ][R2][R3]
       3: [R4][R5][ ]

Lookup: Go to cell (source, dest) and check rules there
```

**Benefits**:
1. **Balanced Memory**: Not as bad as full cross-product
2. **Efficient Lookup**: Direct access to relevant rules
3. **Scalability**: Works with multiple fields
4. **Reduced Pruning**: Only check relevant combinations

**Lookup Example**:
```
Packet: Source 1, Dest 2
Go to Grid[1,2] → Rules: [R2, R3]
Check R2: If matches, use action for R2
```

---

## Scheduling and Queuing in Routers

### "Take the Ticket" Algorithm

**Purpose**: Arbitrate access to shared resource when multiple requests contend.

**Algorithm**:
1. **Issue Ticket**: When packet wants to use output, get ticket number
2. **Queue**: Packets line up in ticket order
3. **Call**: When output available, call lowest ticket number
4. **Process**: Packet with lowest ticket uses output
5. **Repeat**: Next packet gets output

**Example**:
```
Tickets: 1, 2, 3, 4, 5 issued to packets A, B, C, D, E

Processing order: A (ticket 1), B (ticket 2), C (ticket 3), ...
In order they requested

Ensures FIFO fairness
```

**Used For**: Output port arbitration, maintaining fairness

### Head-of-Line (HOL) Problem

**Definition**: Packet at head of input queue blocks all packets behind it, even if they could use different outputs.

**Scenario**:
```
Input 1 Queue:
  [Packet A - to Output 2 (BUSY)]
  [Packet B - to Output 1 (FREE)]
  [Packet C - to Output 1 (FREE)]

Problem:
  - Output 1 is available but Packet B can't send
  - Must wait for Packet A's output to become available
  - Throughput reduced to ~59% of ideal
```

**Analysis**:
- In worst case, only 1 output available while N inputs exist
- Each input sees only head-of-queue packet
- If head packet wants busy output, others blocked
- 59% utilization (for random traffic): one output always busy, others blocked

### HOL Avoidance - Knockout Scheme

**Solution**: Instead of FIFO at input, use multiple parallel copies of switching fabric.

**Architecture**:
```
Traditional (HOL problem):
All inputs → Input Queue (FIFO) → Single Switching Fabric → Outputs

Knockout Scheme:
All inputs → Parallel switches (N copies) → Arbitrate at output
```

**Process**:
1. Multiple replicas of switching fabric running in parallel
2. Each fabric tries to route incoming packet
3. If multiple fabrics want same output, choose one (arbitrate)
4. Only chosen one delivers packet
5. Reduces HOL by providing alternative paths

**Benefit**: Reduces HOL blocking by allowing some packets to bypass blocked ones

**Trade-off**: Requires N copies of fabric (expensive)

### HOL Avoidance - Parallel Iterative Matching

**Solution**: Iteratively match inputs to outputs in multiple rounds.

**Algorithm**:
```
for each iteration (say 4 iterations):
  for each unmatched input:
    if has packet AND has free output:
      Match input-output for this iteration
      Mark as matched

Result: Multiple inputs matched to different outputs
```

**Example (2 iterations)**:
```
Iteration 1:
  Input 1 has packet to Output 1 (free) → Match
  Input 2 has packet to Output 2 (free) → Match
  Input 3 blocked at Output 1 (now matched)

Iteration 2:
  Input 1 already matched, skip
  Input 2 already matched, skip
  Input 3 has packet to Output 3 (free) → Match

Result: 3 packets sent simultaneously (3 inputs to 3 outputs)
vs. FIFO which would send 1 at a time
```

**Benefit**: 
- Reduces HOL blocking
- Multiple packets transfer per cycle
- Through rate approaches theoretical maximum

**Complexity**: Requires matching algorithm (bipartite graph matching)

### FIFO with Tail Drop

**Definition**: Simple queue management: FIFO order, drop packets when queue full.

**Algorithm**:
1. Packets arrive and join queue (enqueue)
2. Packets leave in FIFO order (dequeue)
3. If queue reaches max capacity: drop arriving packet
4. Tail (back of queue) drops → "Tail Drop"

**Example**:
```
Queue: [Pkt1][Pkt2][Pkt3][Pkt4] (capacity=4)
New packet arrives: Queue full → DROP (tail drop)

Then: [Pkt2][Pkt3][Pkt4] (Pkt1 dequeued)
New packet: Space available → ENQUEUE
Queue: [Pkt2][Pkt3][Pkt4][NewPkt]
```

**Characteristics**:
- Simple to implement
- FIFO fairness
- All packets treated equally
- No intelligence in drop decisions

**Problem - Global Synchronization**:
- Multiple TCP flows experience drops simultaneously
- All sources reduce rate together
- Network oscillates between underutilization and overutilization
- "Synchronized" TCP senders → synchronized slowdown → inefficient

### Reasons for More Complex Scheduling Than FIFO

**Reason 1: Fairness Across Flows**
- FIFO doesn't guarantee fairness
- One large flow starves small flows
- Need weighted fairness

**Reason 2: Quality of Service (QoS)**
- Different packets need different treatment
- Real-time (VoIP) needs low delay
- Best effort (email) can tolerate delay
- FIFO doesn't distinguish

**Reason 3: Prevent Congestion Collapse**
- Need to drop packets intelligently (not wait for tail drop)
- Early drop signals congestion (RED - Random Early Drop)
- FIFO reactive (reacts only when full)

**Reason 4: TCP Fairness**
- Need to prevent synchronized drops
- Need to balance slow/fast flows

**Reason 5: Traffic Engineering**
- Route traffic based on policies
- Different traffic classes get different bandwidth
- FIFO single queue doesn't support

### Bit-by-Bit Round Robin Scheduling

**Algorithm**: Give each flow 1 bit of transmission time in round-robin order.

**Process**:
1. Enumerate all flows with pending packets
2. Transmit 1 bit from Flow 1
3. Transmit 1 bit from Flow 2
4. ...
5. Transmit 1 bit from Flow N
6. Loop back to Flow 1

**Example**:
```
Flows: A (1000 bits), B (500 bits), C (100 bits)

Transmission order (simplified - do 1 bit at a time):
A, B, C, A, B, C, A, B, C, A, B, C, ...

Each flow gets equal service over time
After 1500 bits, all empty
```

**Fairness Property**:
- Each flow gets equal bandwidth (1/N of link)
- Fair allocation

**Problem with Bit-by-Bit**:
- Switching overhead between flows per bit!
- CPU overhead extreme (context switch per bit)
- Not practical for real implementation

### Problem with Bit-by-Bit Round Robin

**Problem 1: Per-Bit Context Switching**
- Switching every 1 bit has huge overhead
- Cost of switch > benefit of 1 bit transmission
- Not practical

**Problem 2: Packet Boundaries Ignored**
- Can't transmit partial packets (packets are atomic units)
- Must transmit whole packet

**Better Approach**: Weighted Fair Queuing (WFQ) or Deficit Round Robin (DRR)
- Process whole packets
- Maintain fairness across flows
- No per-bit switching

### Deficit Round Robin (DRR)

**Purpose**: Approximate bit-by-bit fairness but work with packets (not bits).

**Algorithm**:

1. **Initialize**: Each flow gets credit = quantum (e.g., 1500 bytes)
2. **Round**:
   - For each flow with packets:
     - If packet size ≤ credit:
       - Send packet
       - Reduce credit by packet size
     - Else:
       - Skip this round (can't send full packet)
   - Carry forward leftover credit to next round

3. **Repeat**: Continue rounds until all queues empty

**Example**:
```
Quantum = 1000 bytes
Flow A: Packets 500, 1500, 400 bytes
Flow B: Packets 1200, 600 bytes
Flow C: Packets 800 bytes

Round 1:
  Flow A: Credit=1000, Packet=500 → Send, Credit=500
  Flow B: Credit=1000, Packet=1200 > 1000 → Skip
  Flow C: Credit=1000, Packet=800 → Send, Credit=200

Round 2:
  Flow A: Credit=500, Packet=1500 > 500 → Skip
  Flow B: Credit=1000, Packet=1200 > 1000 → Skip
  Flow C: Credit=200, (no packets) → Done

Round 3:
  Flow A: Credit=500+1000=1500, Packet=1500 → Send, Credit=0
  Flow B: Credit=1000, Packet=1200 > 1000 → Skip
  
Round 4:
  Flow B: Credit=1000, Packet=1200 > 1000 → Skip
  
Round 5:
  Flow B: Credit=1000+1000=2000, Packet=1200 → Send, Credit=800
  ...
```

**Fairness**: Approximates bit-by-bit fairness with per-packet overhead

### Token Bucket Shaping

**Purpose**: Control traffic rate by shaping (delaying packets).

**Architecture**:
```
Tokens added: Rate R (tokens/second)
Bucket capacity: B (tokens max)

Packet arrives:
  - If tokens ≥ packet_size:
    - Deduct tokens, send packet immediately
  - Else:
    - Queue packet, wait for tokens
```

**Example**:
```
Rate = 100 tokens/sec (Capacity = 1000 tokens)

Initial tokens = 1000

Packet 1 (500 bytes) arrives → Deduct 500 → tokens=500, Send
Packet 2 (400 bytes) arrives → Deduct 400 → tokens=100, Send
Packet 3 (200 bytes) arrives → Need 200, have 100 → Queue, Wait

While waiting:
  Tokens accumulate at 100/sec
  After 1 sec: tokens = 200 → Send Packet 3

Effective rate: Limited to R (100 tokens/sec) long-term
Burst allowance: B (1000 tokens) short-term
```

**Characteristics**:
- **Smoothing**: Allows traffic bursts up to B
- **Rate Limiting**: Long-term rate capped at R
- **Advantages**: Predictable behavior, controls both rate and burst

### Policing vs. Shaping

**Policing**:
- **Action**: Drop or re-mark packets exceeding rate
- **Effect**: Packets are discarded/marked, not delayed
- **Result**: Bursty traffic becomes less bursty but with losses

**Shaping**:
- **Action**: Queue packets exceeding rate (delay them)
- **Effect**: Packets delayed, not dropped (unless buffer full)
- **Result**: Bursty traffic smoothed, output rate-limited

**Comparison**:

| Aspect | Policing | Shaping |
|--------|----------|---------|
| **Excess packets** | Dropped/marked | Queued/delayed |
| **Packet loss** | Yes | Only if buffer full |
| **Delay** | No | Yes |
| **Throughput** | Lower | Same but spread over time |
| **Use case** | At ingress, enforce SLA | At egress, smooth output |

**Analogy**:
- **Policing**: Bouncer at door (drop people exceeding rate)
- **Shaping**: Queue management (space people out over time)

### Leaky Bucket for Policing and Shaping

**Leaky Bucket Concept**:
```
Bucket
┌────────────┐
│  Packets   │  Water level = queue size
│   Queue    │
└────────────┘
        │
    Leak (drain) at rate R
        ↓
```

**As Policer** (drop excess):
```
Bucket full at capacity C
Leak rate = R
Packets arrive > R: Dropped (overflow)
Packets arrive ≤ R: Queued/forwarded

Result: Traffic rate limited to R
```

**As Shaper** (delay excess):
```
Bucket capacity = B
Leak rate = R
Packets arrive:
  - If buffer space ≤ C: Queue
  - If buffer space = C (full): Might drop or delay longer

Result: Output limited to R, absorbs bursts up to B
```

**Example - Policer**:
```
Rate = 100 packets/sec, Capacity = 50 packets

Burst of 100 packets arrives in 1ms:
  - First 50 queued (fill bucket)
  - Next 50 dropped (bucket full, policer)
  - Output: ~100 packets/sec continuous
  - Lost: 50 packets
```

**Example - Shaper**:
```
Rate = 100 packets/sec, Bucket = 50 packets

Burst of 100 packets:
  - First 50 queued immediately
  - Next 50 queued but held (not enough leak capacity)
  - Output: Spreads 100 packets over 1 second at 100 packets/sec
  - Lost: 0 packets (if buffer large enough)
```

**Command Example** (Linux tc tool):
```bash
# Leaky bucket policing (drop excess)
tc qdisc add dev eth0 root policer rate 1mbit burst 10k drop

# Leaky bucket shaping (queue excess)
tc qdisc add dev eth0 root tbf rate 1mbit burst 10k latency 400ms
```

---

## Lesson 7: SDN (Part 1)

### What Spurred the Development of Software Defined Networking (SDN)?

**Problem 1: Closed, Proprietary Networking Equipment**
- Routers/switches from vendors (Cisco, Juniper, etc.)
- Proprietary hardware/software integration
- No flexibility to implement custom logic
- Vendor lock-in

**Problem 2: Complex Network Management**
- Manual configuration of each device
- Routing protocols run independently on each device
- Difficult to implement end-to-end policies
- No global view of network

**Problem 3: Slow Innovation**
- Features require vendor support
- Experiments require proprietary equipment
- Research innovations take years to deploy
- Gap between academic ideas and production

**Problem 4: Difficulty in Load Balancing and Traffic Engineering**
- Limited ability to influence traffic paths
- Traffic engineering requires manual configuration
- No dynamic adaptation to network conditions

**Problem 5: Expensive and Complex Data Centers**
- Multiple layers of switches/routers
- Complex spanning tree topologies
- Poor bandwidth utilization (blocking tree)
- Difficult to optimize traffic flows

**SDN Solution**:
- Separate control plane (intelligence) from data plane (forwarding)
- Centralized control plane can see entire network
- Programmable data plane via standard protocols (OpenFlow)
- Open ecosystem vs. proprietary

### Three Phases in the History of SDN

**Phase 1: Active Networking (1990s)**
- Research phase exploring programmable networks
- Protocols carrying code in packets
- Router interpret and execute code
- Academic concept, limited real deployment

**Phase 2: Intermediary Research (2000s)**
- Various proposals for network control decoupling
- 4D Project (Stanford/UCB): Centralized decision engine
- Ethane (Stanford): Centralized security policy
- Still mostly research

**Phase 3: OpenFlow and Commoditization (2008-present)**
- OpenFlow protocol (Stanford) for switch-controller communication
- Simple, implementable, vendor-agnostic
- Explosion in SDN adoption and commercial products
- Multiple SDN controllers (ONOS, Opendaylight)
- Industry standard

### Summary of Each Phase

**Phase 1: Active Networking**
```
Concept: Programs embedded in packets, executed by routers
Impact: Academic interest, proved concept possible
Result: Limited deployment, impractical
```

**Phase 2: Intermediary Research**
```
Concept: Separate control plane from data plane
Research: Ethane (campus networks), 4D (carrier networks)
Impact: Identified benefits, proposed architectures
Result: Proof of concept, but no standard
```

**Phase 3: OpenFlow/Current**
```
Concept: Simple protocol (OpenFlow) for controller-switch communication
Implementation: Commodity switches support OpenFlow
Impact: Widespread adoption, commercial products
Result: SDN widely deployed (data centers, ISP networks)
```

### Control Plane vs. Data Plane Functions

**Control Plane (Intelligence)**:
- **Purpose**: Make routing decisions, policies
- **Functions**:
  - Run routing protocols (BGP, OSPF)
  - Compute forwarding tables
  - Implement security policies
  - Traffic engineering decisions
  - Network-wide decisions
- **Timescale**: Milliseconds to seconds
- **Centralized**: Can be centralized or distributed

**Data Plane (Forwarding)**:
- **Purpose**: Forward packets according to control decisions
- **Functions**:
  - Lookup destination in forwarding table
  - Forward to output port
  - Apply access control
  - Perform queuing/scheduling
- **Timescale**: Nanoseconds (per packet)
- **Location**: Every router/switch

**Analogy**:
- **Control Plane** = Brain (decision making)
- **Data Plane** = Muscles (execution)

### Why Separate Control from Data Plane?

**Benefit 1: Simplification of Data Plane**
- Data plane focuses on high-speed forwarding
- No need to run complex routing protocols
- Simpler, faster hardware

**Benefit 2: Centralized Control**
- Single controller sees entire network
- Global optimization possible
- Easier policy implementation
- Consistent decisions

**Benefit 3: Flexibility**
- Control logic can be updated without hardware changes
- New policies deployed quickly
- Easier experimentation

**Benefit 4: Programmability**
- Network behavior can be programmed
- No vendor lock-in to specific protocols
- Custom logic implementable

**Benefit 5: Faster Innovation**
- New routing algorithms deployable immediately
- No need to wait for vendor support
- Easier to experiment with new technologies

**Benefit 6: Better Resource Utilization**
- Central controller can optimize paths globally
- Better bandwidth utilization
- Reduced congestion

### Why SDN Led to Opportunities in Various Areas

**Data Centers**:
- **Before**: Multiple layers of switches, spanning tree limited bandwidth
- **After**: OpenFlow allows dynamic topology, full bisection bandwidth
- **Benefit**: Better utilization, lower latency, easier VM migration

**Routing**:
- **Before**: BGP runs on every router, distributed decisions
- **After**: Central controller makes routing decisions
- **Benefit**: Better traffic engineering, faster adaptation to failures

**Enterprise Networks**:
- **Before**: Manual configuration, separate management
- **After**: Centralized controller manages all network
- **Benefit**: Easier security policies, simpler management

**Research Networks**:
- **Before**: Limited by proprietary equipment
- **After**: OpenFlow on commodity hardware
- **Benefit**: Easy experimentation, innovation platform

### Relationship Between Forwarding and Routing

**Forwarding**:
- Data plane function
- Local per-hop decision
- "Given packet, which output?"
- Fast (nanosecond timescale)

**Routing**:
- Control plane function
- Global network decision
- "Compute best paths to all destinations"
- Slow (millisecond+ timescale)

**Relationship**:
```
Routing computes routing table
          ↓
Forwarding uses routing table
          ↓
Routing table has {Destination → Output Port} mappings
```

**SDN Changes This**:
- Central controller does routing (globally optimal)
- Programs flow tables into switches
- Switches forward using flow tables

### Traditional vs. SDN Coupling

**Traditional Approach**:
```
Control and Data Plane Coupled:
┌─────────────┐
│  Router A   │
├─────────────┤
│ Control:    │ ← Runs routing protocol
│ • OSPF      │   Computes own routing table
│ • BGP       │
├─────────────┤
│ Data:       │ ← Uses locally computed table
│ • Switching │   to forward packets
│ • Queuing   │
└─────────────┘

Problem: Each router decides independently
        No global optimization
```

**SDN Approach**:
```
Control and Data Plane Separated:
┌────────────────┐
│ Controller     │ ← Centralized control
│ (Logically     │   Sees entire network
│  Centralized)  │   Computes all routes
└────────────────┘
         │ (Control messages)
         ↓
┌─────────────┐  ┌─────────────┐  ┌─────────────┐
│ Switch A    │  │ Switch B    │  │ Switch C    │
├─────────────┤  ├─────────────┤  ├─────────────┤
│ Flow Table  │  │ Flow Table  │  │ Flow Table  │
├─────────────┤  ├─────────────┤  ├─────────────┤
│ Switching   │  │ Switching   │  │ Switching   │
└─────────────┘  └─────────────┘  └─────────────┘

Benefit: Global optimization
        Controller programs optimal flows
```

**Key Difference**:
- **Traditional**: Control and data tightly bound (each device independent)
- **SDN**: Control centralized, data distributed (switches only forward)

### Main Components of SDN Network and Their Responsibilities

**1. SDN Controller (Control Plane)**
- **Responsibility**:
  - Run network applications
  - Implement routing logic
  - Make forwarding decisions
  - Manage flow tables
  - Respond to network events
- **Example**: ONOS, Opendaylight, Floodlight

**2. OpenFlow Protocol**
- **Responsibility**:
  - Communicate between controller and switches
  - Controller → Switches: "Install flow rules"
  - Switches → Controller: "Packet arrived, don't know how to handle"
  - Bidirectional communication

**3. OpenFlow Switches (Data Plane)**
- **Responsibility**:
  - Maintain flow tables
  - Forward packets according to flow rules
  - Report statistics to controller
  - Handle table misses (send to controller)
- **Example**: Commodity switches with OpenFlow support

**4. Network Applications**
- **Responsibility**:
  - Run on top of controller
  - Implement routing, load balancing, security, etc.
  - Request forwarding changes from controller
- **Examples**: Routing app, Firewall app, Load Balancer app

**Architecture Diagram**:
```
┌───────────────────────────────────────┐
│  SDN Applications Layer               │
│ (Routing, Firewall, Load Balancer)   │
└───────────────┬───────────────────────┘
                │ (Northbound API)
┌───────────────┴───────────────────────┐
│  SDN Controller                       │
│  (Centralized decision engine)        │
└───────────────┬───────────────────────┘
                │ (OpenFlow Protocol)
    ┌───────────┼───────────┬───────────┐
    ↓           ↓           ↓           ↓
┌────────┐ ┌────────┐ ┌────────┐ ┌────────┐
│Switch 1│ │Switch 2│ │Switch 3│ │Switch 4│
└────────┘ └────────┘ └────────┘ └────────┘
```

### Four Defining Features of SDN Architecture

**Feature 1: Flow-Based Forwarding**
- Forwarding decisions based on flow rules (not just destination IP)
- Rules can match on: source/destination IP, ports, protocols, etc.
- Actions: forward, drop, modify headers, encapsulate

**Feature 2: Separation of Control and Data Plane**
- Control logic centralized in controller
- Data plane focuses on fast forwarding
- Clean interface between planes (OpenFlow)

**Feature 3: Programmable Network Behavior**
- Network behavior specified by applications
- Controller programs switches
- Behavior can be changed without hardware changes
- Enables innovation

**Feature 4: Centralized Network View**
- Controller has global view of network
- Knows topology, state of all switches
- Can make globally optimal decisions
- Easier to implement end-to-end policies

### Three Layers of SDN Controllers

**Layer 1: Infrastructure Layer (Data Plane)**
- **Components**: OpenFlow switches
- **Responsibility**: Forwarding packets
- **Characteristics**: Distributed, fast, simple

**Layer 2: Control Layer (SDN Controller)**
- **Components**: Controller platform (ONOS, ODL)
- **Responsibility**:
  - Maintain network state (topology, link stats)
  - Provide APIs for applications
  - Communicate with switches
  - Handle failures/topology changes
- **Characteristics**: Logically centralized, might be physically distributed for redundancy

**Layer 3: Application Layer**
- **Components**: Network applications
- **Responsibility**:
  - Implement network services (routing, firewall, etc.)
  - Make policy decisions
  - Request forwarding actions from controller
- **Examples**:
  - Routing application (compute best paths)
  - Firewall application (implement security)
  - Load balancer application (distribute traffic)
  - Traffic engineering (optimize paths)

**Data Flow**:
```
Applications → Request flow rules
        ↓
    Controller → Installs rules
        ↓
    Switches → Forwards according to rules
        ↓
Packets → Follow programmed flows
```

---

## Additional Resources and Practical Commands

### TCP/IP Networking Commands

```bash
# View current routes
route -n
ip route

# View neighbor information (ARP)
arp -n
ip neigh

# Show socket statistics
ss -tulpn
netstat -tulpn

# Monitor network traffic
tcpdump -i eth0 -n

# Trace route to destination
traceroute 8.8.8.8
mtr -r -c 100 8.8.8.8

# Test connectivity
ping -c 4 8.8.8.8

# DNS lookup
nslookup google.com
dig google.com

# Show interface statistics
ifconfig -a
ip link show

# BGP neighbor information (Cisco)
show bgp neighbors
show bgp summary

# OSPF information (Cisco)
show ospf interface
show ospf database

# Routing table (Cisco)
show ip route
show ip route ospf
show ip route bgp
```

### Packet Analysis

```bash
# Capture TCP handshake
tcpdump -i eth0 'tcp[tcpflags] & (tcp-syn|tcp-ack) != 0' -n

# Capture specific port
tcpdump -i eth0 port 80 -n

# Show detailed packet content
tcpdump -i eth0 -A -X

# Save to file
tcpdump -i eth0 -w capture.pcap

# Read capture file
tcpdump -r capture.pcap
```

---

**End of Study Guide**

## Lesson 8: SDN (Part 2)

### Three Perspectives of the SDN Landscape

1.  **Plane Perspective**: Focuses on the separation of the control plane (decision-making) and the data plane (packet forwarding). This is the fundamental architectural shift in SDN.
2.  **SDN Layer Perspective**: Views the architecture as a stack of layers: Infrastructure (switches), Control (controller), and Application (network apps).
3.  **System Design Perspective**: Focuses on the implementation details, such as centralized vs. distributed controllers, and the trade-offs involved (consistency, availability, partition tolerance).

### Responsibility of Each Layer in the SDN Layer Perspective

1.  **Infrastructure Layer (Data Plane)**:
    *   Consists of network devices (switches, routers).
    *   Responsible for forwarding packets based on rules installed by the controller.
    *   Collects statistics (packet counts, byte counts).
    *   Handles "table misses" by sending packets to the controller.

2.  **Control Layer (Control Plane)**:
    *   Consists of the SDN controller (or cluster of controllers).
    *   Maintains the global view of the network (topology, link state).
    *   Translates high-level requests from applications into low-level rules for switches.
    *   Manages the network state and provides a platform for applications.

3.  **Application Layer (Management Plane)**:
    *   Consists of network applications (routing, firewall, load balancing).
    *   Defines the network policy and behavior.
    *   Communicates with the controller via Northbound APIs to request network services.

### Pipeline of Flow Tables in OpenFlow

**Definition**: OpenFlow switches use a pipeline of flow tables to process packets.

**Process**:
1.  **Ingress**: Packet enters the switch and starts at Table 0.
2.  **Matching**: Packet header fields are matched against rules in Table 0.
3.  **Actions**:
    *   **Forward**: Send to output port.
    *   **Drop**: Discard packet.
    *   **Modify**: Change header fields (e.g., rewrite MAC/IP, add VLAN tag).
    *   **Goto-Table**: Send packet to a subsequent table (e.g., Table 1, Table 5) for further processing.
4.  **Egress**: After passing through the pipeline, the packet is forwarded or dropped.

**Benefit**: Allows complex logic (e.g., first table for MAC filtering, second for IP routing, third for ACLs) without requiring a Cartesian product of all rules in a single table.

### Main Purpose of Southbound Interfaces

**Purpose**: To enable communication between the SDN controller (Control Plane) and the network devices (Data Plane).

**Functions**:
*   Allows controller to discover switches and topology.
*   Allows controller to install flow rules into switches.
*   Allows switches to report statistics and events to the controller.
*   **Standard Protocol**: OpenFlow is the most common southbound interface, but others exist (Netconf, OVSDB).

### Three Information Sources Provided by OpenFlow Protocol

1.  **Event-Based Messages**: Switches send messages to the controller when link status changes (port up/down) or when a packet arrives that doesn't match any rule (Packet-In).
2.  **Flow Statistics**: Controller can query switches for counters (packet count, byte count) for specific flows or ports.
3.  **Packet-In Messages**: When a switch doesn't know how to handle a packet, it sends the packet (or header) to the controller for a decision.

### Core Functions of an SDN Controller

1.  **Topology Discovery**: Learning the network graph (switches, links).
2.  **Device Management**: Managing switch connections and configurations.
3.  **Path Computation**: Calculating routes (shortest path, etc.).
4.  **Flow Management**: Installing and managing flow rules on switches.
5.  **Northbound API**: Providing an interface for applications to interact with the network.
6.  **Southbound API**: Communicating with network devices (OpenFlow).

### Centralized vs. Distributed Architectures of SDN Controllers

**Centralized Controller**:
*   **Single Entity**: One controller manages the entire network.
*   **Pros**: Simple design, strong consistency, easy to implement global policies.
*   **Cons**: Single point of failure, scalability bottlenecks (can be overwhelmed by events).

**Distributed Controller**:
*   **Cluster**: Multiple controller instances work together.
*   **Pros**: Scalability (load balancing), high availability (fault tolerance), lower latency (controllers closer to switches).
*   **Cons**: Complexity (synchronization), potential consistency issues (CAP theorem trade-offs).

### When Distributed Controller is Preferred

*   **Large Scale Networks**: When the number of switches/flows exceeds the capacity of a single server.
*   **High Availability Requirements**: When network downtime is unacceptable (need redundancy).
*   **Geographically Dispersed Networks**: To reduce latency between switches and controllers (place controllers in different regions).

### ONOS (Open Networking Operating System) Components

1.  **Distributed Core**: Provides the state management and synchronization between controller instances. Ensures consistency and fault tolerance.
2.  **Northbound API**: Abstractions for applications (Intent Framework). Allows apps to request "connectivity" without knowing low-level details.
3.  **Southbound API**: Abstractions for devices. Supports multiple protocols (OpenFlow, Netconf) to talk to different hardware.
4.  **Device Subsystem**: Manages device inventory and status.
5.  **Topology Subsystem**: Manages the network graph.
6.  **Packet Subsystem**: Handles packet-in/out processing.

### How ONOS Achieves Fault Tolerance

*   **Clustering**: Multiple ONOS instances run in a cluster.
*   **Mastership**: Each switch connects to multiple ONOS instances, but only one is "Master" for that switch. Others are "Standby".
*   **Failure Handling**: If the Master fails, a Standby instance is immediately elected as the new Master.
*   **State Replication**: Network state (topology, flows) is replicated across instances using a distributed data store (based on Raft consensus algorithm) to ensure consistency.

### What is P4?

**Definition**: P4 (Programming Protocol-independent Packet Processors) is a high-level language for programming the data plane of network devices.

**Difference from OpenFlow**: OpenFlow assumes a fixed set of header fields (IP, TCP, Ethernet). P4 allows defining *custom* headers and *custom* parsing logic.

### Primary Goals of P4

1.  **Protocol Independence**: Devices should not be tied to specific protocols. Users can define new protocols.
2.  **Target Independence**: The same P4 program should compile and run on different hardware (ASICs, FPGAs, software switches).
3.  **Reconfigurability**: The parsing and processing logic can be changed in the field (programmable hardware).

### Two Main Operations of P4 Forwarding Model

1.  **Configure**: The P4 program defines the parser (how to read headers) and the pipeline (match-action tables). This is done at compile/boot time.
2.  **Populate**: The control plane adds entries to the match-action tables defined by the P4 program. This happens at runtime.

### Applications of SDN

1.  **Traffic Engineering**: Optimizing traffic flow to maximize bandwidth utilization.
    *   *Example*: Google's B4 (connecting data centers) uses SDN to drive links to near 100% utilization.
2.  **Network Virtualization**: Creating logical networks on top of physical infrastructure.
    *   *Example*: Cloud providers (AWS, Azure) use SDN to isolate tenant networks (VPCs).
3.  **Security/Access Control**: Implementing dynamic firewalls and policies.
    *   *Example*: Ethane (enterprise network) allows/denies traffic based on user identity, not just IP.
4.  **Monitoring and Measurement**: Dynamic tapping of traffic.
    *   *Example*: Bismark (broadband monitoring) uses SDN to measure performance.
5.  **Service Chaining**: Steering traffic through a sequence of middleboxes (Firewall → IDS → Load Balancer).

### BGP Limitations Addressed by SDN

1.  **Routing Scalability**: BGP tables are huge. SDN can optimize rule placement.
2.  **Convergence Time**: BGP is slow to converge. SDN controller can pre-calculate backup paths and switch instantly.
3.  **Traffic Engineering**: BGP offers poor control over path selection (only AS-path length). SDN allows optimizing based on latency, bandwidth, cost, etc.
4.  **Policy Enforcement**: BGP policies are complex and error-prone. SDN allows declarative policy definitions.

### Purpose of SDX (Software Defined IXP)

**Goal**: To bring the benefits of SDN to Internet Exchange Points (IXPs).

**Problem**: Traditional IXPs just switch packets based on BGP. They can't handle complex policies like "application-specific peering" or "DDoS mitigation".

**Solution**: SDX allows participants to run SDN applications at the IXP to implement custom peering policies.

### SDX Architecture

1.  **SDX Controller**: Manages the IXP fabric.
2.  **SDX Switch**: Programmable switch (OpenFlow) at the IXP core.
3.  **Participant Controllers**: Each ISP has its own controller that sends policy requests to the SDX controller.
4.  **Policy Compiler**: Combines policies from different participants into a single set of consistent flow rules for the switch.

### Applications of SDX in Wide-Area Traffic Delivery

1.  **Application-Specific Peering**: "Route Netflix traffic via ISP A, but YouTube traffic via ISP B."
2.  **Inbound Traffic Engineering**: Controlling how traffic enters your network.
3.  **Wide-Area Load Balancing**: Distributing traffic across multiple peering links.
4.  **DDoS Mitigation**: Blocking attack traffic at the IXP before it reaches the victim's link.

---

## Lesson 9: Internet Security

### Properties of Secure Communication

1.  **Confidentiality**: Only the intended receiver can read the message. (Encryption)
2.  **Integrity**: The message was not altered in transit. (Checksums/Hashing)
3.  **Authentication**: Identifying the communicating parties. (Certificates/Signatures)
4.  **Availability**: The system is accessible when needed. (Protection against DDoS)

### Round Robin DNS (RRDNS)

**Mechanism**: A DNS server responds to a query with a list of IP addresses, cycling the order for each request.

**Example**:
*   Query 1: Returns [IP1, IP2, IP3] → Client uses IP1
*   Query 2: Returns [IP2, IP3, IP1] → Client uses IP2
*   Query 3: Returns [IP3, IP1, IP2] → Client uses IP3

**Purpose**: Simple load balancing. Distributes traffic across multiple servers.

### DNS-Based Content Delivery

**Mechanism**: CDNs use DNS to direct users to the nearest/best server.
*   User queries `www.example.com`.
*   DNS server (authoritative for CDN) checks user's IP address.
*   Calculates the closest edge server.
*   Returns the IP of that specific edge server.

### Fast-Flux Service Networks

**Definition**: A technique used by botnets/malware to hide phishing or malware sites.

**How it works**:
*   The domain name (e.g., `evil.com`) is associated with hundreds of IP addresses (compromised bots).
*   The DNS records have a very short TTL (Time To Live).
*   The set of IP addresses changes constantly (flux).
*   **Double Flux**: Both the A-records (web servers) and the NS-records (DNS servers) change constantly.

**Goal**: Makes it hard to take down the site because the IP addresses keep changing.

### FIRE (FInding Rogue nEtworks) Data Sources

1.  **Botnet Command & Control Providers**: Lists of known C&C servers.
2.  **Drive-by-Download URLs**: Lists of malicious URLs.
3.  **Phishing URLs**: Lists of phishing sites.
4.  **Spam**: Data from spam traps.

**Goal**: Correlate these sources to identify Autonomous Systems (ASes) that harbor a high concentration of malicious activity (Rogue Networks).

### ASwatch: Two Phases

**Concept**: Detect malicious ASes by monitoring their BGP behavior (control plane).

1.  **Training Phase**:
    *   Learn the "normal" BGP behavior of legitimate ASes vs. known malicious ASes.
    *   Features: Number of prefixes advertised, frequency of updates, path changes.
    *   Build a statistical model.

2.  **Operational Phase**:
    *   Monitor real-time BGP updates.
    *   Compute scores for each AS based on the model.
    *   Flag ASes with high "malicious" scores.

### Three Classes of Features for Security Breach Detection

1.  **Volume-Based Features**: Flow byte count, packet count, duration. (Did a host download a huge file?)
2.  **Temporal Features**: Inter-arrival time of packets, periodicity. (Is the host beaconing to a C&C server every 5 minutes?)
3.  **Spatial Features**: Distribution of destination IPs and ports. (Is the host scanning many internal IPs?)

### BGP Hijacking Classification

**1. By Affected Prefix**:
*   **Exact Prefix Hijacking**: Attacker announces the *exact same* prefix as the victim. (e.g., Victim: 10.0.0.0/24, Attacker: 10.0.0.0/24). Routes depend on path length.
*   **Sub-Prefix Hijacking**: Attacker announces a *more specific* prefix. (e.g., Victim: 10.0.0.0/24, Attacker: 10.0.0.128/25). **More specific always wins** in longest prefix match. Very effective.
*   **Squatting**: Attacker announces a prefix that is not currently owned/announced by anyone.

**2. By AS-Path Announcement**:
*   **Type-0**: Attacker announces prefix as if it owns it (Origin AS = Attacker).
*   **Type-N**: Attacker forges the AS path to look like it's a legitimate path, preserving the victim's origin but inserting itself.

**3. By Data Plane Traffic Manipulation**:
*   **Blackholing**: Attacker drops all traffic (Denial of Service).
*   **Impersonation**: Attacker responds to traffic (Phishing/Fake Site).
*   **Man-in-the-Middle (MitM)**: Attacker intercepts, inspects/modifies, and forwards traffic to the real victim. Hardest to detect.

### Causes/Motivations Behind BGP Attacks

1.  **Human Error**: Configuration mistakes (Fat finger). Most common.
2.  **Targeted Attack**: Stealing data, interception.
3.  **High Profile Address Hijacking**: Stealing unused IPs for spamming (to bypass reputation filters).
4.  **Side-Channel Attack**: Censorship or surveillance.

### Scenario of Prefix Hijacking

**Scenario**:
*   **Victim (AS 1)** owns `100.1.0.0/16`.
*   **Attacker (AS 666)** announces `100.1.0.0/16` (Exact) or `100.1.0.0/24` (Sub-prefix).
*   **Result**: Neighbors of AS 666 hear the announcement.
    *   If sub-prefix: All traffic for that /24 goes to AS 666 (Global hijack).
    *   If exact prefix: Traffic closest to AS 666 goes to AS 666; traffic closest to AS 1 goes to AS 1 (Partial hijack).

### Scenario of Hijacking a Path

**Scenario**:
*   Attacker wants to intercept traffic but not break connectivity.
*   Attacker announces a path that is preferred (shorter) than the real path.
*   Traffic flows: Source → Attacker → Victim.
*   Victim still receives traffic, so they might not notice.
*   Attacker can record/modify data.

### ARTEMIS: Key Ideas

**Goal**: Real-time detection and mitigation of BGP hijacking.

**Ideas**:
*   **Local Monitoring**: Monitor BGP updates received by the AS's own routers.
*   **Global Monitoring**: Use public BGP monitoring services (RIPE RIS, RouteViews).
*   **Automated Mitigation**: Automatically announce specific updates to regain control.

### Two Automated Techniques Used by ARTEMIS

1.  **Prefix De-aggregation**:
    *   If attacker hijacks `10.0.0.0/24`, the victim (who owns /24) immediately announces `10.0.0.0/25` and `10.0.0.128/25`.
    *   Rationale: More specific prefixes propagate globally and override the attacker's /24.

2.  **MOAS (Multiple Origin AS) Announcement**:
    *   Announce the prefix again but with higher specificity or different attributes to pull traffic back.
    *   (Note: The primary and most effective technique is De-aggregation).

### Two Findings from ARTEMIS

1.  **Outsourced mitigation is slow**: Relying on upstream ISPs or human intervention takes too long. Automated local response is necessary.
2.  **De-aggregation is highly effective**: It recovers >99% of traffic almost immediately.

### Structure of a DDoS Attack

1.  **Attacker**: The mastermind.
2.  **Master/Handler**: Compromised machines controlling the bots.
3.  **Bots/Zombies**: Large network of compromised devices (IoT, PCs).
4.  **Victim**: The target server/network.

**Flow**: Attacker → Handlers → Bots → Victim (Traffic flood).

### Spoofing and DDoS

**Spoofing**: Faking the source IP address in a packet.
**Relation to DDoS**:
*   **Hides Identity**: Victim sees traffic coming from random IPs, not the bot's real IP.
*   **Bypasses Filters**: Hard to block "random" IPs.
*   **Enables Reflection**: Essential for reflection attacks (spoof victim's IP).

### Reflection and Amplification Attack

**Mechanism**:
1.  **Reflection**: Attacker sends a request to a server (e.g., DNS, NTP) spoofing the *Victim's IP* as the source.
2.  **Amplification**: The response from the server is much larger than the request.
    *   *Example*: Small DNS query (60 bytes) → Large DNS response (3000 bytes).
3.  **Attack**: Server sends the large response to the Victim.
4.  **Result**: Victim is flooded with huge responses from legitimate servers. Attacker uses small bandwidth to generate huge volume.

### Defenses Against DDoS Attacks

1.  **Traffic Scrubbing**: Diverting traffic to specialized centers that filter out bad packets.
2.  **ACLs / Firewalls**: Blocking known bad IPs/ports.
3.  **Rate Limiting**: Limiting the rate of incoming traffic.
4.  **Blackholing**: Dropping traffic to the victim entirely (last resort).
5.  **BCP 38 (Ingress Filtering)**: ISPs should drop packets with spoofed source IPs originating from their network (prevents spoofing at the source).

### Provider-Based Blackholing

**Mechanism**:
*   Victim asks its upstream ISP to drop all traffic destined to the victim's IP.
*   ISP adds a null-route for the victim's IP.
*   **Result**: Attack stops clogging the link, but the victim goes offline. "Suicide" defense.

### IXP Blackholing

**Mechanism**:
*   Victim sends a BGP Blackhole request to the IXP Route Server.
*   Route Server signals all IXP members to drop traffic to that IP.
*   **Benefit**: Drops attack traffic at the source (at the IXP members), preventing it from even entering the IXP fabric or the victim's link.

### Major Drawback of BGP Blackholing

*   **Collateral Damage**: It drops *all* traffic to the victim, including legitimate traffic. The victim becomes unreachable. It solves the link congestion but completes the DDoS goal (denial of service).

---

## Lesson 10: Internet Surveillance and Censorship

### DNS Censorship

**Definition**: Preventing users from accessing a site by interfering with the DNS resolution process. The user gets no IP or a wrong IP for `forbidden.com`.

### Properties of GFW (Great Firewall of China)

1.  **Locality**: Not a single firewall, but distributed at the edge/gateways of ISPs.
2.  **Bidirectional**: Blocks traffic entering and leaving.
3.  **Centralized Control**: Policies are managed centrally.
4.  **Multi-layer**: Uses DNS poisoning, IP blocking, and Keyword filtering.

### DNS Injection (How it works)

**Mechanism**: The censor monitors DNS queries. When it sees a query for a banned domain, it injects a fake DNS response.

**Race Condition**: The fake response (usually closer) reaches the user before the legitimate response. The user accepts the fake one and ignores the real one.

### Three Steps in DNS Injection

1.  **Capture**: Censor mirrors/taps traffic and detects DNS query for banned keyword.
2.  **Inject**: Censor constructs a fake DNS response with a random/invalid IP.
3.  **Race**: Censor sends fake response. Since censor is usually on the path or close, it wins the race against the real DNS server.

### Five DNS Censorship Techniques

1.  **Packet Dropping**: All DNS traffic to a specific server is dropped.
2.  **DNS Poisoning**: Injecting fake responses (as described above).
3.  **Content Inspection**: Inspecting full packet payloads and dropping if keywords found.
4.  **Blocking with Resets**: Sending TCP RST packets to terminate connection.
5.  **Immediate Reset**: Resetting connection immediately upon handshake.

### Technique Susceptible to Overblocking

**Packet Dropping**: If you block the IP of a DNS server (e.g., Google's 8.8.8.8) to stop access to one site, you block *all* DNS queries sent to that server, affecting legitimate sites too.

### Strengths and Weaknesses of Censorship Techniques

**Packet Dropping**:
*   *Strength*: Easy to implement.
*   *Weakness*: Overblocking (collateral damage).

**DNS Poisoning**:
*   *Strength*: No overblocking (targets specific domains). Hard for user to detect.
*   *Weakness*: Requires stateful inspection (or fast injection). Can be bypassed by using different DNS resolvers or encryption (DoH).

**Content Inspection**:
*   *Strength*: Very precise.
*   *Weakness*: Expensive (high processing power). Hard to do at line speed. Fails with encryption (HTTPS/TLS).

**Blocking with Resets**:
*   *Strength*: Frees up resources on censor's side (stateless).
*   *Weakness*: Can be noisy/detectable.

### Challenges in Understanding Global Censorship

1.  **Lack of Vantage Points**: Hard to get measurement machines inside restrictive countries.
2.  **Diversity**: Censorship methods vary by ISP, region, and time.
3.  **Risk**: Probing censorship can be risky for the person running the probe.
4.  **Transience**: Censorship rules change frequently.

### Limitations of Main Censorship Detection Systems

*   **OONI / RIPE Atlas**: Require volunteers to install probes (hardware/software). Limited coverage.
*   **UBI**: Relies on users visiting sites (bias).

### Augur

**Focus**: Identifying **TCP/IP side channel** disruptions.
**Mechanism**: Uses TCP/IP side channels (IP ID field) to measure connectivity between two remote hosts without having control over either.

### Iris: Countering Lack of Diversity

**Problem**: Most measurements come from few vantage points.
**Iris Solution**: Uses **Open DNS Resolvers** globally as vantage points.
**Steps**:
1.  Scan the Internet for open DNS resolvers.
2.  Query these resolvers for sensitive domains.
3.  Compare responses with trusted control responses.
4.  If response differs (and matches known blockpages or bogon IPs), flag as censorship.

### Global Measurement Process Using DNS Resolvers

1.  **Scanning**: Find open resolvers.
2.  **Measurement**: Query restricted domains via these resolvers.
3.  **Annotation**: Classify the returned IP addresses (Are they valid? Are they block pages?).
4.  **Analysis**: Determine if manipulation occurred.

### Iris Metrics for DNS Manipulation

1.  **Consistency**: Does the resolver return the same IP as the control resolver (trusted)?
2.  **Independent Verifiability**: Is the returned IP hosted by the organization that owns the domain? (e.g., Does `facebook.com` resolve to a Facebook IP?)
3.  **HTTP Connectivity**: Can we connect to the returned IP on port 80? (Block pages often host a web server).

**Declaration of Manipulation**: If Consistency fails AND Independent Verifiability fails.

### Identifying DNS Manipulation via Machine Learning (Iris)

*   Train a classifier on features of the DNS response:
    *   IP address structure.
    *   AS owner.
    *   HTTP headers returned.
    *   Page content (block page text).
*   Use this to automatically tag responses as "Censored" or "Valid".

### Connectivity Disruption: Routing vs. Packet Filtering

**Routing Disruption**: The censor withdraws BGP prefixes for the banned site. The routers literally have no path to the destination.
**Packet Filtering**: The path exists, but a firewall (middlebox) inspects packets and drops them based on ACLs.

### Scenario: Connectivity Disruption Detection (No Filtering)

*   **Traceroute**: Shows the path stops at a certain hop.
*   **BGP Looking Glass**: Shows the prefix is not advertised or withdrawn.

### Scenario: Inbound vs. Outbound Blocking

*   **Inbound Blocking**: User can send SYN, but site's SYN-ACK is blocked. OR Site receives SYN, sends SYN-ACK, but firewall drops SYN.
*   **Outbound Blocking**: User's SYN is dropped before leaving the country.

---

## Lesson 11: Applications (Video)

### Bit Rates: Video vs. Photos vs. Audio

*   **Video**: High bit rate (Mbps to Gbps). Compression is essential.
*   **Photos**: Medium.
*   **Audio**: Low bit rate (Kbps).

### Characteristics of Streaming Stored Video

*   **Stored**: Content is pre-recorded.
*   **Interactive**: Users can pause, rewind, fast-forward.
*   **Delay Tolerance**: Users can wait a few seconds for buffering (start-up delay).
*   **Loss Tolerance**: Some loss is okay, but buffering usually hides it.

### Characteristics of Streaming Live Audio/Video

*   **Real-time**: Content generated live.
*   **Non-interactive**: Cannot fast-forward.
*   **Delay Sensitive**: Latency must be low (seconds).
*   **Loss Tolerant**: Occasional glitch acceptable.

### Characteristics of Conversational Voice/Video over IP

*   **Real-time**: Interaction between humans.
*   **Highly Delay Sensitive**: Latency > 150ms degrades experience significantly.
*   **Loss Tolerant**: Small loss acceptable, but high loss makes speech unintelligible.
*   **Jitter Sensitive**: Variation in delay is bad.

### Analog Audio Encoding (PCM)

**Process**:
1.  **Sampling**: Measure the amplitude of the sound wave at regular intervals (e.g., 8000 times/sec).
2.  **Quantization**: Round the measure to the nearest integer value (discrete levels).
3.  **Encoding**: Convert the integer to binary.

### Three Categories of VoIP Encoding Schemes

1.  **Narrowband**: Low quality, low bandwidth (e.g., G.711, 64kbps). Like traditional phone.
2.  **Broadband/Wideband**: Better quality (e.g., G.722).
3.  **Multimode/Codecs**: Adaptive (e.g., Opus, AMR). Can change bitrate dynamically.

### Functions of Signaling Protocols (e.g., SIP)

1.  **User Location**: Finding the IP of the callee.
2.  **Session Establishment**: Setting up the call (handshake).
3.  **Session Management**: Modifying (mute, hold) and terminating the call.
4.  **Feature Invocation**: Call forwarding, voicemail.

### Three QoS VoIP Metrics

1.  **End-to-End Delay**: Total time from mouth to ear.
2.  **Jitter**: Variation in packet arrival time.
3.  **Packet Loss**: Percentage of lost packets.

### Delays Included in End-to-End Delay

1.  **Processing/Encoding Delay**: Time to encode audio.
2.  **Packetization Delay**: Time to fill a packet.
3.  **Transmission Delay**: Time to push bits onto link.
4.  **Propagation Delay**: Time to travel across wire.
5.  **Queuing Delay**: Time waiting in routers.
6.  **Buffering Delay**: Jitter buffer at receiver.
7.  **Decoding Delay**: Time to decode.

### Delay Jitter

**Occurrence**: Packets travel through the network; some get stuck in queues, others pass quickly. The spacing between packets changes.
**Mitigation**: **Playout Buffer (Jitter Buffer)**. Receiver holds packets briefly and plays them out at a steady rate.

### Dealing with Packet Loss in VoIP

1.  **FEC (Forward Error Correction)**: Send redundant data.
    *   *Tradeoff*: Increases bandwidth and delay, but recovers loss without retransmission.
2.  **Interleaving**: Shuffle chunks of audio.
    *   *Tradeoff*: Spreads loss out (less noticeable), but increases latency significantly.
3.  **Error Concealment**: Guess the missing audio.
    *   *Method*: Repeat previous packet, or interpolate.

### Adaptive Video Streaming (DASH)

**High-Level Overview**:
*   Video is encoded into multiple versions (quality levels/bitrates).
*   Each version is chopped into small chunks (e.g., 4 seconds).
*   Client requests chunks via HTTP.
*   **Adaptive**: Client monitors network bandwidth. If high, request High-Quality chunk. If low, request Low-Quality chunk.
*   **Intelligence at Client**: Client decides what to request.

### (Optional) Video Compression

*   **I-Frames (Intra)**: Full image. Independent. (Reference point).
*   **P-Frames (Predicted)**: Stores differences from previous frame.
*   **B-Frames (Bidirectional)**: Stores differences from previous AND next frame.
*   **Why not P-frames only?**: Errors propagate. Need I-frames to reset/resync.

### CBR vs VBR

*   **CBR (Constant Bit Rate)**: Same bandwidth usage always. Wasted space for simple scenes, quality loss for complex scenes. Predictable.
*   **VBR (Variable Bit Rate)**: High bandwidth for complex scenes, low for simple. Better quality/size ratio. Harder to stream (bursty).

### Preferred Protocol for Video Delivery: TCP vs UDP

*   **UDP**: Used for RTP/VoIP. Low latency, no retransmission delays.
*   **TCP (HTTP)**: Used for YouTube/Netflix (DASH).
    *   **Why?**:
        *   Passes through firewalls (port 80).
        *   Reliable (no artifacts).
        *   Buffering hides TCP retransmission delays.
        *   Easy to deploy (standard web servers).

### Progressive Download

**Mechanism**:
*   Client downloads video file via HTTP.
*   Player starts playing as soon as enough data is buffered (even before download completes).
*   If download < playback rate: Buffering pause.

### Bitrate Adaptation in DASH

**Goal**: Match video bitrate to available network bandwidth.
**Signals**:
1.  **Throughput**: Measured download speed of previous chunks.
2.  **Buffer Occupancy**: How much video is buffered.
**Algorithm**:
*   If buffer is filling up → Increase quality.
*   If buffer is draining → Decrease quality.
*   **Buffer-filling rate**: Download Rate / Playback Rate.

**Bandwidth Estimation Problems**:
*   **Over-estimation**: Requesting quality too high → Stalling.
*   **Under-estimation**: Requesting quality too low → Poor video when network is good.

---

## Lesson 12: Applications (CDNs and Overlay Networks)

### Drawback of Single Public Web Server

*   **Scalability**: Cannot handle millions of users.
*   **Reliability**: Single point of failure.
*   **Performance**: Users far away experience high latency.

### What is a CDN (Content Delivery Network)?

**Definition**: A network of distributed servers that deliver content (web, video) to users based on their geographical location.
**Goal**: High availability, high performance (low latency).

### Six Major Challenges for Internet Applications

1.  **Peering Point Congestion**: Middle of network is slow.
2.  **Inefficient Routing Protocol**: BGP not optimized for performance.
3.  **Unreliable Networks**: Outages, loss.
4.  **Inefficient Protocols**: TCP slow start.
5.  **Scalability**: Handling flash crowds.
6.  **Security**: DDoS attacks.

### "Enter Deep" vs "Bring Home" CDN Placement

1.  **Enter Deep** (e.g., Akamai):
    *   Place many small clusters deep inside ISPs (thousands of locations).
    *   *Pro*: Closest to user.
    *   *Con*: Hard to manage/maintain.

2.  **Bring Home** (e.g., Limelight):
    *   Place few huge clusters at key IXPs (dozens of locations).
    *   *Pro*: Easier to manage, lower maintenance.
    *   *Con*: Slightly higher latency than deep.

### Role of DNS in CDN

**Role**: DNS is the redirection mechanism. It maps the user's request (hostname) to the optimal CDN server IP.

### Two Main Steps in CDN Server Selection

1.  **Map to Cluster**: Which geographic cluster is best? (e.g., New York vs. London).
2.  **Map to Server**: Which specific server in that cluster? (Load balancing).

### Cluster Selection Strategies

1.  **Simplest**: Geographic (IP Geolocation). Pick closest by distance.
    *   *Limit*: Internet topology doesn't always match geography.
2.  **Metrics-Based**: Use active measurements.
    *   *Metrics*: Delay (Ping), Loss, BGP hop count.

### Distributed System 2-Layered System

*   **Control Core**: Maintains global view, computes policies.
*   **Data Plane (Agents)**: Monitors specific paths, executes decisions.
*   **Challenge**: Keeping state consistent, handling failures.

### Consistent Hashing

**Problem**: In standard hashing (Key % N), changing N (adding/removing server) reshuffles ALL keys. Cache invalidation!
**Consistent Hashing**:
*   Map servers and keys to a ring (0-360 degrees).
*   Key is assigned to the next server on the ring (clockwise).
*   **Benefit**: Adding/removing a server only affects immediate neighbors. Minimal reshuffling.

### DNS Hierarchy

1.  **Root Servers**: The top (.). Know TLD servers.
2.  **TLD Servers**: (.com, .org). Know Authoritative servers.
3.  **Authoritative Servers**: (google.com). Know the actual records.
4.  **Local/Recursive Resolver**: Client's ISP server.

**Why Hierarchical?**: Scalability, decentralized administration.

### Iterative vs. Recursive DNS Queries

*   **Recursive**: "Find the answer for me." (Client → Resolver). Resolver does all the work.
*   **Iterative**: "Tell me who to ask next." (Resolver → Root → TLD → Auth). Resolver does the referrals.

### DNS Caching

**Definition**: Storing DNS results locally for a TTL (Time To Live).
**Benefit**: Reduces load on root/TLD servers, improves latency for user.

### DNS Resource Records

*   **Type A**: Hostname → IP Address (IPv4).
*   **Type AAAA**: Hostname → IP Address (IPv6).
*   **Type NS**: Domain → Name Server.
*   **Type CNAME**: Alias → Canonical Name (nickname → real name).
*   **Type MX**: Mail Exchange server.

### IP Anycast

**Definition**: Assigning the same IP address to multiple servers in different locations.
**Routing**: BGP routes the user to the *topologically closest* server announcing that IP.
**Use**: DNS Root servers, CDN entry points.

### HTTP Redirection

**Mechanism**: Server responds with `301 Moved Permanently` or `302 Found` and a `Location` header.
**Use in CDN**: Direct user to a specific media server.
**Downside**: Requires extra RTT (Round Trip Time).
