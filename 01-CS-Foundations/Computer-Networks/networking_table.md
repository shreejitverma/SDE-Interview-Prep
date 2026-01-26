# Complete Computer Networking Reference - Table Format

## Summary
This document contains an exhaustive list of **235 essential computer networking keywords and concepts** organized by category with detailed explanations.

---

## OSI MODEL LAYERS (52 keywords total)

### Physical Layer (Layer 1) - 5 keywords

| Keyword | Explanation | Example/Context |
|---------|-------------|-----------------|
| Bits | Smallest unit of data - 0s and 1s | Electrical signals over cables |
| Physical Media | Tangible transmission mediums | Ethernet, fiber optic, wireless |
| Hub | Broadcasts data to all ports without filtering | Basic shared medium device |
| Repeater | Regenerates weak signals to extend distance | Signal amplification |
| Modem | Converts digital to analog and vice versa | Internet connectivity |

### Data Link Layer (Layer 2) - 7 keywords

| Keyword | Explanation | Example/Context |
|---------|-------------|-----------------|
| MAC Address | Unique 48-bit device identifier | 00:1A:2B:3C:4D:5E |
| Switch | Forwards frames by MAC address to ports | LAN connectivity |
| Frame | Layer 2 data unit with MAC/VLAN/CRC | Local network transmission |
| Ethernet | LAN protocol standard | Most common LAN technology |
| PPP | Point-to-Point Protocol for direct link | Serial/dial-up connections |
| ARP | Maps IP addresses to MAC addresses | Local address resolution |
| VLAN | Logically segments physical LAN | Network isolation |

### Network Layer (Layer 3) - 11 keywords

| Keyword | Explanation | Example/Context |
|---------|-------------|-----------------|
| IP Address | Unique logical network identifier | 192.168.1.1 or 2001:db8::1 |
| IPv4 | 32-bit addressing scheme | Most widely deployed |
| IPv6 | 128-bit addressing scheme | Next-generation protocol |
| Packet | Layer 3 data unit with routing | Fundamental routing unit |
| Router | Forwards packets by IP address | Inter-network communication |
| Routing | Determines best packet delivery path | Network pathfinding |
| ICMP | Diagnostic and error reporting | ping, traceroute utilities |
| Subnet | Logical network subdivision | Address space partitioning |
| Subnet Mask | Separates network from host bits | 255.255.255.0 or /24 |
| Default Gateway | Exit router for other networks | Primary routing point |
| NAT | Maps internal to external IPs | Address translation |
| IGMP | Manages multicast group memberships | Group-based communication |

### Transport Layer (Layer 4) - 9 keywords

| Keyword | Explanation | Example/Context |
|---------|-------------|-----------------|
| TCP | Reliable, ordered delivery protocol | Email, web, file transfer |
| UDP | Fast, unreliable delivery protocol | VoIP, streaming, DNS |
| Port | Virtual service endpoint (0-65535) | Service identification |
| Socket | IP address + port combination | Network endpoint |
| Segment | TCP data unit | Connection-oriented delivery |
| Datagram | UDP data unit | Connectionless delivery |
| Flow Control | Prevents receiver overflow | TCP sliding window |
| Error Detection | Checks transmission integrity | Checksum calculation |
| Connection Establishment | Three-way TCP handshake | SYN, SYN-ACK, ACK |

### Session Layer (Layer 5) - 4 keywords

| Keyword | Explanation | Example/Context |
|---------|-------------|-----------------|
| Session | Logical application connection | Maintains dialogue |
| Authentication | Verifies user identity | Credential checking |
| Authorization | Determines access rights | Permission enforcement |
| Session Management | Handles connection lifecycle | Start, maintain, end |

### Presentation Layer (Layer 6) - 4 keywords

| Keyword | Explanation | Example/Context |
|---------|-------------|-----------------|
| Encryption | Plaintext to ciphertext conversion | Data security |
| Compression | Reduces data size | GZIP, ZIP, BZIP2 |
| Character Encoding | Text representation format | ASCII, UTF-8, Unicode |
| SSL/TLS | Secure encryption protocol | HTTPS, secure communications |

