# Networking Complete Reference - Full Markdown with Tables and Examples

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

## OSI MODEL LAYERS

### Physical Layer (Layer 1) - 5 Keywords

| Keyword | Explanation | Example/Context |
|---------|-------------|-----------------|
| Bits | Smallest unit of data transmission - 0s and 1s | Transmitted as electrical signals over cables |
| Physical Media | Tangible transmission mediums - cables fiber optics wireless | Ethernet cables fiber optic radio waves |
| Hub | Device that broadcasts data to all connected ports without filtering | Basic network device |
| Repeater | Device that regenerates weak signals to extend transmission distance | Signal amplification over long distances |
| Modem | Modulator/demodulator - converts digital signals to analog and vice versa | Dial-up connections DSL cable internet |

### Data Link Layer (Layer 2) - 7 Keywords

| Keyword | Explanation | Example/Context |
|---------|-------------|-----------------|
| MAC Address | Media Access Control address - unique 48-bit identifier for network devices | Example: 00:1A:2B:3C:4D:5E |
| Switch | Device that forwards data frames based on MAC addresses to specific ports | LAN connectivity and segmentation |
| Frame | Data unit at Layer 2 containing MAC addresses VLAN info and error checking | Encapsulation of Layer 2 data |
| Ethernet | Protocol for LAN communication - defines frame format and access methods | Most common LAN technology |
| PPP | Point-to-Point Protocol - direct communication between two nodes | Serial connections dialup |
| ARP | Address Resolution Protocol - maps IP addresses to MAC addresses | Resolves IP-to-MAC mappings |
| VLAN | Virtual Local Area Network - logically segments a physical LAN | Network segmentation without physical separation |

### Network Layer (Layer 3) - 12 Keywords

| Keyword | Explanation | Example/Context |
|---------|-------------|-----------------|
| IP Address | Internet Protocol address - unique logical identifier (IPv4 32-bit IPv6 128-bit) | Example: 192.168.1.1 |
| IPv4 | Internet Protocol version 4 - 32-bit addressing scheme | Most widely used 4.3 billion addresses |
| IPv6 | Internet Protocol version 6 - 128-bit addressing scheme | Future protocol 340 undecillion addresses |
| Packet | Data unit at Layer 3 containing source/dest IP addresses and routing info | Basic routing unit across networks |
| Router | Device that forwards packets between networks based on IP addresses | Inter-network communication |
| Routing | Process of determining the best path for packet delivery | Determines data path through networks |
| ICMP | Internet Control Message Protocol - diagnostic and error reporting | ping traceroute commands |
| Subnet | Logical subdivision of an IP network | Segmentation of network address space |
| Subnet Mask | Determines which portion of IP is network vs. host | Example: 255.255.255.0 (/24) |
| Default Gateway | Router through which devices send traffic to other networks | Exit point from local network |
| NAT | Network Address Translation - maps internal IPs to external IPs | Translates between private and public IPs |
| IGMP | Internet Group Management Protocol - manages multicast group membership | Multicast traffic management |

### Transport Layer (Layer 4) - 9 Keywords

| Keyword | Explanation | Example/Context |
|---------|-------------|-----------------|
| TCP | Transmission Control Protocol - reliable connection-oriented delivery | Email web FTP - ordered delivery |
| UDP | User Datagram Protocol - unreliable connectionless delivery | VoIP streaming - speed over reliability |
| Port | Virtual endpoint for network communication (0-65535) | Identifies specific service/application |
| Socket | Combination of IP address and port number | Endpoint for network communication |
| Segment | Data unit at Layer 4 for TCP | TCP packet with headers and data |
| Datagram | Data unit at Layer 4 for UDP | UDP packet with headers and data |
| Flow Control | Mechanism to prevent receiver from being overwhelmed | TCP window management |
| Error Detection | Checks for transmission errors in data | Checksum calculation and verification |
| Connection Establishment | TCP three-way handshake - SYN SYN-ACK ACK | Initiates reliable connection |

### Session Layer (Layer 5) - 4 Keywords

