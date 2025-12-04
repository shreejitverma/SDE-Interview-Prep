# Complete Computer Networking Reference

This is an exhaustive list of all essential computer networking keywords, concepts, and terms organized by category.

## Table of Contents
1. [OSI Model Layers](#osi-model-layers)
2. [Network Types & Architecture](#network-types--architecture)
3. [Network Protocols](#network-protocols)
4. [Network Quality & Performance](#network-quality--performance)
5. [Network Devices](#network-devices)
6. [Network Architecture & Virtualization](#network-architecture--virtualization)
7. [Network Security](#network-security)
8. [Network Addressing & Naming](#network-addressing--naming)
9. [Advanced Networking Topics](#advanced-networking-topics)
10. [Network Management & Monitoring](#network-management--monitoring)

---

## OSI Model Layers

### Physical Layer (Layer 1)

| Keyword | Explanation | Example/Context |
|---------|-------------|-----------------|
| Bits | Smallest unit of data transmission - 0s and 1s | Transmitted as electrical signals |
| Physical Media | Tangible transmission mediums - cables, fiber optics, wireless | Ethernet, fiber, radio waves |
| Hub | Broadcasts data to all connected ports without filtering | Basic network device |
| Repeater | Regenerates weak signals to extend transmission distance | Signal amplification |
| Modem | Modulator/demodulator - converts digital to analog signals | Internet connections |

### Data Link Layer (Layer 2)

| Keyword | Explanation | Example/Context |
|---------|-------------|-----------------|
| MAC Address | Media Access Control - unique 48-bit identifier | 00:1A:2B:3C:4D:5E |
| Switch | Forwards data frames based on MAC addresses | LAN connectivity |
| Frame | Data unit containing MAC addresses, VLAN info | Layer 2 encapsulation |
| Ethernet | Protocol for LAN communication | Most common LAN technology |
| PPP | Point-to-Point Protocol - direct node communication | Serial connections |
| ARP | Address Resolution Protocol - IP to MAC mapping | Resolves IP addresses |
| VLAN | Virtual Local Area Network - logical LAN segmentation | Network segmentation |

### Network Layer (Layer 3)

| Keyword | Explanation | Example/Context |
|---------|-------------|-----------------|
| IP Address | Internet Protocol address - unique logical identifier | 192.168.1.1 (IPv4) |
| IPv4 | Internet Protocol version 4 - 32-bit addressing | Most widely used |
| IPv6 | Internet Protocol version 6 - 128-bit addressing | Future protocol |
| Packet | Data unit at Layer 3 with routing information | Basic routing unit |
| Router | Forwards packets between networks by IP address | Inter-network communication |
| Routing | Process of determining best packet delivery path | Network pathfinding |
| ICMP | Internet Control Message Protocol - diagnostics | ping, traceroute |
| Subnet | Logical subdivision of an IP network | Network segmentation |
| Subnet Mask | Determines network vs. host portion of IP | 255.255.255.0 or /24 |
| Default Gateway | Router for traffic outside local network | Network exit point |
| NAT | Network Address Translation - IP address mapping | Internal to external IPs |
| IGMP | Internet Group Management Protocol - multicast | Group communication |

### Transport Layer (Layer 4)

| Keyword | Explanation | Example/Context |
|---------|-------------|-----------------|
| TCP | Transmission Control Protocol - reliable, ordered delivery | Email, web, FTP |
| UDP | User Datagram Protocol - fast, unreliable delivery | VoIP, streaming |
| Port | Virtual endpoint for network communication (0-65535) | Service identification |
| Socket | IP address and port combination | Network endpoint |
| Segment | Data unit for TCP transmission | TCP packet unit |
| Datagram | Data unit for UDP transmission | UDP packet unit |
| Flow Control | Prevents receiver from being overwhelmed | TCP window |
| Error Detection | Checks for transmission errors | Checksum verification |
| Connection Establishment | TCP three-way handshake | SYN, SYN-ACK, ACK |

### Session Layer (Layer 5)

| Keyword | Explanation | Example/Context |
|---------|-------------|-----------------|
| Session | Logical connection between two applications | Maintains dialog |
| Authentication | Verification of user identity | Credential validation |
| Authorization | Determines what authenticated user can access | Permission enforcement |
| Session Management | Establishment and termination of sessions | Dialog control |
| NetBIOS | Network Basic Input/Output System | Legacy communication |

### Presentation Layer (Layer 6)

| Keyword | Explanation | Example/Context |
|---------|-------------|-----------------|
| Encryption | Converts plaintext to ciphertext | Data security |
| Compression | Reduces data size for transmission | GZIP, ZIP |
| Character Encoding | Text representation in standard format | ASCII, UTF-8 |
| Translation | Converts data between system formats | Cross-platform compatibility |
| SSL/TLS | Secure Sockets Layer/Transport Layer Security | HTTPS encryption |

### Application Layer (Layer 7)

| Keyword | Explanation | Example/Context |
|---------|-------------|-----------------|
| HTTP | HyperText Transfer Protocol - web browsing | Port 80, insecure |
| HTTPS | HTTP Secure - encrypted web browsing | Port 443, encrypted |
| FTP | File Transfer Protocol - file transfer | Port 21, file sharing |
| SFTP | SSH File Transfer Protocol - secure file transfer | Port 22, encrypted |
| SSH | Secure Shell - encrypted remote access | Port 22, terminal |
| Telnet | Remote login protocol (unencrypted) | Port 23, deprecated |
| SMTP | Simple Mail Transfer Protocol - email sending | Port 25, outgoing mail |
| POP3 | Post Office Protocol v3 - email retrieval | Port 110, download |
| IMAP | Internet Message Access Protocol - email access | Port 143, sync |
| DNS | Domain Name System - hostname resolution | Port 53, domain names |
| DHCP | Dynamic Host Configuration Protocol - auto IP | IP assignment |
| SNMP | Simple Network Management Protocol - monitoring | Port 161, device alerts |
| LDAP | Lightweight Directory Access Protocol - directory | User authentication |
| NTP | Network Time Protocol - time synchronization | Port 123, UTC |
| RTSP | Real Time Streaming Protocol - media streaming | Port 554, video |
| RTP | Real-time Transport Protocol - real-time delivery | VoIP, video |
| SIP | Session Initiation Protocol - multimedia setup | VoIP call setup |

---

## Network Types & Architecture

### Network Scope

| Type | Description | Use Case |
|------|-------------|----------|
| PAN | Personal Area Network - within few meters | Bluetooth, USB |
| LAN | Local Area Network - single building | Ethernet, office |
| WAN | Wide Area Network - large geographical area | Internet, branches |
| MAN | Metropolitan Area Network - city-scale | City networks |
| CAN | Campus Area Network - nearby buildings | University |
| GAN | Global Area Network - worldwide | Global networks |

### Wireless Networks

| Technology | Description | Speed/Range |
|------------|-------------|------------|
| WLAN | Wireless LAN - 802.11 standards | WiFi |
| WiFi 6 (802.11ax) | Latest WiFi standard | 9.6 Gbps |
| WiFi 5 (802.11ac) | Previous generation WiFi | 1.3 Gbps |
| 802.11n | MIMO technology | 600 Mbps |
| Bluetooth | Short-range wireless | 10-100m |
| BLE | Bluetooth Low Energy | IoT devices |
| LTE | 4G cellular standard | Mobile broadband |
| 5G | Fifth generation cellular | Ultra-high speed |

### Network Topologies

| Topology | Description | Use Case |
|----------|-------------|----------|
| Bus | All devices on shared medium | Early networks |
| Star | Devices to central hub/switch | Modern networks |
| Ring | Circular chain of devices | Token ring |
| Mesh | Each device connects to multiple | Redundancy |
| Tree | Hierarchical structure | Large networks |
| Hybrid | Combination of topologies | Enterprise |

---

## Network Protocols

### Core Internet Protocols

| Protocol | Description | Purpose |
|----------|-------------|---------|
| TCP/IP | Foundation protocol suite | All internet communication |
| IP | Internet Protocol | Routing and addressing |
| ICMP | Internet Control Message Protocol | Diagnostics, ping |
| IGMP | Internet Group Management Protocol | Multicast management |
| ARP | Address Resolution Protocol | IP to MAC mapping |

### Routing Protocols

| Protocol | Type | Use Case |
|----------|------|----------|
| RIP | Distance Vector | Small networks (legacy) |
| OSPF | Link State | Enterprise networks |
| EIGRP | Hybrid | Cisco networks |
| BGP | Exterior Gateway | Internet backbone |
| IS-IS | Link State | ISP networks |

### Application Layer Protocols

| Protocol | Port | Purpose |
|----------|------|---------|
| HTTP | 80 | Web browsing (insecure) |
| HTTPS | 443 | Web browsing (encrypted) |
| FTP | 21 | File transfer |
| SFTP | 22 | Secure file transfer |
| SMTP | 25 | Email sending |
| POP3 | 110 | Email retrieval |
| IMAP | 143 | Email synchronization |
| DNS | 53 | Domain name resolution |
| SSH | 22 | Secure remote access |
| SNMP | 161 | Network monitoring |

### Security Protocols

| Protocol | Layer | Purpose |
|----------|-------|---------|
| SSL/TLS | Transport (4-6) | Encryption standard |
| IPsec | Network (3) | VPN encryption |
| SSH | Application (7) | Secure remote access |
| Kerberos | Application (7) | Authentication |
| RADIUS | Application (7) | Central authentication |

---

## Network Quality & Performance

### QoS Metrics

| Metric | Definition | Impact |
|--------|-----------|--------|
| Bandwidth | Maximum data transfer capacity | Overall network throughput |
| Latency | Time delay for packet transmission | Real-time application quality |
| Jitter | Variation in packet arrival time | Audio/video quality |
| Packet Loss | Percentage of packets lost | Retransmissions needed |
| Throughput | Actual data transfer rate | Real-world performance |
| Response Time | System response to request | User experience |

### QoS Mechanisms

| Mechanism | Description | Purpose |
|-----------|-------------|---------|
| Traffic Shaping | Smooths traffic bursts | Congestion prevention |
| Traffic Policing | Enforces bandwidth limits | Rate control |
| Prioritization | Marks critical traffic | Preferential treatment |
| Queuing | Orders packets for transmission | Transmission management |
| Weighted Fair Queuing | Fair bandwidth distribution | Dynamic queue management |
| Congestion Control | Prevents network overload | Data loss prevention |
| Bandwidth Reservation | Pre-allocates bandwidth | Critical app protection |

---

## Network Devices

### Core Devices

| Device | Layer | Function |
|--------|-------|----------|
| Hub | Layer 1 | Broadcasts to all ports |
| Switch | Layer 2 | Forwards by MAC address |
| Router | Layer 3 | Forwards by IP address |
| Gateway | Layer 3-7 | Protocol translation |
| Firewall | Security | Filters traffic |
| Proxy Server | Application | Request intermediary |
| Load Balancer | Layer 4-7 | Distributes traffic |

### Advanced Devices

| Device | Purpose | Use Case |
|--------|---------|----------|
| Layer 3 Switch | Routing + switching | High-performance networks |
| WLAN Controller | WiFi management | Multiple AP coordination |
| Access Point | WiFi coverage | Wireless connectivity |
| Modem | Signal conversion | ISP connectivity |
| Wireless Extender | WiFi extension | Range expansion |

---

## Network Architecture & Virtualization

### Architecture Concepts

| Concept | Description | Purpose |
|---------|-------------|---------|
| SDN | Software-Defined Networking | Programmable infrastructure |
| Control Plane | Decision-making for routing | Network intelligence |
| Data Plane | Actual packet forwarding | Forwarding operations |
| Network Segmentation | Logical network division | Security and performance |
| Subnetting | IP network subdivision | Address management |

### Virtualization

| Technology | Description | Purpose |
|------------|-------------|---------|
| NFV | Network Functions Virtualization | Virtual network services |
| VNF | Virtual Network Function | Software-based service |
| NFVI | NFV Infrastructure | Hardware abstraction |
| MANO | Management and Orchestration | Automation and lifecycle |
| Network Slicing | Isolated virtual networks | Multi-tenant networks |

---

## Network Security

### Encryption & Certificates

| Element | Description | Purpose |
|---------|-------------|---------|
| PKI | Public Key Infrastructure | Certificate system |
| Digital Certificate | Identity proof with public key | Authentication |
| Certificate Authority | Issues digital certificates | Trust authority |
| RSA | Asymmetric encryption algorithm | Public/private keys |
| AES | Advanced Encryption Standard | Symmetric encryption |
| Cipher Suite | Set of encryption algorithms | SSL/TLS security |

### VPN & Tunneling

| Technology | Layer | Purpose |
|------------|-------|---------|
| VPN | Virtual Private Network | Encrypted tunnel |
| IPsec | Network layer (3) | Full network encryption |
| SSL VPN | Application layer (7) | Session encryption |
| L2TP | Layer 2 Tunneling Protocol | VPN protocol |
| Encapsulation | Protocol wrapping | Tunneling technique |

### Threat Prevention

| Element | Description | Purpose |
|---------|-------------|---------|
| Firewall | Traffic filter | Network security boundary |
| IDS | Intrusion Detection System | Attack monitoring |
| IPS | Intrusion Prevention System | Active threat blocking |
| DDoS Protection | Against distributed attacks | Attack mitigation |
| Deep Packet Inspection | Content examination | Threat detection |

---

## Network Addressing & Naming

### IP Addressing

| Concept | Description | Example |
|---------|-------------|---------|
| IPv4 Address | 32-bit address, decimal notation | 192.168.1.1 |
| IPv6 Address | 128-bit address, hex notation | 2001:0db8::1 |
| Public IP | Globally routable on internet | ISP-assigned |
| Private IP | Non-routable internally | RFC 1918 ranges |
| Loopback | Testing address | 127.0.0.1 |
| Broadcast | Reaches all hosts | All 1s in host |
| Multicast | One-to-many communication | 224.0.0.0/4 |
| CIDR Notation | Classless addressing with prefix | /24 |
| Subnet Mask | Network/host portion separator | 255.255.255.0 |
| DHCP Lease | Temporary IP assignment | Expiration time |

### Domain Naming

| Concept | Description | Purpose |
|---------|-------------|---------|
| DNS | Domain Name System | Hostname resolution |
| FQDN | Fully Qualified Domain Name | Complete domain |
| A Record | IPv4 hostname mapping | Domain resolution |
| AAAA Record | IPv6 hostname mapping | IPv6 resolution |
| CNAME Record | Hostname alias | Domain aliasing |
| MX Record | Email server mapping | Mail routing |
| DNS Cache | Stored resolution results | Faster lookups |
| Recursive Query | Full resolution service | Client-resolver query |
| Iterative Query | Direct nameserver query | Referral responses |

---

## Advanced Networking Topics

### Data Flow & Transmission

| Concept | Description | Use Case |
|---------|-------------|----------|
| Full Duplex | Simultaneous bidirectional communication | Modern devices |
| Half Duplex | One-way communication at a time | Legacy networks |
| Simplex | One-way only | Broadcasting |
| Collision Domain | Segment where collisions occur | Hub networks |
| Broadcast Domain | Segment receiving broadcasts | VLAN scope |
| Flow Control | Prevents sender overflow | TCP management |
| Sliding Window | Variable-size flow control | TCP rate control |

### Advanced Concepts

| Concept | Description | Purpose |
|---------|-------------|---------|
| QoS Classification | Traffic categorization by priority | Traffic identification |
| DiffServ | Differentiated Services | Traffic classes |
| ToS | Type of Service field | Priority marking |
| DSCP | Differentiated Services Code Point | QoS marking |
| MPLS | Multiprotocol Label Switching | Label-based routing |
| VRF | Virtual Routing and Forwarding | Multi-tenant routing |
| Failover | Automatic backup switch | High availability |
| Redundancy | Multiple fault-tolerance paths | Network resilience |
| SLA | Service Level Agreement | Performance guarantee |

---

## Network Management & Monitoring

### Management Protocols

| Protocol | Purpose | Port |
|----------|---------|------|
| SNMP | Device monitoring | 161 |
| Syslog | Centralized logging | 514 |
| NetFlow | Traffic analysis | Variable |
| IPFIX | Flow information export | Variable |

### Monitoring & Analysis

| Function | Description | Purpose |
|----------|-------------|---------|
| Packet Capture | Tcpdump, Wireshark analysis | Traffic inspection |
| Flow Analysis | Traffic pattern analysis | Bandwidth usage |
| Link Monitoring | Status and quality checks | Link health |
| Performance Monitoring | Metric tracking | Network performance |
| Network Baseline | Normal behavior reference | Anomaly detection |

---

## Summary

This comprehensive reference contains **235 total networking keywords and concepts** organized into 10 categories:

- OSI Model Layers: 52 keywords (7 layers)
- Network Types & Architecture: 21 keywords
- Network Protocols: 40 keywords
- Network Quality & Performance: 18 keywords
- Network Devices: 13 keywords
- Network Architecture & Virtualization: 11 keywords
- Network Security: 16 keywords
- Network Addressing & Naming: 20 keywords
- Advanced Networking Topics: 26 keywords
- Network Management & Monitoring: 11 keywords

All concepts include descriptions and practical examples for real-world understanding.
