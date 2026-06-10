# Network Protocol Fuzzing


### 84.1 What is Fuzzing?
Fuzzing is the process of sending malformed or semi-structured data to a network service to identify crashes and security vulnerabilities.

### 84.2 Scapy: Packet Manipulation
Scapy allows you to craft packets for almost any protocol (Ethernet, IP, TCP, UDP, DNS, etc.).
*   **Internals**: It uses raw sockets (Chapter 38) to bypass the OS networking stack, allowing for the creation of packets with invalid checksums or out-of-order sequences.

---