| Keyword | Explanation | Example/Context |
|---------|-------------|-----------------|
| Session | Logical connection between two applications | Maintains communication dialogue |
| Authentication | Verification of user identity before communication | User credentials validation |
| Authorization | Determines what authenticated user can access | Permission enforcement |
| Session Management | Establishment maintenance and termination of sessions | Dialog control |

### Presentation Layer (Layer 6) - 4 Keywords

| Keyword | Explanation | Example/Context |
|---------|-------------|-----------------|
| Encryption | Converts plaintext to ciphertext for security | Protects data confidentiality |
| Compression | Reduces data size for efficient transmission | GZIP ZIP compression |
| Character Encoding | Represents text in standardized format | ASCII UTF-8 Unicode |
| SSL/TLS | Secure Sockets Layer/Transport Layer Security - encryption protocol | HTTPS encryption |

### Application Layer (Layer 7) - 18 Keywords

| Keyword | Port(s) | Purpose | Security |
|---------|---------|---------|----------|
| HTTP | 80 | Web browsing protocol (insecure) | No |
| HTTPS | 443 | Secure web pages | TLS/SSL |
| FTP | 21 (control), 20 (data) | File transfer protocol | No |
| SFTP | 22 | SSH File Transfer Protocol | SSH encrypted |
| SSH | 22 | Secure remote login | Encrypted |
| Telnet | 23 | Remote login protocol (deprecated) | No - Insecure |
| SMTP | 25, 587 | Simple Mail Transfer Protocol - email sending | Optional |
| POP3 | 110 | Post Office Protocol v3 - email retrieval | Optional |
| IMAP | 143 | Internet Message Access Protocol - email access | Optional |
| DNS | 53 | Domain Name System - hostname to IP translation | No |
| DHCP | 67, 68 | Dynamic Host Configuration Protocol - automatic IP assignment | No |
| SNMP | 161 | Simple Network Management Protocol - device monitoring | No |
| LDAP | 389 | Lightweight Directory Access Protocol - directory services | No |
| NTP | 123 | Network Time Protocol - time synchronization | No |
| RTSP | 554 | Real Time Streaming Protocol - media streaming control | Optional |
| RTP | Varies | Real-time Transport Protocol - real-time data delivery | No |
| SIP | 5060 | Session Initiation Protocol - multimedia session setup | Optional |

**Total Layer 7 Keywords: 18**

---

## NETWORK TYPES & ARCHITECTURE

### Network Geographic Scope - 6 Keywords

| Keyword | Coverage | Use Case |
|---------|----------|----------|
| PAN | Personal Area Network - very small within few meters | Bluetooth USB connections |
| LAN | Local Area Network - covers single building/campus | Ethernet typical office network |
| WAN | Wide Area Network - covers large geographical area | Internet branch office connections |
| MAN | Metropolitan Area Network - city-scale network | Between LANs in same city |
| CAN | Campus Area Network - multiple buildings in nearby area | University/corporate campus |
| GAN | Global Area Network - worldwide scale | Global corporate networks |

### Wireless Technologies - 14 Keywords

| Standard | Frequency | Speed | Technology | Year |
|----------|-----------|-------|------------|------|
| 802.11a | 5 GHz | 54 Mbps | OFDM | 1999 |
| 802.11b | 2.4 GHz | 11 Mbps | DSSS | 1999 |
| 802.11g | 2.4 GHz | 54 Mbps | OFDM | 2003 |
| 802.11n (WiFi 4) | 2.4/5 GHz | 600 Mbps | MIMO | 2009 |
| 802.11ac (WiFi 5) | 5 GHz | 1.3 Gbps | Wide Channel | 2013 |
| 802.11ax (WiFi 6) | 2.4/5/6 GHz | 9.6 Gbps | OFDMA | 2019 |
| Bluetooth | 2.4 GHz | 1-3 Mbps | FHSS | 1998 |
| BLE | 2.4 GHz | 1-2 Mbps | Low Power | 2010 |
| WLAN | Multiple | Variable | 802.11 | N/A |
| WMAN | 2.3-2.7 GHz | 40-70 Mbps | WiMAX | N/A |
| WPAN | 2.4 GHz | Variable | Bluetooth/ZigBee | N/A |
| WiFi | 2.4/5 GHz | Variable | 802.11 standards | N/A |
| LTE (4G) | 700 MHz-2.6 GHz | 100 Mbps | Cellular | 2009 |
| 5G | Sub-1 GHz - mmWave | 1-10 Gbps | Cellular | 2019 |

