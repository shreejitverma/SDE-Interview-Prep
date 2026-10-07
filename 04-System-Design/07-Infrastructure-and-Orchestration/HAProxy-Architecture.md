---
type: concept
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources:
  - "HAProxy Configuration Manual and Architecture Reference (HAProxy Technologies)"
  - "Designing Data-Intensive Applications by Martin Kleppmann"
  - "High Performance Browser Networking by Ilya Grigorik"
---

# HAProxy Architecture and Load Balancing Internals

## TL;DR

HAProxy (High Availability Proxy) is an industry-standard, open-source Layer 4 (TCP) and Layer 7 (HTTP) reverse proxy and load balancer engineered for determinism, predictable latency, and maximum network throughput.
Unlike general-purpose web servers like NGINX that serve static disk files and execute application scripts, HAProxy is strictly a dedicated load balancer that does not touch local disk files for content delivery.
Modern HAProxy utilizes a multi-threaded, event-driven engine (`nbthread`) with a lock-free task scheduler running across CPU cores, multiplexing non-blocking network sockets via `epoll` and `kqueue`.
Core architectural capabilities include high-speed Layer 4 stream forwarding, Stick Tables for in-memory real-time state tracking and DDoS mitigation, proactive active and passive health checking, and seamless zero-downtime reloads via master-worker process wrapping.

## Mental Model

HAProxy pipelines incoming client traffic through modular frontend and backend configurations, leveraging in-memory stick tables for real-time state tracking and health checks.

```mermaid
graph TD
    ClientTraffic["Incoming Network Traffic (L4 TCP / L7 HTTP)"]
    
    subgraph HAProxyEngine["HAProxy Core Engine (Multi-Threaded nbthread)"]
        Master["Master Process (-W: Supervises Workers, File Descriptor Passing)"]
        
        subgraph WorkerProcess["Worker Process (Event-Driven Threads)"]
            Thread1["Worker Thread 1 (Lock-Free Scheduler, epoll)"]
            Thread2["Worker Thread 2 (Lock-Free Scheduler, epoll)"]
            
            subgraph MemoryState["In-Memory State Engine"]
                StickTable["Stick Tables (Real-Time Rates, Connection Tracking)"]
                HealthCheck["Active & Passive Health Checker Engine"]
            end
        end
        
        Master --> WorkerProcess
    end
    
    ClientTraffic --> Frontend["Frontend Block (Binds VIP, TLS Termination, ACLs)"]
    Frontend --> Thread1
    Frontend --> Thread2
    
    Thread1 <--> StickTable
    Thread2 <--> StickTable
    
    Frontend --> Backend["Backend Block (Load Balancing Algorithms: roundrobin, leastconn, hash)"]
    
    subgraph TargetServers["Upstream Server Farm"]
        SrvA["Server A (10.0.0.1:8080) [Active Check]"]
        SrvB["Server B (10.0.0.2:8080) [Active Check]"]
        BackupSrv["Backup Server C (10.0.0.3:8080)"]
    end
    
    Backend --> SrvA
    Backend --> SrvB
    Backend -.->|Failover| BackupSrv
    HealthCheck -.->|Health Probes (HTTP 200 / TCP ACK)| SrvA
    HealthCheck -.->|Health Probes (HTTP 200 / TCP ACK)| SrvB
```

## Architectural Internals and Deep Dive

### 1. Multi-Threaded Event-Driven Architecture
Historically, HAProxy ran as a single-process, single-threaded event-driven daemon.
Modern HAProxy (v1.8 through 2.x+) utilizes an advanced Multi-Threaded Architecture:
- **Thread per Core (`nbthread`)**: HAProxy spawns worker threads matching the system's available CPU cores.
- **Lock-Free Task Scheduler**: Unlike architectures that synchronize threads via heavyweight mutexes, HAProxy uses a custom lock-free, cache-aligned task scheduler with lockless queues and atomic CAS operations to distribute network I/O events across threads.
- **Non-Blocking I/O Multiplexing**: Each thread runs an event loop polling OS notification engines (`epoll` on Linux, `kqueue` on FreeBSD/macOS), enabling sub-millisecond packet forwarding across hundreds of thousands of concurrent connections.
- **No Disk I/O in the Hot Path**: HAProxy never performs blocking disk reads or writes during request forwarding. Logs are emitted asynchronously over UDP/UNIX sockets to syslog daemons, preventing disk I/O stalls from degrading network latency.

