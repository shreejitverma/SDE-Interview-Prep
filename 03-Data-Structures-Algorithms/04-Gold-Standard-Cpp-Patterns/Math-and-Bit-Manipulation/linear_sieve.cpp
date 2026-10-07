/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Euler's Linear Sieve & Prime Factorization
 * Standard: Modern C++20
 * Description: Strictly linear-time O(N) prime sieve that also precomputes the
 *              Smallest Prime Factor (SPF) for every integer, enabling O(log N) prime factorization.
 * 
 * Complexity:
 * - Precomputation: O(N) Time, O(N) Space
 * - is_prime(x): O(1) Time
 * - factorize(x): O(log x) Time
 */

#include <iostream>
#include <vector>
#include <map>
#include <cassert>

class LinearSieve {
public:
    explicit LinearSieve(size_t max_n) : max_n_(max_n), spf_(max_n + 1, 0) {
        primes_.reserve(max_n / 10);
        for (size_t i = 2; i <= max_n_; ++i) {
            if (spf_[i] == 0) {
                spf_[i] = i;
                primes_.push_back(i);
            }
            // Invariant: each composite number is visited exactly once by its smallest prime factor
            for (size_t p : primes_) {
                if (p > spf_[i] || i * p > max_n_) {
                    break;
                }
                spf_[i * p] = p;
            }
        }
    }

    [[nodiscard]] bool is_prime(size_t x) const noexcept {
        if (x <= 1 || x > max_n_) return false;
        return spf_[x] == x;
    }

    [[nodiscard]] const std::vector<size_t>& get_primes() const noexcept {
        return primes_;
    }

    // Decomposes x into prime factors with their exponents in O(log x)
    [[nodiscard]] std::vector<std::pair<size_t, size_t>> factorize(size_t x) const {
        assert(x >= 1 && x <= max_n_ && "Input out of range for factorization");
        std::vector<std::pair<size_t, size_t>> factors;

        while (x > 1) {
            size_t p = spf_[x];
            size_t count = 0;
            while (x % p == 0) {
                count++;
                x /= p;
            }
            factors.emplace_back(p, count);
        }
        return factors;
    }

private:
    size_t max_n_;
    std::vector<size_t> spf_;    // Smallest Prime Factor
    std::vector<size_t> primes_; // List of primes up to max_n
};

int main() {
    LinearSieve sieve(100);

    // 1. Prime checks
    assert(sieve.is_prime(2));
    assert(sieve.is_prime(3));
    assert(sieve.is_prime(97));
    assert(!sieve.is_prime(1));
    assert(!sieve.is_prime(4));
    assert(!sieve.is_prime(91)); // 7 * 13

    // 2. Count primes up to 100 (should be 25)
    assert(sieve.get_primes().size() == 25);

    // 3. Fast factorization: 60 = 2^2 * 3^1 * 5^1
    auto factors_60 = sieve.factorize(60);
    std::vector<std::pair<size_t, size_t>> expected_60 = {{2, 2}, {3, 1}, {5, 1}};
    assert(factors_60 == expected_60);

    // Factorization: 84 = 2^2 * 3^1 * 7^1
    auto factors_84 = sieve.factorize(84);
    std::vector<std::pair<size_t, size_t>> expected_84 = {{2, 2}, {3, 1}, {7, 1}};
    assert(factors_84 == expected_84);

    std::cout << "All Linear Sieve and Factorization tests passed successfully.\n";
    return 0;
}
