# Networking Reference - Table-Formatted with Statistics & Quick Reference

## PART 1: OSI LAYERS QUICK REFERENCE

### Layer Overview Table

| Layer # | Layer Name | Function | Data Unit | Devices | Key Protocols |
|---------|-----------|----------|-----------|---------|---------------|
| 7 | Application | User interface and services | Message | N/A | HTTP HTTPS FTP SSH SMTP POP3 IMAP DNS DHCP SNMP LDAP NTP RTSP RTP SIP |
| 6 | Presentation | Data formatting and encryption | Data | N/A | SSL/TLS Encryption Compression |
| 5 | Session | Session management | Data | N/A | Session Auth |
| 4 | Transport | End-to-end delivery | Segment/Datagram | N/A | TCP UDP QUIC |
| 3 | Network | Routing and logical addressing | Packet | Router | IP ICMP IGMP RIP OSPF BGP |
| 2 | Data Link | Frame delivery and MAC addressing | Frame | Switch Hub Bridge | Ethernet ARP PPP |
| 1 | Physical | Bit transmission and signals | Bit | Repeater Hub Modem | Cables Fiber |

### Protocol Summary by Layer

```
LAYER 7 (Application) - 18 Keywords
─────────────────────────────────
HTTP (80) - Web browsing
HTTPS (443) - Secure web
FTP (21) - File transfer
SFTP (22) - Secure file transfer
SSH (22) - Remote shell
Telnet (23) - Remote login (deprecated)
SMTP (25/587) - Email sending
POP3 (110) - Email download
IMAP (143) - Email sync
DNS (53) - Name resolution
DHCP (67/68) - IP assignment
SNMP (161) - Monitoring
LDAP (389) - Directory services
NTP (123) - Time sync
RTSP (554) - Media streaming
RTP - Real-time transport
SIP - VoIP setup

LAYER 4 (Transport) - 9 Keywords
────────────────────────────────
TCP - Reliable ordered
UDP - Fast unreliable
Port - Endpoint identifier
Socket - IP + Port
Segment - TCP data unit
Datagram - UDP data unit
Flow Control - Rate limiting
Error Detection - Checksum
Connection Establishment - Handshake

LAYER 3 (Network) - 12 Keywords
───────────────────────────────
IP - Internet Protocol
IPv4 - 32-bit addressing
IPv6 - 128-bit addressing
Packet - Routing data unit
Router - IP forwarding
Routing - Path selection
ICMP - Diagnostics
IGMP - Multicast management
Subnet - Network division
Subnet Mask - Network boundary
Default Gateway - Router exit
NAT - IP translation

LAYER 2 (Data Link) - 7 Keywords
────────────────────────────────
MAC Address - Physical identifier
Switch - Frame forwarding
Frame - Data link unit
Ethernet - LAN protocol
PPP - Point-to-point
ARP - IP-to-MAC mapping
VLAN - Virtual segmentation

LAYER 1 (Physical) - 5 Keywords
───────────────────────────────
Bits - Binary units
Physical Media - Cables/wireless
Hub - Broadcast device
Repeater - Signal amplifier
Modem - Signal converter
```

---

## PART 2: PROTOCOL PORT QUICK REFERENCE

### Commonly Used Ports

| Service | Port(s) | Protocol | Encryption | Layer | Use |
|---------|---------|----------|-----------|-------|-----|
| HTTP | 80 | TCP | No | 7 | Web browsing |
| HTTPS | 443 | TCP | TLS | 7 | Secure web |
| SSH | 22 | TCP | SSH | 7 | Remote shell |
| SFTP | 22 | TCP | SSH | 7 | Secure file transfer |
| FTP Control | 21 | TCP | No | 7 | File transfer |
| FTP Data | 20 | TCP | No | 7 | File data |
| Telnet | 23 | TCP | No | 7 | Remote login (deprecated) |
| SMTP | 25 | TCP | No/TLS | 7 | Email send |
| SMTP Submit | 587 | TCP | STARTTLS | 7 | Email submission |
| POP3 | 110 | TCP | No/SSL | 7 | Email download |
| IMAP | 143 | TCP | No/STARTTLS | 7 | Email sync |
| DNS | 53 | TCP/UDP | No | 7 | Name resolution |
| DHCP | 67/68 | UDP | No | 7 | IP assignment |
| NTP | 123 | UDP | No | 7 | Time sync |
| SNMP | 161 | UDP | No | 7 | Monitoring |
| LDAP | 389 | TCP | No | 7 | Directory |
| RTSP | 554 | TCP | Optional | 7 | Media control |
| BGP | 179 | TCP | No | 3 | Internet routing |
| HTTPS Alt | 8443 | TCP | TLS | 7 | Alternative HTTPS |
| SOCKS Proxy | 1080 | TCP | No | 5 | Proxy |
| MQTT | 1883 | TCP | Optional | 7 | IoT messaging |
| MySQL | 3306 | TCP | Optional | 7 | Database |
| RDP | 3389 | TCP | No | 7 | Remote desktop |
| DNS Alt | 5353 | UDP | No | 7 | mDNS |
| SIP | 5060/5061 | TCP/UDP | Optional | 7 | VoIP |

