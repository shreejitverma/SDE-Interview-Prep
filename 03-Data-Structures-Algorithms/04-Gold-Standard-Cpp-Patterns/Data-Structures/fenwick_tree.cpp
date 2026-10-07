/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Fenwick Tree (Binary Indexed Tree / BIT)
 * Standard: Modern C++20
 * Description: Space-efficient prefix accumulator supporting point updates and range queries in O(log N).
 *              Includes O(log N) binary lifting to find the smallest index with a given prefix sum.
 * 
 * Complexity:
 * - Build: O(N) linear time
 * - Point Update: O(log N) time
 * - Prefix Sum: O(log N) time
 * - Range Sum: O(log N) time
 * - Binary Lifting (lower_bound): O(log N) time
 * - Space: O(N) contiguous vector
 */

#include <iostream>
#include <vector>
#include <concepts>
#include <cassert>
#include <bit>

template <std::integral T = long long>
class FenwickTree {
public:
    explicit FenwickTree(size_t n) : n_(n), tree_(n + 1, T{0}) {}

    // Linear-time O(N) constructor from initial values (0-indexed input)
    explicit FenwickTree(const std::vector<T>& values) : n_(values.size()), tree_(values.size() + 1, T{0}) {
        for (size_t i = 0; i < n_; ++i) {
            tree_[i + 1] = values[i];
        }
        for (size_t i = 1; i <= n_; ++i) {
            size_t parent = i + (i & -i);
            if (parent <= n_) {
                tree_[parent] += tree_[i];
            }
        }
    }

    // Adds delta to 0-indexed position idx in O(log N)
    void add(size_t idx, T delta) {
        assert(idx < n_ && "Index out of bounds");
        for (size_t i = idx + 1; i <= n_; i += (i & -i)) {
            tree_[i] += delta;
        }
    }

    // Computes sum of elements in range [0, idx] (0-indexed inclusive)
    [[nodiscard]] T prefix_sum(size_t idx) const {
        assert(idx < n_ && "Index out of bounds");
        T sum = 0;
        for (size_t i = idx + 1; i > 0; i -= (i & -i)) {
            sum += tree_[i];
        }
        return sum;
    }

    // Computes sum of elements in range [left, right] (0-indexed inclusive)
    [[nodiscard]] T range_sum(size_t left, size_t right) const {
        assert(left <= right && right < n_ && "Invalid query range");
        return prefix_sum(right) - (left > 0 ? prefix_sum(left - 1) : T{0});
    }

    // Binary lifting: Finds smallest 0-indexed pos such that prefix_sum(pos) >= target.
    // Requires all elements to be non-negative.
    [[nodiscard]] size_t lower_bound(T target) const {
        size_t idx = 0;
        T current_sum = 0;
        // Largest power of 2 <= n_
        size_t mask = std::bit_floor(n_);

        for (size_t step = mask; step > 0; step >>= 1) {
            if (idx + step <= n_ && current_sum + tree_[idx + step] < target) {
                idx += step;
                current_sum += tree_[idx];
            }
        }
        return idx; // 0-indexed position
    }

    [[nodiscard]] size_t size() const noexcept {
        return n_;
    }

private:
    size_t n_;
    std::vector<T> tree_;
};

int main() {
    std::vector<long long> arr = {1, 3, 5, 7, 9, 11};
    FenwickTree<long long> bit(arr);

    assert(bit.prefix_sum(2) == 9);    // 1 + 3 + 5
    assert(bit.range_sum(1, 3) == 15); // 3 + 5 + 7

    bit.add(1, 4); // arr[1] becomes 7 (was 3)
    assert(bit.prefix_sum(2) == 13);   // 1 + 7 + 5
    assert(bit.range_sum(1, 3) == 19); // 7 + 5 + 7

    // Binary lifting check
    // prefix sums are: pos 0 -> 1, pos 1 -> 8, pos 2 -> 13
    assert(bit.lower_bound(8) == 1);
    assert(bit.lower_bound(9) == 2);

    std::cout << "All Fenwick Tree tests passed successfully.\n";
    return 0;
}
