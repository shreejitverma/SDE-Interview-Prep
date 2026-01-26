# Complete Networking Protocols Reference - Extensive Guide

## Table of Contents

1. [Application Layer Protocols (Layer 7)](#application-layer-protocols-layer-7)
2. [Transport Layer Protocols (Layer 4)](#transport-layer-protocols-layer-4)
3. [Network Layer Protocols (Layer 3)](#network-layer-protocols-layer-3)
4. [Data Link Layer Protocols (Layer 2)](#data-link-layer-protocols-layer-2)
5. [Physical Layer Specifications (Layer 1)](#physical-layer-specifications-layer-1)
6. [Routing Protocols](#routing-protocols)
7. [Security & Encryption Protocols](#security--encryption-protocols)
8. [VPN & Tunneling Protocols](#vpn--tunneling-protocols)
9. [Wireless Protocols](#wireless-protocols)
10. [IoT & Lightweight Protocols](#iot--lightweight-protocols)
11. [Real-Time & Streaming Protocols](#real-time--streaming-protocols)
12. [Specialized & Emerging Protocols](#specialized--emerging-protocols)

---

## APPLICATION LAYER PROTOCOLS (Layer 7)

### HTTP (HyperText Transfer Protocol)

**Overview:**
HTTP is the foundational protocol of the World Wide Web, enabling retrieval of hypertext documents. It's a stateless, client-server protocol built on top of TCP.

**Key Characteristics:**
- Stateless protocol (each request independent)
- Request-response model
- Text-based protocol
- TCP port 80
- HTTP/1.1, HTTP/2, HTTP/3 versions available
- Methods: GET, POST, PUT, DELETE, PATCH, HEAD, OPTIONS, TRACE, CONNECT

**Detailed Usage:**

```
Request Format:
METHOD /path HTTP/1.1
Host: example.com
User-Agent: Mozilla/5.0
Accept: text/html
Accept-Language: en-US
Connection: keep-alive
Cache-Control: no-cache
[blank line]
[optional request body]

Response Format:
HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: 1234
Set-Cookie: session=abc123
Connection: keep-alive
[blank line]
<!DOCTYPE html>
<html>...
</html>
```

**Common HTTP Methods:**
- GET: Retrieve resource (idempotent, no body)
- POST: Submit data (can have side effects)
- PUT: Replace entire resource (idempotent)
- DELETE: Remove resource (idempotent)
- PATCH: Partial modification (not idempotent always)
- HEAD: Same as GET but no response body
- OPTIONS: Describe communication options
- TRACE: Echo request (diagnostic)
- CONNECT: Establish tunnel (proxy connection)

**Status Codes:**
- 1xx (100-199): Informational
- 2xx (200-299): Success (200 OK, 201 Created, 204 No Content)
- 3xx (300-399): Redirection (301 Moved Permanently, 302 Found, 304 Not Modified)
- 4xx (400-499): Client Error (400 Bad Request, 401 Unauthorized, 403 Forbidden, 404 Not Found)
- 5xx (500-599): Server Error (500 Internal Server Error, 502 Bad Gateway, 503 Service Unavailable)

**Practical Examples:**

```bash
# Simple GET request
curl http://example.com/

# GET with headers
curl -H "Authorization: Bearer token123" http://example.com/api/users

# POST request with JSON data
curl -X POST http://example.com/api/data \
  -H "Content-Type: application/json" \
  -d '{"name": "John", "age": 30}'

# PUT request (update entire resource)
curl -X PUT http://example.com/api/users/1 \
  -H "Content-Type: application/json" \
  -d '{"name": "Jane", "age": 25}'

# DELETE request
curl -X DELETE http://example.com/api/users/1

# HEAD request (get headers only, no body)
curl -I http://example.com/

# Multiple headers and authentication
curl -H "Authorization: Bearer token" \
  -H "X-Custom-Header: value" \
  http://example.com/secure/endpoint
```

**HTTP Versions Comparison:**

HTTP/1.1:
- Persistent connections (keep-alive)
- Pipelining support
- Chunked transfer encoding
- Cache control headers

HTTP/2:
- Binary framing
- Multiplexing (multiple streams over single connection)
- Server push
- Header compression (HPACK)
- Improved performance

HTTP/3:
- Built on QUIC protocol (UDP-based)
- Faster connection establishment
- Better mobile support
- Improved congestion control

**Use Cases:**
- Web page retrieval
- REST APIs
- Mobile app backends
- Microservices communication
- File downloads
- Form submissions

---

### HTTPS (HTTP Secure)

**Overview:**
HTTPS adds encryption layer (TLS/SSL) to HTTP for secure communication. Encrypts data in transit preventing eavesdropping and man-in-the-middle attacks.

**Key Characteristics:**
- HTTP over TLS/SSL
- TCP port 443
- Encrypted payload
- Certificate-based authentication
- Prevents data tampering
- Browser lock icon indicates HTTPS

**TLS Handshake Process:**

```
1. Client Hello: Sends supported TLS versions, cipher suites, random number
2. Server Hello: Selects TLS version, cipher suite, sends certificate
3. Certificate Verification: Client verifies server certificate validity
4. Key Exchange: Client and server establish shared symmetric key
5. Change Cipher Spec: Notify switching to encrypted communication
6. Finished: Send encrypted verification message
7. Connection Established: Encrypted communication begins
```

**Practical Examples:**

```bash
# HTTPS GET request
curl https://api.example.com/data

# HTTPS with client certificate
curl --cert client.crt --key client.key https://api.example.com/secure

# View SSL certificate
openssl s_client -connect example.com:443

# Check certificate validity
openssl x509 -in certificate.crt -text -noout

# Generate self-signed certificate (testing)
openssl req -x509 -newkey rsa:2048 -keyout key.pem -out cert.pem -days 365

# Python HTTPS request
import requests
response = requests.get('https://api.example.com/data')
print(response.json())

# Disable SSL verification (not recommended)
requests.get('https://api.example.com', verify=False)
```

**Certificate Validation:**
- Certificate chain verification
- Expiration date check
- Subject Alternative Name (SAN) matching
- Certificate Authority (CA) trust
- Revocation status (OCSP, CRL)

**Use Cases:**
- Banking and financial transactions
- E-commerce transactions
- Authentication systems
- Secure API endpoints
- Email transmission
- Admin dashboards
- Any sensitive data transfer

---

### FTP (File Transfer Protocol)

**Overview:**
Legacy protocol for transferring files between computers. Uses two TCP connections: one for control commands (port 21) and one for data transfer (port 20).

**Key Characteristics:**
- Two-channel architecture (control + data)
- ASCII and binary transfer modes
- Stateful connection
- Limited security (credentials in plaintext)
- Active mode: Server initiates data connection
- Passive mode: Client initiates data connection

**Connection Modes:**

Active Mode:
```
Client initiates connection to server port 21 (control)
Client sends PORT command with its data port
Server connects back to client's data port to transfer file
Data flows from server to client
```

Passive Mode:
```
Client initiates connection to server port 21 (control)
Client sends PASV command
Server responds with IP:port to connect to
Client initiates data connection to server's data port
Data flows from server to client
```

**FTP Commands:**

```
USER <username>           - Send username
PASS <password>          - Send password
LIST [path]             - List directory
RETR <filename>         - Download file
STOR <filename>         - Upload file
DELE <filename>         - Delete file
MKD <dirname>           - Create directory
RMD <dirname>           - Remove directory
CWD <path>              - Change working directory
PWD                     - Print working directory
TYPE A                  - ASCII mode
TYPE I                  - Binary (Image) mode
QUIT                    - Close connection
```

**Practical Examples:**

```bash
# Command line FTP
ftp ftp.example.com
> user john
> pass password123
> ls
> get filename.zip
> put localfile.txt
> quit

# FTP via shell script
ftp -n -v ftp.example.com << EOF
user john password123
get filename.zip
put localfile.txt
quit
EOF

# Python FTP
from ftplib import FTP
ftp = FTP('ftp.example.com')
ftp.login('john', 'password123')
ftp.retrlines('LIST', print)
ftp.retrbinary('RETR filename.zip', open('filename.zip', 'wb').write)
ftp.storbinary('STOR localfile.txt', open('localfile.txt', 'rb'))
ftp.quit()

# FTP URL in browser
ftp://username:password@ftp.example.com/path/to/file

# Wget FTP download
wget ftp://user:pass@ftp.example.com/file.zip
```

**Security Issues:**
- Credentials sent in plaintext
- Data not encrypted
- Vulnerable to man-in-the-middle attacks
- Commands and passwords visible to eavesdroppers

**Use Cases:**
- Legacy file sharing (web hosting uploads)
- Automated backups
- Large file distribution
- Legacy systems integration

**Note:** SFTP and FTPS are secure alternatives.

---

### SFTP (SSH File Transfer Protocol)

**Overview:**
Secure file transfer protocol built on top of SSH (Secure Shell). Provides encryption, authentication, and reliable data transfer.

**Key Characteristics:**
- Built on SSH protocol (port 22)
- End-to-end encryption
- Public key authentication support
- Password authentication
- Reliable with resume capability
- Firewall-friendly (single port)

**Connection Process:**

```
1. TCP connection to port 22
2. SSH protocol negotiation
3. Host key verification
4. User authentication (password/key)
5. SFTP subsystem request
6. SFTP channel established
7. File transfer commands
```

**SFTP Commands:**

```
open <host>                    - Connect to remote server
user <username>               - Specify username
ls [-la] [path]              - List directory
pwd                          - Print working directory
cd <path>                    - Change directory
mkdir <dirname>              - Create directory
rmdir <dirname>              - Remove directory
get <remote> [local]         - Download file
put <local> [remote]         - Upload file
mget <pattern>               - Download multiple files
mput <pattern>               - Upload multiple files
delete <filename>            - Delete file
rename <old> <new>           - Rename file
chmod <mode> <file>          - Change permissions
quit                         - Close connection
```

**Practical Examples:**

```bash
# Basic SFTP connection
sftp user@server.com

# SFTP commands in interactive mode
sftp> ls
sftp> cd /home/user/documents
sftp> get report.pdf
sftp> put local_file.txt
sftp> mkdir new_folder
sftp> quit

# Non-interactive SFTP (batch)
sftp -b batch_commands.txt user@server.com

# Using SSH key for authentication
sftp -i ~/.ssh/id_rsa user@server.com

# Python SFTP
import paramiko
ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
ssh.connect('server.com', username='user', password='pass')
sftp = ssh.open_sftp()
sftp.get('remote.txt', 'local.txt')
sftp.put('local.txt', 'remote.txt')
sftp.close()

# Wget SFTP
wget sftp://user:pass@server.com/path/to/file

# rsync over SSH (similar to SFTP)
rsync -avz -e ssh user@server.com:/remote/path /local/path
```

**Authentication Methods:**
- Password authentication
- Public key authentication (more secure)
- Certificate-based authentication
- Host-based authentication

**Use Cases:**
- Secure file uploads/downloads
- DevOps deployment automation
- Backup over network
- Secure data transfer
- Remote server administration
- Cross-server file synchronization

**Advantages over FTP:**
- Encrypted communication
- Single port (22)
- Firewall-friendly
- Public key authentication
- No plaintext passwords

---

### SSH (Secure Shell)

**Overview:**
Cryptographic network protocol for secure remote login and command execution. Provides encrypted and authenticated access to remote systems.

**Key Characteristics:**
- TCP port 22
- Encrypted communication
- Public key and password authentication
- Remote command execution
- Port forwarding (tunneling)
- Secure file transfer (SFTP)
- X11 forwarding (remote GUI)

**SSH Protocol Layers:**

```
Transport Layer:
- Server authentication
- Encryption
- Integrity checking
- Compression

User Authentication Layer:
- Password authentication
- Public key authentication
- Host-based authentication
- Keyboard-interactive

Connection Protocol Layer:
- Remote command execution
- X11 forwarding
- Port forwarding
- File transfer (SFTP)
```

**SSH Key Types:**

RSA (Rivest-Shamir-Adleman):
- 2048-bit, 4096-bit standard
- Widely supported
- Slower but stronger

Ed25519:
- 256-bit elliptic curve
- Modern standard
- Faster and more secure
- Recommended

ECDSA:
- Elliptic curve cryptography
- 256-bit, 384-bit, 521-bit
- Good performance

**Practical Examples:**

```bash
# Basic SSH login
ssh user@server.com

# SSH with specific key
ssh -i ~/.ssh/id_rsa user@server.com

# SSH with custom port
ssh -p 2222 user@server.com

# Execute remote command
ssh user@server.com "ls -la"

# SSH with X11 forwarding (remote GUI)
ssh -X user@server.com

# Port forwarding (local)
ssh -L 8080:localhost:80 user@server.com

# Port forwarding (remote)
ssh -R 8080:localhost:80 user@server.com

# SSH tunneling (proxy through jump host)
ssh -J jumphost.com user@final.com

# Generate SSH key pair
ssh-keygen -t ed25519 -C "user@example.com"
ssh-keygen -t rsa -b 4096 -C "user@example.com"

# Copy public key to server
ssh-copy-id -i ~/.ssh/id_rsa.pub user@server.com

# Manual key addition to authorized_keys
cat ~/.ssh/id_rsa.pub | ssh user@server.com 'cat >> .ssh/authorized_keys'

# SSH config file
# ~/.ssh/config
Host myserver
    HostName server.com
    User john
    IdentityFile ~/.ssh/id_rsa
    Port 22

# Then simply: ssh myserver

# Python SSH
import paramiko
ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
ssh.connect('server.com', username='user', password='pass')
stdin, stdout, stderr = ssh.exec_command('ls -la')
print(stdout.read().decode())
ssh.close()

# Disable password auth, use keys only
ssh -o PubkeyAuthentication=yes -o PasswordAuthentication=no user@server.com

# Enable debug output
ssh -v user@server.com
```

**SSH Configuration File (~/.ssh/config):**

```
Host *
    AddKeysToAgent yes
    IdentityFile ~/.ssh/id_ed25519
    IdentityFile ~/.ssh/id_rsa
    ServerAliveInterval 60
    ServerAliveCountMax 10

Host github.com
    HostName github.com
    User git
    IdentityFile ~/.ssh/github_key

Host prod-server
    HostName 10.0.0.1
    User admin
    Port 2222
    IdentityFile ~/.ssh/prod_key
    ProxyCommand ssh -W %h:%p jumphost.com
```

**Port Forwarding Use Cases:**

Local Port Forwarding (access internal service):
```bash
ssh -L 3306:internal-db.local:3306 user@server.com
# Connect to localhost:3306 as if database is local
mysql -h localhost -u root -p
```

Remote Port Forwarding (expose local service):
```bash
ssh -R 8080:localhost:80 user@server.com
# Allows server.com to access your localhost:80 on server.com:8080
```

SOCKS Proxy (dynamic tunneling):
```bash
ssh -D 9050 user@server.com
# Configure browser to use localhost:9050 as SOCKS proxy
# All traffic routed through server
```

**Use Cases:**
- Remote server administration
- Secure login to production systems
- Running remote commands
- Secure file transfer (SFTP)
- Port forwarding and tunneling
- Git repository access (GitHub, GitLab)
- Bastion host jumping
- CI/CD pipeline authentication
- Cron job execution
- System monitoring

**Security Best Practices:**
- Use strong keys (RSA 4096-bit or Ed25519)
- Disable password authentication
- Use SSH keys instead of passwords
- Disable root login
- Use non-standard ports (optional)
- Implement rate limiting
- Monitor failed login attempts
- Use SSH certificates for large deployments
- Implement multi-factor authentication

---

### SMTP (Simple Mail Transfer Protocol)

**Overview:**
Protocol for sending emails from clients to mail servers (and between servers). Uses TCP ports 25 (for server-to-server) and 587 (for client submission).

**Key Characteristics:**
- Text-based protocol
- Port 25 (SMTP relay between servers)
- Port 587 (submission, client to server)
- Port 465 (SMTPS - deprecated but still used)
- Stateful connection
- Line-ending: CRLF (\r\n)
- Case-insensitive commands

**SMTP Authentication:**
- PLAIN: Base64 encoded credentials
- LOGIN: Challenge-response with Base64
- CRAM-MD5: MD5 hash-based authentication

**SMTP Commands:**

```
HELO <hostname>              - Hello from client
MAIL FROM:<sender@domain>    - Specify sender
RCPT TO:<recipient@domain>   - Specify recipient (can repeat)
DATA                         - Start email content
Subject: <subject>          - Email subject line
From: <sender@domain>       - From header
To: <recipient@domain>      - To header
<blank line>
<email body>
.                           - End of email (period on own line)
QUIT                        - Close connection
AUTH <method> <credentials> - Authenticate
STARTTLS                    - Upgrade to encrypted connection
```

**Practical Examples:**

```bash
# Telnet to SMTP server (basic test)
telnet smtp.gmail.com 587
EHLO client.example.com
STARTTLS
AUTH LOGIN
base64_encoded_username
base64_encoded_password
MAIL FROM:<sender@gmail.com>
RCPT TO:<recipient@example.com>
DATA
Subject: Test Email
From: sender@gmail.com
To: recipient@example.com

This is a test email.
.
QUIT

# Using sendmail (mail command)
echo "Email body" | mail -s "Subject" recipient@example.com

# Using mail utility
echo "This is the body" | mail -s "Test Subject" recipient@example.com

# Using Python smtplib
import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart

# Create message
msg = MIMEMultipart()
msg['From'] = 'sender@gmail.com'
msg['To'] = 'recipient@example.com'
msg['Subject'] = 'Test Email'

body = 'This is a test email'
msg.attach(MIMEText(body, 'plain'))

# Send email
server = smtplib.SMTP('smtp.gmail.com', 587)
server.starttls()
server.login('sender@gmail.com', 'password')
server.send_message(msg)
server.quit()

# With attachment
from email.mime.base import MIMEBase
from email import encoders

attachment = open('file.pdf', 'rb')
part = MIMEBase('application', 'octet-stream')
part.set_payload(attachment.read())
encoders.encode_base64(part)
part.add_header('Content-Disposition', f'attachment; filename= file.pdf')
msg.attach(part)

# Using Node.js nodemailer
const nodemailer = require('nodemailer');
const transporter = nodemailer.createTransport({
  host: 'smtp.gmail.com',
  port: 587,
  secure: false,
  auth: {
    user: 'sender@gmail.com',
    pass: 'password'
  }
});

transporter.sendMail({
  from: 'sender@gmail.com',
  to: 'recipient@example.com',
  subject: 'Test Email',
  text: 'Email body',
  html: '<h1>Test</h1><p>Email body</p>'
}, (error, info) => {
  if (error) console.log(error);
  else console.log('Email sent: ' + info.response);
});

# Using cURL for SMTP
curl --ssl-reqd \
  --url 'smtps://smtp.gmail.com:465' \
  --user 'sender@gmail.com:password' \
  --mail-from 'sender@gmail.com' \
  --mail-rcpt 'recipient@example.com' \
  -T mail.txt
```

**Email Format (MIME):**

```
From: sender@example.com
To: recipient@example.com
Subject: Test Email
MIME-Version: 1.0
Content-Type: multipart/mixed; boundary="boundary123"

--boundary123
Content-Type: text/plain; charset="UTF-8"

This is the email body.

--boundary123
Content-Type: application/pdf; name="document.pdf"
Content-Transfer-Encoding: base64
Content-Disposition: attachment

[base64 encoded file content]

--boundary123--
```

**Use Cases:**
- Application email notifications
- Alert notifications
- Transactional emails (confirmations, receipts)
- Marketing campaigns
- Bulk email distribution
- System monitoring alerts
- Password reset emails
- Account verification

**SMTP vs ESMTP:**
- ESMTP (Extended SMTP) adds authentication, encryption (STARTTLS)
- ESMTP supports additional commands
- ESMTP recommended over basic SMTP

---

### POP3 (Post Office Protocol v3)

**Overview:**
Protocol for retrieving emails from mail server. Designed for single-client download model where emails are typically deleted after retrieval.

**Key Characteristics:**
- TCP port 110 (plain), 995 (POP3S - encrypted)
- Download-and-delete model
- Simple protocol
- Stateful connection
- Limited offline browsing (no server-side folders)
- Ideal for single client per account

**POP3 States:**

Authorization State:
- USER: Send username
- PASS: Send password
- QUIT: Exit (no commands executed)

Transaction State:
- STAT: Server response: +OK num messages size bytes
- LIST: List messages with sizes
- RETR: Retrieve message
- DELE: Mark for deletion
- NOOP: No operation
- RSET: Reset (unmark deletions)
- QUIT: Commit deletions and close

**POP3 Commands:**

```
USER <username>              - Send username
PASS <password>             - Send password
STAT                        - Get mailbox statistics
LIST [msg]                  - List messages
RETR <msg>                  - Retrieve message
DELE <msg>                  - Mark for deletion
NOOP                        - No-op (keep connection alive)
RSET                        - Unmark deleted messages
TOP <msg> <lines>           - Get message headers + n lines
UIDL [msg]                  - Get unique ID list
QUIT                        - Commit deletions, close
```

**Practical Examples:**

```bash
# Telnet to POP3 server
telnet pop.example.com 110
USER john
PASS password123
STAT
LIST
RETR 1
DELE 1
QUIT

# Using Thunderbird email client (POP3 config)
Server: pop.example.com
Port: 110
Username: john
Password: password123
Security: None / STARTTLS / SSL/TLS

# Python POP3
import poplib
pop = poplib.POP3('pop.example.com')
pop.user('john')
pop.pass_('password123')
num_messages = len(pop.list()[1])
for i in range(num_messages):
    print(pop.retr(i+1)[0])
pop.quit()

# Using POP3S (encrypted)
import poplib
pop = poplib.POP3_SSL('pop.example.com', 995)
pop.user('john')
pop.pass_('password123')
```

**Use Cases:**
- Email clients (Outlook, Thunderbird)
- Single-device email retrieval
- Automated email processing
- Email backup and archival
- Legacy email system integration

**Limitations:**
- No server-side folder structure
- Emails typically deleted after download
- No synchronization across devices
- Limited for multi-device access

**POP3 vs IMAP:**
- POP3: Download and delete (single device)
- IMAP: Keep on server (multi-device synchronization)

---

### IMAP (Internet Message Access Protocol)

**Overview:**
Advanced protocol for retrieving emails from mail server. Designed for multi-client, multi-device synchronization with server-side folder management.

**Key Characteristics:**
- TCP port 143 (plain), 993 (IMAPS - encrypted)
- Server-side folder management
- Email stays on server (unless explicitly deleted)
- Multi-device synchronization
- Offline read (mailbox cached locally)
- Supports server-side searching
- Advanced protocol with many features

**IMAP States:**

Not Authenticated State:
- LOGIN: Authenticate user
- AUTHENTICATE: SASL authentication
- STARTTLS: Start encryption
- CAPABILITY: List server capabilities

Authenticated State:
- SELECT: Select mailbox
- EXAMINE: Read-only mailbox access
- CREATE: Create mailbox
- DELETE: Delete mailbox
- LIST: List mailboxes

Selected State:
- FETCH: Retrieve message data
- STORE: Modify flags
- COPY: Copy messages to another mailbox
- MOVE: Move messages (IMAP4rev1 extension)
- SEARCH: Search mailbox
- UID: Work with message UIDs

---

## SMTP, POP3, IMAP Commands Summary

**Common Email Server Ports:**

```
SMTP: 25 (relay), 587 (submission), 465 (SMTPS)
POP3: 110 (plain), 995 (POP3S)
IMAP: 143 (plain), 993 (IMAPS)
```

**IMAP Commands Deep Dive:**

```
LOGIN username password           - Authenticate
CAPABILITY                        - Server capabilities
SELECT INBOX                      - Select folder
EXAMINE INBOX                     - Read-only folder access
LIST "" "*"                       - List all folders
CREATE "folder_name"              - Create folder
DELETE "folder_name"              - Delete folder
RENAME "old_name" "new_name"      - Rename folder
FETCH 1:5 (RFC822)               - Fetch messages 1-5 full
FETCH 1 (BODY[HEADER])           - Fetch headers only
FETCH 1 (BODY[TEXT])             - Fetch body only
FETCH 1 (ENVELOPE)               - Fetch envelope (metadata)
STORE 1 +FLAGS (\Seen)          - Mark as read
STORE 1 +FLAGS (\Flagged)       - Star message
STORE 1 -FLAGS (\Seen)          - Mark as unread
SEARCH UNSEEN                     - Search unread messages
SEARCH FROM "john@example.com"    - Search by sender
SEARCH SUBJECT "test"             - Search by subject
SEARCH SINCE 1-JAN-2024           - Search by date
COPY 1:5 "Archive"                - Copy messages
MOVE 1:5 "Trash"                  - Move messages
UID FETCH 123 (RFC822)            - Fetch by unique ID
LOGOUT                            - Close connection
```

**Practical Examples:**

```bash
# Telnet to IMAP server
telnet imap.gmail.com 143
LOGIN john password123
CAPABILITY
LIST "" "*"
SELECT INBOX
FETCH 1:5 (RFC822)
SEARCH UNSEEN
STORE 1 +FLAGS (\Seen)
LOGOUT

# Python IMAP4
import imaplib
imap = imaplib.IMAP4('imap.gmail.com', 993)
# Note: Use IMAP4_SSL for encrypted connection
imap = imaplib.IMAP4_SSL('imap.gmail.com')
imap.login('john', 'password123')
imap.select('INBOX')
status, data = imap.search(None, 'UNSEEN')
for email_id in data[0].split():
    status, msg_data = imap.fetch(email_id, '(RFC822)')
    print(msg_data[0][1])
imap.close()
imap.logout()

# Python email parsing
import email
for response_part in msg_data:
    if isinstance(response_part, tuple):
        msg = email.message_from_bytes(response_part[1])
        print(f"From: {msg['From']}")
        print(f"Subject: {msg['Subject']}")
        print(f"Body: {msg.get_payload()}")

# Thunderbird IMAP configuration
Server: imap.gmail.com
Port: 993
Username: john@gmail.com
Password: app_password (not regular password for Gmail)
Security: SSL/TLS

# Gmail IMAP enablement
# Need to enable "Less secure app access" or use "App Password"
```

**Gmail IMAP Labels:**
```
[Gmail]/All Mail
[Gmail]/Drafts
[Gmail]/Important
[Gmail]/Sent Mail
[Gmail]/Spam
[Gmail]/Starred
[Gmail]/Trash
```

**IMAP vs POP3:**

| Feature | IMAP | POP3 |
|---------|------|------|
| Server-side folder | Yes | No |
| Multi-device | Yes | No |
| Synchronization | Yes | No |
| Offline mode | Cached | Downloaded |
| Email deletion | Optional | Typical |
| Complexity | High | Low |
| Best for | Multiple devices | Single device |

**Use Cases:**
- Modern email clients (Gmail, Outlook, Apple Mail)
- Multi-device email access
- Server-side folder management
- Synchronized inbox across devices
- Mobile email applications
- Enterprise email systems

---

### DNS (Domain Name System)

**Overview:**
Distributed hierarchical naming system converting domain names to IP addresses. Fundamental to internet functionality, translates human-readable domain names to machine-readable IP addresses.

**DNS Hierarchy:**

```
Root Nameserver (.)
    ↓
Top-Level Domain (com, org, net, edu, etc.)
    ↓
Authoritative Nameserver (example.com)
    ↓
Recursive Resolver (ISP DNS, 8.8.8.8, 1.1.1.1)
```

**DNS Record Types:**

| Type | Purpose | Example |
|------|---------|---------|
| A | IPv4 address | example.com → 93.184.216.34 |
| AAAA | IPv6 address | example.com → 2606:2800:220:1:248:1893:25c8:1946 |
| CNAME | Canonical name (alias) | www.example.com → example.com |
| MX | Mail exchange server | example.com MX 10 mail.example.com |
| NS | Nameserver | example.com NS ns1.example.com |
| SOA | Start of Authority | Zone info, serial, refresh, retry |
| TXT | Text record | SPF, DKIM, DMARC policies |
| SRV | Service record | _sip._tcp.example.com |
| PTR | Reverse DNS | 34.216.184.93 → example.com |

**DNS Query Process:**

```
1. User enters example.com in browser
2. Recursive resolver queries root nameserver
3. Root returns TLD nameserver address
4. Recursive resolver queries TLD nameserver
5. TLD returns authoritative nameserver address
6. Recursive resolver queries authoritative nameserver
7. Authoritative returns IP address
8. Recursive resolver returns IP to user
9. Browser connects to IP address
```

**Practical Examples:**

```bash
# nslookup - query DNS
nslookup example.com
nslookup -type=MX example.com
nslookup -type=TXT example.com
nslookup google.com 8.8.8.8  # Query specific resolver

# dig - detailed DNS lookup
dig example.com
dig +short example.com
dig example.com MX
dig example.com TXT
dig @ns1.example.com example.com
dig +trace example.com  # Show resolution path

# host - simple DNS lookup
host example.com
host -t MX example.com
host -t TXT example.com

# Reverse DNS lookup
nslookup 93.184.216.34
dig -x 93.184.216.34

# Check name servers
dig example.com NS

# DNS propagation check
nslookup example.com

# Python DNS queries
import dns.resolver
answers = dns.resolver.resolve('example.com', 'A')
for rdata in answers:
    print(rdata)

# Python reverse DNS
import socket
socket.gethostbyaddr('93.184.216.34')

# Configure DNS server
# Linux /etc/resolv.conf
nameserver 8.8.8.8
nameserver 8.8.4.4

# Windows
# Control Panel → Network and Internet → Network Connections
# Properties → Internet Protocol Version 4 → Properties
# Use following DNS servers:
# Preferred: 8.8.8.8
# Alternate: 8.8.4.4
```

**Public DNS Servers:**

```
Google DNS: 8.8.8.8, 8.8.4.4
Cloudflare: 1.1.1.1, 1.0.0.1
OpenDNS: 208.67.222.222, 208.67.220.220
Quad9: 9.9.9.9, 149.112.112.112
```

**DNS Security (DNSSEC):**

```bash
# Validate DNSSEC
dig +dnssec example.com

# Check DNSSEC status
delv @8.8.8.8 example.com
```

**Common DNS Issues:**

```
NXDOMAIN: Non-existent domain
SERVFAIL: Server failure
REFUSED: Server refused query
TIMEOUT: No response from nameserver
```

**Use Cases:**
- Website access
- Email delivery (MX records)
- Service discovery (SRV records)
- Security policies (SPF, DKIM, DMARC)
- Content delivery networks (CDN)
- Load balancing
- Disaster recovery

---

### DHCP (Dynamic Host Configuration Protocol)

**Overview:**
Protocol for automatically assigning IP addresses and network configuration to devices on a network. Eliminates manual IP configuration.

**Key Characteristics:**
- UDP ports 67 (server), 68 (client)
- Automatic IP allocation
- Lease-based allocation
- Configuration parameters (gateway, DNS, subnet mask)
- Stateful protocol
- DHCP Discover, Offer, Request, Acknowledge (DORA)

**DHCP Process (DORA):**

```
DISCOVER: Client broadcasts DHCPDiscover
          (looking for DHCP servers)

OFFER: Server responds DHCPOffer
       (offers IP address, lease time)

REQUEST: Client broadcasts DHCPRequest
         (requests offered IP)

ACKNOWLEDGE: Server responds DHCPAck
             (confirms IP lease)
```

**DHCP Lease Lifecycle:**

```
T0: Lease acquired, lease_time = 8 hours
T1: After 4 hours (T/2), renewal attempt
T2: After 7 hours (7*T/8), rebinding attempt
T8: Lease expires, new DHCP request required
```

**DHCP Parameters (Options):**

```
Option 1: Subnet Mask
Option 3: Router (default gateway)
Option 6: DNS servers
Option 15: Domain name
Option 44: WINS servers
Option 51: Lease time
Option 67: Bootfile name
```

**Practical Examples:**

```bash
# Check DHCP configuration on Linux
cat /etc/dhcp/dhcpd.conf

# DHCP server configuration example
subnet 192.168.1.0 netmask 255.255.255.0 {
  range 192.168.1.100 192.168.1.200;
  option routers 192.168.1.1;
  option domain-name-servers 8.8.8.8, 8.8.4.4;
  default-lease-time 86400;
  max-lease-time 604800;
}

# Renew DHCP lease on Linux
sudo dhclient -r eth0  # Release
sudo dhclient eth0     # Renew

# Check current DHCP lease
cat /var/lib/dhcp/dhclient.eth0.leases

# DHCP server startup (ISC DHCP)
sudo systemctl start isc-dhcp-server

# View DHCP leases
cat /var/lib/dhcp/dhcpd.leases

# Python DHCP client
from dhcplib import DhcpClient
client = DhcpClient('eth0')
config = client.get_ip()
print(f"IP: {config['ip']}")
print(f"Gateway: {config['gateway']}")
print(f"DNS: {config['dns']}")
```

**Static IP Configuration:**

```bash
# Linux network config
# /etc/network/interfaces
auto eth0
iface eth0 inet static
    address 192.168.1.100
    netmask 255.255.255.0
    gateway 192.168.1.1
    dns-nameservers 8.8.8.8 8.8.4.4

# Apply changes
sudo systemctl restart networking

# netplan (modern Ubuntu)
# /etc/netplan/01-netcfg.yaml
network:
  version: 2
  ethernets:
    eth0:
      dhcp4: no
      addresses: [192.168.1.100/24]
      gateway4: 192.168.1.1
      nameservers:
        addresses: [8.8.8.8, 8.8.4.4]
```

**Use Cases:**
- Automatic IP configuration in home networks
- Office network deployment
- Mobile network management
- IoT device provisioning
- Cloud network configuration
- Virtual machine networking

---

### SNMP (Simple Network Management Protocol)

**Overview:**
Protocol for monitoring and managing network devices. Collects performance metrics from routers, switches, servers, printers, etc.

**Key Characteristics:**
- UDP ports 161 (agent), 162 (trap/inform)
- Three versions: SNMPv1 (no security), SNMPv2c (limited), SNMPv3 (secure)
- Agent-manager model
- Hierarchical object naming (OID tree)
- Management Information Base (MIB)
- Traps (unsolicited notifications)

**SNMP Versions:**

SNMPv1:
- Basic protocol
- Community-based security (public/private)
- No encryption
- Limited functionality

SNMPv2c:
- Improved error handling
- Community-based
- Better performance

SNMPv3:
- User-based security
- Authentication
- Encryption
- Recommended for production

**SNMP Operations:**

```
GET: Retrieve single object value
GETNEXT: Retrieve next object in MIB tree
GETBULK: Retrieve multiple objects efficiently
SET: Modify object value
TRAP: Unsolicited notification from agent
INFORM: Acknowledged notification
```

**Common SNMP Objects (OIDs):**

```
1.3.6.1.2.1.1.1.0 - sysDescr (system description)
1.3.6.1.2.1.1.3.0 - sysUpTime (system uptime)
1.3.6.1.2.1.1.5.0 - sysName (system name)
1.3.6.1.2.1.25.3.2.1.5.1 - CPU usage
1.3.6.1.2.1.25.2.3.1.6.1 - Memory usage
1.3.6.1.2.1.2.2.1.10.1 - Bytes in
1.3.6.1.2.1.2.2.1.16.1 - Bytes out
1.3.6.1.2.1.4.3.0 - TCP connections
```

**Practical Examples:**

```bash
# SNMP GET (retrieve value)
snmpget -v2c -c public 192.168.1.1 1.3.6.1.2.1.1.1.0

# SNMP GETNEXT
snmpgetnext -v2c -c public 192.168.1.1 1.3.6.1.2.1.1

# SNMP WALK (traverse entire tree)
snmpwalk -v2c -c public 192.168.1.1 1.3.6.1.2.1.1

# SNMP with hostname/OID name
snmpget -v2c -c public 192.168.1.1 sysDescr.0
snmpwalk -v2c -c public 192.168.1.1 system

# SNMPv3 (with authentication)
snmpget -v3 -u username -a SHA -A password \
  -x AES -X privpassword 192.168.1.1 sysDescr.0

# SNMP SET (modify value)
snmpset -v2c -c private 192.168.1.1 1.3.6.1.2.1.1.5.0 \
  s "New System Name"

# Monitor device metrics
snmpget -v2c -c public 192.168.1.1 \
  1.3.6.1.2.1.1.1.0 \  # sysDescr
  1.3.6.1.2.1.1.3.0 \  # sysUpTime
  1.3.6.1.2.1.25.3.2.1.5.1  # CPU

# Python SNMP
from pysnmp.hlapi import *
error = False
for errorIndication, errorStatus, errorIndex, varBinds in getCmd(
    SnmpEngine(),
    CommunityData('public', mpModel=0),
    UdpTransportTarget(('192.168.1.1', 161), timeout=1.0, retries=2),
    ContextData(),
    '1.3.6.1.2.1.1.1.0'
):
    if errorIndication:
        print(errorIndication)
        error = True
    elif errorStatus:
        print(f'{errorStatus.prettyPrint()}')
        error = True
    else:
        for varBind in varBinds:
            print(' = '.join([x.prettyPrint() for x in varBind]))
```

**Use Cases:**
- Network device monitoring (routers, switches)
- Server performance monitoring
- Printer monitoring
- UPS monitoring
- Temperature/humidity sensors
- Network traffic analysis
- Automated alerts and notifications

**Monitoring Platforms Using SNMP:**
- Nagios
- Zabbix
- Icinga
- PRTG Network Monitor
- Cacti
- Observium

---

### LDAP (Lightweight Directory Access Protocol)

**Overview:**
Protocol for accessing and maintaining distributed directory information. Used for centralized user authentication and directory services in enterprises.

**Key Characteristics:**
- TCP port 389 (unencrypted), 636 (LDAPS - TLS)
- Client-server model
- Directory Information Tree (DIT)
- Distinguished Names (DN)
- Search filters
- LDIF (LDAP Data Interchange Format)

**LDAP Structure:**

```
DC (Domain Component): dc=example,dc=com
OU (Organizational Unit): ou=Users,ou=IT
CN (Common Name): cn=John Doe
UID (User ID): uid=john

Full DN: cn=John Doe,ou=Users,dc=example,dc=com
```

**LDAP Operations:**

```
BIND: Authenticate to directory
UNBIND: Close connection
SEARCH: Search directory entries
MODIFY: Change entry attributes
ADD: Add new entry
DELETE: Delete entry
MODDN: Rename/move entry
COMPARE: Compare attribute value
```

**Practical Examples:**

```bash
# LDAP search (find user)
ldapsearch -H ldap://ldap.example.com \
  -D "cn=admin,dc=example,dc=com" \
  -w password \
  -b "dc=example,dc=com" \
  "(uid=john)"

# Search specific attributes
ldapsearch -H ldap://ldap.example.com \
  -b "dc=example,dc=com" \
  "(uid=john)" uid mail cn

# Add new user entry (LDIF format)
# add_user.ldif
dn: uid=jane,ou=Users,dc=example,dc=com
objectClass: inetOrgPerson
uid: jane
cn: Jane Smith
sn: Smith
mail: jane@example.com
userPassword: {SHA}password_hash

# Apply LDIF changes
ldapadd -H ldap://ldap.example.com \
  -D "cn=admin,dc=example,dc=com" \
  -w password \
  -f add_user.ldif

# Modify user entry
ldapmodify -H ldap://ldap.example.com \
  -D "cn=admin,dc=example,dc=com" \
  -w password << EOF
dn: uid=john,ou=Users,dc=example,dc=com
changetype: modify
replace: mail
mail: john.newmail@example.com
EOF

# Delete entry
ldapdelete -H ldap://ldap.example.com \
  -D "cn=admin,dc=example,dc=com" \
  -w password \
  "uid=john,ou=Users,dc=example,dc=com"

# Python LDAP
import ldap3
server = ldap3.Server('ldap.example.com', get_info=ldap3.ALL)
conn = ldap3.Connection(server, user='cn=admin,dc=example,dc=com',
                        password='password', auto_bind=True)
conn.search('dc=example,dc=com', '(uid=john)',
            attributes=['uid', 'mail', 'cn'])
for entry in conn.entries:
    print(entry)
conn.unbind()
```

**LDAP Search Filters:**

```
(uid=john)                          - Exact match
(cn=*Smith)                        - Wildcard
(|(uid=john)(uid=jane))            - OR operator
(&(objectClass=person)(uid=john))  - AND operator
(!(uid=admin))                     - NOT operator
(mail=*@example.com)               - Domain matching
```

**Use Cases:**
- Centralized user authentication (Active Directory)
- Employee directory
- Access control and authorization
- Email address books
- Organization hierarchies
- Linux/Unix user management
- Application authentication

**Active Directory (Microsoft LDAP):**

```
# Common LDAP names in Active Directory
cn=John Doe,cn=Users,dc=example,dc=com
```

---

### NTP (Network Time Protocol)

**Overview:**
Protocol for synchronizing time across networked devices. Ensures accurate time on all systems, critical for logs, security, and distributed systems.

**Key Characteristics:**
- UDP port 123
- Hierarchical time servers (stratum levels)
- Sub-millisecond accuracy
- Stratum 0: Atomic clock/GPS
- Stratum 1: Directly connected to stratum 0
- Stratum 16: Unsynchronized
- NTP v3, v4 current versions

**Practical Examples:**

```bash
# Check NTP status
ntpstat

# View NTP peers
ntpq -p

# Set time manually (if needed before NTP)
sudo date -s "$(curl -s --head http://www.example.com | grep '^Date:' | sed 's/Date: //')"

# Start NTP daemon
sudo systemctl start ntp
sudo systemctl start ntpd

# NTP configuration (/etc/ntp.conf)
# Specify time servers
server 0.pool.ntp.org iburst
server 1.pool.ntp.org iburst
server 2.pool.ntp.org iburst
server 3.pool.ntp.org iburst

# Allow clients to query
restrict default nomodify notrap
restrict 127.0.0.1

# Check NTP timing
ntpq -p

# Chrony (modern NTP alternative)
chronyc tracking
chronyc sources

# Verify time sync
date
timedatectl

# Python NTP
from ntplib import NTPClient
client = NTPClient()
response = client.request('pool.ntp.org', version=3)
print(f"Time: {response.tx_time}")
```

**NTP Pool Public Servers:**

```
pool.ntp.org
0.pool.ntp.org
1.pool.ntp.org
2.pool.ntp.org
3.pool.ntp.org
```

**Use Cases:**
- System clock synchronization
- Log timestamp accuracy
- Distributed system coordination
- Security (authentication uses time)
- Trading systems (critical for orders)
- Data center time management
- Cloud infrastructure

---

### RTSP (Real Time Streaming Protocol)

**Overview:**
Protocol for controlling delivery of real-time media (audio/video). Not for streaming data itself, but for controlling streams.

**Key Characteristics:**
- TCP port 554
- Client-server model
- Similar to HTTP but for streaming
- Works with RTP (Real-time Transport Protocol)
- Methods: SETUP, PLAY, PAUSE, TEARDOWN, DESCRIBE

**RTSP Methods:**

```
DESCRIBE: Get media description (SDP)
SETUP: Set up media stream (specify transport)
PLAY: Start playback
PAUSE: Pause playback
TEARDOWN: Stop playback and close connection
OPTIONS: Query server capabilities
REDIRECT: Redirect client to another server
RECORD: Record media stream
ANNOUNCE: Send session description
GET_PARAMETER: Get parameter value
SET_PARAMETER: Set parameter value
```

**Practical Examples:**

```bash
# Play RTSP stream with VLC
vlc rtsp://streaming.example.com/video.mp4

# Telnet to RTSP server
telnet streaming.example.com 554
DESCRIBE rtsp://streaming.example.com/video.mp4 RTSP/1.0
CSeq: 1
User-Agent: Custom

SETUP rtsp://streaming.example.com/video.mp4 RTSP/1.0
Transport: RTP/AVP/TCP;unicast
CSeq: 2

PLAY rtsp://streaming.example.com/video.mp4 RTSP/1.0
CSeq: 3

PAUSE rtsp://streaming.example.com/video.mp4 RTSP/1.0
CSeq: 4

TEARDOWN rtsp://streaming.example.com/video.mp4 RTSP/1.0
CSeq: 5

# FFmpeg with RTSP
ffmpeg -rtsp_transport tcp -i rtsp://streaming.example.com/stream.mp4 -c copy output.mp4

# Python RTSP client
import rtplib
client = rtplib.RTPClient('streaming.example.com', 554)
client.describe('video.mp4')
client.setup('video.mp4')
client.play('video.mp4')
```

**Use Cases:**
- IP camera surveillance
- Video streaming services
- Live broadcasts
- Remote monitoring systems
- Video conferencing

**Related Protocols:**
- RTCP: Real-time Control Protocol (feedback)
- RTP: Real-time Transport Protocol (actual data)
- SDP: Session Description Protocol (media format info)

---

## TRANSPORT LAYER PROTOCOLS (Layer 4)

### TCP (Transmission Control Protocol)

**Overview:**
Reliable, connection-oriented protocol ensuring ordered delivery of data. TCP establishes connections, ensures data integrity, and controls flow.

**Key Characteristics:**
- Connection-oriented (three-way handshake)
- Reliable delivery (retransmission)
- Ordered delivery
- Flow control (windowing)
- Congestion control
- Bidirectional communication
- Port-based addressing (ports 0-65535)

**TCP Three-Way Handshake:**

```
SYN: Client sends SYN packet to server (seq=x)
     Server receives, records client sequence

SYN-ACK: Server responds with SYN-ACK (seq=y, ack=x+1)
         Client receives, records server sequence

ACK: Client sends ACK (seq=x+1, ack=y+1)
     Server receives, connection established

Data can now flow in both directions
```

**TCP Flags:**

```
SYN: Synchronize sequence numbers (initiate connection)
ACK: Acknowledge received data
FIN: Finish connection (graceful close)
RST: Reset connection
PSH: Push data immediately
URG: Urgent data pointer valid
```

**TCP Connection Closure:**

```
FIN-WAIT-1: One side sends FIN
CLOSE-WAIT: Other side receives FIN, sends ACK
FIN-WAIT-2: Sender waits for FIN
TIME-WAIT: Waiting to ensure FIN received
CLOSED: Connection closed

Full sequence:
Client sends FIN → Server sends ACK → Server sends FIN → Client sends ACK
```

**TCP Options:**

```
MSS (Maximum Segment Size): Largest segment size
Window Scale: Increase window size for high-speed links
Selective Acknowledgment (SACK): Acknowledge specific segments
Timestamps: Timestamp each segment
EOF (End of Options): Mark end of options
```

**Practical Examples:**

```bash
# View TCP connections
netstat -tuln
netstat -an | grep ESTABLISHED
ss -tuln

# Monitor TCP traffic
tcpdump -i eth0 'tcp port 80'
tcpdump -i eth0 'tcp port 443'

# TCP socket testing with nc (netcat)
# Server: Listen on port 5000
nc -l -p 5000

# Client: Connect to server
nc localhost 5000
# Can type messages, transmitted reliably

# Python TCP server
import socket
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(('0.0.0.0', 5000))
server.listen(1)
client, addr = server.accept()
data = client.recv(1024)
print(f"Received: {data.decode()}")
client.send(b"Message received")
client.close()

# Python TCP client
import socket
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect(('localhost', 5000))
client.send(b"Hello Server")
response = client.recv(1024)
print(f"Response: {response.decode()}")
client.close()

# Telnet (TCP client)
telnet example.com 80
GET / HTTP/1.1
Host: example.com

# Check TCP timeout
ss -tapi  # Shows TCP state and timers

# TCP performance tuning
# /etc/sysctl.conf
net.ipv4.tcp_tw_reuse = 1
net.ipv4.tcp_fin_timeout = 30
net.core.somaxconn = 1024
net.ipv4.tcp_max_syn_backlog = 4096
```

**TCP Congestion Control:**

```
Slow Start: Exponentially increase send rate
Congestion Avoidance: Linearly increase send rate
Fast Retransmit: Detect packet loss quickly
Fast Recovery: Recover from congestion
```

**Use Cases:**
- HTTP/HTTPS (web)
- Email (SMTP, POP3, IMAP)
- File transfer (FTP, SFTP)
- Remote login (SSH, Telnet)
- Database connections
- Any application requiring reliable delivery

---

### UDP (User Datagram Protocol)

**Overview:**
Unreliable, connectionless protocol for fast transmission of data. No connection establishment, no delivery guarantees, but lower overhead than TCP.

**Key Characteristics:**
- Connectionless (no handshake)
- Unreliable delivery (no retransmission)
- No flow control
- No congestion control
- Low latency
- Fixed header size (8 bytes)
- Broadcast/multicast capable
- Better for real-time applications

**UDP Packet Structure:**

```
Source Port (16 bits)
Destination Port (16 bits)
Length (16 bits)
Checksum (16 bits)
Data (payload)
```

**Practical Examples:**

```bash
# UDP netcat server
nc -u -l -p 5000

# UDP netcat client
nc -u localhost 5000
# Type messages

# Monitor UDP traffic
tcpdump -i eth0 'udp port 53'

# Python UDP server
import socket
server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server.bind(('0.0.0.0', 5000))
data, addr = server.recvfrom(1024)
print(f"Received from {addr}: {data.decode()}")
server.sendto(b"Message received", addr)

# Python UDP client
import socket
client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client.sendto(b"Hello", ('localhost', 5000))
response, addr = client.recvfrom(1024)
print(f"Response: {response.decode()}")

# Check UDP sockets
netstat -un
ss -un

# UDP performance testing
iperf -s -u  # Server
iperf -c localhost -u -b 100M  # Client
```

**Use Cases:**
- DNS queries (fast single-query-response)
- VoIP (real-time voice, packet loss acceptable)
- Online gaming (low latency priority)
- Video streaming (buffering handles loss)
- IoT sensor data (fire-and-forget)
- SNMP monitoring (simple queries)
- Network diagnostics (ping, traceroute)
- Multicast/broadcast applications

**TCP vs UDP:**

| Feature | TCP | UDP |
|---------|-----|-----|
| Connection | Required | Not needed |
| Reliability | Guaranteed | No guarantees |
| Ordering | Guaranteed | Not guaranteed |
| Speed | Slower | Faster |
| Overhead | Higher | Lower |
| Flow control | Yes | No |
| Broadcasting | No | Yes |
| Use case | Reliability critical | Speed critical |

---

## NETWORK LAYER PROTOCOLS (Layer 3)

### IP (Internet Protocol)

**Overview:**
Fundamental protocol for routing and addressing on the internet. Provides logical addressing (IP addresses) and packet forwarding.

**IPv4:**
- 32-bit address space (4.3 billion addresses)
- Dotted decimal notation (192.168.1.1)
- Subnet mask for address space partitioning
- Fragmentation when packet exceeds MTU

**IPv6:**
- 128-bit address space (340 undecillion addresses)
- Hexadecimal colon notation (2001:db8::1)
- No fragmentation (handled by transport layer)
- Built-in security features
- Improved header efficiency

**Practical Examples:**

```bash
# Check IP address
ifconfig
ip addr show
ip -4 addr show  # IPv4 only
ip -6 addr show  # IPv6 only

# Configure IP address (temporary)
sudo ip addr add 192.168.1.100/24 dev eth0

# Configure IP address (permanent)
# /etc/network/interfaces
auto eth0
iface eth0 inet static
    address 192.168.1.100
    netmask 255.255.255.0
    gateway 192.168.1.1

# Check routing table
route -n
ip route show

# Add route
sudo ip route add 10.0.0.0/8 via 192.168.1.1

# Ping to test IP connectivity
ping example.com
ping -c 4 8.8.8.8

# Traceroute to see packet path
traceroute example.com

# Check IPv6 configuration
ip -6 addr show
ping6 example.com

# Enable IPv6 (if disabled)
sudo sysctl -w net.ipv6.conf.all.disable_ipv6=0
```

---

### ICMP (Internet Control Message Protocol)

**Overview:**
Protocol for diagnostics and error reporting in IP networks. Used for ping and traceroute.

**ICMP Message Types:**

```
Echo Request (Type 8): Ping request
Echo Reply (Type 0): Ping response
Destination Unreachable (Type 3): Cannot reach destination
Time Exceeded (Type 11): TTL reached 0
Redirect (Type 5): Better route available
Parameter Problem (Type 12): Invalid IP header
Timestamp Request (Type 13): Request time
Timestamp Reply (Type 14): Send time
```

**Practical Examples:**

```bash
# Ping (ICMP Echo)
ping example.com
ping -c 4 example.com
ping -i 0.2 -c 100 example.com  # High-frequency ping (stress test)

# Ping options
ping -s 1472 example.com  # Large packet size
ping -t 1 example.com    # TTL 1 (only local network)

# Traceroute (ICMP Time Exceeded)
traceroute example.com
traceroute -m 30 example.com  # Max 30 hops
traceroute -I example.com     # Use ICMP (instead of UDP)

# Windows tracert
tracert example.com

# Detailed ICMP analysis with tcpdump
tcpdump -i eth0 icmp

# Disable ICMP responses (security)
echo 1 | sudo tee /proc/sys/net/ipv4/icmp_echo_ignore_all

# Python ICMP (ping)
import subprocess
result = subprocess.run(['ping', '-c', '4', 'example.com'],
                       capture_output=True, text=True)
print(result.stdout)
```

**Use Cases:**
- Network connectivity testing (ping)
- Route diagnosis (traceroute)
- Error reporting
- Network performance analysis

---

### IGMP (Internet Group Management Protocol)

**Overview:**
Protocol for managing multicast group membership. Allows devices to join multicast groups to receive group traffic.

**IGMP Versions:**
- IGMPv1: Basic multicast
- IGMPv2: Leave Group message
- IGMPv3: Source-specific multicast

**Practical Examples:**

```bash
# Join multicast group
# Command varies by system, typically in application code

# Monitor multicast with tcpdump
tcpdump -i eth0 'host 224.0.0.0/4'

# Python multicast sender
import socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 2)
sock.sendto(b"Hello Multicast", ('224.0.0.1', 5000))

# Python multicast receiver
import socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.bind(('', 5000))
group = socket.inet_aton('224.0.0.1')
mreq = group + socket.inet_aton('0.0.0.0')
sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)
while True:
    data, addr = sock.recvfrom(1024)
    print(f"Received: {data} from {addr}")
```

**Use Cases:**
- Video streaming (IPTV)
- Multicast messaging
- Network-wide announcements
- Audio distribution

---

## DATA LINK LAYER PROTOCOLS (Layer 2)

### Ethernet

**Overview:**
Most common LAN technology. Defines frame format and access methods for local network communication.

**Ethernet Frame Structure:**

```
Preamble (7 bytes): 10101010...
Start Frame Delimiter (1 byte): 10101011
Destination MAC (6 bytes): Target hardware address
Source MAC (6 bytes): Sender hardware address
Type/Length (2 bytes): Protocol type or frame length
Payload (46-1500 bytes): Data
FCS/CRC (4 bytes): Frame check sequence
```

**MAC Address Format:**

```
48-bit address: 48 bits split as AA:BB:CC:DD:EE:FF
First 3 bytes: Manufacturer ID (OUI)
Last 3 bytes: Device unique ID
```

**Ethernet Speeds:**

```
Fast Ethernet: 100 Mbps (100BASE-TX)
Gigabit Ethernet: 1 Gbps (1000BASE-T)
10 Gigabit Ethernet: 10 Gbps (10GBASE-T)
100 Gigabit Ethernet: 100 Gbps (100GBASE-R)
```

**Practical Examples:**

```bash
# View network interfaces
ifconfig
ip link show

# View MAC addresses
arp -a
ip neigh show

# Change MAC address (MAC spoofing)
sudo ip link set dev eth0 address aa:bb:cc:dd:ee:ff

# Monitor Ethernet traffic
tcpdump -i eth0
tcpdump -i eth0 'ether dst aa:bb:cc:dd:ee:ff'
```

---

### ARP (Address Resolution Protocol)

**Overview:**
Protocol for mapping IP addresses to MAC addresses on local network. Essential for Layer 2 frame delivery.

**ARP Process:**

```
1. Sender knows IP, needs MAC
2. Sender broadcasts ARP Request "Who has this IP?"
3. Device with IP responds ARP Reply "This is my MAC"
4. Sender caches ARP entry
5. Sender sends frame to MAC address
```

**Practical Examples:**

```bash
# View ARP cache
arp -a
arp -e
ip neigh show

# Clear ARP cache entry
arp -d 192.168.1.1
sudo ip neigh del 192.168.1.1 dev eth0

# Flush all ARP cache
sudo arp -n -a | awk '{print $1}' | xargs -i arp -d {}

# Monitor ARP traffic
tcpdump -i eth0 'arp'

# ARP spoofing detection
# Tools: Arpwatch, snort

# Python ARP
import scapy.all as scapy
arp_request = scapy.ARP(pdst='192.168.1.1')
broadcast = scapy.Ether(dst='ff:ff:ff:ff:ff:ff')
arp_request_broadcast = broadcast / arp_request
answered, unanswered = scapy.srp(arp_request_broadcast, timeout=1, verbose=False)
for send, receive in answered:
    print(f"IP: {receive.psrc}, MAC: {receive.hwsrc}")
```

**Use Cases:**
- Local network address resolution
- Network discovery
- ARP spoofing attacks (security concern)

---

## ROUTING PROTOCOLS

### RIP (Routing Information Protocol)

**Overview:**
Distance-vector routing protocol using hop count as metric. Simple but limited (max 15 hops).

**Characteristics:**
- Distance-vector algorithm
- Hop count metric (max 15)
- RIPv1, RIPv2 versions
- Slow convergence
- Limited scalability
- Largely replaced by OSPF

**Practical Examples:**

```bash
# Quagga/FRR RIP configuration
# /etc/frr/ripd.conf
router rip
  network 10.0.0.0/8
  network 172.16.0.0/12
  neighbor 10.0.0.1

# View RIP routes
show ip rip
show ip rip database
```

---

### OSPF (Open Shortest Path First)

**Overview:**
Link-state routing protocol with flexible metrics. Widely used in enterprise networks.

**Characteristics:**
- Link-state algorithm
- Flexible metrics (cost based)
- Hierarchical areas
- Fast convergence
- OSPF v2 (IPv4), v3 (IPv6)
- Uses Dijkstra algorithm

**Practical Examples:**

```bash
# Quagga/FRR OSPF configuration
# /etc/frr/ospfd.conf
router ospf
  ospf router-id 10.0.0.1
  network 10.0.0.0/24 area 0
  network 172.16.0.0/24 area 1

# View OSPF routes
show ip ospf route
show ip ospf database

# Quagga CLI
vtysh
# In vtysh
show ip ospf neighbor
show ip route ospf
```

---

### BGP (Border Gateway Protocol)

**Overview:**
Exterior Gateway Protocol for routing between autonomous systems (internet backbone).

**Characteristics:**
- Path-vector algorithm
- Policy-based routing
- Scalable to internet size
- BGP v4 (current standard)
- Complex configuration

**Practical Examples:**

```bash
# BGP configuration
# /etc/frr/bgpd.conf
router bgp 65001
  bgp router-id 10.0.0.1
  neighbor 10.0.0.2 remote-as 65002
  neighbor 10.0.0.2 description "ISP Connection"

  address-family ipv4 unicast
    network 192.168.0.0/24
    neighbor 10.0.0.2 activate
    neighbor 10.0.0.2 prefix-list EXPORT out
  exit-address-family

# View BGP routes
show ip bgp
show ip bgp neighbors
show ip route bgp
```

---

## SECURITY & ENCRYPTION PROTOCOLS

### SSL/TLS (Secure Sockets Layer / Transport Layer Security)

**Overview:**
Cryptographic protocols providing encrypted communication and authentication over networks.

**Versions:**
- SSL 2.0: Deprecated
- SSL 3.0: Deprecated (POODLE vulnerability)
- TLS 1.0: Deprecated
- TLS 1.1: Deprecated
- TLS 1.2: Current standard
- TLS 1.3: Latest (2018)

**TLS Handshake:**

```
1. ClientHello: Supported ciphers, protocols, random
2. ServerHello: Selected cipher, protocol, certificate, random
3. Certificate: Server sends public certificate
4. ServerKeyExchange: Additional key exchange data
5. ServerHelloDone: End of server messages
6. ClientKeyExchange: Client sends key material
7. ChangeCipherSpec: Switch to encrypted communication
8. Finished: Encrypted verification
9. ServerFinished: Server verification
```

**Practical Examples:**

```bash
# Check TLS certificate
openssl s_client -connect example.com:443

# View certificate details
openssl x509 -in certificate.crt -text -noout

# Generate certificate request
openssl req -new -key private.key -out certificate.csr

# Check certificate validity
openssl verify certificate.crt

# Python TLS
import ssl
import socket
context = ssl.create_default_context()
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
    with context.wrap_socket(sock, server_hostname='example.com') as ssock:
        ssock.connect(('example.com', 443))
        print(ssock.getpeercert())
```

---

### IPsec (IP Security)

**Overview:**
Network-layer encryption protocol protecting IP traffic. Used in VPNs.

**Components:**
- Authentication Header (AH): Authentication only
- Encapsulating Security Payload (ESP): Encryption + authentication
- Internet Key Exchange (IKE): Key negotiation

**Modes:**
- Transport mode: Encrypt payload only
- Tunnel mode: Encrypt entire packet (used in VPNs)

**Practical Examples:**

```bash
# IPsec VPN configuration (strongSwan)
# /etc/ipsec.conf
conn site-to-site
    type=tunnel
    left=10.0.0.1
    right=10.0.0.2
    leftsubnets=192.168.0.0/24
    rightsubnets=192.168.1.0/24
    ike=aes256-sha256-modp2048!
    esp=aes256-sha256!
    keyingtries=0
    ikelifetime=28800s
    lifetime=3600s
    dpddelay=30s
    dpdtimeout=120s
    dpdaction=restart
    auto=start

# Start IPsec
sudo systemctl start strongswan
sudo ipsec up site-to-site

# Monitor IPsec connections
sudo ipsec status
```

---

## VPN & TUNNELING PROTOCOLS

### PPTP (Point-to-Point Tunneling Protocol)

**Overview:**
VPN protocol creating encrypted tunnels (legacy, weak security).

**Characteristics:**
- TCP port 1723
- GRE tunnel
- Weak encryption (MS-CHAPv2)
- Fast but insecure
- Deprecated for new deployments

---

### L2TP (Layer 2 Tunneling Protocol)

**Overview:**
VPN protocol similar to PPTP but stronger. Often used with IPsec for security.

**Characteristics:**
- Layer 2 tunneling
- Usually combined with IPsec
- UDP ports 500, 4500
- Better than PPTP

---

## WIRELESS PROTOCOLS

### 802.11 (WiFi)

**Overview:**
Wireless LAN standard for local network access.

**Standards:**
- 802.11a: 5 GHz, 54 Mbps
- 802.11b: 2.4 GHz, 11 Mbps
- 802.11g: 2.4 GHz, 54 Mbps
- 802.11n: 2.4/5 GHz, 600 Mbps (MIMO)
- 802.11ac (WiFi 5): 5 GHz, 1.3 Gbps
- 802.11ax (WiFi 6): 2.4/5/6 GHz, 9.6 Gbps (OFDMA)

**Practical Examples:**

```bash
# Scan wireless networks
sudo iwlist wlan0 scan
nmcli dev wifi list

# Connect to WiFi
nmcli device wifi connect "SSID" password "password"

# WiFi interface configuration
iwconfig wlan0
iwconfig wlan0 essid "SSID"
iwconfig wlan0 key xxxxxxxxxxxxxxx
```

---

### Bluetooth

**Overview:**
Short-range wireless technology for personal devices.

**Characteristics:**
- 2.4 GHz frequency
- 10-100 meter range
- Bluetooth Classic, Bluetooth Low Energy (BLE)
- Pairing required
- Packet-based protocol

---

## IoT & LIGHTWEIGHT PROTOCOLS

### MQTT (Message Queuing Telemetry Transport)

**Overview:**
Lightweight publish-subscribe protocol for IoT devices.

**Characteristics:**
- TCP port 1883
- Publish-subscribe model
- Topics hierarchy
- Three QoS levels
- Low bandwidth consumption
- Broker-based architecture

**Practical Examples:**

```bash
# MQTT broker (Mosquitto)
sudo apt install mosquitto mosquitto-clients

# Start broker
sudo systemctl start mosquitto

# Subscribe to topic
mosquitto_sub -h localhost -t "sensor/temperature"

# Publish to topic
mosquitto_pub -h localhost -t "sensor/temperature" -m "25.5"

# Python MQTT
import paho.mqtt.client as mqtt
client = mqtt.Client()
client.connect("localhost", 1883, 60)
client.publish("sensor/temperature", "25.5")

# Subscribe to topic
def on_message(client, userdata, msg):
    print(f"Topic: {msg.topic}, Message: {msg.payload.decode()}")

client.on_message = on_message
client.subscribe("sensor/#")
client.loop_forever()
```

**QoS Levels:**
- 0: At most once (fire and forget)
- 1: At least once (with acknowledgment)
- 2: Exactly once (two-phase commit)

**Use Cases:**
- IoT sensor networks
- Home automation
- Industrial monitoring
- Real-time data collection

---

### CoAP (Constrained Application Protocol)

**Overview:**
Ultra-lightweight protocol for severely constrained IoT devices.

**Characteristics:**
- UDP-based (lighter than TCP)
- Designed for 8-bit microcontrollers
- Very low bandwidth usage
- REST-like interface
- Binary protocol

**Practical Examples:**

```bash
# CoAP client (libcoap)
coap-client coap://coap.me/hello

# Python CoAP
import aiocoap
from aiocoap import resource

async def main():
    context = await aiocoap.Context.create_client_context()
    request = aiocoap.Message(code=aiocoap.GET)
    request.set_request_uri('coap://coap.me/hello')
    response = await context.request(request).response
    print(response.payload.decode())

import asyncio
asyncio.run(main())
```

---

### Zigbee

**Overview:**
Low-power wireless protocol for mesh networks.

**Characteristics:**
- 802.15.4 PHY/MAC layer
- 2.4 GHz frequency
- Very low power
- Mesh networking
- Typical range 10-100 meters

**Use Cases:**
- Home automation
- Industrial sensors
- Wearable devices
- Building control

---

## REAL-TIME & STREAMING PROTOCOLS

### RTP (Real-time Transport Protocol)

**Overview:**
Protocol for delivering real-time media (audio/video) over networks.

**Key Characteristics:**
- UDP-based
- Sequence numbers for ordering
- Timestamps for timing
- Payload type identification
- Works with RTCP for feedback

**Header Fields:**
- Version (2 bits)
- Padding (1 bit)
- Extension (1 bit)
- CSRC count (4 bits)
- Marker (1 bit)
- Payload type (7 bits)
- Sequence number (16 bits)
- Timestamp (32 bits)
- SSRC (32 bits)
- CSRC list (variable)
- Payload

**Use Cases:**
- VoIP
- Video conferencing
- Live streaming
- Audio streaming

---

### RTCP (Real-time Control Protocol)

**Overview:**
Provides feedback and statistics for RTP streams.

**Characteristics:**
- UDP-based
- Works with RTP
- Sender/receiver reports
- Quality feedback
- Synchronization

**Packet Types:**
- SR (Sender Report): Sender statistics
- RR (Receiver Report): Receiver statistics
- SDES (Source Description): Participant info
- BYE: End of participation
- APP: Application-specific

---

## SPECIALIZED & EMERGING PROTOCOLS

### QUIC (Quick UDP Internet Connections)

**Overview:**
Modern protocol combining benefits of TCP and UDP for faster web.

**Characteristics:**
- UDP-based (low overhead)
- 0-RTT connection establishment
- Built-in encryption (TLS 1.3)
- Multiplexing like HTTP/2
- Connection migration
- Used by HTTP/3

**Advantages:**
- Faster connection establishment
- Improved mobile experience
- Reduced head-of-line blocking
- Connection persistence

**Practical Examples:**

```bash
# Test QUIC/HTTP3 connectivity
curl --http3 https://example.com

# Check if server supports HTTP/3
curl -I --http3 https://example.com
```

---

### DNS over HTTPS (DoH)

**Overview:**
DNS queries encrypted over HTTPS for privacy.

**Characteristics:**
- DNS over TLS (DoT) or HTTPS (DoH)
- Port 443 or 853
- Encrypted queries
- Privacy-preserving

**Practical Examples:**

```bash
# Query DNS over HTTPS using curl
curl -H "Accept: application/dns-json" \
  "https://8.8.8.8/resolve?name=example.com&type=A"

# Configure system to use DoH
# In network settings, set DNS server to DoH provider
```

---

### Wireguard VPN

**Overview:**
Modern, streamlined VPN protocol with improved performance.

**Characteristics:**
- Kernel implementation
- 4500 lines of code (vs 100,000+ for OpenVPN)
- Faster than traditional VPNs
- Better mobile support
- Simpler configuration

**Practical Examples:**

```bash
# Wireguard installation
sudo apt install wireguard

# Generate keys
wg genkey | tee privatekey | wg pubkey > publickey

# Configuration file (/etc/wireguard/wg0.conf)
[Interface]
Address = 10.0.0.1/24
ListenPort = 51820
PrivateKey = <server-private-key>

[Peer]
PublicKey = <client-public-key>
AllowedIPs = 10.0.0.2/32

# Enable interface
sudo ip link add dev wg0 type wireguard
sudo ip addr add 10.0.0.1/24 dev wg0
sudo wg set wg0 private-key <(wg genkey)
sudo ip link set wg0 up

# View status
sudo wg show
sudo wg-quick up wg0
```

---

## Protocol Selection Guide

**By Use Case:**

**File Transfer:**
- SFTP (secure, preferred)
- FTP (legacy, insecure)
- SCP (secure copy)

**Remote Access:**
- SSH (encrypted, preferred)
- RDP (Windows remote desktop)
- VNC (remote GUI)

**Web Services:**
- HTTPS (encrypted, preferred)
- HTTP (legacy, insecure)
- HTTP/3 (fastest modern)

**Email:**
- SMTP (sending)
- IMAP (receiving, preferred)
- POP3 (receiving, legacy)

**Real-Time Communication:**
- SIP + RTP (VoIP)
- WebRTC (browser-based)
- RTMP (streaming, legacy)

**IoT:**
- MQTT (general purpose)
- CoAP (extreme constraints)
- Zigbee (mesh networks)

**Monitoring:**
- SNMP (device metrics)
- Syslog (log collection)
- NetFlow (traffic analysis)

**Streaming:**
- RTSP + RTP (video)
- HTTP/HTTPS (progressive)
- HLS (adaptive)

---

## Protocol Stack Examples

**Web Server:**
```
Application: HTTP/HTTPS
Transport: TCP
Network: IP (v4/v6)
Data Link: Ethernet
Physical: Copper/Fiber
```

**Email System:**
```
Application: SMTP (send), IMAP (receive)
Transport: TCP
Network: IP
Data Link: Ethernet
Physical: ISP infrastructure
```

**VoIP Call:**
```
Application: SIP (signaling), RTP (media)
Transport: UDP (RTP), TCP (SIP)
Network: IP
Data Link: Ethernet / WiFi
Physical: Network infrastructure
```

**Video Streaming:**
```
Application: RTSP (control), RTP (media)
Transport: UDP
Network: IP
Data Link: WiFi / Ethernet
Physical: ISP connection
```

---

## Performance Tuning Protocol Stack

**For Low Latency:**
- Use UDP instead of TCP
- Minimize protocol layers
- Enable TCP_NODELAY (disable Nagle)
- Use kernel bypass techniques (DPDK)

**For Reliability:**
- Use TCP with retransmission
- Implement error checking
- Use application-level heartbeats
- Monitor connection health

**For Throughput:**
- Large TCP window size
- Enable jumbo frames (9000 MTU)
- Tune buffer sizes
- Use parallel connections

**For Security:**
- Encrypt with TLS/SSL
- Authenticate all connections
- Validate certificates
- Implement rate limiting

---

## Summary Table: All Major Protocols

| Protocol | Layer | Port | Purpose | Speed | Reliability | Encryption |
|----------|-------|------|---------|-------|-------------|------------|
| HTTP | 7 | 80 | Web browsing | Medium | High | No |
| HTTPS | 7 | 443 | Secure web | Medium | High | Yes (TLS) |
| FTP | 7 | 21 | File transfer | Medium | High | No |
| SFTP | 7 | 22 | Secure transfer | Medium | High | Yes (SSH) |
| SSH | 7 | 22 | Remote shell | Medium | High | Yes |
| SMTP | 7 | 25/587 | Email send | Slow | High | Optional |
| POP3 | 7 | 110 | Email download | Medium | High | Optional |
| IMAP | 7 | 143 | Email sync | Medium | High | Optional |
| DNS | 7 | 53 | Name resolution | Fast | Medium | No |
| DHCP | 7 | 67/68 | IP assignment | Fast | Medium | No |
| SNMP | 7 | 161 | Monitoring | Fast | Low | No |
| LDAP | 7 | 389 | Directory | Medium | Medium | Optional |
| NTP | 7 | 123 | Time sync | Fast | High | No |
| RTSP | 7 | 554 | Stream control | Medium | Medium | Optional |
| TCP | 4 | Varies | Reliable transport | Slower | High | No |
| UDP | 4 | Varies | Fast transport | Faster | Low | No |
| IP | 3 | N/A | Routing | Fast | Medium | No |
| ICMP | 3 | N/A | Diagnostics | Fast | Medium | No |
| IGMP | 3 | N/A | Multicast | Fast | Medium | No |
| OSPF | 3 | N/A | Routing | N/A | High | No |
| BGP | 3 | 179 | Internet routing | Slow | High | Optional |
| RIP | 3 | 520 | Routing | Slow | Low | No |
| Ethernet | 2 | N/A | LAN frame | Very Fast | Medium | No |
| ARP | 2 | N/A | MAC mapping | Fast | Medium | No |
| PPTP | N/A | 1723 | VPN | Medium | Low | Yes |
| L2TP | N/A | 500/4500 | VPN | Medium | High | Yes |
| IPsec | 3 | N/A | VPN encryption | Medium | High | Yes |
| SSL/TLS | 5-6 | Varies | Encryption | Medium | High | Yes |
| 802.11 | 1-2 | N/A | WiFi | Varies | Medium | Optional |
| Bluetooth | 1-2 | N/A | Short-range | Medium | Low | Yes |
| MQTT | 7 | 1883 | IoT messaging | Fast | Configurable | Optional |
| CoAP | 7 | 5683 | IoT protocol | Very Fast | Medium | Optional |
| RTP | 4 | Varies | Media transport | Fast | Low | No |
| RTCP | 4 | Varies | Media feedback | Fast | Low | No |
| QUIC | 4 | Varies | HTTP/3 | Very Fast | High | Yes (TLS) |
| Zigbee | 1-2 | N/A | Mesh IoT | Slow | Medium | Yes |

---

## Conclusion

This comprehensive guide covers **all major networking protocols** with detailed explanations, practical examples, and usage guidance. Understanding these protocols is essential for:

- System administration
- Network engineering
- Application development
- Security implementation
- DevOps operations
- Trading systems architecture
- Cloud infrastructure
- IoT development

Each protocol serves specific purposes in the network stack, and selecting the right protocol for your use case is critical for performance, security, and reliability.