---

## PART 3: NETWORK TYPES & TECHNOLOGIES MATRIX

### Network Geographic Scope

| Type | Coverage | Speed | Use Case | Cost | Latency |
|------|----------|-------|----------|------|---------|
| PAN | <10m | 1-10 Mbps | Personal devices | $ | <5ms |
| LAN | <500m | 10-1000 Mbps | Office/building | $$ | <1ms |
| MAN | <40km | 1-100 Mbps | City | $$$ | <10ms |
| CAN | <5km | 10-1000 Mbps | Campus | $$ | <5ms |
| WAN | Unlimited | 1-100 Mbps | Internet | $$$$ | 10-100ms |
| GAN | Global | Variable | Enterprise | $$$$$ | 50-500ms |

### Wireless Standards Comparison

| Standard | Frequency | Speed | Tech | Year | Range | Power |
|----------|-----------|-------|------|------|-------|-------|
| 802.11a | 5 GHz | 54 Mbps | OFDM | 1999 | 30m | Medium |
| 802.11b | 2.4 GHz | 11 Mbps | DSSS | 1999 | 100m | Medium |
| 802.11g | 2.4 GHz | 54 Mbps | OFDM | 2003 | 100m | Medium |
| 802.11n (WiFi 4) | 2.4/5 GHz | 600 Mbps | MIMO | 2009 | 100-200m | Medium |
| 802.11ac (WiFi 5) | 5 GHz | 1.3 Gbps | MU-MIMO | 2013 | 50-100m | Medium |
| 802.11ax (WiFi 6) | 2.4/5/6 GHz | 9.6 Gbps | OFDMA | 2019 | 30-50m | Medium |
| Bluetooth Classic | 2.4 GHz | 1-3 Mbps | FHSS | 1998 | 10-100m | Medium |
| BLE | 2.4 GHz | 1-2 Mbps | FHSS | 2010 | 50m | Low |
| LTE (4G) | 700-2600 MHz | 100 Mbps | Cellular | 2009 | Several km | Medium |
| 5G | <1 GHz to mmWave | 1-10 Gbps | Cellular | 2019 | 1-4 km | Medium |

---

## PART 4: PERFORMANCE METRICS REFERENCE

### QoS Metrics Target Values

| Metric | Acceptable | Good | Excellent | Critical |
|--------|-----------|------|-----------|----------|
| Bandwidth | >1 Mbps | >10 Mbps | >100 Mbps | >1000 Mbps |
| Latency | <100ms | <50ms | <10ms | <5ms |
| Jitter | <50ms | <20ms | <5ms | <2ms |
| Packet Loss | <1% | <0.5% | <0.1% | <0.01% |
| Throughput | 80% of BW | 90% of BW | 95% of BW | 99%+ of BW |

### Network Performance by Application

| Application | Bandwidth Need | Latency Requirement | Jitter Sensitivity | Packet Loss Tolerance |
|-------------|---------------|--------------------|-------------------|----------------------|
| Web Browsing | Low (1-5 Mbps) | Medium (<500ms) | Low | Low (<5%) |
| Email | Very Low (<1 Mbps) | High | None | Very Low (<1%) |
| Video Streaming | Medium (5-25 Mbps) | Low | High | Low (<2%) |
| VoIP | Low (64-128 Kbps) | Critical (<150ms) | Critical | Critical (<2%) |
| Online Gaming | Medium (5-20 Mbps) | Critical (<50ms) | Critical | Critical (<1%) |
| Video Conferencing | Medium (2-5 Mbps) | Critical (<150ms) | Critical | Low (<3%) |
| Bulk File Transfer | High (>100 Mbps) | High | None | Very Low (<0.1%) |
| IoT Sensors | Very Low (<100 Kbps) | Low | None | Medium (<10%) |