### Application Layer (Layer 7) - 17 keywords

| Keyword | Explanation | Example/Context |
|---------|-------------|-----------------|
| HTTP | HyperText Transfer Protocol (insecure) | Port 80, web browsing |
| HTTPS | HTTP with encryption | Port 443, secure web |
| FTP | File Transfer Protocol | Port 21, file exchange |
| SFTP | Secure FTP via SSH | Port 22, encrypted files |
| SSH | Secure Shell remote access | Port 22, encrypted terminal |
| Telnet | Unencrypted remote login (deprecated) | Port 23, legacy access |
| SMTP | Simple Mail Transfer Protocol | Port 25, sending email |
| POP3 | Post Office Protocol v3 | Port 110, download email |
| IMAP | Internet Message Access Protocol | Port 143, sync email |
| DNS | Domain Name System | Port 53, hostname resolution |
| DHCP | Dynamic Host Configuration | Auto IP assignment |
| SNMP | Simple Network Management Protocol | Port 161, device monitoring |
| LDAP | Lightweight Directory Access Protocol | Port 389, directory services |
| NTP | Network Time Protocol | Port 123, time sync |
| RTSP | Real Time Streaming Protocol | Port 554, media streaming |
| RTP | Real-time Transport Protocol | Audio/video streaming |
| SIP | Session Initiation Protocol | VoIP setup |

---

## NETWORK TYPES & ARCHITECTURE (21 keywords)

### Network Geographic Scope - 6 keywords

| Type | Description | Typical Coverage | Use Case |
|------|-------------|------------------|----------|
| PAN | Personal Area Network | Few meters | Bluetooth, USB devices |
| LAN | Local Area Network | Single building | Ethernet, offices |
| WAN | Wide Area Network | Worldwide | Internet, branches |
| MAN | Metropolitan Area Network | City-scale | City networks, campuses |
| CAN | Campus Area Network | Multiple buildings | University, corporate |
| GAN | Global Area Network | Worldwide | Global enterprises |

### Wireless Network Technologies - 12 keywords

| Technology | Frequency | Speed | Range | Use Case |
|------------|-----------|-------|-------|----------|
| WiFi 6 (802.11ax) | 2.4/5/6 GHz | 9.6 Gbps | 30-200m | Latest WiFi standard |
| WiFi 5 (802.11ac) | 5 GHz | 1.3 Gbps | 50-100m | High-speed wireless |
| 802.11n | 2.4/5 GHz | 600 Mbps | 50-100m | MIMO technology |
| 802.11g | 2.4 GHz | 54 Mbps | 50-100m | Common legacy |
| 802.11b | 2.4 GHz | 11 Mbps | 30-100m | Deprecated |
| Bluetooth | 2.4 GHz | 2 Mbps | 10-100m | Device pairing |
| BLE | 2.4 GHz | 1-2 Mbps | 50-250m | Low-power IoT |
| LTE | 2G-4G bands | 10-100 Mbps | Cellular | Mobile broadband |
| 5G | Multiple bands | Up to 10 Gbps | Cellular | Next-gen mobile |
| WLAN | WiFi-based | Varies | Building | Wireless LAN |
| WMAN | WiMAX | Varies | City | Wireless MAN |
| WPAN | Bluetooth/ZigBee | Varies | Local | Wireless PAN |

### Network Topologies - 3 keywords

| Topology | Structure | Characteristics | Use Case |
|----------|-----------|-----------------|----------|
| Star | Central hub/switch | Centralized, easy to manage | Modern networks |
| Mesh | Multi-path connections | Redundant, self-healing | Critical systems |
| Ring | Circular chain | Sequential flow | Token ring (legacy) |
| Bus | Shared medium | Simple, collision-prone | Early networks |
| Tree | Hierarchical | Scalable | Large networks |
| Hybrid | Mixed topologies | Flexible, complex | Enterprise |

---

## NETWORK PROTOCOLS (40 keywords)

### Core Internet Protocols - 5 keywords

