/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Sparse Table (Static Range Minimum / Idempotent Queries)
 * Standard: Modern C++20
 * Description: Data structure for static arrays that answers idempotent range queries
 *              (Range Minimum Query RMQ, Range Maximum Query, Range GCD) in strictly O(1) time
 *              after O(N log N) preprocessing.
 * 
 * Complexity:
 * - Build: O(N log N) time
 * - Query: O(1) strictly constant time for idempotent functions (min, max, gcd)
 * - Space: O(N log N) contiguous tabular storage
 */

#include <iostream>
#include <vector>
#include <span>
#include <concepts>
#include <cassert>
#include <bit>
#include <numeric>
#include <functional>

template <typename T, typename Op = std::ranges::less>
class SparseTable {
public:
    using ValueType = T;

    // Constructs the sparse table from a contiguous span of elements
    explicit SparseTable(std::span<const T> values, Op op = Op{}) 
        : n_(values.size()), op_(op) {
        if (n_ == 0) return;

        const size_t k_max = std::bit_width(n_);
        table_.resize(k_max, std::vector<T>(n_));

        // Base layer 2^0 = 1
        for (size_t i = 0; i < n_; ++i) {
            table_[0][i] = values[i];
        }

        // DP transitions: table_[k][i] combines table_[k-1][i] and table_[k-1][i + 2^(k-1)]
        for (size_t k = 1; k < k_max; ++k) {
            const size_t half_len = 1ULL << (k - 1);
            for (size_t i = 0; i + (1ULL << k) <= n_; ++i) {
                table_[k][i] = evaluate(table_[k - 1][i], table_[k - 1][i + half_len]);
            }
        }
    }

    // Constructor accepting std::vector
    explicit SparseTable(const std::vector<T>& values, Op op = Op{})
        : SparseTable(std::span<const T>(values), op) {}

    // Answers range query on [left, right] (0-indexed inclusive) in strictly O(1)
    [[nodiscard]] T query(size_t left, size_t right) const {
        assert(left <= right && right < n_ && "Query range out of bounds");
        const size_t len = right - left + 1;
        // Fast floor(log2(len)) using C++20 std::bit_width
        const size_t k = std::bit_width(len) - 1;
        return evaluate(table_[k][left], table_[k][right - (1ULL << k) + 1]);
    }

    [[nodiscard]] size_t size() const noexcept {
        return n_;
    }

private:
    size_t n_{0};
    Op op_{};
    std::vector<std::vector<T>> table_;

    // Evaluates the binary operator to select the preferred element
    [[nodiscard]] T evaluate(const T& a, const T& b) const {
        if constexpr (std::is_same_v<Op, std::ranges::less> || std::is_same_v<Op, std::less<T>>) {
            return std::min(a, b);
        } else if constexpr (std::is_same_v<Op, std::ranges::greater> || std::is_same_v<Op, std::greater<T>>) {
            return std::max(a, b);
        } else {
            // General custom binary reducer (e.g. std::gcd)
            return op_(a, b);
        }
    }
};

// Custom functor for Range GCD query
struct GcdOp {
    template <std::integral IntType>
    constexpr IntType operator()(IntType a, IntType b) const noexcept {
        return std::gcd(a, b);
    }
};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    // 1. Range Minimum Query (RMQ)
    std::vector<int> nums = {4, 2, 7, 1, 9, 3, 6, 8, 5};
    SparseTable<int, std::less<int>> rmq(nums);

    assert(rmq.query(0, 8) == 1);
    assert(rmq.query(0, 2) == 2);
    assert(rmq.query(2, 4) == 1);
    assert(rmq.query(4, 7) == 3);
    assert(rmq.query(7, 8) == 5);
    assert(rmq.query(3, 3) == 1);

    // 2. Range Maximum Query
    SparseTable<int, std::greater<int>> rmq_max(nums);
    assert(rmq_max.query(0, 8) == 9);
    assert(rmq_max.query(0, 2) == 7);
    assert(rmq_max.query(5, 8) == 8);

    // 3. Range GCD Query
    std::vector<int> gcd_vals = {12, 18, 24, 36, 60, 48};
    SparseTable<int, GcdOp> gcd_table(gcd_vals);
    assert(gcd_table.query(0, 1) == 6);  // gcd(12, 18) = 6
    assert(gcd_table.query(0, 3) == 6);  // gcd(12, 18, 24, 36) = 6
    assert(gcd_table.query(4, 5) == 12); // gcd(60, 48) = 12

    // 4. Single element test
    std::vector<int> single = {42};
    SparseTable<int, std::less<int>> single_table(single);
    assert(single_table.query(0, 0) == 42);

    std::cout << "All Sparse Table C++20 tests passed successfully." << std::endl;
    return 0;
}