### Network Topologies - 6 Keywords

| Topology | Structure | Advantages | Disadvantages |
|----------|-----------|-----------|----------------|
| Bus | Single shared medium | Simple inexpensive | Limited bandwidth collision domain |
| Star | Central hub/switch | Easy troubleshooting scalable | Single point of failure |
| Ring | Circular chain | Fair access predictable | Single break affects all |
| Mesh | Multi-path connections | Redundancy fault tolerant | Complex expensive |
| Tree | Hierarchical structure | Scalable organized | Multiple failure points |
| Hybrid | Combination of topologies | Flexible optimized | Complex management |

**Network Architecture Total: 26 Keywords**

---

## NETWORK PROTOCOLS

### Core Internet Protocols - 5 Keywords

| Protocol | Layer | Purpose | Example |
|----------|-------|---------|---------|
| TCP/IP | 4 & 3 | Foundation protocol suite | All internet communication |
| IP | Layer 3 | Routing and addressing | Moves packets between networks |
| ICMP | Layer 3 | Diagnostics and errors | ping traceroute utilities |
| IGMP | Layer 3 | Multicast management | Group communication |
| ARP | Layer 2 | IP to MAC resolution | Local network addressing |

### Routing Protocols - 5 Keywords

| Protocol | Type | Metric | Best For | Distance Limit |
|----------|------|--------|----------|-----------------|
| RIP | Distance-Vector | Hop count | Small networks | 15 hops max |
| OSPF | Link-State | Cost | Enterprise networks | Unlimited |
| EIGRP | Hybrid | Composite | Cisco networks | Unlimited |
| BGP | Path-Vector | AS path | Internet backbone | Unlimited |
| IS-IS | Link-State | Metric | ISP networks | Unlimited |

### Application Layer Protocols - 15 Keywords

| Protocol | Layer 7 | Port(s) | Purpose | Connection Type |
|----------|---------|---------|---------|-----------------|
| HTTP | Web | 80 | Web browsing | Stateless |
| HTTPS | Web | 443 | Secure web | Encrypted |
| FTP | File Transfer | 21/20 | File transfers | Binary |
| SFTP | File Transfer | 22 | Secure transfer | SSH-based |
| SMTP | Email | 25/587 | Email sending | SMTP server |
| POP3 | Email | 110 | Email download | Download model |
| IMAP | Email | 143 | Email sync | Sync model |
| DNS | Directory | 53 | Name resolution | Query/response |
| DHCP | Configuration | 67/68 | IP assignment | Broadcast |
| SSH | Remote | 22 | Secure shell | Encrypted |
| Telnet | Remote | 23 | Remote login | Unencrypted (deprecated) |
| SNMP | Management | 161 | Device monitoring | Agent/manager |
| LDAP | Directory | 389 | Directory services | LDAP queries |
| NTP | Timing | 123 | Time sync | UDP single packet |
| RTSP | Streaming | 554 | Media control | Media streaming |

### Security Protocols - 8 Keywords

| Protocol | Type | Use | Status |
|----------|------|-----|--------|
| SSL | Encryption | Legacy HTTPS | Deprecated |
| TLS | Encryption | Modern HTTPS SMTP-TLS | Current |
| IPsec | Network Layer VPN | Site-to-site VPN | Active |
| PPTP | VPN | Remote access VPN | Weak - Legacy |
| L2TP | VPN | Remote access VPN | Better than PPTP |
| VPN | Tunnel | Secure remote access | Active |
| Kerberos | Authentication | Enterprise auth | Active |
| RADIUS | Authentication | Central auth | Active |

### IoT & Other Protocols - 5 Keywords

| Protocol | Purpose | Use Case | Power |
|----------|---------|----------|-------|
| MQTT | IoT Messaging | Lightweight publish-subscribe | Low |
| CoAP | IoT Protocol | Extreme constraints | Very Low |
| AMQP | Message Broker | Enterprise messaging | N/A |
| WebSocket | Web Communication | Bidirectional real-time web | N/A |
| QUIC | Transport | HTTP/3 lower latency | N/A |

