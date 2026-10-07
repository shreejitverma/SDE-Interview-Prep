/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Kruskal's Algorithm (Minimum Spanning Tree / Forest)
 * Standard: Modern C++20
 * Description: Idiomatic C++ implementation of Kruskal's MST using Disjoint Set Union
 *              with path compression and union by rank. Supports disconnected graphs (Spanning Forest).
 * 
 * Complexity:
 * - Time Complexity: O(E log E)
 * - Space Complexity: O(V + E)
 */

#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <cassert>

struct UndirectedEdge {
    size_t u;
    size_t v;
    long long weight;

    auto operator<=>(const UndirectedEdge& other) const = default;
};

class DSU {
public:
    explicit DSU(size_t n) : parent_(n), rank_(n, 0) {
        std::iota(parent_.begin(), parent_.end(), 0);
    }

    size_t find(size_t i) {
        if (parent_[i] != i) {
            parent_[i] = find(parent_[i]); // Path compression
        }
        return parent_[i];
    }

    bool unite(size_t i, size_t j) {
        size_t root_i = find(i);
        size_t root_j = find(j);
        if (root_i == root_j) {
            return false; // Already in the same component
        }
        if (rank_[root_i] < rank_[root_j]) {
            std::swap(root_i, root_j);
        }
        parent_[root_j] = root_i;
        if (rank_[root_i] == rank_[root_j]) {
            rank_[root_i]++;
        }
        return true;
    }

private:
    std::vector<size_t> parent_;
    std::vector<size_t> rank_;
};

struct MSTResult {
    long long total_weight = 0;
    std::vector<UndirectedEdge> mst_edges;
    bool is_connected = false;
};

class KruskalMST {
public:
    static MSTResult solve(size_t vertices, std::vector<UndirectedEdge> edges) {
        // Sort edges by weight in ascending order
        std::sort(edges.begin(), edges.end(), [](const UndirectedEdge& a, const UndirectedEdge& b) {
            return a.weight < b.weight;
        });

        DSU dsu(vertices);
        MSTResult result;
        result.mst_edges.reserve(vertices > 0 ? vertices - 1 : 0);

        for (const auto& edge : edges) {
            if (dsu.unite(edge.u, edge.v)) {
                result.total_weight += edge.weight;
                result.mst_edges.push_back(edge);
                if (result.mst_edges.size() == vertices - 1) {
                    break;
                }
            }
        }

        result.is_connected = (vertices <= 1) || (result.mst_edges.size() == vertices - 1);
        return result;
    }
};

int main() {
    std::vector<UndirectedEdge> edges = {
        {0, 1, 10},
        {0, 2, 6},
        {0, 3, 5},
        {1, 3, 15},
        {2, 3, 4}
    };

    auto res = KruskalMST::solve(4, edges);
    assert(res.is_connected);
    // MST should take edges with weights 4 (2-3), 5 (0-3), and 10 (0-1) => total 19
    assert(res.total_weight == 19);
    assert(res.mst_edges.size() == 3);

    std::cout << "All Kruskal's MST tests passed successfully.\n";
    return 0;
}