| Protocol | Layer | Purpose | Function |
|----------|-------|---------|----------|
| TCP/IP | 3-4 | Protocol suite foundation | All internet communication |
| IP | Layer 3 | Routing and addressing | Packet forwarding |
| ICMP | Layer 3 | Diagnostics and errors | ping, traceroute |
| IGMP | Layer 3 | Multicast management | Group membership |
| ARP | Layer 2/3 | IP to MAC resolution | Local address mapping |

### Routing Protocols - 5 keywords

| Protocol | Type | Metric | Best For | Scalability |
|----------|------|--------|----------|------------|
| RIP | Distance Vector | Hop count | Small networks | Poor (max 15 hops) |
| OSPF | Link State | Cost/bandwidth | Enterprise | Good (unlimited) |
| EIGRP | Hybrid | Bandwidth/delay | Cisco-centric | Excellent |
| BGP | Exterior | Best path | Internet backbone | Excellent |
| IS-IS | Link State | Cost | ISPs | Excellent |

### Application Layer Protocols - 15 keywords

| Protocol | Port | Type | Purpose | Security |
|----------|------|------|---------|----------|
| HTTP | 80 | Text | Web browsing | None |
| HTTPS | 443 | Text | Secure web | TLS |
| FTP | 21 | Binary | File transfer | None |
| SFTP | 22 | Binary | Secure transfer | SSH |
| SMTP | 25 | Text | Email sending | Optional |
| POP3 | 110 | Text | Email download | Optional |
| IMAP | 143 | Text | Email sync | Optional |
| DNS | 53 | Both | Name resolution | DNSSEC |
| SSH | 22 | Text | Remote access | Built-in |
| SNMP | 161 | Binary | Monitoring | Community string |
| LDAP | 389 | Binary | Directory | LDAP-TLS |
| NTP | 123 | Binary | Time sync | Key-based |
| RTSP | 554 | Text | Media control | Optional |
| RTP | Varies | Binary | Real-time | Optional |
| SIP | 5060 | Text | VoIP setup | Optional |

### Security Protocols - 5 keywords

| Protocol | Layer | Purpose | Key Feature | Use Case |
|----------|-------|---------|-------------|----------|
| SSL | 4-6 | Encryption (deprecated) | Legacy | HTTPS (historical) |
| TLS | 4-6 | Encryption (modern) | Handshake | HTTPS, secure email |
| IPsec | Layer 3 | Full network encryption | Tunnel/transport | VPNs, site-to-site |
| SSH | Layer 7 | Secure remote access | Key-based auth | Secure shell |
| Kerberos | Layer 7 | Authentication | Ticket system | Enterprise auth |
| RADIUS | Layer 7 | AAA server | Centralized auth | Network access |

### Other Protocols - 5 keywords

| Protocol | Purpose | Bandwidth | Latency | Use Case |
|----------|---------|-----------|---------|----------|
| MQTT | IoT messaging | Low | Low | IoT applications |
| CoAP | IoT protocol | Very low | Low | Constrained devices |
| AMQP | Message queuing | Medium | Medium | Enterprise messaging |
| WebSocket | Bidirectional web | Varies | Low | Real-time web |
| QUIC | Modern protocol | High | Very low | Next-gen internet |

---

## NETWORK QUALITY & PERFORMANCE (18 keywords)

### QoS Metrics - 6 keywords

| Metric | Definition | Acceptable Range | Impact |
|--------|-----------|------------------|--------|
| Bandwidth | Capacity per timeframe | 1 Mbps - 1 Gbps | Overall throughput |
| Latency | Round-trip delay | < 150 ms (good) | Real-time apps |
| Jitter | Delay variation | < 30 ms | Streaming quality |
| Packet Loss | Loss percentage | < 1% (good) | Retransmissions |
| Throughput | Actual data rate | 80-95% of bandwidth | Real performance |
| Response Time | System delay | < 500 ms (good) | User experience |

### QoS Mechanisms & Congestion Control - 12 keywords