**Network Protocols Total: 38 Keywords**

---

## NETWORK QUALITY & PERFORMANCE

### QoS Metrics - 6 Keywords

| Metric | Measurement | Target | Impact |
|--------|-------------|--------|--------|
| Bandwidth | Mbps/Gbps | High | Max capacity |
| Latency | Milliseconds (ms) | Low (<50ms) | User experience |
| Jitter | ms variation | Low | Audio/video quality |
| Packet Loss | Percentage (%) | <0.1% | Retransmissions |
| Throughput | Mbps actual | Match bandwidth | Real performance |
| Response Time | Seconds | Low | System reaction |

### QoS Mechanisms - 8 Keywords

| Mechanism | Function | Benefit |
|-----------|----------|---------|
| Traffic Shaping | Smooth burst traffic | Prevents congestion |
| Traffic Policing | Enforce bandwidth limits | Limits abuse |
| Prioritization | Mark critical traffic | Better service |
| Queuing | Order transmission | Fair distribution |
| Weighted Fair Queuing | Dynamic allocation | Optimized bandwidth |
| Congestion Control | Prevent overload | Stability |
| Congestion Avoidance | Proactive prevention | Early response |
| Bandwidth Reservation | Pre-allocate bandwidth | Guaranteed service |

**QoS Total: 14 Keywords**

---

## NETWORK DEVICES

### Core Devices - 7 Keywords

| Device | Layer | Function | Full Duplex |
|--------|-------|----------|------------|
| Hub | Layer 1 | Broadcasts all traffic | No |
| Switch | Layer 2 | Forwards based on MAC | Yes |
| Router | Layer 3 | Forwards based on IP | Yes |
| Gateway | Layer 3-7 | Protocol translation | Yes |
| Firewall | Security | Traffic filtering | N/A |
| Proxy Server | Application | Request intermediary | Yes |
| Load Balancer | Application | Traffic distribution | Yes |

### Advanced Devices - 6 Keywords

| Device | Type | Purpose | Key Feature |
|--------|------|---------|------------|
| Layer 3 Switch | Hybrid | Switch + Router | High-performance |
| WLAN Controller | Wireless | WiFi management | Central control |
| Access Point | Wireless | WiFi coverage | WLAN connectivity |
| Modem | Converter | Signal conversion | ISP connection |
| Bridge | Layer 2 | Segment connection | MAC forwarding |
| Wireless Extender | Wireless | Coverage extension | Range boost |

**Network Devices Total: 13 Keywords**

---

## NETWORK ARCHITECTURE & VIRTUALIZATION

### Architecture Concepts - 6 Keywords

| Concept | Definition | Benefit |
|---------|-----------|---------|
| SDN | Software-Defined Networking | Programmable control |
| Control Plane | Decision-making | Network intelligence |
| Data Plane | Forwarding | Actual transmission |
| Management Plane | Configuration/monitoring | System oversight |
| Network Segmentation | Logical division | Security/performance |
| Subnetting | IP division | Address management |

### Virtualization - 6 Keywords

| Technology | Function | Use |
|------------|----------|-----|
| NFV | Virtual network functions | Routing/firewall as software |
| VNF | Software-based service | Runs on servers |
| NFVI | Infrastructure | Hardware abstraction |
| MANO | Management layer | Automation |
| Virtual Switch | Software switching | vSwitch in hypervisors |
| Network Slicing | Virtual networks | Multi-tenant isolation |

**Architecture & Virtualization Total: 12 Keywords**

---

## NETWORK SECURITY

### Encryption & Certificates - 6 Keywords

| Component | Purpose | Use |
|-----------|---------|-----|
| PKI | Digital certificate system | CA validation |
| Digital Certificate | Identity proof | SSL/TLS |
| Certificate Authority | Issues certificates | Trust authority |
| RSA | Asymmetric encryption | Public/private key |
| AES | Symmetric encryption | 256-bit standard |
| Cipher Suite | Algorithm set | SSL/TLS selection |

### VPN & Tunneling - 6 Keywords

