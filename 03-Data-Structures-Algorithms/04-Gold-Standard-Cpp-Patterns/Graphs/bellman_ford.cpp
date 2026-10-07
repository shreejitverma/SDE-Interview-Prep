/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Bellman-Ford Algorithm (SSSP & Negative Cycle Detection)
 * Standard: Modern C++20
 * Description: Idiomatic C++ implementation computing shortest paths from a source
 *              with negative edge weights, detecting reachable negative cycles,
 *              and reconstructing shortest paths.
 * 
 * Complexity:
 * - Time Complexity: O(V * E)
 * - Space Complexity: O(V)
 */

#include <iostream>
#include <vector>
#include <limits>
#include <optional>
#include <algorithm>
#include <cassert>

struct Edge {
    size_t u;
    size_t v;
    long long weight;
};

class BellmanFord {
public:
    static constexpr long long INF = std::numeric_limits<long long>::max() / 4;

    BellmanFord(size_t vertices, std::vector<Edge> edges)
        : v_(vertices), edges_(std::move(edges)) {}

    struct Result {
        std::vector<long long> dist;
        std::vector<size_t> parent;
        bool has_negative_cycle;

        [[nodiscard]] std::optional<std::vector<size_t>> reconstruct_path(size_t src, size_t dest) const {
            if (dist[dest] >= INF || has_negative_cycle) {
                return std::nullopt;
            }
            std::vector<size_t> path;
            for (size_t at = dest; at != src; at = parent[at]) {
                path.push_back(at);
                if (at == parent[at]) return std::nullopt; // Unreachable
            }
            path.push_back(src);
            std::reverse(path.begin(), path.end());
            return path;
        }
    };

    [[nodiscard]] Result solve(size_t src) const {
        std::vector<long long> dist(v_, INF);
        std::vector<size_t> parent(v_);
        for (size_t i = 0; i < v_; ++i) parent[i] = i;

        dist[src] = 0;

        // Relax all edges V - 1 times
        for (size_t i = 1; i < v_; ++i) {
            bool any_relaxed = false;
            for (const auto& edge : edges_) {
                if (dist[edge.u] < INF && dist[edge.u] + edge.weight < dist[edge.v]) {
                    dist[edge.v] = dist[edge.u] + edge.weight;
                    parent[edge.v] = edge.u;
                    any_relaxed = true;
                }
            }
            if (!any_relaxed) {
                break; // Early termination if converged
            }
        }

        // V-th iteration: Check for negative weight cycles
        bool has_negative_cycle = false;
        for (const auto& edge : edges_) {
            if (dist[edge.u] < INF && dist[edge.u] + edge.weight < dist[edge.v]) {
                has_negative_cycle = true;
                break;
            }
        }

        return Result{dist, parent, has_negative_cycle};
    }

private:
    size_t v_;
    std::vector<Edge> edges_;
};

int main() {
    std::vector<Edge> edges = {
        {0, 1, -1},
        {0, 2, 4},
        {1, 2, 3},
        {1, 3, 2},
        {1, 4, 2},
        {3, 2, 5},
        {3, 1, 1},
        {4, 3, -3}
    };

    BellmanFord solver(5, edges);
    auto res = solver.solve(0);

    assert(!res.has_negative_cycle);
    assert(res.dist[0] == 0);
    assert(res.dist[1] == -1);
    assert(res.dist[2] == 2);
    assert(res.dist[3] == -2);
    assert(res.dist[4] == 1);

    auto path_to_3 = res.reconstruct_path(0, 3);
    assert(path_to_3.has_value());
    // Path should be 0 -> 1 -> 4 -> 3
    std::vector<size_t> expected = {0, 1, 4, 3};
    assert(*path_to_3 == expected);

    std::cout << "All Bellman-Ford tests passed successfully.\n";
    return 0;
}