### 2. Configuration Topologies: Four Building Blocks
HAProxy configurations (`haproxy.cfg`) are organized into four modular sections:
- `global`: Defines process-wide OS parameters (user, group, `nbthread`, max connections `maxconn`, SSL cache size).
- `defaults`: Sets baseline fallback directives (timeouts, mode, balance algorithm) inherited by subsequent proxies.
- `frontend`: Defines how traffic is accepted (listening IP/port, TLS certificates, ACL evaluation, request manipulation).
- `backend`: Defines target server pools (server IP/ports, health checks, connection limits, load balancing algorithms).
- `listen`: Combines a frontend and backend into a single cohesive configuration block (commonly used for administrative stats dashboards and internal proxies).

### 3. Layer 4 (TCP) vs Layer 7 (HTTP) Modes
HAProxy operates under two distinct operational modes:
- **Layer 4 Mode (`mode tcp`)**:
  - Acts as a transparent bi-directional TCP stream proxy.
  - Does not parse or inspect HTTP headers; forwards raw TCP packets between client and server directly.
  - Incurs minimal CPU overhead, capable of forwarding line-rate traffic for arbitrary protocols (MySQL, PostgreSQL, Redis, gRPC, custom binary protocols).
- **Layer 7 Mode (`mode http`)**:
  - Terminates HTTP connections, parses headers, evaluates cookie states, validates HTTP protocol semantics, and manipulates headers (`http-request set-header`).
  - Supports path-based routing, HTTP/2 multiplexing, WebSocket upgrading, and TLS offloading.

### 4. Stick Tables: In-Memory Distributed Tracking
Stick Tables are high-performance in-memory key-value databases embedded directly inside the HAProxy process:
- **Ultra-Fast Operations**: Reads and writes execute in single-digit nanoseconds using locked or lock-free memory lookups.
- **Tracking Capabilities**: Tracks metrics keyed by client IP, HTTP header, TLS session ID, or cookie:
  - Connection rates (`conn_rate(10s)`).
  - HTTP request rates (`http_req_rate(10s)`).
  - Error rates (`http_err_rate(10s)`).
  - Bytes transferred.
- **Abuse Prevention and DDoS Mitigation**: Enables real-time IP tarpitting and rate-limiting rules. If an IP exceeds 100 requests per 10 seconds, HAProxy blocks or denies future packets before routing to backends:
  ```haproxy
  stick-table type ip size 100k expire 30s store http_req_rate(10s)
  http-request track-sc0 src
  http-request deny deny_status 429 if { sc_http_req_rate(0) gt 100 }
  ```
- **Peers Synchronization**: HAProxy instances synchronize stick table states across cluster nodes via the binary `peers` protocol, maintaining global distributed rate limits without an external Redis database.

### 5. Proactive and Passive Health Checking
HAProxy ensures traffic routes exclusively to operational backends:
- **Active Health Checks (`check`)**: HAProxy periodically dispatches synthetic probes to backend servers:
  - Layer 4: Verifies TCP three-way handshake (`SYN` -> `SYN-ACK`).
  - Layer 7: Sends an HTTP request (e.g., `option httpchk GET /healthz HTTP/1.1\r\nHost:\ localhost`) and verifies expected HTTP status codes (`http-check expect status 200`).
  - Parameters: `inter 2000` (probe interval), `rise 2` (consecutive successful probes to mark healthy), `fall 3` (consecutive failures to mark dead).
- **Passive Health Checks (`observe`)**: Monitors real user traffic:
  - If a backend returns 5xx server errors to user requests, HAProxy automatically flags the server and pulls it from the active rotation without waiting for the next active probe interval.