| Type | Layer | Purpose | Use |
|------|-------|---------|-----|
| VPN | Multiple | Encrypted tunnel | Secure access |
| IPsec | Network | Encryption protocol | Site-to-site VPN |
| SSL VPN | Application | Remote access | Clientless |
| Tunnel Mode | IPsec | Full encryption | Public networks |
| Transport Mode | IPsec | Payload encryption | Trusted networks |
| Encapsulation | Protocol wrapping | Tunneling | Protocol translation |

### Threat Prevention - 4 Keywords

| Tool | Function | Action |
|------|----------|--------|
| Firewall | Traffic filtering | Block/allow |
| IDS | Intrusion detection | Alert |
| IPS | Intrusion prevention | Block |
| DDoS Protection | Attack mitigation | Defend |

**Network Security Total: 16 Keywords**

---

## NETWORK ADDRESSING & NAMING

### IP Addressing - 11 Keywords

| Concept | Description | Example |
|---------|-------------|---------|
| IPv4 Address | 32-bit dotted decimal | 192.168.1.1 |
| IPv6 Address | 128-bit hexadecimal | 2001:0db8::1 |
| Public IP | Internet-routable unique | ISP-assigned |
| Private IP | Non-routable internal | RFC 1918 ranges |
| Loopback Address | Local testing | 127.0.0.1 |
| Broadcast Address | Network-wide | All 1s in host portion |
| Multicast Address | Group targeting | 224.0.0.0/4 |
| CIDR Notation | Flexible prefix | /24 |
| Subnet Mask | Network boundary | 255.255.255.0 |
| Default Gateway | First-hop router | Network exit |
| DHCP Lease | Temporary assignment | IP + expiration |

### Domain Naming - 9 Keywords

| Component | Purpose | Example |
|-----------|---------|---------|
| DNS | Name resolution | Domain lookup |
| FQDN | Complete domain name | www.example.com |
| DNS Record | Name-to-value mapping | Database entry |
| A Record | IPv4 mapping | Domain resolution |
| AAAA Record | IPv6 mapping | IPv6 resolution |
| CNAME Record | Alias | Domain alias |
| MX Record | Email server | Mail routing |
| NS Record | Nameserver | DNS delegation |
| TXT Record | Text/policy | SPF DKIM DMARC |

**Addressing & Naming Total: 20 Keywords**

---

## ADVANCED NETWORKING TOPICS

### Data Flow & Transmission - 7 Keywords

| Concept | Direction | Speed | Use |
|---------|-----------|-------|-----|
| Full Duplex | Both ways simultaneous | Fastest | Modern devices |
| Half Duplex | Alternate one-way | Medium | Some wireless |
| Simplex | One-way only | N/A | Broadcasting |
| Collision Domain | Segment with collisions | Limited | Hub-based |
| Broadcast Domain | Segment with broadcasts | Local | VLAN-based |
| Flow Control | Receiver regulation | Optimized | TCP window |
| Sliding Window | Variable window | Dynamic | TCP rate control |

### Advanced Concepts - 10 Keywords

| Concept | Function | Benefit |
|---------|----------|---------|
| QoS Classification | Traffic categorization | Prioritization |
| DiffServ | Traffic classes | Layer 3 marking |
| ToS | Priority marking | IPv4 header |
| DSCP | QoS marking | 6-bit encoding |
| MPLS | Label-based routing | Performance |
| VRF | Virtual routing table | Multi-tenant |
| Failover | Automatic backup | High availability |
| Redundancy | Fault tolerance | Resilience |
| SLA | Performance guarantee | Availability |
| Throughput Optimization | Maximize data rate | Performance |

**Advanced Topics Total: 17 Keywords**

---

## NETWORK MANAGEMENT & MONITORING

### Management Protocols - 6 Keywords

| Protocol | Port | Function |
|----------|------|----------|
| SNMP | 161 | Device monitoring |
| SNMP Trap | 162 | Event notification |
| MIB | N/A | Database of metrics |
| Syslog | 514 | Centralized logging |
| NetFlow | 2055 | Flow data export |
| IPFIX | 4739 | Extended flow reporting |

### Monitoring & Analysis - 5 Keywords