---

## PART 5: PROTOCOL SELECTION GUIDE

### Choose Protocol By Use Case

```
FILE TRANSFER
├─ SFTP - ✓ Secure (Preferred)
├─ SCP - ✓ Secure copy
├─ FTP - ✗ Insecure (Legacy)
└─ FTPS - ✓ FTP over SSL

EMAIL
├─ SMTP - Sending
│   ├─ Port 25 (relay)
│   └─ Port 587 (submission)
├─ IMAP - Receiving ✓ (Preferred)
│   └─ Port 143/993
└─ POP3 - Receiving
    └─ Port 110/995

REMOTE ACCESS
├─ SSH - ✓ Encrypted (Preferred)
│   └─ Port 22
├─ RDP - Windows remote
│   └─ Port 3389
├─ Telnet - ✗ Unencrypted (Deprecated)
│   └─ Port 23
└─ VNC - Remote GUI
    └─ Port 5900

WEB SERVICES
├─ HTTPS - ✓ Encrypted (Always Use)
│   └─ Port 443
├─ HTTP - ✗ Unencrypted
│   └─ Port 80
└─ HTTP/3 - ✓ Fastest (QUIC)
    └─ UDP-based

REAL-TIME COMMUNICATION
├─ RTP + SIP - VoIP
├─ WebRTC - Browser-based
├─ RTSP - Media streaming
└─ QUIC - HTTP/3 video

MONITORING
├─ SNMP - Device metrics
├─ Syslog - Log collection
├─ NetFlow - Traffic analysis
└─ Prometheus - Metrics collection

IoT & LIGHTWEIGHT
├─ MQTT - General IoT ✓
├─ CoAP - Extreme constraints
├─ Zigbee - Mesh networks
└─ LoRaWAN - Long range
```

---

## PART 6: NETWORKING STATISTICS & SUMMARY

### Keywords by Category Count

```
TOTAL: 235 KEYWORDS

OSI Model Layers ........................... 57 (24%)
├─ Layer 7 (Application) ................. 18
├─ Layer 3 (Network) ..................... 12
├─ Layer 4 (Transport) ................... 9
├─ Layer 2 (Data Link) ................... 7
├─ Layer 1 (Physical) .................... 5
├─ Layer 5 (Session) ..................... 4
└─ Layer 6 (Presentation) ................ 4

Network Types & Architecture .............. 26 (11%)
├─ Wireless Technologies ................. 14
├─ Geographic Scope ....................... 6
└─ Network Topologies ..................... 6

Network Protocols ......................... 38 (16%)
├─ Application Protocols ................. 15
├─ Routing Protocols ...................... 5
├─ Core Protocols ......................... 5
├─ Security Protocols ..................... 8
└─ IoT & Other Protocols ................. 5

Advanced Networking Topics ................ 17 (7%)
├─ Advanced Concepts ..................... 10
└─ Data Flow & Transmission .............. 7

Network Devices ........................... 13 (6%)
├─ Core Devices ........................... 7
└─ Advanced Devices ....................... 6

Network Architecture & Virtualization .... 12 (5%)
├─ Virtualization ......................... 6
└─ Architecture Concepts ................. 6

Network Security .......................... 16 (7%)
├─ Encryption & Certificates ............. 6
├─ VPN & Tunneling ........................ 6
└─ Threat Prevention ....................... 4

Network Addressing & Naming .............. 20 (9%)
├─ IP Addressing ......................... 11
└─ Domain Naming .......................... 9

Network Quality & Performance ............ 14 (6%)
├─ QoS Metrics ............................ 6
└─ QoS Mechanisms ......................... 8

Network Management & Monitoring .......... 11 (5%)
├─ Management Protocols ................... 6
└─ Monitoring & Analysis ................. 5
```

### Protocols by Type