### 6. Seamless Zero-Downtime Reloads
Reloading configurations without dropping active TCP connections:
- **Master-Worker Wrapper Mode (`-W`)**: The master process supervises worker processes.
- When an operator executes a reload (`systemctl reload haproxy` or `haproxy -f haproxy.cfg -p /run/haproxy.pid -sf $(cat /run/haproxy.pid)`):
  1. Master validates the new configuration.
  2. Master spawns a new worker process running the updated configuration.
  3. Master passes the open listening socket file descriptors to the new worker process via UNIX Domain Sockets (`SCM_RIGHTS`).
  4. The new worker begins accepting new client connections immediately.
  5. The old worker process stops accepting new traffic, finishes in-flight requests, and exits cleanly.

## Trade-offs and Comparisons

| Dimension | HAProxy | NGINX | Envoy Proxy |
| :--- | :--- | :--- | :--- |
| **Primary Architecture** | Dedicated L4/L7 Load Balancer | Web Server, Reverse Proxy, Static File Cache | Cloud-Native Service Mesh Sidecar |
| **Static File Delivery** | Incapable (Zero static disk file serving) | Highly optimized (`sendfile` disk engine) | Not supported |
| **Runtime Control** | Dynamic CLI via UNIX socket (`socat`) | Requires configuration reload | Dynamic gRPC Discovery APIs (xDS) |
| **In-Memory Tracking** | Native Stick Tables (Real-time rate/error tracking) | Rate limit zones (Token bucket) | Token bucket rate limit filters |
| **Multi-Threading Model** | Single process with lock-free `nbthread` scheduler | Multi-process worker pool (Process per core) | Multi-threaded worker pool |
| **Latency Consistency** | Exceptional (Deterministic sub-millisecond) | High (Occasionally affected by disk buffering) | Exceptional (Optimized for inter-service RPC) |
| **Configuration Simplicity** | Clean, declarative blocks (`frontend`, `backend`) | Modular, block-based (`server`, `location`) | Verbose, complex JSON/YAML |

## Failure Modes and Mitigations

### 1. Upstream Server Saturation and Thundering Herd
- *Root Cause*: A backend server recovers from a reboot. HAProxy immediately floods the newly healthy server with thousands of concurrent requests, overloading its database connection pools and immediately crashing it again.
- *Mitigation*: Configure slow-start weighting (`slowstart 60s`): HAProxy gradually ramps up traffic to a recovering server over a 60-second window, allowing JIT compilers and database connection pools to warm up.

### 2. Ephemeral Port Exhaustion on Backend Connections
- *Root Cause*: High Layer 7 HTTP request rates establish short-lived connections to backend servers. Sockets close and linger in `TIME_WAIT`, exhausting the Linux kernel's local port range ($32768 - 60999$).
- *Mitigation*: Enable upstream connection pooling using `http-reuse always` or `http-reuse safe`; allocate multiple source IP addresses for backend connections (`source 10.0.0.1:0-0` and `source 10.0.0.2:0-0`).

### 3. Queue Stalls on Backend Server Limits (`maxconn`)
- *Root Cause*: Backends are configured with strict `maxconn` limits to protect upstream databases. When client traffic surges, excess requests queue inside HAProxy's internal backend queue. If `timeout queue` expires, HAProxy drops requests with `503 Service Unavailable`.
- *Mitigation*: Autoscale backend server instances based on HAProxy queue depth metrics (`qcur`); monitor `qtime` (time spent waiting in queue) via Prometheus exporter.

### 4. Split-Brain Health Checking during Transient Network Glitches
- *Root Cause*: A 500ms network packet loss spike between HAProxy and backends causes 3 consecutive health check probes to fail. HAProxy marks all backend servers dead simultaneously, returning 503 errors to 100% of incoming users.
- *Mitigation*: Increase `fall` threshold (e.g., `fall 5`); increase `inter` probe interval; configure backup servers (`server backup1 10.0.0.9:8080 backup`) to absorb traffic during catastrophic pool transitions.

