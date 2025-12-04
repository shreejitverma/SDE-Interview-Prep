# Complete Networking Keywords Reference - 235 Keywords

## 1. OSI MODEL LAYERS (52 KEYWORDS)

### Physical Layer (Layer 1) - 5 Keywords
1. Bits - Smallest unit of data transmission (0s and 1s) transmitted as electrical signals
2. Physical Media - Tangible transmission mediums (cables, fiber optics, wireless)
3. Hub - Device broadcasting data to all ports without filtering
4. Repeater - Regenerates weak signals to extend transmission distance
5. Modem - Converts digital signals to analog and vice versa

### Data Link Layer (Layer 2) - 7 Keywords
6. MAC Address - Unique 48-bit identifier for network devices (Example: 00:1A:2B:3C:4D:5E)
7. Switch - Forwards data frames based on MAC addresses to specific ports
8. Frame - Layer 2 data unit with MAC addresses, VLAN info, error checking
9. Ethernet - LAN protocol defining frame format and access methods
10. PPP - Point-to-Point Protocol for direct communication between two nodes
11. ARP - Address Resolution Protocol mapping IP addresses to MAC addresses
12. VLAN - Virtual Local Area Network logically segmenting physical LAN

### Network Layer (Layer 3) - 11 Keywords
13. IP Address - Internet Protocol unique logical identifier (IPv4 32-bit, IPv6 128-bit)
14. IPv4 - 32-bit addressing scheme (4.3 billion addresses)
15. IPv6 - 128-bit addressing scheme (340 undecillion addresses)
16. Packet - Layer 3 data unit with source/dest IP addresses and routing info
17. Router - Device forwarding packets between networks based on IP addresses
18. Routing - Process determining best path for packet delivery
19. ICMP - Internet Control Message Protocol for diagnostics and error reporting
20. Subnet - Logical subdivision of an IP network
21. Subnet Mask - Determines network vs host portion of IP (255.255.255.0 or /24)
22. Default Gateway - Router through which devices send traffic to other networks
23. NAT - Network Address Translation mapping internal IPs to external IPs
24. IGMP - Internet Group Management Protocol managing multicast group membership

### Transport Layer (Layer 4) - 9 Keywords
25. TCP - Transmission Control Protocol with reliable, connection-oriented delivery
26. UDP - User Datagram Protocol with unreliable, connectionless delivery
27. Port - Virtual endpoint for network communication (0-65535)
28. Socket - Combination of IP address and port number
29. Segment - Data unit at Layer 4 for TCP
30. Datagram - Data unit at Layer 4 for UDP
31. Flow Control - Mechanism preventing receiver from being overwhelmed
32. Error Detection - Checking transmission errors in data
33. Connection Establishment - TCP three-way handshake (SYN, SYN-ACK, ACK)

### Session Layer (Layer 5) - 4 Keywords
34. Session - Logical connection between two applications
35. Authentication - Verification of user identity before communication
36. Authorization - Determines what authenticated user can access
37. Session Management - Establishment, maintenance, termination of sessions

### Presentation Layer (Layer 6) - 4 Keywords
38. Encryption - Converts plaintext to ciphertext for security
39. Compression - Reduces data size for efficient transmission
40. Character Encoding - Represents text in standardized format (ASCII, UTF-8)
41. SSL/TLS - Secure Sockets Layer/Transport Layer Security encryption protocol

### Application Layer (Layer 7) - 17 Keywords
42. HTTP - HyperText Transfer Protocol (Port 80, insecure web browsing)
43. HTTPS - HTTP Secure with encryption (Port 443)
44. FTP - File Transfer Protocol (Port 21)
45. SFTP - SSH File Transfer Protocol (Port 22)
46. SSH - Secure Shell remote login (Port 22)
47. Telnet - Remote login protocol unencrypted (Port 23, deprecated)
48. SMTP - Simple Mail Transfer Protocol email sending (Port 25)
49. POP3 - Post Office Protocol v3 email retrieval (Port 110)
50. IMAP - Internet Message Access Protocol email access (Port 143)
51. DNS - Domain Name System hostname resolution (Port 53)
52. DHCP - Dynamic Host Configuration Protocol automatic IP assignment
53. SNMP - Simple Network Management Protocol device monitoring (Port 161)
54. LDAP - Lightweight Directory Access Protocol directory services (Port 389)
55. NTP - Network Time Protocol time synchronization (Port 123)
56. RTSP - Real Time Streaming Protocol media streaming (Port 554)
57. RTP - Real-time Transport Protocol real-time data delivery
58. SIP - Session Initiation Protocol multimedia session setup

## 2. NETWORK TYPES & ARCHITECTURE (21 KEYWORDS)