| Tool/Concept | Purpose | Use |
|-------------|---------|-----|
| Packet Capture | Traffic inspection | Network diagnosis |
| Flow Analysis | Pattern analysis | Bandwidth tracking |
| Link Monitoring | Health checking | Status verification |
| Performance Monitoring | Metric tracking | Trend analysis |
| Network Analytics | Prediction | Capacity planning |

**Management & Monitoring Total: 11 Keywords**

---

## QUICK REFERENCE STATISTICS

### By Category Count

```
OSI Model Layers: 57 keywords
├─ Physical Layer (1): 5
├─ Data Link Layer (2): 7
├─ Network Layer (3): 12
├─ Transport Layer (4): 9
├─ Session Layer (5): 4
├─ Presentation Layer (6): 4
└─ Application Layer (7): 18

Network Types & Architecture: 26 keywords
├─ Geographic Scope: 6
├─ Wireless Technologies: 14
└─ Topologies: 6

Network Protocols: 38 keywords
├─ Core Internet: 5
├─ Routing: 5
├─ Application Layer: 15
├─ Security: 8
└─ IoT & Other: 5

Network Quality & Performance: 14 keywords
├─ QoS Metrics: 6
└─ QoS Mechanisms: 8

Network Devices: 13 keywords
├─ Core Devices: 7
└─ Advanced Devices: 6

Network Architecture & Virtualization: 12 keywords
├─ Architecture: 6
└─ Virtualization: 6

Network Security: 16 keywords
├─ Encryption: 6
├─ VPN & Tunneling: 6
└─ Threat Prevention: 4

Network Addressing & Naming: 20 keywords
├─ IP Addressing: 11
└─ Domain Naming: 9

Advanced Networking Topics: 17 keywords
├─ Data Flow: 7
└─ Advanced Concepts: 10

Network Management & Monitoring: 11 keywords
├─ Management: 6
└─ Monitoring: 5

TOTAL KEYWORDS: 235
```

### Protocol Port Reference

| Service | Port(s) | Protocol | Layer |
|---------|---------|----------|-------|
| HTTP | 80 | TCP | 7 |
| HTTPS | 443 | TCP | 7 |
| FTP Control | 21 | TCP | 7 |
| FTP Data | 20 | TCP | 7 |
| SSH | 22 | TCP | 7 |
| Telnet | 23 | TCP | 7 |
| SMTP | 25 | TCP | 7 |
| SMTP Submission | 587 | TCP | 7 |
| POP3 | 110 | TCP | 7 |
| IMAP | 143 | TCP | 7 |
| DNS | 53 | TCP/UDP | 7 |
| DHCP | 67/68 | UDP | 7 |
| SNMP | 161 | UDP | 7 |
| LDAP | 389 | TCP | 7 |
| NTP | 123 | UDP | 7 |
| RTSP | 554 | TCP | 7 |
| BGP | 179 | TCP | 3 |
| NetFlow | 2055 | UDP | 7 |
| IPFIX | 4739 | TCP/UDP | 7 |

### Technology Comparison Matrix

| Aspect | TCP | UDP | HTTP | HTTPS | FTP | SFTP |
|--------|-----|-----|------|-------|-----|------|
| Reliable | Yes | No | Yes | Yes | Yes | Yes |
| Connection | Oriented | Connectionless | Stateless | Stateless | Stateful | Stateful |
| Encryption | No | No | No | Yes | No | Yes |
| Speed | Slower | Faster | Medium | Medium | Medium | Medium |
| Port | Varies | Varies | 80 | 443 | 21/20 | 22 |
| Use | General | Real-time | Web | Secure Web | Legacy | Secure |

---

## CONCLUSION

This comprehensive networking reference covers all 235 essential networking keywords organized by:

1. **OSI Model Layers** - All 7 layers with detailed protocols
2. **Network Architecture** - Types and topologies
3. **Protocols** - Internet, routing, application, security
4. **Performance** - QoS metrics and mechanisms
5. **Devices** - Hardware and virtualization
6. **Security** - Encryption and protection
7. **Addressing** - IP and DNS systems
8. **Advanced Topics** - Optimization and routing
9. **Management** - Monitoring and administration

Perfect for network engineers, system administrators, and IT professionals!
