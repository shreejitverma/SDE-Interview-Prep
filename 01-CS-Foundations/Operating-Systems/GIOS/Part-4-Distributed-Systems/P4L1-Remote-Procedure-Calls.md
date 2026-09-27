---
type: concept
track: [sde]
level: advanced
status: complete
last_reviewed:
sources:
  - "Georgia Tech CS 6200 P4L1"
  - "Implementing Remote Procedure Calls, Birrell & Nelson (1984)"
  - "gRPC documentation - grpc.io"
  - "Sun RPC / ONC RPC RFC 5531"
---

# P4L1: Remote Procedure Calls

## Table of Contents
1. [Why RPC?](#1-why-rpc)
2. [RPC Requirements](#2-rpc-requirements)
3. [RPC Structure and Components](#3-rpc-structure-and-components)
4. [Interface Definition Language (IDL)](#4-interface-definition-language-idl)
5. [Marshaling and Serialization](#5-marshaling-and-serialization)
6. [Binding and Registry](#6-binding-and-registry)
7. [Transport and Protocols](#7-transport-and-protocols)
8. [Error Handling and Semantics](#8-error-handling-and-semantics)
9. [Sun RPC / ONC RPC (Practical)](#9-sun-rpc--onc-rpc-practical)
10. [gRPC (Modern)](#10-grpc-modern)
11. [Windows RPC (MSRPC)](#11-windows-rpc-msrpc)
12. [Performance and Optimization](#12-performance-and-optimization)
13. [Security in RPC](#13-security-in-rpc)
14. [Quiz Callouts](#14-quiz-callouts)

---

## 1. Why RPC?

Distributed computing requires processes on different machines to cooperate. Without RPC, developers must:
- Manually open sockets
- Serialize/deserialize data
- Handle network errors explicitly
- Implement retry logic

**Goal of RPC**: Make calling a remote function look *as much like a local call as possible* - hiding the network entirely from application code.

```
WITHOUT RPC:                        WITH RPC:
┌─────────────────────────────┐    ┌──────────────────────────────┐
│ int sock = socket(...);     │    │ // Looks like a local call!  │
│ connect(sock, &addr, len);  │    │ result = add(3, 5);          │
│ char buf[64];               │    └──────────────────────────────┘
│ sprintf(buf,"add 3 5\n");   │
│ write(sock, buf, len);      │    Underneath, RPC stub handles
│ read(sock, buf, 64);        │    all the socket/serialization
│ result = atoi(buf);         │    complexity automatically.
└─────────────────────────────┘
```

**Birrell & Nelson (1984)** defined the seminal RPC model at Xerox PARC, which all modern RPC systems descend from.

---

## 2. RPC Requirements

| Requirement | Description |
|-------------|-------------|
| **Transparency** | Remote call looks like local call to caller |
| **Efficiency** | Minimize per-call overhead (marshaling, network RTT) |
| **Generality** | Work with arbitrary argument/return types |
| **Safety** | Handle partial failures, network errors gracefully |
| **Interoperability** | Different languages, OSes, architectures |

### Transparency Levels

```
Level 0 - No transparency:   programmer writes all network code
Level 1 - Location:          client doesn't know server's address
Level 2 - Access:            remote call syntax == local call syntax
Level 3 - Migration:         service can move without client change
Level 4 - Replication:       multiple servers appear as one
Level 5 - Failure:           partial failures hidden (controversial!)
```

> [!NOTE]
> Full failure transparency is **impossible** (Deutsch's Fallacies of Distributed Computing). RPC systems expose failures via exceptions or error returns.

**Fallacies of Distributed Computing** (Peter Deutsch):
1. The network is reliable
2. Latency is zero
3. Bandwidth is infinite
4. The network is secure
5. Topology doesn't change
6. There is one administrator
7. Transport cost is zero
8. The network is homogeneous

---

## 3. RPC Structure and Components

```
CLIENT SIDE                              SERVER SIDE
┌────────────────────────────────────┐  ┌────────────────────────────────────┐
│                                    │  │                                    │
│  Client Application Code           │  │  Server Application Code           │
│  ┌─────────────────────────────┐   │  │  ┌─────────────────────────────┐   │
│  │  result = add(x, y);       │   │  │  │  int add(int a, int b) {    │   │
│  └──────────┬──────────────────┘   │  │  │    return a + b;           │   │
│             │                      │  │  │  }                          │   │
│  Client Stub (generated)           │  │  └──────────────────────────────┘   │
│  ┌─────────────────────────────┐   │  │                                    │
│  │  - Marshal arguments        │   │  │  Server Stub (generated)           │
│  │  - Send request             │   │  │  ┌─────────────────────────────┐   │
│  │  - Receive response         │◄──┼──┼──┤  - Receive request          │   │
│  │  - Unmarshal result         │───┼──┼──►  - Unmarshal arguments      │   │
│  └──────────┬──────────────────┘   │  │  │  - Call actual function     │   │
│             │                      │  │  │  - Marshal return value     │   │
│  RPC Runtime Library               │  │  │  - Send response            │   │
│  ┌─────────────────────────────┐   │  │  └─────────────────────────────┘   │
│  │  - Connection management    │   │  │                                    │
│  │  - Transport (TCP/UDP)      │   │  │  RPC Runtime Library               │
│  │  - Error handling           │   │  │  ┌─────────────────────────────┐   │
│  └─────────────────────────────┘   │  │  │  - Listen on port           │   │
└────────────────────────────────────┘  │  │  - Dispatch to stub         │   │
                                        │  │  - Thread pool              │   │
                                        │  └─────────────────────────────┘   │
                                        └────────────────────────────────────┘
```

### Key Components

| Component | Role |
|-----------|------|
| **Client stub** | Proxy; marshals args, sends over network, returns result |
| **Server stub** (skeleton) | Demarshal args, dispatch to impl, marshal response |
| **RPC runtime** | Transport, connection mgmt, threading, error handling |
| **Name/bind service** | Maps service name → server address |
| **IDL compiler** | Generates stubs from interface definition |

---

## 4. Interface Definition Language (IDL)

IDL is a **language-neutral specification** of a service's interface. The IDL compiler generates client and server stubs in the target programming language.

### Sun RPC XDR IDL (`.x` files)

```c
/* calculator.x - Sun RPC IDL */

/* Define the program, version, and procedure numbers */
program CALCULATOR_PROG {
    version CALCULATOR_VERS {
        /* procedure number, args, return type */
        int ADD(int) = 1;          /* add(x) -- actually passes struct */
        int MULTIPLY(int) = 2;
        string GREET(string) = 3;
    } = 1;
} = 0x20000001;  /* program number (must be unique) */
```

```c
/* Better: using struct for multiple args */
struct add_args {
    int a;
    int b;
};

program CALCULATOR_PROG {
    version CALCULATOR_VERS {
        int ADDTWO(add_args) = 1;
        void CLEAR(void)     = 2;
    } = 1;
} = 0x20000001;
```

Generate stubs:
```bash
# Install rpcgen (part of libtirpc-dev on Linux)
sudo apt-get install libtirpc-dev rpcgen

# Generate client/server stubs from IDL
rpcgen -a calculator.x
# Generates:
#   calculator.h        - data type definitions
#   calculator_clnt.c   - client stub
#   calculator_svc.c    - server main
#   calculator_xdr.c    - XDR marshaling routines
#   Makefile.calculator - build rules

ls -la calculator*
# calculator.h calculator_clnt.c calculator_svc.c calculator_xdr.c Makefile.calculator
```

### Protocol Buffers IDL (`.proto` files for gRPC)

```protobuf
// calculator.proto
syntax = "proto3";

package calculator;

// Service definition
service Calculator {
    rpc Add (AddRequest) returns (AddResponse);
    rpc Multiply (MultiplyRequest) returns (MultiplyResponse);
    
    // Server streaming RPC
    rpc SquareStream (SquareRequest) returns (stream SquareResponse);
    
    // Bidirectional streaming
    rpc Chat (stream ChatMessage) returns (stream ChatMessage);
}

// Message definitions
message AddRequest {
    int32 a = 1;
    int32 b = 2;
}

message AddResponse {
    int32 result = 1;
    string error_msg = 2;
}

message MultiplyRequest {
    int32 a = 1;
    int32 b = 2;
}

message MultiplyResponse {
    int32 result = 1;
}

message SquareRequest {
    int32 n = 1;  // compute squares 1..n
}

message SquareResponse {
    int32 value = 1;
}

message ChatMessage {
    string sender = 1;
    string text   = 2;
    int64  timestamp = 3;
}
```

```bash
# Install protoc and gRPC plugin
sudo apt-get install protobuf-compiler
pip install grpcio grpcio-tools

# Generate Python stubs
python -m grpc_tools.protoc \
    -I. \
    --python_out=. \
    --grpc_python_out=. \
    calculator.proto
# Generates:
#   calculator_pb2.py       - message classes
#   calculator_pb2_grpc.py  - service stubs

# Generate C++ stubs
protoc --cpp_out=. --grpc_out=. \
    --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) \
    calculator.proto
# Generates: calculator.pb.h calculator.pb.cc
#            calculator.grpc.pb.h calculator.grpc.pb.cc
```

### Thrift IDL (Apache Thrift)

```thrift
// calculator.thrift
namespace py calculator
namespace cpp calculator
namespace java com.example.calculator

// Custom exception
exception CalculatorException {
    1: i32 code,
    2: string message
}

// Data types
struct Operands {
    1: i32 a,
    2: i32 b
}

// Service definition
service Calculator {
    i32 add(1: Operands ops) throws (1: CalculatorException ex),
    i32 multiply(1: Operands ops),
    string ping(),
    oneway void shutdown()  // fire-and-forget
}
```

```bash
# Generate Python from Thrift IDL
thrift --gen py calculator.thrift

# Generate C++ from Thrift IDL
thrift --gen cpp calculator.thrift
```

---

## 5. Marshaling and Serialization

**Marshaling** = encoding arguments into a byte stream for network transmission.
**Unmarshaling** (demarshaling) = decoding the byte stream back into typed values.

### Key Problems to Solve

1. **Byte order** (endianness): x86 is little-endian, network is big-endian
2. **Data alignment**: structures may have padding
3. **Pointer semantics**: can't send raw pointers across the network
4. **Variable-length data**: strings, arrays, dynamic lists
5. **Heterogeneous types**: unions, optional fields

```
Little-endian (x86):  int 0x01020304
  Memory: [04][03][02][01]  ← LSB at lowest address

Big-endian (network):
  Memory: [01][02][03][04]  ← MSB at lowest address
```

### XDR (External Data Representation) - Sun RPC

XDR is the serialization format used by Sun/ONC RPC. All data is big-endian, 4-byte aligned.

```c
/* xdr_demo.c - Manual XDR encoding/decoding */
#include <stdio.h>
#include <rpc/xdr.h>
#include <string.h>

typedef struct {
    int a;
    int b;
    char name[64];
} MyData;

bool_t xdr_mydata(XDR *xdrs, MyData *obj) {
    return xdr_int(xdrs, &obj->a) &&
           xdr_int(xdrs, &obj->b) &&
           xdr_string(xdrs, (char **)&obj->name, 64);
}

int main(void) {
    char buf[256];
    XDR xdrs;
    MyData original = {42, 99, "hello"};
    MyData decoded;

    /* Encode (serialize) */
    xdrmem_create(&xdrs, buf, sizeof(buf), XDR_ENCODE);
    xdr_mydata(&xdrs, &original);
    size_t encoded_len = xdr_getpos(&xdrs);
    printf("Encoded %zu bytes\n", encoded_len);
    xdr_destroy(&xdrs);

    /* Decode (deserialize) */
    xdrmem_create(&xdrs, buf, sizeof(buf), XDR_DECODE);
    memset(&decoded, 0, sizeof(decoded));
    xdr_mydata(&xdrs, &decoded);
    printf("Decoded: a=%d b=%d name=%s\n", decoded.a, decoded.b, decoded.name);
    xdr_destroy(&xdrs);
    return 0;
}
```

```bash
gcc -o xdr_demo xdr_demo.c -lnsl -ltirpc
./xdr_demo
# Encoded 20 bytes
# Decoded: a=42 b=99 name=hello
```

### Protocol Buffers Encoding (gRPC)

Protobuf uses **varint encoding** for integers (variable-length, efficient for small values) and **tag-length-value** wire format:

```
Wire format: [field_number << 3 | wire_type] [length] [data]

Wire types:
  0 = Varint (int32, int64, bool, enum)
  1 = 64-bit (double, fixed64)
  2 = Length-delimited (string, bytes, embedded message, repeated)
  5 = 32-bit (float, fixed32)
```

```python
# proto_encoding_demo.py - Inspect protobuf wire format
import subprocess, sys

# Install: pip install grpcio grpcio-tools protobuf

# Manually encode a message
from google.protobuf import descriptor_pool, message_factory
import struct

# Varint encoding (LEB128)
def encode_varint(value):
    bits = []
    while value > 0x7F:
        bits.append((value & 0x7F) | 0x80)
        value >>= 7
    bits.append(value)
    return bytes(bits)

# Field 1 (a=42): tag = (1 << 3) | 0 = 0x08, value = 42
tag_a = encode_varint((1 << 3) | 0)  # field 1, wire type 0 (varint)
val_a = encode_varint(42)

# Field 2 (b=99): tag = (2 << 3) | 0 = 0x10, value = 99
tag_b = encode_varint((2 << 3) | 0)  # field 2, wire type 0
val_b = encode_varint(99)

msg = tag_a + val_a + tag_b + val_b
print(f"Wire bytes: {msg.hex()}")
print(f"Size: {len(msg)} bytes (vs 8 bytes for two int32s in XDR)")
# Output: Wire bytes: 08 2a 10 63
# That's 4 bytes vs 8 bytes for two packed int32s!
```

### Comparing Serialization Formats

```bash
# Benchmark serialization formats
pip install protobuf msgpack ujson

python3 << 'EOF'
import json, struct, time, msgpack

data = {"a": 42, "b": 99, "name": "hello world", "values": list(range(100))}
N = 100_000

# JSON
start = time.perf_counter()
for _ in range(N):
    s = json.dumps(data)
    json.loads(s)
json_time = time.perf_counter() - start
json_size = len(json.dumps(data).encode())

# MessagePack (binary JSON)
start = time.perf_counter()
for _ in range(N):
    s = msgpack.packb(data)
    msgpack.unpackb(s)
msgpack_time = time.perf_counter() - start
msgpack_size = len(msgpack.packb(data))

print(f"JSON:       {json_time:.3f}s  size={json_size} bytes")
print(f"MessagePack:{msgpack_time:.3f}s  size={msgpack_size} bytes")
# Protobuf would be even smaller and faster
EOF
```

---

## 6. Binding and Registry

**Binding** = the process by which a client stub discovers the server's network address and establishes a connection.

### Static Binding
Server address is hardcoded at compile/config time. Simple but inflexible.

### Dynamic Binding via Portmapper / RPCBIND

```
                      ┌──────────────────┐
   Client             │   RPCBIND/       │       Server
   ┌────────┐         │   Portmapper     │       ┌──────────────────┐
   │        │ 1. Query│   (port 111)     │       │ 1. Register with │
   │  RPC   │────────►│                  │◄──────│    portmapper    │
   │ client │         │  prog → port map │       │                  │
   │        │◄────────│                  │       │  e.g. prog       │
   │        │ 2. Got  └──────────────────┘       │  0x20000001 →    │
   │        │    port=                           │  port 40001      │
   │        │    40001                           └──────────────────┘
   │        │ 3. Connect directly to server:40001
   │        │────────────────────────────────────►
   └────────┘
```

```bash
# View registered RPC services on this machine
rpcinfo -p localhost
# Output:
#    program vers proto   port  service
#     100000    4   tcp    111  portmapper
#     100000    3   tcp    111  portmapper
#     100000    2   tcp    111  portmapper
#     100000    4   udp    111  portmapper
#     100005    1   udp  45678  mountd
#     100003    3   tcp   2049  nfs
#     100003    4   tcp   2049  nfs

# Show all RPC services on a remote host
rpcinfo -p nfs-server.local

# Test specific program/version
rpcinfo -u localhost 100000 2    # UDP ping to portmapper v2
rpcinfo -t localhost 100000 4    # TCP ping to portmapper v4

# Show NFS-specific registration
rpcinfo -s localhost | grep nfs
```

### DNS-SD / mDNS Binding (Modern Approach)

```bash
# Register a service via Avahi (Linux mDNS)
avahi-publish-service "My RPC Server" _rpc._tcp 40001 &

# Discover services
avahi-browse _rpc._tcp -t
# Output:
# + eth0 IPv4 My RPC Server _rpc._tcp local
# = eth0 IPv4 My RPC Server _rpc._tcp local
#    hostname = [mymachine.local]
#    address = [192.168.1.100]
#    port = [40001]
```

### Service Discovery with etcd (Kubernetes/Cloud style)

```bash
# Install etcd
sudo apt-get install etcd

# Register service
ETCDCTL_API=3 etcdctl put /services/calculator/instance-1 '{"host":"192.168.1.10","port":40001,"version":"1.0"}'
ETCDCTL_API=3 etcdctl put /services/calculator/instance-2 '{"host":"192.168.1.11","port":40001,"version":"1.0"}'

# Discover service
ETCDCTL_API=3 etcdctl get --prefix /services/calculator/
# Key: /services/calculator/instance-1
# Value: {"host":"192.168.1.10","port":40001,"version":"1.0"}

# Watch for changes (live discovery)
ETCDCTL_API=3 etcdctl watch --prefix /services/calculator/ &
```

---

## 7. Transport and Protocols

### TCP vs UDP for RPC

| Property | TCP | UDP |
|----------|-----|-----|
| Reliability | Guaranteed delivery | Best-effort |
| Ordering | In-order | Unordered |
| Overhead | Connection setup + ACKs | Minimal |
| Good for | Large messages, complex RPCs | Short, idempotent operations |

### Connection Management

```c
/* rpc_connection_pool.c - Simple connection pool for RPC */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define POOL_SIZE 8

typedef struct {
    int fd;
    int in_use;
    pthread_mutex_t lock;
} Connection;

typedef struct {
    Connection conns[POOL_SIZE];
    const char *host;
    int port;
    pthread_mutex_t pool_lock;
} ConnectionPool;

static ConnectionPool g_pool;

void pool_init(const char *host, int port) {
    g_pool.host = host;
    g_pool.port = port;
    pthread_mutex_init(&g_pool.pool_lock, NULL);
    for (int i = 0; i < POOL_SIZE; i++) {
        g_pool.conns[i].fd     = -1;
        g_pool.conns[i].in_use = 0;
        pthread_mutex_init(&g_pool.conns[i].lock, NULL);
    }
}

int connect_to_server(const char *host, int port) {
    struct sockaddr_in addr;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    struct hostent *he = gethostbyname(host);
    if (!he) { close(fd); return -1; }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(fd); return -1;
    }
    return fd;
}

/* Acquire a connection from pool */
int pool_acquire(void) {
    pthread_mutex_lock(&g_pool.pool_lock);
    for (int i = 0; i < POOL_SIZE; i++) {
        if (!g_pool.conns[i].in_use) {
            g_pool.conns[i].in_use = 1;
            if (g_pool.conns[i].fd < 0) {
                /* Lazy connect */
                g_pool.conns[i].fd = connect_to_server(g_pool.host, g_pool.port);
            }
            int fd = g_pool.conns[i].fd;
            pthread_mutex_unlock(&g_pool.pool_lock);
            return fd;
        }
    }
    pthread_mutex_unlock(&g_pool.pool_lock);
    return -1; /* Pool exhausted */
}

void pool_release(int fd) {
    pthread_mutex_lock(&g_pool.pool_lock);
    for (int i = 0; i < POOL_SIZE; i++) {
        if (g_pool.conns[i].fd == fd) {
            g_pool.conns[i].in_use = 0;
            break;
        }
    }
    pthread_mutex_unlock(&g_pool.pool_lock);
}
```

---

## 8. Error Handling and Semantics

### Call Semantics

The critical question in RPC: **what happens when the call fails?**

```
              Client sends request
                      │
                      ▼
              ┌──────────────┐
              │  Network     │◄─── Packet lost here → server never sees it
              └──────────────┘
                      │
                      ▼
              Server receives & executes
                      │
                      ▼
              ┌──────────────┐
              │  Network     │◄─── Packet lost here → client never gets reply
              └──────────────┘
                      │
                      ▼
              Client receives response
```

| Semantic | Guarantee | Implementation | Use Case |
|----------|-----------|----------------|----------|
| **Maybe** | 0 or 1 execution | No retry, no ack | Monitoring, unreliable events |
| **At-least-once** | ≥1 executions | Retry until ack received | Idempotent ops (read, set) |
| **At-most-once** | ≤1 executions | Dedup by request ID | Non-idempotent (transfer money) |
| **Exactly-once** | =1 execution | 2PC or idempotency keys | Critical transactions |

### Implementing At-Most-Once with Request IDs

```c
/* server_dedup.c - Server-side deduplication for at-most-once semantics */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define DEDUP_TABLE_SIZE 1024
#define DEDUP_TTL_SECONDS 60

typedef struct DedupEntry {
    uint64_t request_id;
    uint64_t client_id;
    int      result;
    time_t   timestamp;
    struct DedupEntry *next;
} DedupEntry;

static DedupEntry *dedup_table[DEDUP_TABLE_SIZE];

static size_t hash(uint64_t client_id, uint64_t req_id) {
    return (client_id * 2654435761ULL ^ req_id) % DEDUP_TABLE_SIZE;
}

/* Check if request was already processed; -1 if not found */
int dedup_lookup(uint64_t client_id, uint64_t req_id, int *result) {
    size_t slot = hash(client_id, req_id);
    DedupEntry *e = dedup_table[slot];
    while (e) {
        if (e->client_id == client_id && e->request_id == req_id) {
            *result = e->result;
            return 1; /* Found - duplicate! */
        }
        e = e->next;
    }
    return 0; /* Not found - new request */
}

/* Store result for future deduplication */
void dedup_store(uint64_t client_id, uint64_t req_id, int result) {
    size_t slot = hash(client_id, req_id);
    DedupEntry *e = malloc(sizeof(DedupEntry));
    e->client_id  = client_id;
    e->request_id = req_id;
    e->result     = result;
    e->timestamp  = time(NULL);
    e->next       = dedup_table[slot];
    dedup_table[slot] = e;
}

/* Process RPC request with at-most-once semantics */
int handle_rpc_add(uint64_t client_id, uint64_t req_id, int a, int b) {
    int cached_result;
    if (dedup_lookup(client_id, req_id, &cached_result)) {
        printf("DUPLICATE request %lu from client %lu → returning cached %d\n",
               req_id, client_id, cached_result);
        return cached_result;
    }

    /* Actually execute (only once!) */
    int result = a + b;
    printf("NEW request %lu from client %lu: %d+%d=%d\n",
           req_id, client_id, a, b, result);
    dedup_store(client_id, req_id, result);
    return result;
}

int main(void) {
    /* Simulate duplicate requests */
    handle_rpc_add(1001, 1, 3, 5);  /* New */
    handle_rpc_add(1001, 2, 10, 20); /* New */
    handle_rpc_add(1001, 1, 3, 5);  /* Duplicate! */
    handle_rpc_add(1001, 1, 3, 5);  /* Duplicate again! */
    return 0;
}
```

```bash
gcc -o server_dedup server_dedup.c
./server_dedup
# NEW request 1 from client 1001: 3+5=8
# NEW request 2 from client 1001: 10+20=30
# DUPLICATE request 1 from client 1001 → returning cached 8
# DUPLICATE request 1 from client 1001 → returning cached 8
```

---

## 9. Sun RPC / ONC RPC (Practical)

Complete working example of a calculator service using Sun RPC.

### Step 1: IDL file

```c
/* calc.x */
struct calc_args {
    int a;
    int b;
};

program CALC_PROG {
    version CALC_VERS {
        int ADDTWO(calc_args)   = 1;
        int SUBTWO(calc_args)   = 2;
        int MULTWO(calc_args)   = 3;
        double DIVTWO(calc_args) = 4;
    } = 1;
} = 0x20000099;
```

### Step 2: Generate stubs

```bash
rpcgen -a calc.x
# Creates: calc.h  calc_clnt.c  calc_svc.c  calc_xdr.c  Makefile.calc
```

### Step 3: Server implementation

```c
/* calc_server.c - Server implementation (fill in generated calc_svc.c) */
#include "calc.h"
#include <stdio.h>

int *addtwo_1_svc(calc_args *args, struct svc_req *rqstp) {
    static int result;
    result = args->a + args->b;
    printf("Server: ADD %d + %d = %d\n", args->a, args->b, result);
    return &result;
}

int *subtwo_1_svc(calc_args *args, struct svc_req *rqstp) {
    static int result;
    result = args->a - args->b;
    return &result;
}

int *multwo_1_svc(calc_args *args, struct svc_req *rqstp) {
    static int result;
    result = args->a * args->b;
    return &result;
}

double *divtwo_1_svc(calc_args *args, struct svc_req *rqstp) {
    static double result;
    if (args->b == 0) {
        fprintf(stderr, "Division by zero!\n");
        return NULL;  /* RPC error */
    }
    result = (double)args->a / args->b;
    return &result;
}
```

### Step 4: Client implementation

```c
/* calc_client.c - Client that calls the remote calculator */
#include "calc.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server_host>\n", argv[0]);
        exit(1);
    }

    CLIENT *cl = clnt_create(argv[1], CALC_PROG, CALC_VERS, "tcp");
    if (!cl) {
        clnt_pcreateerror(argv[1]);
        exit(1);
    }

    calc_args args = {10, 3};

    /* Call ADD */
    int *add_result = addtwo_1(&args, cl);
    if (!add_result) {
        clnt_perror(cl, "add call failed");
    } else {
        printf("Remote ADD: %d + %d = %d\n", args.a, args.b, *add_result);
    }

    /* Call DIV */
    double *div_result = divtwo_1(&args, cl);
    if (!div_result) {
        clnt_perror(cl, "div call failed");
    } else {
        printf("Remote DIV: %d / %d = %.2f\n", args.a, args.b, *div_result);
    }

    clnt_destroy(cl);
    return 0;
}
```

### Step 5: Build and run

```bash
# Build (using the generated Makefile)
make -f Makefile.calc

# OR build manually:
gcc -o calc_server calc_svc.c calc_xdr.c calc_server.c -lnsl -ltirpc
gcc -o calc_client calc_clnt.c calc_xdr.c calc_client.c -lnsl -ltirpc

# Terminal 1: start server
./calc_server &
echo "Server PID: $!"

# Check it registered with portmapper
rpcinfo -p localhost | grep 0x20000099

# Terminal 2: run client
./calc_client localhost
# Remote ADD: 10 + 3 = 13
# Remote DIV: 10 / 3 = 3.33

# Debug with strace
strace -e trace=network ./calc_client localhost 2>&1 | grep -E "socket|connect|send|recv"
```

---

## 10. gRPC (Modern)

gRPC is the modern, high-performance RPC framework from Google, built on HTTP/2 and Protocol Buffers.

### Complete Python gRPC Calculator

```protobuf
// calculator.proto
syntax = "proto3";
package calculator;

service Calculator {
    rpc Add (BinaryOp) returns (Result);
    rpc Multiply (BinaryOp) returns (Result);
    // Server streaming: stream squares from 1 to n
    rpc Squares (SquareRequest) returns (stream Result);
    // Bidirectional streaming: sum a stream of numbers
    rpc RunningSum (stream Number) returns (stream Result);
}

message BinaryOp {
    double a = 1;
    double b = 2;
}

message Result {
    double value = 1;
    string note  = 2;
}

message SquareRequest {
    int32 n = 1;
}

message Number {
    double value = 1;
}
```

```bash
# Generate Python gRPC stubs
python -m grpc_tools.protoc \
    -I. \
    --python_out=. \
    --grpc_python_out=. \
    calculator.proto
```

```python
# grpc_server.py - gRPC server implementation
import grpc
import time
import logging
from concurrent import futures
import calculator_pb2
import calculator_pb2_grpc

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class CalculatorServicer(calculator_pb2_grpc.CalculatorServicer):
    """Implements the Calculator gRPC service."""

    def Add(self, request, context):
        result = request.a + request.b
        logger.info(f"Add({request.a}, {request.b}) = {result}")
        return calculator_pb2.Result(value=result, note="unary RPC")

    def Multiply(self, request, context):
        result = request.a * request.b
        return calculator_pb2.Result(value=result)

    def Squares(self, request, context):
        """Server-streaming RPC: yields n results."""
        logger.info(f"Squares(1..{request.n})")
        for i in range(1, request.n + 1):
            time.sleep(0.1)  # Simulate work
            yield calculator_pb2.Result(
                value=i * i,
                note=f"square of {i}"
            )

    def RunningSum(self, request_iterator, context):
        """Bidirectional streaming RPC."""
        running_total = 0.0
        for number in request_iterator:
            running_total += number.value
            yield calculator_pb2.Result(
                value=running_total,
                note=f"running sum after adding {number.value}"
            )

def serve():
    # Thread pool server (gRPC default)
    server = grpc.server(
        futures.ThreadPoolExecutor(max_workers=10),
        options=[
            ('grpc.max_send_message_length', 50 * 1024 * 1024),
            ('grpc.max_receive_message_length', 50 * 1024 * 1024),
            ('grpc.keepalive_time_ms', 10000),
            ('grpc.keepalive_timeout_ms', 5000),
        ]
    )
    calculator_pb2_grpc.add_CalculatorServicer_to_server(
        CalculatorServicer(), server
    )

    # Listen on port 50051 (insecure - use TLS in production!)
    server.add_insecure_port('[::]:50051')
    server.start()
    logger.info("Server started on port 50051")

    try:
        server.wait_for_termination()
    except KeyboardInterrupt:
        server.stop(grace=5)

if __name__ == '__main__':
    serve()
```

```python
# grpc_client.py - gRPC client with all RPC types
import grpc
import time
import calculator_pb2
import calculator_pb2_grpc

def run_client():
    # Create channel with keepalive
    channel = grpc.insecure_channel(
        'localhost:50051',
        options=[
            ('grpc.keepalive_time_ms', 10000),
            ('grpc.keepalive_timeout_ms', 5000),
            ('grpc.keepalive_permit_without_calls', True),
        ]
    )
    stub = calculator_pb2_grpc.CalculatorStub(channel)

    # ──────────────────────────────────────────
    # 1. Unary RPC
    print("=== Unary RPC ===")
    request = calculator_pb2.BinaryOp(a=10, b=3)
    response = stub.Add(request, timeout=5)
    print(f"Add(10, 3) = {response.value}  [{response.note}]")

    # ──────────────────────────────────────────
    # 2. Server-streaming RPC
    print("\n=== Server Streaming ===")
    squares_req = calculator_pb2.SquareRequest(n=5)
    for result in stub.Squares(squares_req, timeout=10):
        print(f"  {result.note} = {result.value}")

    # ──────────────────────────────────────────
    # 3. Bidirectional streaming RPC
    print("\n=== Bidirectional Streaming ===")
    def number_generator():
        for val in [1, 2, 3, 4, 5]:
            yield calculator_pb2.Number(value=float(val))
            time.sleep(0.1)

    for result in stub.RunningSum(number_generator()):
        print(f"  {result.note}")

    # ──────────────────────────────────────────
    # 4. Error handling
    print("\n=== Error Handling ===")
    try:
        response = stub.Add(request, timeout=0.001)  # Too short timeout
    except grpc.RpcError as e:
        print(f"RPC error: code={e.code()}, details={e.details()}")

    channel.close()

if __name__ == '__main__':
    run_client()
```

```bash
# Run gRPC calculator
pip install grpcio grpcio-tools

# Generate stubs
python -m grpc_tools.protoc -I. --python_out=. --grpc_python_out=. calculator.proto

# Terminal 1
python grpc_server.py

# Terminal 2
python grpc_client.py

# Inspect gRPC traffic with grpcurl
brew install grpcurl

# List services (requires server reflection)
grpcurl -plaintext localhost:50051 list

# Call Add
grpcurl -plaintext -d '{"a": 10, "b": 3}' \
    localhost:50051 calculator.Calculator/Add
```

### gRPC with TLS (Production)

```bash
# Generate self-signed certs for testing
openssl req -x509 -newkey rsa:4096 -keyout server.key -out server.crt \
    -days 365 -nodes -subj '/CN=localhost'

# Server with TLS
python3 << 'EOF'
import grpc
from concurrent import futures
import calculator_pb2_grpc
from grpc_server import CalculatorServicer

with open('server.key', 'rb') as f:
    private_key = f.read()
with open('server.crt', 'rb') as f:
    certificate_chain = f.read()

server_credentials = grpc.ssl_server_credentials(
    [(private_key, certificate_chain)]
)

server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
calculator_pb2_grpc.add_CalculatorServicer_to_server(CalculatorServicer(), server)
server.add_secure_port('[::]:50052', server_credentials)
server.start()
print("Secure server on :50052")
server.wait_for_termination()
EOF
```

### gRPC Interceptors (Middleware)

```python
# grpc_interceptors.py - Authentication, logging, metrics interceptors
import grpc
import time
import logging

class LoggingInterceptor(grpc.ServerInterceptor):
    """Log all RPC calls with timing."""

    def intercept_service(self, continuation, handler_call_details):
        def log_wrapper(request, context):
            method = handler_call_details.method
            start = time.perf_counter()
            logging.info(f"RPC START: {method}")
            try:
                result = continuation(handler_call_details)(request, context)
                elapsed = (time.perf_counter() - start) * 1000
                logging.info(f"RPC OK:    {method} [{elapsed:.1f}ms]")
                return result
            except Exception as e:
                elapsed = (time.perf_counter() - start) * 1000
                logging.error(f"RPC ERR:   {method} [{elapsed:.1f}ms] {e}")
                raise
        handler = continuation(handler_call_details)
        return grpc.unary_unary_rpc_method_handler(
            log_wrapper,
            request_deserializer=handler.request_deserializer,
            response_serializer=handler.response_serializer,
        )


class AuthInterceptor(grpc.ServerInterceptor):
    """Validate Bearer token in metadata."""

    VALID_TOKENS = {"secret-token-123", "another-valid-token"}

    def intercept_service(self, continuation, handler_call_details):
        def auth_wrapper(request, context):
            metadata = dict(context.invocation_metadata())
            token = metadata.get("authorization", "").removeprefix("Bearer ")
            if token not in self.VALID_TOKENS:
                context.abort(grpc.StatusCode.UNAUTHENTICATED, "Invalid token")
            return continuation(handler_call_details)(request, context)
        handler = continuation(handler_call_details)
        return grpc.unary_unary_rpc_method_handler(
            auth_wrapper,
            request_deserializer=handler.request_deserializer,
            response_serializer=handler.response_serializer,
        )
```

---

## 11. Windows RPC (MSRPC)

Windows uses **MS-RPC**, an extension of DCE/RPC (Distributed Computing Environment), implemented in `rpcrt4.dll`.

### MIDL IDL (Microsoft Interface Definition Language)

```idl
// calculator.idl - Windows MIDL
import "oaidl.idl";

[
    uuid(6BFFD098-A112-3610-9833-46C3F87E345A),
    version(1.0),
    helpstring("Calculator RPC Interface")
]
interface ICalculator {
    long Add([in] long a, [in] long b);
    long Subtract([in] long a, [in] long b);
    double Divide([in] long a, [in] long b, [out] long *error_code);
    void Shutdown(void);
}
```

```batch
:: Generate server/client stubs from MIDL
midl /protocol all /out . calculator.idl
:: Generates: calculator.h  calculator_c.c  calculator_s.c
```

### Windows RPC Server (C)

```c
/* win_rpc_server.c */
#include <windows.h>
#include <stdio.h>
#include "calculator.h"  // Generated by MIDL

// ── Interface implementation ──────────────────────────────────────
long Add(long a, long b) {
    printf("Server: Add(%ld, %ld)\n", a, b);
    return a + b;
}

long Subtract(long a, long b) { return a - b; }

double Divide(long a, long b, long *error_code) {
    if (b == 0) { *error_code = 1; return 0.0; }
    *error_code = 0;
    return (double)a / b;
}

void Shutdown(void) {
    printf("Server shutting down...\n");
    RpcMgmtStopServerListening(NULL);
}

// ── Main server setup ─────────────────────────────────────────────
int main(void) {
    RPC_STATUS status;

    // Use named pipe transport (ncacn_np)
    status = RpcServerUseProtseqEp(
        (RPC_WSTR)L"ncacn_ip_tcp",  // TCP/IP protocol
        RPC_C_LISTEN_MAX_CALLS_DEFAULT,
        (RPC_WSTR)L"4747",           // Port number
        NULL
    );
    if (status != RPC_S_OK) {
        fprintf(stderr, "RpcServerUseProtseqEp failed: %ld\n", status);
        return 1;
    }

    // Register interface
    status = RpcServerRegisterIf(
        ICalculator_v1_0_s_ifspec,  // Generated server interface spec
        NULL, NULL
    );
    if (status != RPC_S_OK) {
        fprintf(stderr, "RpcServerRegisterIf failed: %ld\n", status);
        return 1;
    }

    printf("RPC Server listening on TCP port 4747...\n");

    // Start listening (blocks until shutdown)
    status = RpcServerListen(
        1,                                   // min threads
        RPC_C_LISTEN_MAX_CALLS_DEFAULT,      // max concurrent calls
        FALSE                                // wait = block here
    );
    return 0;
}

// Required by MSRPC runtime
void __RPC_FAR * __RPC_USER midl_user_allocate(size_t len) {
    return malloc(len);
}

void __RPC_USER midl_user_free(void __RPC_FAR *ptr) {
    free(ptr);
}
```

### Windows RPC Client (C)

```c
/* win_rpc_client.c */
#include <windows.h>
#include <stdio.h>
#include "calculator.h"

int main(void) {
    RPC_STATUS status;
    RPC_WSTR binding_string;
    handle_t binding_handle;

    // Compose binding string: protocol + server + endpoint
    status = RpcStringBindingCompose(
        NULL,                      // UUID (NULL = use interface UUID)
        (RPC_WSTR)L"ncacn_ip_tcp", // Protocol
        (RPC_WSTR)L"localhost",    // Server
        (RPC_WSTR)L"4747",         // Endpoint (port)
        NULL,
        &binding_string
    );

    status = RpcBindingFromStringBinding(binding_string, &binding_handle);
    RpcStringFree(&binding_string);

    // Make RPC calls (looks just like local function calls!)
    RpcTryExcept {
        long result = Add(binding_handle, 10, 3);
        printf("Remote Add(10, 3) = %ld\n", result);

        long err;
        double div_result = Divide(binding_handle, 10, 3, &err);
        printf("Remote Divide(10, 3) = %.4f (err=%ld)\n", div_result, err);

        Shutdown(binding_handle);
    }
    RpcExcept(1) {
        printf("RPC exception: %lu\n", RpcExceptionCode());
    }
    RpcEndExcept;

    RpcBindingFree(&binding_handle);
    return 0;
}
```

```batch
:: Build with Visual Studio
cl /W4 /D_UNICODE /DUNICODE win_rpc_server.c calculator_s.c /link rpcrt4.lib
cl /W4 /D_UNICODE /DUNICODE win_rpc_client.c calculator_c.c /link rpcrt4.lib

:: PowerShell: inspect MSRPC endpoints
Get-WmiObject -Class Win32_ServerFeature | Where Name -match RPC

:: View RPC endpoint registrations
netsh rpc show
```

---

## 12. Performance and Optimization

### Latency Breakdown

```
Total RPC latency = client marshal + network send + 
                    server unmarshal + server execute +
                    server marshal + network send back +
                    client unmarshal

Typical values on LAN:
  Marshal/unmarshal:   ~1-10 µs  (protobuf)
  TCP syscall overhead: ~5-20 µs
  Network (LAN 1Gbps): ~50-200 µs RTT
  Total:               ~60-250 µs per call
```

### Optimization Techniques

```bash
# 1. Measure RPC latency
python3 << 'EOF'
import grpc, time, statistics
import calculator_pb2, calculator_pb2_grpc

channel = grpc.insecure_channel('localhost:50051')
stub = calculator_pb2_grpc.CalculatorStub(channel)
request = calculator_pb2.BinaryOp(a=1, b=2)

# Warmup
for _ in range(10):
    stub.Add(request)

# Measure
latencies = []
for _ in range(1000):
    t0 = time.perf_counter()
    stub.Add(request)
    latencies.append((time.perf_counter() - t0) * 1000)

print(f"Latency (ms): p50={statistics.median(latencies):.2f} "
      f"p99={sorted(latencies)[990]:.2f} "
      f"mean={statistics.mean(latencies):.2f}")
EOF
```

```python
# 2. Batch requests to amortize per-call overhead
# Bad: N sequential calls
results = [stub.Add(calculator_pb2.BinaryOp(a=i, b=i)) for i in range(100)]

# Better: async calls (futures)
import grpc
futures_list = [stub.Add.future(calculator_pb2.BinaryOp(a=i, b=i))
                for i in range(100)]
results = [f.result() for f in futures_list]

# Best: use streaming RPC for bulk operations
def number_pairs():
    for i in range(100):
        yield calculator_pb2.BinaryOp(a=i, b=i)

# results = list(stub.AddStream(number_pairs()))  # hypothetical streaming Add
```

```c
/* 3. Use shared memory for local "RPC" (avoid network entirely) */
/* See P3L3-IPC notes for full mmap/shmem implementation */

/* 4. Request coalescing in the RPC runtime */
struct PendingRequest {
    struct request req;
    sem_t          done;
    int            result;
};

/* Batch collector thread groups requests, sends as single network msg */
```

---

## 13. Security in RPC

### Authentication Models

```
None (insecure):   Anyone can call your service
PKI/TLS:          Mutual TLS - both sides present certificates
Token-based:       Bearer token in metadata (JWT, OAuth2)
Kerberos:          Ticket-based auth (Windows MSRPC default)
```

### gRPC with JWT Authentication

```python
# grpc_auth_demo.py
import grpc
import jwt
import time

SECRET = "my-secret-key"

def generate_token(user_id: str, expires_in: int = 3600) -> str:
    payload = {
        "sub": user_id,
        "iat": int(time.time()),
        "exp": int(time.time()) + expires_in,
    }
    return jwt.encode(payload, SECRET, algorithm="HS256")

def validate_token(token: str) -> dict:
    return jwt.decode(token, SECRET, algorithms=["HS256"])

# Client: attach token to every call
class TokenCallCredentials(grpc.AuthMetadataPlugin):
    def __init__(self, token):
        self._token = token

    def __call__(self, context, callback):
        callback([("authorization", f"Bearer {self._token}")], None)

def make_authenticated_channel(server_addr: str, token: str):
    creds = grpc.composite_channel_credentials(
        grpc.ssl_channel_credentials(),       # TLS
        grpc.metadata_call_credentials(TokenCallCredentials(token))  # JWT
    )
    return grpc.secure_channel(server_addr, creds)

# Usage
token = generate_token("user-123")
channel = make_authenticated_channel("api.example.com:443", token)
```

---

## 14. Quiz Callouts

> [!NOTE]
> **Quiz: Why is RPC hard?**
> 1. **Partial failure** - server may crash after executing but before responding
> 2. **Heterogeneous data** - different endianness, alignment, type sizes
> 3. **Pointer semantics** - can't send pointers; must copy data
> 4. **Latency** - network adds milliseconds; local calls take nanoseconds
> 5. **No shared state** - client and server have separate address spaces

> [!NOTE]
> **Quiz: At-least-once vs. at-most-once**
> - **At-least-once**: Simple retry on timeout. Safe only for **idempotent** operations (read, set-to-value, delete-if-exists)
> - **At-most-once**: Server deduplicates by (client_id, request_id). Required for **non-idempotent** ops (transfer $100, append to log)
> - At-most-once is **much harder** but needed for correctness in payment/banking systems

> [!NOTE]
> **Quiz: What does the stub do?**
> Client stub: (1) pack args into network message, (2) find server, (3) send request, (4) wait for reply, (5) unpack return value, (6) return to caller.
> Server stub: (1) wait for request, (2) unpack args, (3) call local function, (4) pack result, (5) send reply.

> [!NOTE]
> **Quiz: XDR vs Protobuf**
> - XDR: Fixed 4-byte aligned big-endian, simple, used by NFS/NIS
> - Protobuf: Variable-length varint encoding, schema evolution via field numbers, ~2-3x smaller, ~3-5x faster parse than JSON

> [!IMPORTANT]
> **Key Exam Concept**: In Birrell & Nelson's RPC design, there are **5 pieces**: client, client stub, RPCRuntime, server stub, server. The RPC runtime handles transport, binding, and reliability. Stubs handle marshaling. Application code handles only business logic.

---

*Cross-links: [[P4L2-Distributed-File-Systems]] (NFS uses Sun RPC) | [[P3L3-Inter-Process-Communication]] (local IPC vs. RPC) | [[P2L1-Processes-and-Process-Management]] (process isolation)*