### Network Geographic Scope - 6 Keywords
59. PAN - Personal Area Network within few meters (Bluetooth, USB)
60. LAN - Local Area Network single building/campus (Ethernet)
61. WAN - Wide Area Network large geographical area (Internet)
62. MAN - Metropolitan Area Network city-scale
63. CAN - Campus Area Network multiple buildings nearby
64. GAN - Global Area Network worldwide scale

### Wireless Technologies - 12 Keywords
65. WLAN - Wireless Local Area Network (WiFi standards)
66. WMAN - Wireless Metropolitan Area Network (WiMAX)
67. WPAN - Wireless Personal Area Network (Bluetooth, ZigBee)
68. WiFi - Wireless Fidelity 802.11 standards
69. 802.11a - 5 GHz, 54 Mbps (legacy)
70. 802.11b - 2.4 GHz, 11 Mbps (legacy)
71. 802.11g - 2.4 GHz, 54 Mbps
72. 802.11n - 2.4/5 GHz, 600 Mbps MIMO
73. 802.11ac - WiFi 5, 5 GHz, 1.3 Gbps
74. 802.11ax - WiFi 6, 2.4/5/6 GHz, 9.6 Gbps
75. Bluetooth - Short-range 2.4 GHz, 10-100m
76. BLE - Bluetooth Low Energy variant
77. LTE - 4G cellular standard
78. 5G - Fifth generation cellular

### Network Topologies - 3 Keywords
79. Bus - Devices on shared medium
80. Star - Devices to central hub/switch
81. Ring - Devices in circular chain
82. Mesh - Multi-path device connections
83. Tree - Hierarchical structure
84. Hybrid - Combination of topologies

## 3. NETWORK PROTOCOLS (40 KEYWORDS)

### Core Protocols - 5 Keywords
85. TCP/IP - Foundation protocol suite
86. IP - Internet Protocol routing
87. ICMP - Diagnostics and errors
88. IGMP - Multicast management
89. ARP - IP to MAC resolution

### Routing Protocols - 5 Keywords
90. RIP - Distance vector, 15-hop limit
91. OSPF - Link state, hierarchical
92. EIGRP - Hybrid protocol (Cisco)
93. BGP - Internet backbone routing
94. IS-IS - Link state for ISPs

### Application Protocols - 15 Keywords
95. HTTP - Web (Port 80)
96. HTTPS - Secure web (Port 443)
97. FTP - File transfer (Port 21)
98. SFTP - Secure transfer (Port 22)
99. SMTP - Email send (Port 25/587)
100. POP3 - Email download (Port 110)
101. IMAP - Email sync (Port 143)
102. DNS - Name resolution (Port 53)
103. DHCP - IP assignment (67/68)
104. SSH - Secure shell (Port 22)
105. Telnet - Remote login (Port 23)
106. SNMP - Monitoring (Port 161)
107. LDAP - Directory (Port 389)
108. NTP - Time sync (Port 123)
109. RTSP - Media control (Port 554)

### Security Protocols - 8 Keywords
110. SSL - Encryption (deprecated)
111. TLS - Modern encryption
112. IPsec - VPN encryption
113. PPTP - VPN protocol (weak)
114. L2TP - VPN protocol
115. VPN - Virtual Private Network
116. Kerberos - Authentication
117. RADIUS - Central authentication

### Other Protocols - 5 Keywords
118. MQTT - IoT messaging
119. CoAP - IoT protocol
120. AMQP - Message broker
121. WebSocket - Bidirectional web
122. QUIC - HTTP/3 protocol

## 4. NETWORK QUALITY & PERFORMANCE (18 KEYWORDS)

### QoS Metrics - 6 Keywords
123. Bandwidth - Max data capacity (Mbps/Gbps)
124. Latency - Time delay (milliseconds)
125. Jitter - Packet arrival variation
126. Packet Loss - Lost packet percentage
127. Throughput - Actual transfer rate
128. Response Time - System response delay

### QoS Mechanisms - 12 Keywords
129. Traffic Shaping - Smooth burst traffic
130. Traffic Policing - Enforce bandwidth limits
131. Prioritization - Mark critical traffic
132. Queuing - Order packets transmission
133. Weighted Fair Queuing - Fair bandwidth distribution
134. Congestion Control - Prevent overload
135. Congestion Avoidance - Proactive prevention
136. Bandwidth Reservation - Pre-allocate bandwidth
137. DiffServ - Differentiated Services
138. ToS - Type of Service field
139. DSCP - Differentiated Services Code Point
140. MPLS - Label-based routing

## 5. NETWORK DEVICES (13 KEYWORDS)

### Core Devices - 7 Keywords
141. Hub - Layer 1 broadcast device
142. Switch - Layer 2 MAC-based forwarding
143. Router - Layer 3 IP-based forwarding
144. Gateway - Layer 3-7 protocol translation
145. Firewall - Security filtering device
146. Proxy Server - Request intermediary
147. Load Balancer - Traffic distribution

