/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Modular Arithmetic & Combinatorics (nCr % MOD)
 * Standard: Modern C++20
 * Description: Idiomatic C++ implementation of fast modular exponentiation,
 *              modular multiplicative inverse, and O(1) query combinations nCr % p.
 * 
 * Complexity:
 * - Modular Power: O(log exp) Time
 * - Modular Inverse: O(log MOD) Time
 * - Combinatorics Precomputation: O(N) Time, O(N) Space
 * - nCr Query: O(1) Time
 */

#include <iostream>
#include <vector>
#include <cassert>

class ModularArithmetic {
public:
    static constexpr long long DEFAULT_MOD = 1'000'000'007LL;

    // Binary exponentiation: (base^exp) % mod in O(log exp)
    [[nodiscard]] static constexpr long long power(long long base, long long exp, long long mod = DEFAULT_MOD) noexcept {
        long long res = 1;
        base %= mod;
        if (base < 0) base += mod;
        while (exp > 0) {
            if (exp & 1) {
                res = static_cast<long long>((static_cast<__int128>(res) * base) % mod);
            }
            base = static_cast<long long>((static_cast<__int128>(base) * base) % mod);
            exp >>= 1;
        }
        return res;
    }

    // Extended Euclidean Algorithm: finds x, y such that a*x + b*y = gcd(a, b)
    static long long ext_gcd(long long a, long long b, long long& x, long long& y) noexcept {
        if (b == 0) {
            x = 1;
            y = 0;
            return a;
        }
        long long x1, y1;
        long long g = ext_gcd(b, a % b, x1, y1);
        x = y1;
        y = x1 - (a / b) * y1;
        return g;
    }

    // Modular inverse for any coprime modulus: a^(-1) % mod
    [[nodiscard]] static long long mod_inverse(long long a, long long mod = DEFAULT_MOD) noexcept {
        long long x, y;
        long long g = ext_gcd(a, mod, x, y);
        assert(g == 1 && "Inverse does not exist (not coprime)");
        return (x % mod + mod) % mod;
    }
};

class Combinatorics {
public:
    Combinatorics(size_t max_n, long long mod = ModularArithmetic::DEFAULT_MOD)
        : max_n_(max_n), mod_(mod), fact_(max_n + 1, 1), inv_fact_(max_n + 1, 1) {
        // Precompute factorials in O(N)
        for (size_t i = 1; i <= max_n_; ++i) {
            fact_[i] = (fact_[i - 1] * i) % mod_;
        }
        // Precompute inverse factorials in O(N) using backward propagation
        inv_fact_[max_n_] = ModularArithmetic::power(fact_[max_n_], mod_ - 2, mod_);
        for (size_t i = max_n_; i > 0; --i) {
            inv_fact_[i - 1] = (inv_fact_[i] * i) % mod_;
        }
    }

    // Queries nCr % MOD in O(1) time
    [[nodiscard]] long long nCr(size_t n, size_t r) const noexcept {
        if (r > n || n > max_n_) {
            return 0;
        }
        long long num = fact_[n];
        long long den = (inv_fact_[r] * inv_fact_[n - r]) % mod_;
        return (num * den) % mod_;
    }

private:
    size_t max_n_;
    long long mod_;
    std::vector<long long> fact_;
    std::vector<long long> inv_fact_;
};

int main() {
    // 1. Test Modular Exponentiation: 2^10 = 1024
    assert(ModularArithmetic::power(2, 10) == 1024);
    assert(ModularArithmetic::power(3, 0) == 1);

    // 2. Test Modular Inverse: (3 * mod_inv(3)) % MOD == 1
    long long inv3 = ModularArithmetic::mod_inverse(3);
    assert((3 * inv3) % ModularArithmetic::DEFAULT_MOD == 1);

    // 3. Test Combinatorics: 5C2 = 10, 10C3 = 120
    Combinatorics comb(100);
    assert(comb.nCr(5, 2) == 10);
    assert(comb.nCr(10, 3) == 120);
    assert(comb.nCr(5, 5) == 1);
    assert(comb.nCr(5, 0) == 1);
    assert(comb.nCr(5, 6) == 0);

    std::cout << "All Modular Arithmetic and Combinatorics tests passed successfully.\n";
    return 0;
}