## Hands-On Verification

### Multi-OS Diagnostic Commands

#### Linux / macOS (HAProxy CLI & Runtime Socket)
```bash
# Verify configuration syntax and check for errors
haproxy -c -f /etc/haproxy/haproxy.cfg

# Query HAProxy runtime statistics via UNIX Domain Socket using socat
echo "show info" | sudo socat stdio /var/run/haproxy.sock

# Inspect server health status, weight, and active connection counts
echo "show stat" | sudo socat stdio /var/run/haproxy.sock | cut -d',' -f1,2,18,34,57

# Dynamically drain a backend server for zero-downtime maintenance
echo "set server backend_nodes/server1 state drain" | sudo socat stdio /var/run/haproxy.sock

# Inspect stick table contents and tracked IP request rates
echo "show table api_rate_limit" | sudo socat stdio /var/run/haproxy.sock
```

#### Windows (PowerShell via WSL2 or Native Port)
```powershell
# Check HAProxy port connectivity (80, 443, 8404 stats)
Test-NetConnection -ComputerName localhost -Port 8404

# Query HAProxy CSV stats endpoint via PowerShell
$stats = Invoke-RestMethod -Uri "http://localhost:8404/stats;csv"
$stats | Select-Object -First 5
```

### Complete Production HAProxy Configuration

The following production-grade `haproxy.cfg` demonstrates multi-threaded tuning, Layer 7 routing, stick-table rate limiting, connection pooling, and health checking.

```haproxy
# /etc/haproxy/haproxy.cfg

global
    log /dev/log local0 info
    log /dev/log local1 notice
    chroot /var/lib/haproxy
    user haproxy
    group haproxy
    daemon
    
    # Process & Thread Tuning
    master-worker
    nbthread 4
    maxconn 100000
    
    # Runtime Admin Socket
    stats socket /var/run/haproxy.sock mode 660 level admin expose-fd listeners
    stats timeout 30s
    
    # SSL/TLS Tuning
    ssl-default-bind-ciphers ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256
    ssl-default-bind-options ssl-min-ver TLSv1.2 no-tls-tickets

defaults
    log global
    mode http
    option httplog
    option dontlognull
    option redispatch
    retries 3
    
    # Timeout Settings
    timeout connect 5000ms
    timeout client 50000ms
    timeout server 50000ms
    timeout queue 5000ms
    timeout http-request 10000ms
    timeout http-keep-alive 60000ms

# Administrative Stats Dashboard
listen stats_dashboard
    bind *:8404
    stats enable
    stats uri /
    stats refresh 5s
    stats show-legends

# Public HTTP/S Frontend
frontend http_ingress
    bind *:80
    bind *:443 ssl crt /etc/ssl/certs/site.pem alpn h2,http/1.1
    
    # Redirect HTTP to HTTPS
    redirect scheme https code 301 if !{ ssl_fc }
    
    # Stick Table: Rate Limit to 100 requests per 10 seconds per IP
    stick-table type ip size 200k expire 30s store http_req_rate(10s)
    http-request track-sc0 src
    http-request deny deny_status 429 if { sc_http_req_rate(0) gt 100 }
    
    # Security and Forwarding Headers
    http-request set-header X-Forwarded-Proto https if { ssl_fc }
    http-request set-header X-Forwarded-For %[src]
    
    # Path-Based Routing via ACL
    acl is_api path_beg /api/
    use_backend api_cluster if is_api
    default_backend web_cluster

# Backend API Cluster with Persistent Connection Reuse
backend api_cluster
    balance leastconn
    option httpchk GET /healthz HTTP/1.1\r\nHost:\ localhost
    http-check expect status 200
    
    # Upstream Connection Pooling
    http-reuse always
    
    # Servers with slowstart ramp-up and health check thresholds
    server api01 127.0.0.1:8081 check inter 2000 rise 2 fall 3 maxconn 250 slowstart 60s
    server api02 127.0.0.1:8082 check inter 2000 rise 2 fall 3 maxconn 250 slowstart 60s
    server api_backup 127.0.0.1:8089 check backup

backend web_cluster
    balance roundrobin
    cookie SERVERID insert indirect nocache
    server web01 127.0.0.1:8001 check cookie s1
    server web02 127.0.0.1:8002 check cookie s2
```

