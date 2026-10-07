/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Topological Sort (Kahn's BFS & Lexicographical Priority Queue)
 * Standard: Modern C++20
 * Description: Idiomatic C++ implementation of Kahn's in-degree algorithm
 *              for Directed Acyclic Graphs (DAGs), with cycle detection and
 *              optional lexicographical ordering via min-heaps.
 * 
 * Complexity:
 * - Standard Kahn's: O(V + E) Time, O(V + E) Space
 * - Lexicographical: O(V log V + E) Time, O(V + E) Space
 */

#include <iostream>
#include <vector>
#include <queue>
#include <optional>
#include <cassert>

class DAG {
public:
    explicit DAG(size_t vertices) : v_(vertices), adj_(vertices), in_degree_(vertices, 0) {}

    void add_edge(size_t u, size_t v) {
        assert(u < v_ && v < v_ && "Vertex index out of bounds");
        adj_[u].push_back(v);
        in_degree_[v]++;
    }

    // Standard O(V + E) Kahn's Algorithm
    [[nodiscard]] std::optional<std::vector<size_t>> topological_sort() const {
        std::vector<size_t> in_deg = in_degree_;
        std::queue<size_t> q;

        for (size_t i = 0; i < v_; ++i) {
            if (in_deg[i] == 0) {
                q.push(i);
            }
        }

        std::vector<size_t> order;
        order.reserve(v_);

        while (!q.empty()) {
            size_t u = q.front();
            q.pop();
            order.push_back(u);

            for (size_t v : adj_[u]) {
                if (--in_deg[v] == 0) {
                    q.push(v);
                }
            }
        }

        if (order.size() < v_) {
            return std::nullopt; // Cycle detected
        }
        return order;
    }

    // Lexicographically smallest topological order in O(V log V + E)
    [[nodiscard]] std::optional<std::vector<size_t>> lexicographical_topological_sort() const {
        std::vector<size_t> in_deg = in_degree_;
        std::priority_queue<size_t, std::vector<size_t>, std::greater<size_t>> min_heap;

        for (size_t i = 0; i < v_; ++i) {
            if (in_deg[i] == 0) {
                min_heap.push(i);
            }
        }

        std::vector<size_t> order;
        order.reserve(v_);

        while (!min_heap.empty()) {
            size_t u = min_heap.top();
            min_heap.pop();
            order.push_back(u);

            for (size_t v : adj_[u]) {
                if (--in_deg[v] == 0) {
                    min_heap.push(v);
                }
            }
        }

        if (order.size() < v_) {
            return std::nullopt; // Cycle detected
        }
        return order;
    }

private:
    size_t v_;
    std::vector<std::vector<size_t>> adj_;
    std::vector<size_t> in_degree_;
};

int main() {
    DAG dag(6);
    dag.add_edge(5, 2);
    dag.add_edge(5, 0);
    dag.add_edge(4, 0);
    dag.add_edge(4, 1);
    dag.add_edge(2, 3);
    dag.add_edge(3, 1);

    auto standard_order = dag.topological_sort();
    assert(standard_order.has_value());
    assert(standard_order->size() == 6);

    auto lexi_order = dag.lexicographical_topological_sort();
    assert(lexi_order.has_value());
    // Lexicographically smallest chooses 4 before 5, 0 before 2, etc.
    std::cout << "Lexicographical Topological Order: ";
    for (size_t node : *lexi_order) {
        std::cout << node << " ";
    }
    std::cout << "\n";

    // Cycle test
    DAG cyclic(3);
    cyclic.add_edge(0, 1);
    cyclic.add_edge(1, 2);
    cyclic.add_edge(2, 0);
    assert(!cyclic.topological_sort().has_value());

    std::cout << "All Topological Sort tests passed successfully.\n";
    return 0;
}
