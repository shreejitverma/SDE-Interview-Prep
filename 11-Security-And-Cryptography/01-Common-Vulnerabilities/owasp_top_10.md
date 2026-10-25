---
type: concept
track: [sde]
level:
status: draft
last_reviewed:
sources: []
---

# OWASP Top 10:2025 (Security Checklist)

This note covers the current OWASP Top 10 edition (2025 edition).
Official standard documentation: [OWASP Top 10:2025](https://owasp.org/Top10/2025/).

1. **A01:2025 - Broken Access Control**
   Enforce the principle of least privilege and deny access by default across all endpoints.
   Verify user authorization and resource ownership on every API call to prevent unauthorized access.
   Allowlist outbound destinations for server-side fetches to prevent SSRF, which the 2025 edition maps into this category.
2. **A02:2025 - Security Misconfiguration**
   Harden configurations using automated security baselines and disable unneeded services or debug endpoints.
   Remove default credentials, restrict error stack traces in production, and enforce secure headers.
3. **A03:2025 - Software Supply Chain Failures**
   Maintain an accurate Software Bill of Materials (SBOM) and continuously audit third-party dependencies.
   Pin dependency versions, verify cryptographic hashes, and validate signatures across CI/CD pipelines.
4. **A04:2025 - Cryptographic Failures**
   Encrypt sensitive data in transit using TLS 1.2 or later (prefer TLS 1.3) and at rest using authenticated encryption like AES-GCM.
   Hash passwords with a slow, salted password-hashing function such as Argon2id (or scrypt, bcrypt, or PBKDF2 where required), and manage cryptographic keys securely.
5. **A05:2025 - Injection**
   Use parameterized queries and prepared statements for all database operations to prevent SQL injection.
   Apply context-aware output encoding and strict input validation to prevent command and interpreter injection.
6. **A06:2025 - Insecure Design**
   Integrate threat modeling and security architecture reviews into early design phases before writing code.
   Establish clear trust boundaries and enforce rate limiting to defend against automated abuse.
7. **A07:2025 - Authentication Failures**
   Enforce multi-factor authentication (MFA) and strong password policies for all accounts.
   Implement rate limiting against brute-force attempts and use secure session tokens.
8. **A08:2025 - Software or Data Integrity Failures**
   Verify cryptographic signatures on software binaries, updates, and pipeline dependencies before execution.
   Avoid unsafe deserialization of untrusted input and use structured interchange formats like JSON or Protocol Buffers.
9. **A09:2025 - Security Logging and Alerting Failures**
   Log security-critical events with sufficient context to an append-only centralized log service.
   Configure real-time monitoring and alerting on anomalous access patterns to detect breaches promptly.
10. **A10:2025 - Mishandling of Exceptional Conditions**
    Handle errors explicitly and ensure that failure paths fail closed without exposing internal diagnostics.
    Ensure systems remain in a secure state during resource exhaustion, network timeouts, or unexpected inputs.

---

## Advanced Deep-Dives and Technical Whitepapers

For rigorous exploit mechanics, penetration testing methodologies, and defensive benchmarks:
- [[../../15-Technical-Whitepapers/05-Offensive-Security-and-Exploitation/Database-Exploitation-and-SQL-Injection|Database Exploitation & SQL Injection Canon]]: Chris Anley's advanced SQL Server vectors, blind inferential extraction, and second-order injection.
- [[../../15-Technical-Whitepapers/05-Offensive-Security-and-Exploitation/Binary-Exploitation-and-Reverse-Eng|Binary Exploitation & Reverse Engineering]]: Stack buffer overflows, EIP hijacking, and shellcode mechanics.
- [[../../15-Technical-Whitepapers/06-Defensive-Security-and-Hardening/Application-and-Infrastructure-Sec|Enterprise Application Security Standards]]: OWASP Application Security Verification Standard (ASVS 3.0.1) and OWASP Testing Guide v4.
- [[../../15-Technical-Whitepapers/06-Defensive-Security-and-Hardening/Linux-Operating-System-Hardening|Linux OS Hardening]]: Sysctl baseline, kernel self-protection, and SELinux mandatory access control.