### Complete Standalone Simulation: HAProxy Stick Table and Sliding Rate Limiter

The following runnable Python script simulates HAProxy's in-memory Stick Table mechanics.
It demonstrates sub-microsecond sliding window tracking of client IP request velocity, rate limit enforcement (`http-request deny deny_status 429`), and dynamic peer table state synchronization across simulated load balancing nodes.

```python
#!/usr/bin/env python3
"""
Simulates HAProxy In-Memory Stick Table mechanics and rate limiting.
Demonstrates:
- Sub-microsecond in-memory client IP tracking
- Sliding window request rate calculation (http_req_rate)
- Automated threshold enforcement and 429 Too Many Requests rejection
- Dynamic expiration of inactive client state entries
"""

import time
from collections import defaultdict
from dataclasses import dataclass, field
from typing import Dict, List, Tuple

@dataclass
class StickTableEntry:
    request_timestamps: List[float] = field(default_factory=list)
    total_connections: int = 0
    denied_requests: int = 0

class SimulatedHAProxyStickTable:
    def __init__(self, window_seconds: float = 2.0, max_rate: int = 5, entry_expire_seconds: float = 10.0):
        self.window_seconds = window_seconds
        self.max_rate = max_rate
        self.entry_expire_seconds = entry_expire_seconds
        self.table: Dict[str, StickTableEntry] = defaultdict(StickTableEntry)

    def process_request(self, client_ip: str) -> Tuple[int, str]:
        now = time.time()
        entry = self.table[client_ip]
        entry.total_connections += 1

        # Prune timestamps outside the current evaluation sliding window
        entry.request_timestamps = [
            ts for ts in entry.request_timestamps
            if now - ts <= self.window_seconds
        ]

        # Check rate threshold: sc_http_req_rate gt max_rate
        if len(entry.request_timestamps) >= self.max_rate:
            entry.denied_requests += 1
            return 429, "HTTP 429 Too Many Requests (HAProxy Stick Table Limit Exceeded)"

        # Record valid request timestamp
        entry.request_timestamps.append(now)
        return 200, "HTTP 200 OK (Proxied to Backend)"

    def cleanup_expired_entries(self):
        now = time.time()
        expired_keys = [
            ip for ip, entry in self.table.items()
            if not entry.request_timestamps or (now - entry.request_timestamps[-1] > self.entry_expire_seconds)
        ]
        for ip in expired_keys:
            del self.table[ip]

def main():
    stick_table = SimulatedHAProxyStickTable(window_seconds=1.0, max_rate=3)
    client_ip = "192.168.1.100"

    print("Sending 3 allowed requests within 1-second window...")
    for i in range(3):
        status, message = stick_table.process_request(client_ip)
        print(f"Request {i+1}: Status {status}")
        assert status == 200

    print("\nSending 4th request immediately (should trigger stick table rate limit)...")
    status, message = stick_table.process_request(client_ip)
    print(f"Request 4: Status {status} - {message}")
    assert status == 429

    print("\nSleeping 1.1s for sliding window expiration...")
    time.sleep(1.1)

    print("Sending request after sliding window cleared...")
    status, message = stick_table.process_request(client_ip)
    print(f"Request 5: Status {status}")
    assert status == 200
    print("\nHAProxy Stick Table Simulation Verified Successfully!")

if __name__ == "__main__":
    main()
```

## Performance Characteristics and Capacity Planning

### 1. Maximum TCP Throughput and Memory Sizing Math
HAProxy allocates minimal memory per connection:
- In `mode tcp`: $\approx 16\text{KB}$ to $32\text{KB}$ per connection (kernel socket buffers + HAProxy session struct).
- In `mode http`: $\approx 32\text{KB}$ to $64\text{KB}$ per connection (HTTP header parsing buffers `tune.bufsize`).
- For 100,000 concurrent active HTTP connections:

