/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Matrix Exponentiation for Fast Linear Recurrences
 * Standard: Modern C++20
 * Description: High-performance square matrix multiplication and binary exponentiation
 *              under modulo arithmetic. Solves general linear recurrences (Fibonacci, Tribonacci)
 *              and counts paths of length N in directed graphs in O(K^3 log N) time.
 * 
 * Complexity:
 * - Matrix Multiplication: O(K^3) time
 * - Matrix Power M^N: O(K^3 log N) time
 * - Space: O(K^2) memory for KxK matrix
 */

#include <iostream>
#include <vector>
#include <concepts>
#include <cassert>

template <std::integral T = long long>
class Matrix {
public:
    explicit Matrix(size_t rows, size_t cols, T fill_val = T{0})
        : rows_(rows), cols_(cols), data_(rows, std::vector<T>(cols, fill_val)) {}

    // Generates an identity matrix of dimension n x n
    [[nodiscard]] static Matrix identity(size_t n) {
        Matrix res(n, n, 0);
        for (size_t i = 0; i < n; ++i) {
            res.data_[i][i] = 1;
        }
        return res;
    }

    [[nodiscard]] size_t rows() const noexcept { return rows_; }
    [[nodiscard]] size_t cols() const noexcept { return cols_; }

    [[nodiscard]] const std::vector<T>& operator[](size_t r) const noexcept {
        return data_[r];
    }

    [[nodiscard]] std::vector<T>& operator[](size_t r) noexcept {
        return data_[r];
    }

    // Multiplies two matrices modulo 'mod' in cache-friendly i-k-j order
    [[nodiscard]] Matrix multiply(const Matrix& other, T mod) const {
        assert(cols_ == other.rows_ && "Incompatible matrix dimensions for multiplication");
        Matrix result(rows_, other.cols_, 0);

        for (size_t i = 0; i < rows_; ++i) {
            for (size_t k = 0; k < cols_; ++k) {
                if (data_[i][k] == 0) continue;
                for (size_t j = 0; j < other.cols_; ++j) {
                    result.data_[i][j] = (result.data_[i][j] + 
                        (static_cast<__int128>(data_[i][k]) * other.data_[k][j])) % mod;
                }
            }
        }
        return result;
    }

    // Raises square matrix to power 'exp' modulo 'mod' in O(K^3 log exp)
    [[nodiscard]] Matrix power(unsigned long long exp, T mod) const {
        assert(rows_ == cols_ && "Matrix must be square for exponentiation");
        Matrix result = Matrix::identity(rows_);
        Matrix base = *this;

        while (exp > 0) {
            if (exp & 1) {
                result = result.multiply(base, mod);
            }
            if (exp > 1) {
                base = base.multiply(base, mod);
            }
            exp >>= 1;
        }
        return result;
    }

private:
    size_t rows_{0};
    size_t cols_{0};
    std::vector<std::vector<T>> data_;
};

// Solves N-th Fibonacci number in O(log N) modulo MOD
// F(0) = 0, F(1) = 1, F(2) = 1, ...
long long fibonacci(unsigned long long n, long long mod = 1'000'000'007LL) {
    if (n == 0) return 0;
    if (n == 1) return 1;

    // Transition matrix T = [[1, 1], [1, 0]]
    // [F(n+1), F(n)]^T = T^n * [F(1), F(0)]^T
    Matrix<long long> t(2, 2);
    t[0][0] = 1; t[0][1] = 1;
    t[1][0] = 1; t[1][1] = 0;

    Matrix<long long> tn = t.power(n - 1, mod);
    // F(n) = tn[0][0] * F(1) + tn[0][1] * F(0) = tn[0][0]
    return tn[0][0] % mod;
}

// Solves N-th Tribonacci number in O(log N) modulo MOD
// T(0) = 0, T(1) = 1, T(2) = 1, T(n) = T(n-1) + T(n-2) + T(n-3)
long long tribonacci(unsigned long long n, long long mod = 1'000'000'007LL) {
    if (n == 0) return 0;
    if (n == 1 || n == 2) return 1;

    // Transition matrix:
    // [[1, 1, 1],
    //  [1, 0, 0],
    //  [0, 1, 0]]
    Matrix<long long> t(3, 3);
    t[0][0] = 1; t[0][1] = 1; t[0][2] = 1;
    t[1][0] = 1; t[1][1] = 0; t[1][2] = 0;
    t[2][0] = 0; t[2][1] = 1; t[2][2] = 0;

    Matrix<long long> tn = t.power(n - 2, mod);
    // [T(n), T(n-1), T(n-2)]^T = T^(n-2) * [T(2), T(1), T(0)]^T
    // T(n) = tn[0][0]*1 + tn[0][1]*1 + tn[0][2]*0
    return (tn[0][0] + tn[0][1]) % mod;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    constexpr long long MOD = 1'000'000'007LL;

    // 1. Fibonacci verification
    assert(fibonacci(0, MOD) == 0);
    assert(fibonacci(1, MOD) == 1);
    assert(fibonacci(2, MOD) == 1);
    assert(fibonacci(3, MOD) == 2);
    assert(fibonacci(4, MOD) == 3);
    assert(fibonacci(5, MOD) == 5);
    assert(fibonacci(6, MOD) == 8);
    assert(fibonacci(7, MOD) == 13);
    assert(fibonacci(10, MOD) == 55);
    assert(fibonacci(20, MOD) == 6765);
    // Large N test: F(10^9) mod 10^9 + 7
    long long f_huge = fibonacci(1'000'000'000ULL, MOD);
    assert(f_huge >= 0 && f_huge < MOD);

    // 2. Tribonacci verification
    // T(0)=0, T(1)=1, T(2)=1, T(3)=2, T(4)=4, T(5)=7, T(6)=13, T(7)=24
    assert(tribonacci(0, MOD) == 0);
    assert(tribonacci(1, MOD) == 1);
    assert(tribonacci(2, MOD) == 1);
    assert(tribonacci(3, MOD) == 2);
    assert(tribonacci(4, MOD) == 4);
    assert(tribonacci(5, MOD) == 7);
    assert(tribonacci(6, MOD) == 13);
    assert(tribonacci(7, MOD) == 24);

    // 3. Graph path counting test:
    // Directed graph with 3 vertices: 0->1, 1->2, 2->0, 0->2
    Matrix<long long> adj(3, 3);
    adj[0][1] = 1; adj[0][2] = 1;
    adj[1][2] = 1;
    adj[2][0] = 1;

    // Paths of length 2 from 0 to 2: 0->1->2 (1 path)
    Matrix<long long> paths2 = adj.power(2, MOD);
    assert(paths2[0][2] == 1);

    // Paths of length 3 from 0:
    // 0->1->2->0 (ends at 0)
    // 0->2->0->1 (ends at 1)
    // 0->2->0->2 (ends at 2)
    Matrix<long long> paths3 = adj.power(3, MOD);
    assert(paths3[0][0] == 1);
    assert(paths3[0][1] == 1);
    assert(paths3[0][2] == 1);

    std::cout << "All Matrix Exponentiation C++20 tests passed successfully." << std::endl;
    return 0;
}
