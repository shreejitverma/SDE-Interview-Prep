/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Binary Lifting and Lowest Common Ancestor (LCA)
 * Standard: Modern C++20
 * Description: Tree ancestor queries, Lowest Common Ancestor, K-th ancestor, and tree distance
 *              using binary lifting and Euler tour entry/exit timestamps.
 * 
 * Complexity:
 * - Preprocessing: O(N log N) time and space
 * - LCA Query: O(log N) time
 * - Is-Ancestor Check: O(1) time
 * - K-th Ancestor Query: O(log N) time
 * - Tree Distance Query: O(log N) time
 * - Space: O(N log N) auxiliary space
 */

#include <iostream>
#include <vector>
#include <span>
#include <cassert>
#include <bit>
#include <optional>

class TreeBinaryLifting {
public:
    // Constructs the tree from an adjacency list rooted at 'root'
    explicit TreeBinaryLifting(const std::vector<std::vector<int>>& adj, int root = 0)
        : n_(adj.size()), root_(root) {
        if (n_ == 0) return;

        log_n_ = std::bit_width(n_);
        up_.assign(log_n_ + 1, std::vector<int>(n_, root_));
        tin_.resize(n_);
        tout_.resize(n_);
        depth_.resize(n_, 0);

        int timer = 0;
        dfs(root_, root_, 0, timer, adj);
    }

    // Checks whether 'u' is an ancestor of 'v' in strictly O(1)
    [[nodiscard]] bool is_ancestor(int u, int v) const noexcept {
        assert(u >= 0 && u < static_cast<int>(n_));
        assert(v >= 0 && v < static_cast<int>(n_));
        return tin_[u] <= tin_[v] && tout_[u] >= tout_[v];
    }

    // Computes the Lowest Common Ancestor of 'u' and 'v' in O(log N)
    [[nodiscard]] int lca(int u, int v) const noexcept {
        assert(u >= 0 && u < static_cast<int>(n_));
        assert(v >= 0 && v < static_cast<int>(n_));

        if (is_ancestor(u, v)) return u;
        if (is_ancestor(v, u)) return v;

        for (int k = static_cast<int>(log_n_); k >= 0; --k) {
            if (!is_ancestor(up_[k][u], v)) {
                u = up_[k][u];
            }
        }
        return up_[0][u];
    }

    // Returns the k-th ancestor of node 'u', or std::nullopt if out of tree bounds
    [[nodiscard]] std::optional<int> kth_ancestor(int u, int k) const noexcept {
        assert(u >= 0 && u < static_cast<int>(n_));
        if (k < 0 || k > depth_[u]) {
            return std::nullopt;
        }

        int curr = u;
        for (size_t bit = 0; bit <= log_n_; ++bit) {
            if ((k >> bit) & 1) {
                curr = up_[bit][curr];
            }
        }
        return curr;
    }

    // Computes the shortest path edge distance between 'u' and 'v' in O(log N)
    [[nodiscard]] int distance(int u, int v) const noexcept {
        const int ancestor = lca(u, v);
        return depth_[u] + depth_[v] - 2 * depth_[ancestor];
    }

    [[nodiscard]] int depth(int u) const noexcept {
        assert(u >= 0 && u < static_cast<int>(n_));
        return depth_[u];
    }

private:
    size_t n_{0};
    int root_{0};
    size_t log_n_{0};
    std::vector<std::vector<int>> up_;
    std::vector<int> tin_;
    std::vector<int> tout_;
    std::vector<int> depth_;

    void dfs(int u, int p, int d, int& timer, const std::vector<std::vector<int>>& adj) {
        tin_[u] = ++timer;
        depth_[u] = d;
        up_[0][u] = p;

        for (size_t k = 1; k <= log_n_; ++k) {
            up_[k][u] = up_[k - 1][up_[k - 1][u]];
        }

        for (int v : adj[u]) {
            if (v != p) {
                dfs(v, u, d + 1, timer, adj);
            }
        }

        tout_[u] = ++timer;
    }
};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    // Tree topology with 8 nodes (0 to 7):
    //         0
    //       /   \
    //      1     2
    //     / \   / \
    //    3   4 5   6
    //       /
    //      7
    const int n = 8;
    std::vector<std::vector<int>> adj(n);
    auto add_edge = [&](int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    };

    add_edge(0, 1);
    add_edge(0, 2);
    add_edge(1, 3);
    add_edge(1, 4);
    add_edge(2, 5);
    add_edge(2, 6);
    add_edge(4, 7);

    TreeBinaryLifting tree(adj, 0);

    // 1. Ancestor verification
    assert(tree.is_ancestor(0, 7) == true);
    assert(tree.is_ancestor(1, 7) == true);
    assert(tree.is_ancestor(4, 7) == true);
    assert(tree.is_ancestor(3, 7) == false);
    assert(tree.is_ancestor(2, 7) == false);
    assert(tree.is_ancestor(7, 7) == true);

    // 2. Lowest Common Ancestor
    assert(tree.lca(3, 7) == 1);
    assert(tree.lca(7, 4) == 4);
    assert(tree.lca(3, 4) == 1);
    assert(tree.lca(3, 5) == 0);
    assert(tree.lca(5, 6) == 2);
    assert(tree.lca(7, 6) == 0);

    // 3. K-th Ancestor
    assert(tree.kth_ancestor(7, 0) == 7);
    assert(tree.kth_ancestor(7, 1) == 4);
    assert(tree.kth_ancestor(7, 2) == 1);
    assert(tree.kth_ancestor(7, 3) == 0);
    assert(tree.kth_ancestor(7, 4) == std::nullopt);

    // 4. Distance
    assert(tree.distance(3, 7) == 3); // 3 -> 1 -> 4 -> 7
    assert(tree.distance(7, 6) == 5); // 7 -> 4 -> 1 -> 0 -> 2 -> 6
    assert(tree.distance(0, 7) == 3);
    assert(tree.distance(5, 5) == 0);

    std::cout << "All Binary Lifting LCA C++20 tests passed successfully." << std::endl;
    return 0;
}