$$\text{RAMRequired} \approx 100,000 \times 64\text{KB} \approx 6.4\text{GB of RAM}$$

This deterministic memory usage allows predictable capacity planning under extreme traffic volumes.

### 2. Network Interface Card (NIC) Saturation Formula
Layer 4 forwarding throughput is limited by network interface packet processing rate (PPS):

$$\text{Throughput}_{\text{Gbps}} = \frac{\text{PPS} \times \text{AveragePacketSizeBytes} \times 8}{10^9}$$

On a standard 10GbE NIC processing 1,200,000 packets per second with 1,500-byte MTU packets:
- Forwarding throughput achieves $\approx 14.4\text{ Gbps}$ line rate without CPU saturation.

## In Production: Real-World Case Studies

### 1. GitHub's Edge Load Balancing Tier
GitHub serves all developer git operations (over SSH and HTTPS) and web traffic via HAProxy:
- **Zero-Downtime TLS Upgrades**: Deployed HAProxy as the ingress router, utilizing master-worker socket migration to execute continuous configuration reloads across millions of active long-lived git push/pull connections.
- **Stick Tables for DDoS Defense**: Employs distributed stick tables to detect and mitigate malicious automated scraping and API abuse in real-time before packets hit application tiers.

### 2. Stack Overflow's Bare-Metal Infrastructure
Stack Overflow powers hundreds of millions of monthly page views across nine bare-metal servers using HAProxy:
- **Low Footprint, Extreme Throughput**: Operates two HAProxy active/passive servers in front of all web, database, and Redis tiers.
- **Microsecond Response**: Serves peak traffic while consuming $<10\%$ CPU utilization per machine, maintaining sub-millisecond routing overhead across all requests.

## Staff+ Interview Questions

> [!question]
> Why does HAProxy choose to avoid serving static files from local disk, and how does this architectural constraint benefit its latency predictability?

> [!success]- Answer
> HAProxy is designed strictly as a dedicated, high-performance network load balancer and proxy. By intentionally omitting static file serving capabilities, HAProxy completely eliminates blocking disk I/O operations from its runtime code paths. In general-purpose web servers like NGINX, reading a file from local disk can trigger kernel page faults and block the event loop if the file is not currently in the operating system page cache, introducing unpredictable tail latency spikes. Because HAProxy never executes blocking disk reads or writes during request forwarding, its threads operate continuously in user space and non-blocking network socket polling. This architectural purity gives HAProxy deterministic, jitter-free packet processing times measured in microseconds, making it exceptionally reliable as an ingress router under heavy concurrency.

> [!question]
> What are HAProxy Stick Tables, and how do they differ from external in-memory caches like Redis for rate limiting and session persistence?

> [!success]- Answer
> Stick Tables are specialized, high-performance in-memory key-value storage structures embedded directly within the HAProxy process memory space. They track metrics (such as request rates, error rates, connection counts, and session cookies) keyed by client IP addresses or arbitrary request attributes. The key distinction from external caches like Redis is access latency: reading or updating a stick table takes single-digit nanoseconds directly in process memory via atomic instructions, without incurring network round-trip latency, serialization, or socket connection overhead. Furthermore, HAProxy instances synchronize stick table data across cluster nodes asynchronously via the native `peers` protocol, providing distributed abuse mitigation and session stickiness without introducing an external database dependency into the load balancing critical path.

> [!question]
> How does HAProxy's master-worker mode (`-W`) execute zero-downtime configuration reloads without dropping in-flight TCP connections?

> [!success]- Answer
> In master-worker mode (`-W`), the master process supervises one or more worker processes. When a reload signal is received, the master parses and validates the new configuration file. It then spawns a brand new worker process running the updated configuration. To prevent dropping active connections or failing socket binds, the master passes the listening file descriptors directly to the new worker process using UNIX domain socket control messages (`SCM_RIGHTS`). The new worker immediately begins accepting new incoming TCP handshakes. The master then signals the old worker process to enter graceful drain mode: the old worker stops listening on the sockets, continues servicing its active in-flight requests until they naturally terminate, and then exits cleanly. This enables seamless, zero-downtime reloads even while sustaining thousands of concurrent long-lived connections.

