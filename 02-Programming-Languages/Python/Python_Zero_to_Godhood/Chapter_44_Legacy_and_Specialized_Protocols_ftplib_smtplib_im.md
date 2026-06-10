# Legacy and Specialized Protocols (`ftplib`, `smtplib`, `imaplib`)


Python's strength is its "batteries included" philosophy, providing clients for nearly every major internet protocol.

### 55.1 `smtplib`: The SMTP State Machine

SMTP (Simple Mail Transfer Protocol) is a conversational protocol.
*   **The Conversation**: `EHLO` $\rightarrow$ `STARTTLS` $\rightarrow$ `AUTH` $\rightarrow$ `MAIL FROM` $\rightarrow$ `RCPT TO` $\rightarrow$ `DATA` $\rightarrow$ `QUIT`.
*   **Internals**: `smtplib` manages the socket and parses the numeric status codes (e.g., 250 OK, 550 Failure) returned by the server. It handles the transition from a plaintext connection to a secure TLS connection via the `ssl` module.

### 55.2 `ftplib`: Active vs. Passive Mode

FTP is unique because it uses two separate socket connections: one for commands (Control) and one for data.
*   **Passive Mode (Recommended)**: The client initiates the data connection to the server.
*   **Active Mode**: The server attempts to connect back to the client (often blocked by modern firewalls/NAT).
*   **Internals**: `ftplib` handles the complex choreography of listening on a temporary port and coordinating with the control socket to transfer file data.

### 55.3 `imaplib`: Mailbox Synchronization

IMAP (Internet Message Access Protocol) is much more complex than SMTP or POP3 because it is stateful and supports partial downloads.
*   **Literal Handling**: `imaplib` implements the "IMAP Literal" protocol, allowing for the transfer of large binary message parts without crashing the interpreter's string allocation system.

---
