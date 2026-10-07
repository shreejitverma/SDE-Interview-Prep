/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Floyd-Warshall Algorithm (All-Pairs Shortest Path & Cycle Detection)
 * Standard: Modern C++20
 * Description: Idiomatic C++ implementation of the O(V^3) dynamic programming algorithm
 *              computing shortest paths between all pairs of vertices, detecting negative
 *              weight cycles, and reconstructing shortest paths between any pair.
 * 
 * Complexity:
 * - Time Complexity: O(V^3)
 * - Space Complexity: O(V^2)
 */

#include <iostream>
#include <vector>
#include <limits>
#include <optional>
#include <cassert>

class FloydWarshall {
public:
    static constexpr long long INF = std::numeric_limits<long long>::max() / 4;

    explicit FloydWarshall(size_t vertices)
        : v_(vertices),
          dist_(vertices, std::vector<long long>(vertices, INF)),
          next_(vertices, std::vector<std::optional<size_t>>(vertices, std::nullopt)) {
        for (size_t i = 0; i < v_; ++i) {
            dist_[i][i] = 0;
            next_[i][i] = i;
        }
    }

    void add_directed_edge(size_t u, size_t v, long long weight) {
        assert(u < v_ && v < v_ && "Vertex out of bounds");
        if (weight < dist_[u][v]) {
            dist_[u][v] = weight;
            next_[u][v] = v;
        }
    }

    // Computes all-pairs shortest paths. Returns true if no negative cycles exist, false otherwise.
    bool compute() {
        for (size_t k = 0; k < v_; ++k) {
            for (size_t i = 0; i < v_; ++i) {
                for (size_t j = 0; j < v_; ++j) {
                    if (dist_[i][k] < INF && dist_[k][j] < INF) {
                        if (dist_[i][k] + dist_[k][j] < dist_[i][j]) {
                            dist_[i][j] = dist_[i][k] + dist_[k][j];
                            next_[i][j] = next_[i][k];
                        }
                    }
                }
            }
        }

        // Check for negative cycles: any diagonal entry dist[i][i] < 0 indicates a negative cycle
        for (size_t i = 0; i < v_; ++i) {
            if (dist_[i][i] < 0) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] long long get_distance(size_t u, size_t v) const {
        assert(u < v_ && v < v_ && "Vertex out of bounds");
        return dist_[u][v];
    }

    // Reconstructs the complete path from u to v
    [[nodiscard]] std::optional<std::vector<size_t>> reconstruct_path(size_t u, size_t v) const {
        if (dist_[u][v] >= INF) {
            return std::nullopt; // Unreachable
        }
        std::vector<size_t> path = {u};
        size_t curr = u;
        while (curr != v) {
            if (!next_[curr][v].has_value()) {
                return std::nullopt;
            }
            curr = *next_[curr][v];
            path.push_back(curr);
        }
        return path;
    }

private:
    size_t v_;
    std::vector<std::vector<long long>> dist_;
    std::vector<std::vector<std::optional<size_t>>> next_;
};

int main() {
    FloydWarshall fw(4);
    fw.add_directed_edge(0, 1, 5);
    fw.add_directed_edge(0, 3, 10);
    fw.add_directed_edge(1, 2, 3);
    fw.add_directed_edge(2, 3, 1);

    bool no_cycle = fw.compute();
    assert(no_cycle);

    // 0 -> 1 (5), 1 -> 2 (3), 2 -> 3 (1) => 0 -> 3 shortest path is 5 + 3 + 1 = 9 (beats direct edge 10)
    assert(fw.get_distance(0, 3) == 9);
    assert(fw.get_distance(1, 3) == 4);

    auto path_0_to_3 = fw.reconstruct_path(0, 3);
    assert(path_0_to_3.has_value());
    std::vector<size_t> expected_path = {0, 1, 2, 3};
    assert(*path_0_to_3 == expected_path);

    // Negative cycle test
    FloydWarshall neg_fw(3);
    neg_fw.add_directed_edge(0, 1, 1);
    neg_fw.add_directed_edge(1, 2, -3);
    neg_fw.add_directed_edge(2, 0, 1);
    bool neg_result = neg_fw.compute();
    assert(!neg_result); // Cycle with total weight -1 detected

    std::cout << "All Floyd-Warshall tests passed successfully.\n";
    return 0;
}
