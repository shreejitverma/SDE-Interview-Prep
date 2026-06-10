# Secure Coding and Cryptography (Advanced)


### 85.1 Hardening Python Applications
*   **`bandit`**: A tool that scans the AST for common security issues (e.g., using `eval()`, hardcoded passwords).
*   **Constant-Time Comparisons**: Using `hmac.compare_digest` to prevent timing attacks (Chapter 45).

### 85.2 Cryptographic Misuse
*   **Nonce Reuse**: The dangers of using the same initialization vector twice in AES-GCM.
*   **Insecure Randomness**: Why you must never use the `random` module for key generation (Chapter 35).

---


# Appendices