| Mechanism | Description | Purpose | Technique |
|-----------|-------------|---------|-----------|
| Traffic Shaping | Rate limiting with buffering | Smooth bursts | Policing at source |
| Traffic Policing | Hard limit enforcement | Bandwidth control | Drops excess traffic |
| Prioritization | Priority marking (ToS, DSCP) | Critical traffic first | DiffServ |
| Weighted Fair Queuing | Fair bandwidth distribution | Proportional allocation | Dynamic queuing |
| Priority Queuing | Strict priority levels | Highest priority first | Static queuing |
| Congestion Control | TCP window adjustment | Prevent overload | Sliding window |
| Congestion Avoidance | Proactive prevention | Early detection | RED algorithm |
| Bandwidth Reservation | Pre-allocation (RSVP) | Guarantee resources | Resource guarantee |
| Flow Control | Receiver-driven regulation | Prevent overflow | TCP window |
| Rate Limiting | Maximum rate enforcement | Service limits | Hard ceiling |
| Load Balancing | Distribute across resources | Even utilization | Round-robin |
| Admission Control | Accept/reject new flows | Reserve resources | Per-flow reservation |

---

## NETWORK DEVICES (13 keywords)

### Core Networking Devices - 7 keywords

| Device | Layer | Function | Forwarding Method | Scope |
|--------|-------|----------|-------------------|-------|
| Hub | Layer 1 | Broadcast all traffic | All ports | Physical |
| Switch | Layer 2 | Forward by MAC | MAC table | Local segment |
| Router | Layer 3 | Forward by IP | Routing table | Inter-network |
| Gateway | Layer 3-7 | Protocol translation | Application | Network boundary |
| Firewall | Security | Filter traffic | Rule set | Network boundary |
| Proxy Server | Layer 7 | Request intermediary | Cache/rules | Application level |
| Load Balancer | Layer 4-7 | Distribute connections | Hash/round-robin | Server pool |

### Advanced Networking Devices - 6 keywords

| Device | Purpose | Technology | Benefit |
|--------|---------|-----------|---------|
| Layer 3 Switch | Combine L2 and L3 | Hardware switching + routing | High performance |
| WLAN Controller | Centralize WiFi management | Multiple APs | Unified control |
| Access Point | WiFi coverage | 802.11 standards | Wireless access |
| Modem | ISP connection | Modulation/demodulation | Internet connectivity |
| Wireless Extender | Extend WiFi range | Signal repetition | Coverage expansion |
| Bridge | Connect network segments | MAC learning | Segment connection |

---

## NETWORK ARCHITECTURE & VIRTUALIZATION (11 keywords)

### Architecture Concepts - 5 keywords

| Concept | Description | Function | Benefit |
|---------|-------------|----------|---------|
| SDN | Software-Defined Networking | Centralized control | Programmable networks |
| Control Plane | Decision-making logic | Routing decisions | Network intelligence |
| Data Plane | Actual forwarding | Packet movement | Data processing |
| Management Plane | Configuration/monitoring | Device management | Oversight and control |
| Network Segmentation | Logical division | VLAN, subnetting | Security, performance |

### Virtualization Technologies - 6 keywords

| Technology | Description | Component | Function |
|------------|-------------|-----------|----------|
| NFV | Network Functions Virtualization | Virtualizes functions | Software-based services |
| VNF | Virtual Network Function | Software service | Replaces hardware |
| NFVI | NFV Infrastructure | Hypervisor/containers | Resource abstraction |
| MANO | Management and Orchestration | Control layer | Automation and lifecycle |
| Virtual Switch | Software switching | vSwitch | Hypervisor switching |
| Network Slicing | Isolated virtual networks | Slice partition | Multi-tenant isolation |

---

## NETWORK SECURITY (16 keywords)

### Encryption & Certificates - 6 keywords

| Element | Description | Purpose | Standard |
|---------|-------------|---------|----------|
| PKI | Public Key Infrastructure | Certificate system | X.509 |
| Digital Certificate | Identity proof | Authentication | X.509 |
| Certificate Authority | Issues certificates | Trust anchor | Verisign, DigiCert |
| RSA | Asymmetric encryption | Key exchange | 2048-bit |
| AES | Symmetric encryption | Data encryption | 256-bit |
| Cipher Suite | Algorithm set | Protocol security | TLS 1.3 |

### VPN & Tunneling - 6 keywords

