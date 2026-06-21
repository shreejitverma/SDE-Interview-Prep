# SECURITY ENGINEERING


# SECURITY ENGINEERING


### 36.1 Fuzzing (libFuzzer / AFL++)
Unit tests test what you *know*. Fuzzing tests what you *don't*.
*   **Coverage-Guided:** The fuzzer instruments binaries to see which inputs explore new code paths.
*   **Sanitizers:** Always fuzz with AddressSanitizer (ASan) and UndefinedBehaviorSanitizer (UBSan) enabled.

### 36.2 Cryptography & Timing Attacks
NEVER write your own crypto. Use `libsodium` or `BoringSSL`.
*   **The Trap:** `memcmp(hash1, hash2, 32)` exits early if the first byte differs.
*   **The Attack:** Attacker measures time. If it takes longer, they guessed the first byte right.
*   **The Fix:** Constant-time comparison.
    ```cpp
    // libsodium's constant time check
    if (crypto_verify_32(hash1, hash2) != 0) { /* Error */ }
    ```

### 36.3 Side-Channel Mitigations (Spectre)
Modern CPUs execute instructions speculatively.
*   **Scenario:** `if (x < array_len) { val = array[x]; }`
*   **Attack:** CPU predicts `true`, loads `array[x]` (where `x` is out of bounds secret). Even if checks fail, `array[x]` is now in L1 cache.
*   **Mitigation:** `LFENCE` (Load Fence) or `std::clamp` indices to 0 on failure (masking).

---


---
