# OWASP Top 10 (Security Checklist)

Every SDE must know these.

1.  **Injection:** SQL Injection, Command Injection.
    *   *Fix:* Use Parameterized Queries (Prepared Statements). Never concat strings into queries.
2.  **Broken Authentication:** Weak passwords, session hijacking.
    *   *Fix:* MFA, short session timeouts, don't roll your own crypto.
3.  **Sensitive Data Exposure:** Storing passwords in plain text.
    *   *Fix:* Salt & Hash (Argon2, bcrypt). Encrypt PII at rest.
4.  **XXE (XML External Entities):** Parsing hostile XML.
    *   *Fix:* Disable external entities in XML parsers.
5.  **Broken Access Control:** User A accessing User B's data.
    *   *Fix:* Check ownership on every API call (`if user.id != resource.owner_id: raise 403`).
6.  **Security Misconfiguration:** Default passwords, detailed error messages.
    *   *Fix:* Automate hardening, disable stack traces in Prod.
7.  **XSS (Cross-Site Scripting):** Injecting JS into web pages.
    *   *Fix:* Content Security Policy (CSP), Context-aware encoding (React/Angular do this mostly automatically).
8.  **Insecure Deserialization:** Executing code via serialized objects (e.g., Python Pickle, Java ObjectStream).
    *   *Fix:* Use JSON/Protobuf instead of native serialization.
9.  **Using Components with Known Vulnerabilities:** Old npm/maven packages.
    *   *Fix:* `npm audit`, Snyk, Dependabot.
10. **Insufficient Logging:** Not knowing you were hacked.
    *   *Fix:* Centralized logging (ELK/Splunk), Alerting on anomalies.

---

## Advanced Deep-Dives & Technical Whitepapers

For rigorous exploit mechanics, penetration testing methodologies, and defensive benchmarks:
- [[../../15-Technical-Whitepapers/05-Offensive-Security-and-Exploitation/Database-Exploitation-and-SQL-Injection|Database Exploitation & SQL Injection Canon]]: Chris Anley's advanced SQL Server vectors, blind inferential extraction, and second-order injection.
- [[../../15-Technical-Whitepapers/05-Offensive-Security-and-Exploitation/Binary-Exploitation-and-Reverse-Eng|Binary Exploitation & Reverse Engineering]]: Stack buffer overflows, EIP hijacking, and shellcode mechanics.
- [[../../15-Technical-Whitepapers/06-Defensive-Security-and-Hardening/Application-and-Infrastructure-Sec|Enterprise Application Security Standards]]: OWASP Application Security Verification Standard (ASVS 3.0.1) and OWASP Testing Guide v4.
- [[../../15-Technical-Whitepapers/06-Defensive-Security-and-Hardening/Linux-Operating-System-Hardening|Linux OS Hardening]]: Sysctl baseline, kernel self-protection, and SELinux mandatory access control.
