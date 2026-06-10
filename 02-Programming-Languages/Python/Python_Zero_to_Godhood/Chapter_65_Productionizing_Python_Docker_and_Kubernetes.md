# Productionizing Python: Docker and Kubernetes


### 82.1 Dockerizing Python
*   **Base Images**: Always use `python:3.x-slim` or `python:3.x-alpine` to minimize the image size.
*   **Multi-stage Builds**: Compile C-extensions in a build stage and copy the binaries to the final runtime stage to keep the production image clean.

### 82.2 Observability and Monitoring
*   **Prometheus**: Exporting metrics from Python applications.
*   **OpenTelemetry**: Standardized tracing and logging across microservices.

---




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