| Technology | Layer | Scope | Use Case | Setup |
|------------|-------|-------|----------|-------|
| VPN | Variable | Encrypted tunnel | Remote access | Client-server |
| IPsec | Network (3) | All IP traffic | Site-to-site | Complex |
| SSL VPN | Application (7) | Session traffic | Browser access | Simple |
| L2TP | Data Link (2) | Frame level | Point-to-point | Medium |
| Tunnel Mode | IPsec | Full packet | Untrusted networks | Header + payload |
| Transport Mode | IPsec | Payload only | Trusted networks | Payload only |

### Threat Prevention - 4 keywords

| Defense | Description | Detection | Action |
|---------|-------------|-----------|--------|
| Firewall | Filters by rules | Stateful inspection | Allow/block |
| IDS | Intrusion Detection | Pattern matching | Alert only |
| IPS | Intrusion Prevention | Pattern matching | Block/alert |
| DDoS Protection | Against volumetric attacks | Traffic analysis | Mitigation |

---

## NETWORK ADDRESSING & NAMING (20 keywords)

### IP Addressing Concepts - 11 keywords

| Concept | Definition | Example | Purpose |
|---------|-----------|---------|---------|
| IPv4 Address | 32-bit, dotted decimal | 192.168.1.1 | Host identification |
| IPv6 Address | 128-bit, hexadecimal | 2001:db8::1 | Future addressing |
| Public IP | Internet-routable | 8.8.8.8 | Global reachability |
| Private IP | Non-routable internally | 10.0.0.0/8 | Internal networks |
| Loopback | Local testing address | 127.0.0.1 | Self-communication |
| Broadcast | All hosts in network | 192.168.1.255 | Network-wide message |
| Multicast | Specific host group | 224.0.0.1 | Group communication |
| CIDR Notation | Prefix-based addressing | 192.168.1.0/24 | Flexible subnetting |
| Subnet Mask | Network boundary marker | 255.255.255.0 | Network definition |
| Default Gateway | First-hop router | 192.168.1.1 | Network exit |
| DHCP Lease | Temporary IP assignment | 24-hour renewal | Automatic configuration |

### Domain Naming Service - 9 keywords

| Concept | Definition | Purpose | Function |
|---------|-----------|---------|----------|
| DNS | Domain Name System | Hostname resolution | Distributed naming |
| FQDN | Fully Qualified Domain Name | Complete address | www.example.com |
| A Record | IPv4 mapping | Hostname to IPv4 | Resolution |
| AAAA Record | IPv6 mapping | Hostname to IPv6 | Modern resolution |
| CNAME Record | Canonical name | Hostname alias | Aliasing |
| MX Record | Mail exchange | Email server pointer | Mail routing |
| NS Record | Nameserver | Authority delegation | Delegation |
| DNS Cache | Stored results | Speed optimization | Faster lookup |
| Recursive Query | Full resolution | Complete service | Client to resolver |

---

## ADVANCED NETWORKING TOPICS (26 keywords)

### Data Flow & Transmission Modes - 7 keywords

| Concept | Direction | Characteristics | Use Case |
|---------|-----------|-----------------|----------|
| Full Duplex | Bidirectional simultaneous | Two-way at once | Modern switches |
| Half Duplex | Bidirectional alternate | One way at a time | Legacy hubs |
| Simplex | Unidirectional | One-way only | Broadcasting |
| Collision Domain | Shared medium segment | Collisions occur | Hub networks |
| Broadcast Domain | Frame broadcast segment | Broadcast reaches | VLAN scope |
| Flow Control | Receiver regulation | Prevents overflow | TCP |
| Sliding Window | Variable-size buffer | Dynamic rate | TCP |

### Quality & Performance Concepts - 12 keywords