```
TRANSPORT LAYER (Layer 4): 3 Protocols
├─ TCP - Transmission Control Protocol
├─ UDP - User Datagram Protocol
└─ QUIC - Quick UDP Internet Connections

INTERNET LAYER (Layer 3): 5 Protocols
├─ IP - Internet Protocol
├─ ICMP - Internet Control Message Protocol
├─ IGMP - Internet Group Management Protocol
├─ RIP - Routing Information Protocol
└─ OSPF, BGP, IS-IS - Routing protocols

APPLICATION LAYER (Layer 7): 15+ Protocols
├─ HTTP/HTTPS - Web
├─ FTP/SFTP - File Transfer
├─ SSH - Secure Shell
├─ SMTP/POP3/IMAP - Email
├─ DNS - Domain Names
├─ DHCP - IP Assignment
├─ SNMP - Monitoring
├─ LDAP - Directory
├─ NTP - Time
├─ RTSP - Media Streaming
├─ MQTT - IoT
├─ CoAP - IoT
└─ Others

SECURITY PROTOCOLS: 8 Protocols
├─ SSL/TLS - Encryption
├─ IPsec - Network Encryption
├─ VPN Technologies
├─ Kerberos - Authentication
└─ RADIUS - Authentication
```

### Devices by Layer

```
LAYER 1 (Physical): 3 Devices
├─ Repeater - Signal amplification
├─ Hub - Broadcast
└─ Modem - Signal conversion

LAYER 2 (Data Link): 3 Devices
├─ Switch - MAC forwarding
├─ Bridge - Segment connection
└─ Access Point - WiFi coverage

LAYER 3 (Network): 2 Devices
├─ Router - IP forwarding
└─ Layer 3 Switch - Hybrid

LAYER 7+ (Application): 5 Devices
├─ Firewall - Security
├─ Proxy Server - Request intermediary
├─ Load Balancer - Traffic distribution
├─ WLAN Controller - WiFi management
└─ Gateway - Protocol translation
```

### Technology Trends

```
EMERGING TECHNOLOGIES:
─────────────────────
✓ WiFi 6 (802.11ax) - OFDMA, higher bandwidth
✓ 5G Cellular - Ultra-high speed, low latency
✓ QUIC (HTTP/3) - UDP-based, faster connections
✓ Edge Computing - Processing closer to source
✓ Network Slicing - Virtual networks
✓ SD-WAN - Software-defined WANs
✓ Zero Trust - Verify every connection
✓ Network Automation - Programmable networks

DECLINING TECHNOLOGIES:
──────────────────────
✗ 802.11b/g - Older WiFi standards
✗ FTP - Insecure file transfer
✗ Telnet - Unencrypted remote access
✗ SSL 2.0/3.0 - Old encryption
✗ PPP - Legacy dial-up protocol
✗ Dial-up Internet - Very slow
✗ Hub Networks - Collision domains
✗ IPv4 (transitioning) - Limited addresses
```

---

## PART 7: QUICK LOOKUP TABLES

### Acronyms & Full Names

