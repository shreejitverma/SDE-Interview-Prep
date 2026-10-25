---
type: concept
track: [sde, quant-dev, low-latency]
level:
status: draft
last_reviewed:
sources: []
---

# Phase XVIII: Python for CyberSecurity

Senior Python engineers often find themselves in roles requiring security auditing or exploit development. Python's flexibility makes it the premier language for security research.

# Chapter 83: Binary Analysis with Python

### 83.1 Interfacing with Disassemblers
Python is the scripting language for industry-standard binary analysis tools like **IDA Pro** and **Ghidra**.
*   **IDA Python**: Allows you to programmatically traverse function calls, identify cross-references, and rename obscure variables in a compiled binary.

### 83.2 The `pwntools` Library
`pwntools` is a CTF (Capture The Flag) framework used for rapid exploit development.
*   **Assembly/Disassembly**: On-the-fly conversion between machine code and assembly.
*   **ROP (Return-Oriented Programming)**: Programmatic generation of ROP chains to bypass NX/DEP protections.

---

# Chapter 84: Network Protocol Fuzzing

This section is maintained in [[Chapter_66_Network_Protocol_Fuzzing]].

# Chapter 85: Secure Coding and Cryptography (Advanced)

### 85.1 Hardening Python Applications
*   **`bandit`**: A tool that scans the AST for common security issues (e.g., using `eval()`, hardcoded passwords).
*   **Constant-Time Comparisons**: Using `hmac.compare_digest` to prevent timing attacks (Chapter 45).

### 85.2 Cryptographic Misuse
*   **Nonce Reuse**: The dangers of using the same initialization vector twice in AES-GCM.
*   **Insecure Randomness**: Why you must never use the `random` module for key generation (Chapter 35).

---