| Concept | Description | Measurement | Optimization |
|---------|-------------|-------------|--------------|
| QoS Classification | Traffic categorization | Priority levels | Traffic marking |
| DiffServ | Differentiated services | DSCP values | Per-hop behavior |
| ToS | Type of Service field | 8 bits in header | Marking field |
| DSCP | Differentiated Services Code Point | 6 bits | QoS marking |
| MPLS | Multiprotocol Label Switching | Label stack | Label-based routing |
| VRF | Virtual Routing and Forwarding | Per-tenant routing | Multi-tenant |
| Failover | Automatic backup activation | RTO/RPO | Redundancy |
| Redundancy | Fault tolerance paths | Multiple paths | Resilience |
| SLA | Service Level Agreement | Uptime percentage | Guarantee |
| Throughput Optimization | Maximize actual rate | Mbps achieved | Performance |
| Latency Optimization | Minimize delay | Milliseconds | Responsiveness |
| Packet Loss Reduction | Minimize discards | Percentage | Reliability |

### Advanced Routing & Control - 7 keywords

| Concept | Description | Layer | Application |
|---------|-------------|-------|------------|
| BGP | Border Gateway Protocol | Layer 3 | Internet routing |
| AS | Autonomous System | Network | Internet authority |
| ECMP | Equal-Cost Multipath | Layer 3 | Load balancing |
| Route Redistribution | Share routes between protocols | Layer 3 | Integration |
| Route Summarization | Aggregate routes | Layer 3 | Scalability |
| Policy Routing | Route by policy | Layer 3 | Custom rules |
| Traffic Engineering | Explicit path control | Layer 3 | Optimization |

---

## NETWORK MANAGEMENT & MONITORING (11 keywords)

### Management Protocols & Tools - 6 keywords

| Protocol | Purpose | Port | Function | Data Type |
|----------|---------|------|----------|-----------|
| SNMP | Network management | 161 | Device monitoring | Structured |
| Syslog | System logging | 514 | Centralized logging | Text messages |
| NetFlow | Flow analysis | Variable | Traffic reporting | Flow records |
| IPFIX | IP flow information | Variable | Standardized flow | Flow records |
| NETCONF | Network configuration | 830 | Device config | XML-based |
| YANG | Data model | N/A | NETCONF schema | Configuration |

### Monitoring & Analysis Functions - 5 keywords

| Function | Tool/Method | Purpose | Scope |
|----------|------------|---------|-------|
| Packet Capture | Tcpdump, Wireshark | Detailed inspection | Individual packets |
| Flow Analysis | NetFlow, IPFIX | Traffic patterns | Flow-level |
| Link Monitoring | SNMP polling | Health status | Per-interface |
| Performance Monitoring | Probes, agents | Metrics tracking | Real-time |
| Baseline Analysis | Historical comparison | Anomaly detection | Trends |

---

## COMPLETE STATISTICS

**Total Networking Keywords & Concepts: 235**

### Breakdown by Category:

| Category | Keywords | Subcategories |
|----------|----------|----------------|
| OSI Model Layers | 52 | 7 layers |
| Network Types & Architecture | 21 | 3 subcategories |
| Network Protocols | 40 | 5 types |
| Network Quality & Performance | 18 | 2 subcategories |
| Network Devices | 13 | 2 types |
| Network Architecture & Virtualization | 11 | 2 subcategories |
| Network Security | 16 | 3 subcategories |
| Network Addressing & Naming | 20 | 2 subcategories |
| Advanced Networking Topics | 26 | 3 subcategories |
| Network Management & Monitoring | 11 | 2 subcategories |

---

## QUICK REFERENCE BY KNOWLEDGE AREA

### For Network Design
- OSI Model Layers (understanding protocol stack)
- Network Types & Architecture (topology selection)
- Network Devices (component selection)

### For Administration
- Network Protocols (understanding communication)
- Network Addressing & Naming (IP management)
- Network Management & Monitoring (device oversight)

### For Security
- Network Security (encryption, VPN, threats)
- Network Devices (firewalls, IPS/IDS)
- Network Protocols (secure protocols)

### For Performance Optimization
- Network Quality & Performance (QoS metrics)
- Advanced Networking Topics (optimization)
- Network Management & Monitoring (analysis)

### For Development (API/IoT)
- Application Layer Protocols (HTTP, REST)
- Network Addressing & Naming (DNS, IP)
- Advanced Networking Topics (data flow)

All concepts include practical context and real-world applications for comprehensive understanding.