| Acronym | Full Name | Layer | Purpose |
|---------|-----------|-------|---------|
| API | Application Programming Interface | 7 | Software interface |
| ARP | Address Resolution Protocol | 2-3 | IP-to-MAC mapping |
| BGP | Border Gateway Protocol | 3 | Internet routing |
| CIDR | Classless Inter-Domain Routing | 3 | Flexible IP subnetting |
| CSRF | Cross-Site Request Forgery | 7 | Security attack |
| DHCP | Dynamic Host Configuration Protocol | 7 | IP assignment |
| DKIM | DomainKeys Identified Mail | 7 | Email authentication |
| DNS | Domain Name System | 7 | Name resolution |
| DNSBL | DNS Blacklist | 7 | Spam filtering |
| DoH | DNS over HTTPS | 7 | Encrypted DNS |
| DoS | Denial of Service | N/A | Attack type |
| DSCP | Differentiated Services Code Point | 3 | QoS marking |
| DSL | Digital Subscriber Line | 1 | Internet access |
| ECMP | Equal-Cost Multi-Path | 3 | Load balancing |
| EIGRP | Enhanced Interior Gateway Routing Protocol | 3 | Cisco routing |
| FQDN | Fully Qualified Domain Name | 7 | Complete domain |
| FTP | File Transfer Protocol | 7 | File transfer |
| FTPS | FTP Secure | 7 | Encrypted FTP |
| GRE | Generic Routing Encapsulation | 3 | Tunneling |
| HTTPS | HTTP Secure | 7 | Encrypted web |
| HTTP | HyperText Transfer Protocol | 7 | Web protocol |
| ICMP | Internet Control Message Protocol | 3 | Diagnostics |
| IGMP | Internet Group Management Protocol | 3 | Multicast |
| IMAP | Internet Message Access Protocol | 7 | Email protocol |
| IPsec | IP Security | 3 | Encryption |
| IPv4 | Internet Protocol Version 4 | 3 | 32-bit addressing |
| IPv6 | Internet Protocol Version 6 | 3 | 128-bit addressing |
| IS-IS | Intermediate System to Intermediate System | 3 | Routing |
| LDAP | Lightweight Directory Access Protocol | 7 | Directory |
| LTE | Long-Term Evolution | 1 | 4G cellular |
| L2TP | Layer 2 Tunneling Protocol | 2-3 | VPN protocol |
| MAC | Media Access Control | 2 | Physical address |
| MX | Mail Exchange | 7 | DNS record |
| NAT | Network Address Translation | 3 | IP translation |
| NFV | Network Functions Virtualization | N/A | Virtual networking |
| NTP | Network Time Protocol | 7 | Time sync |
| OSPF | Open Shortest Path First | 3 | IGP routing |
| OWASP | Open Web App Security Project | 7 | Security |
| PAN | Personal Area Network | N/A | Small network |
| POP3 | Post Office Protocol 3 | 7 | Email protocol |
| PPP | Point-to-Point Protocol | 2 | Direct link |
| PPTP | Point-to-Point Tunneling Protocol | N/A | VPN protocol |
| QoS | Quality of Service | N/A | Performance |
| QUIC | Quick UDP Internet Connections | 4 | Protocol |
| RADIUS | Remote Authentication Dial-In User Service | 7 | Authentication |
| RDP | Remote Desktop Protocol | 7 | Remote access |
| RFC | Request For Comments | N/A | Standard doc |
| RIP | Routing Information Protocol | 3 | Routing |
| RTP | Real-Time Protocol | 4 | Media transport |
| SDN | Software-Defined Networking | N/A | Programmable |
| SIP | Session Initiation Protocol | 7 | VoIP setup |
| SLA | Service Level Agreement | N/A | Guarantee |
| SMTP | Simple Mail Transfer Protocol | 7 | Email send |
| SNMP | Simple Network Management Protocol | 7 | Monitoring |
| SOCKS | Socket Secure | 5 | Proxy protocol |
| SPA | Spanning Tree Protocol | 2 | Loop prevention |
| SPF | Sender Policy Framework | 7 | Email auth |
| SSH | Secure Shell | 7 | Secure remote |
| SSL | Secure Sockets Layer | 6 | Encryption |
| TCP | Transmission Control Protocol | 4 | Reliable transport |
| TLS | Transport Layer Security | 6 | Encryption |
| ToS | Type of Service | 3 | QoS marking |
| UDP | User Datagram Protocol | 4 | Fast transport |
| URL | Uniform Resource Locator | 7 | Web address |
| VAN | Virtual Area Network | N/A | Corporate VPN |
| VPN | Virtual Private Network | N/A | Encrypted tunnel |
| VRF | Virtual Routing and Forwarding | 3 | Multi-tenant |
| VLAN | Virtual Local Area Network | 2 | Segmentation |
| WAN | Wide Area Network | N/A | Large network |
| WiFi | Wireless Fidelity | 1-2 | Wireless |
| WLAN | Wireless Local Area Network | 1-2 | Wireless LAN |
| WMAN | Wireless Metropolitan Area Network | 1-2 | Wireless MAN |
| WPAN | Wireless Personal Area Network | 1-2 | Wireless PAN |

---

## CONCLUSION

This table-formatted reference provides:

✓ **235 Networking Keywords** organized logically
✓ **Quick Reference Tables** for fast lookup
✓ **Performance Metrics** with target values
✓ **Protocol Selection Guides** for different use cases
✓ **Comprehensive Statistics** showing keyword distribution
✓ **Technology Comparisons** to choose the right tools
✓ **Acronym Dictionary** with 60+ terms
✓ **Port Reference** for all major services

Use this guide as:
- Quick reference during networking projects
- Study material for certifications (Network+, CCNA, etc.)
- Decision-making tool for technology selection
- Teaching resource for networking concepts
- Professional reference for IT infrastructure