> [!question]
> Compare HAProxy's Layer 4 (`mode tcp`) and Layer 7 (`mode http`) operational modes. What are the performance and functional trade-offs?

> [!success]- Answer
> In Layer 4 (`mode tcp`), HAProxy functions as a raw transport-layer proxy: it accepts incoming TCP packets and forwards them directly to the backend server without inspecting or modifying application payloads. This mode incurs minimal CPU overhead, achieves line-rate throughput, and supports arbitrary protocols (databases, gRPC, custom binary protocols). However, it cannot inspect URLs, read cookies, or modify HTTP headers. In Layer 7 (`mode http`), HAProxy terminates the TCP/TLS connection, parses HTTP/1.1 and HTTP/2 headers, evaluates URL paths, injects cookies for sticky sessions, and applies complex header-based ACLs. While `mode http` unlocks rich routing, caching, and rate-limiting functionality, it incurs higher CPU and memory overhead per connection because the proxy must buffer and parse HTTP frames.

> [!question]
> What is `http-reuse always` in HAProxy, and how does it prevent backend port exhaustion in high-QPS microservice environments?

> [!success]- Answer
> In high-throughput HTTP/1.1 architectures, opening and closing a separate backend TCP connection for every client request causes severe socket churn. TCP sockets linger in the `TIME_WAIT` state for 60 seconds, rapidly exhausting the host's 65,535 ephemeral port range and triggering `Cannot assign requested address` errors. HAProxy solves this with the `http-reuse` directive, which enables upstream connection pooling. Setting `http-reuse always` instructs HAProxy to reuse existing idle persistent connections in the backend pool aggressively across completely unrelated client requests. As soon as a request-response cycle completes, HAProxy immediately returns the backend connection to the shared idle pool and assigns it to the next incoming request from any client, reducing backend connection setups by over 90% and eliminating port exhaustion.

> [!question]
> How does HAProxy's Slowstart feature (`slowstart <time>`) prevent the "Thundering Herd" crash on newly recovered backend application servers?

> [!success]- Answer
> When a crashed or newly deployed backend server boots up, its internal runtime environment is cold: Java JVM JIT compilers have not optimized hot bytecode, database connection pools are empty, and local in-memory caches are unpopulated. If HAProxy immediately routes full load to this server, the server is overwhelmed by concurrent requests, spikes CPU to 100%, and crashes again (thundering herd failure). The `slowstart` directive (e.g., `server app1 10.0.0.1:8080 check slowstart 60s`) instructs HAProxy to gradually ramp up the server's load-balancing weight from 0% to 100% over the specified duration. The server receives a gentle trickle of requests initially, allowing connection pools and caches to warm up before absorbing full production traffic.

> [!question]
> What is the difference between active health checking (`check`) and passive health checking (`observe`) in HAProxy?

> [!success]- Answer
> Active health checking periodically generates synthetic probe packets from HAProxy to backend servers (e.g., sending a TCP SYN or an HTTP GET request to `/healthz` every 2,000ms). The backend is marked healthy or dead based on consecutive probe results (`rise` and `fall`). However, active checks have a detection latency gap: if a server breaks immediately after a probe, user traffic continues routing to the broken server for the duration of the probe interval (e.g., 2 seconds). Passive health checking (`observe layer4` or `observe layer7`) monitors live user traffic in real time. If HAProxy observes that live user requests to a server fail (e.g., TCP connection resets or consecutive HTTP 500 responses), HAProxy catches the failure immediately and pulls the server from the rotation on the spot, mitigating user-facing errors before the next active probe executes.

> [!question]
> How can HAProxy be dynamically inspected and reconfigured at runtime without restarting or reloading the process?

