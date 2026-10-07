/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Iterative Segment Tree (Range Minimum / Sum Query)
 * Standard: Modern C++20
 * Description: Non-recursive, flat-array Segment Tree with 2N memory footprint.
 *              Maximizes L1 cache spatial locality, eliminates stack frame overhead,
 *              and supports custom monoid operations (Sum, Min, Max, GCD) via functors.
 * 
 * Complexity:
 * - Build: O(N) linear time
 * - Point Update: O(log N) time
 * - Range Query [L, R): O(log N) time
 * - Space: O(2N) contiguous vector
 */

#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
#include <cassert>

template <typename T, typename Op = std::plus<T>>
class SegmentTree {
public:
    explicit SegmentTree(size_t n, T identity = T{}, Op op = Op{})
        : n_(n), identity_(identity), op_(op), tree_(2 * n, identity) {}

    // Linear-time O(N) constructor from vector
    SegmentTree(const std::vector<T>& values, T identity = T{}, Op op = Op{})
        : n_(values.size()), identity_(identity), op_(op), tree_(2 * values.size(), identity) {
        // Place leaves in tree_[n ... 2n - 1]
        for (size_t i = 0; i < n_; ++i) {
            tree_[n_ + i] = values[i];
        }
        // Build internal nodes from n - 1 down to 1
        for (size_t i = n_ - 1; i > 0; --i) {
            tree_[i] = op_(tree_[i << 1], tree_[(i << 1) | 1]);
        }
    }

    // Updates 0-indexed position idx to new_val in O(log N)
    void update(size_t idx, T new_val) {
        assert(idx < n_ && "Index out of bounds");
        size_t pos = n_ + idx;
        tree_[pos] = new_val;
        // Bubble up changes to root
        for (pos >>= 1; pos > 0; pos >>= 1) {
            tree_[pos] = op_(tree_[pos << 1], tree_[(pos << 1) | 1]);
        }
    }

    // Queries half-open interval [left, right) in O(log N)
    [[nodiscard]] T query(size_t left, size_t right) const {
        assert(left <= right && right <= n_ && "Invalid query interval");
        T res_left = identity_;
        T res_right = identity_;

        for (size_t l = n_ + left, r = n_ + right; l < r; l >>= 1, r >>= 1) {
            if (l & 1) {
                res_left = op_(res_left, tree_[l++]);
            }
            if (r & 1) {
                res_right = op_(tree_[--r], res_right);
            }
        }
        return op_(res_left, res_right);
    }

    [[nodiscard]] size_t size() const noexcept {
        return n_;
    }

private:
    size_t n_;
    T identity_;
    Op op_;
    std::vector<T> tree_;
};

int main() {
    // 1. Range Sum Query Tree
    std::vector<long long> arr = {2, 1, 5, 3, 4};
    SegmentTree<long long, std::plus<long long>> sum_tree(arr, 0LL, std::plus<long long>{});

    assert(sum_tree.query(0, 3) == 8); // 2 + 1 + 5
    assert(sum_tree.query(1, 4) == 9); // 1 + 5 + 3

    sum_tree.update(2, 10);            // arr[2] becomes 10
    assert(sum_tree.query(0, 3) == 13); // 2 + 1 + 10

    // 2. Range Minimum Query Tree
    struct MinOp {
        long long operator()(long long a, long long b) const { return std::min(a, b); }
    };
    SegmentTree<long long, MinOp> min_tree(arr, std::numeric_limits<long long>::max(), MinOp{});

    assert(min_tree.query(0, 5) == 1);
    assert(min_tree.query(2, 5) == 3);

    std::cout << "All Segment Tree tests passed successfully.\n";
    return 0;
}