### Advanced Devices - 6 Keywords
148. Layer 3 Switch - Routing + switching
149. WLAN Controller - Centralized WiFi management
150. Access Point - WiFi coverage device
151. Modem - Signal conversion device
152. Bridge - Layer 2 segmentation
153. Wireless Extender - WiFi range extension

## 6. NETWORK ARCHITECTURE & VIRTUALIZATION (11 KEYWORDS)

### Architecture - 5 Keywords
154. SDN - Software-Defined Networking
155. Control Plane - Routing decision-making
156. Data Plane - Packet forwarding
157. Management Plane - Configuration/monitoring
158. Network Segmentation - Logical division

### Virtualization - 6 Keywords
159. NFV - Network Functions Virtualization
160. VNF - Virtual Network Function
161. NFVI - NFV Infrastructure
162. MANO - Management and Orchestration
163. Virtual Switch - Software switching
164. Network Slicing - Virtual networks

## 7. NETWORK SECURITY (16 KEYWORDS)

### Encryption & Certificates - 6 Keywords
165. PKI - Public Key Infrastructure
166. Digital Certificate - Identity proof
167. Certificate Authority - CA issues certs
168. RSA - Asymmetric encryption
169. AES - Symmetric encryption
170. Cipher Suite - Algorithm set

### VPN & Tunneling - 6 Keywords
171. VPN - Encrypted tunnel
172. IPsec - Network layer encryption
173. SSL VPN - Application layer encryption
174. Tunnel Mode - Encrypt headers+payload
175. Transport Mode - Encrypt payload only
176. Encapsulation - Protocol wrapping

### Threat Prevention - 4 Keywords
177. Firewall - Traffic filtering
178. IDS - Intrusion Detection System
179. IPS - Intrusion Prevention System
180. DDoS Protection - DDoS mitigation

## 8. NETWORK ADDRESSING & NAMING (20 KEYWORDS)

### IP Addressing - 11 Keywords
181. IPv4 Address - 32-bit dotted decimal
182. IPv6 Address - 128-bit hexadecimal
183. Public IP - Internet-routable
184. Private IP - Non-routable internally
185. Loopback Address - Local testing
186. Broadcast Address - Network-wide
187. Multicast Address - Group targeting
188. CIDR Notation - Prefix-based (/24)
189. Subnet Mask - Network boundary
190. Default Gateway - First-hop router
191. DHCP Lease - Temporary IP

### Domain Naming - 9 Keywords
192. DNS - Domain Name System
193. FQDN - Fully Qualified Domain Name
194. DNS Record - Name-to-value mapping
195. A Record - IPv4 mapping
196. AAAA Record - IPv6 mapping
197. CNAME Record - Hostname alias
198. MX Record - Mail server
199. NS Record - Nameserver
200. TXT Record - Text/policy record

## 9. ADVANCED NETWORKING TOPICS (26 KEYWORDS)

### Data Flow - 7 Keywords
201. Full Duplex - Simultaneous two-way
202. Half Duplex - Alternate one-way
203. Simplex - One-way only
204. Collision Domain - Collision segment
205. Broadcast Domain - Broadcast segment
206. Flow Control - Receiver regulation
207. Sliding Window - Variable window

### Advanced Concepts - 12 Keywords
208. QoS Classification - Traffic categorization
209. DiffServ - Traffic classes
210. ToS - Priority marking
211. DSCP - QoS marking
212. MPLS - Label-based routing
213. VRF - Virtual routing table
214. Failover - Automatic backup
215. Redundancy - Fault tolerance
216. SLA - Service Level Agreement
217. Throughput Optimization - Data rate maximization
218. Latency Optimization - Delay minimization
219. Packet Loss Reduction - Reliability improvement

### Routing & Control - 7 Keywords
220. BGP - Internet routing
221. AS - Autonomous System
222. ECMP - Equal-Cost Multipath
223. Route Redistribution - Route sharing
224. Route Summarization - Route aggregation
225. Policy Routing - Custom rules
226. Traffic Engineering - Path control

## 10. NETWORK MANAGEMENT & MONITORING (11 KEYWORDS)

### Management - 6 Keywords
227. SNMP - Device monitoring
228. SNMP Trap - Unsolicited alert
229. MIB - Management Information Base
230. Syslog - Centralized logging
231. NetFlow - Traffic analysis
232. IPFIX - Flow information export

### Monitoring - 5 Keywords
233. Packet Capture - Traffic inspection
234. Flow Analysis - Traffic pattern analysis
235. Link Monitoring - Health checking
236. Performance Monitoring - Metric tracking
237. Network Analytics - Trend analysis

---

## SUMMARY

**Total Networking Keywords: 235**

Organized by:
- OSI Model Layers (7 layers)
- Network Types & Architecture
- Protocols (routing, application, security)
- Performance & Quality
- Devices
- Virtualization
- Security
- Addressing & Naming
- Advanced Topics
- Management & Monitoring

Each keyword includes brief explanation and practical context for understanding.