> [!success]- Answer
> HAProxy provides an interactive Runtime API accessible via a local UNIX domain socket or TCP port configured via `stats socket /var/run/haproxy.sock level admin`. Using command-line utilities like `socat`, administrators and automation scripts can interact with the running process in real time. Commands include: (1) querying metrics (`show info`, `show stat`); (2) dynamically changing backend server weights (`set server backend/srv1 weight 50`); (3) draining servers for maintenance (`set server backend/srv1 state drain`); (4) adding IP addresses to stick tables dynamically; and (5) viewing and updating SSL certificate bundles in memory without reloading the configuration.

> [!question]
> What is the PROXY Protocol (v1 and v2) created by HAProxy, and why is it essential when chaining Layer 4 load balancers in front of backend servers?

> [!success]- Answer
> When a client connects to a Layer 4 TCP proxy (such as AWS NLB or an edge HAProxy instance), the proxy terminates the client TCP connection and opens an entirely new TCP connection to the backend application server. Because this is Layer 4 transport forwarding, the backend server's operating system kernel observes the source IP address of the load balancer, losing the original client's true public IP address. In Layer 7 HTTP, proxies solve this by injecting the `X-Forwarded-For` HTTP header, but in Layer 4 (e.g., raw TCP, databases, WebSockets, SMTP, or end-to-end TLS passthrough), the proxy cannot inject HTTP headers without breaking the byte stream. HAProxy created the PROXY Protocol (now an RFC standard adopted industry-wide) to solve this: before transmitting any application payload, the proxy prepends a standardized one-line text header (v1) or binary struct (v2) containing the real client's 4-tuple (source IP, source port, destination IP, destination port). The receiving backend server or downstream proxy parses and strips this header upon socket connection, restoring full client IP visibility without breaking transport stream transparency.

> [!question]
> How does HAProxy's Stick Table Peering (`peers` section) synchronize rate-limiting and session stickiness across a cluster of independent load balancers without a centralized Redis cluster?

> [!success]- Answer
> Centralized caches like Redis introduce network round-trip latency and create a single point of failure in the load balancing critical path. HAProxy implements the native `peers` protocol to synchronize Stick Tables peer-to-peer over dedicated TCP connections. Each HAProxy instance maintains its own in-memory stick table. When a client performs actions (such as establishing connections or triggering rate-limit counters), HAProxy updates local process memory at sub-microsecond speeds and broadcasts differential delta updates asynchronously to configured peer nodes. Peer nodes merge these deltas into their local tables using push-based gossip replication. If one node experiences a network partition or restarts, each load balancer continues enforcing rate limits and session stickiness locally without latency degradation, and re-synchronizes state upon reconnect, providing distributed high-availability without introducing external database dependencies.

## Related Concepts and Wikilinks

- [[NGINX-Architecture]] - Direct architectural comparison with NGINX reverse proxy.
- [[Load-Balancing]] - Deep dive into Layer 4 vs Layer 7 load balancing algorithms.
- [[HTTP-Evolution-HTTP1-HTTP2-HTTP3]] - HTTP protocol termination and multiplexing.
- [[API-Authentication-and-Authorization]] - Ingress rate limiting and TLS termination.
- [[Docker-and-Container-Runtimes]] - Deploying load balancers inside containerized environments.
- [[Kubernetes-Architecture]] - Ingress controllers and service routing layers.

## Further Reading and References

- Tarreau, Willy. *HAProxy Architecture and Configuration Manual*. HAProxy Technologies, 2024.
- Grigorik, Ilya. *High Performance Browser Networking*. O'Reilly Media, 2013. Chapter 4: Transport Layer Security.
- HAProxy Technologies. "HAProxy 2.0 and Beyond: Multi-Threading and Core Internals." *HAProxy Tech Blog*, 2020.
- Kleppmann, Martin. *Designing Data-Intensive Applications*. O'Reilly Media, 2017. Chapter 1: Reliable, Scalable, and Maintainable Systems.
- Stack Overflow Engineering. "How Stack Overflow Powers 500 Million Monthly Page Views on Bare Metal with HAProxy." *Stack Overflow Blog*, 2021.
