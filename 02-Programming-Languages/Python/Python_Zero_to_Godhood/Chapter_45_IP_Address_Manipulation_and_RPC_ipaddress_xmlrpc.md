# IP Address Manipulation and RPC (`ipaddress`, `xmlrpc`)


### 56.1 `ipaddress`: Vectorized Network Math

Manipulating IP ranges with regex is a recipe for security vulnerabilities. `ipaddress` provides objects for IPv4 and IPv6 addresses and networks.

#### 1. Internal Representations
*   **IPv4**: Stored as a 32-bit Python `int`.
*   **IPv6**: Stored as a 128-bit Python `int`.
*   **Performance**: Operations like `addr in network` are implemented using fast bitwise mask operations (`(addr_int & mask) == network_int`), making them extremely efficient for high-speed firewall log analysis.

### 56.2 `xmlrpc`: Simple Remote Procedure Calls

XML-RPC is a legacy but still widely used protocol for calling functions across the network.
*   **`ServerProxy`**: Uses Python's `__getattr__` dunder method to dynamically map local method calls to remote network requests.
*   **Serialization**: It uses the `xml.etree` module to convert Python types (ints, dicts, lists) into the XML format required by the protocol.

---


