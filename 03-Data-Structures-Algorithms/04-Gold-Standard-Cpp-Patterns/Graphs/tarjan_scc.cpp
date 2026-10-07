/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Tarjan's Algorithm (Strongly Connected Components / SCC)
 * Standard: Modern C++20
 * Description: Linear-time O(V + E) single-pass DFS algorithm identifying all Strongly
 *              Connected Components (SCCs) in a directed graph using discovery times and low-link values.
 * 
 * Complexity:
 * - Time Complexity: O(V + E)
 * - Space Complexity: O(V)
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

class TarjanSCC {
public:
    explicit TarjanSCC(size_t vertices) : v_(vertices), adj_(vertices) {}

    void add_directed_edge(size_t u, size_t v) {
        assert(u < v_ && v < v_ && "Vertex out of bounds");
        adj_[u].push_back(v);
    }

    [[nodiscard]] std::vector<std::vector<size_t>> find_sccs() const {
        std::vector<int> tin(v_, -1);
        std::vector<int> low(v_, -1);
        std::vector<bool> in_stack(v_, false);
        std::vector<size_t> stack;
        std::vector<std::vector<size_t>> sccs;
        int timer = 0;

        auto dfs = [&](auto& self, size_t u) -> void {
            tin[u] = low[u] = timer++;
            stack.push_back(u);
            in_stack[u] = true;

            for (size_t v : adj_[u]) {
                if (tin[v] == -1) {
                    // Tree edge
                    self(self, v);
                    low[u] = std::min(low[u], low[v]);
                } else if (in_stack[v]) {
                    // Back edge / cross edge to node in current stack
                    low[u] = std::min(low[u], tin[v]);
                }
            }

            // If u is a root node of an SCC, pop the stack
            if (low[u] == tin[u]) {
                std::vector<size_t> component;
                while (true) {
                    size_t curr = stack.back();
                    stack.pop_back();
                    in_stack[curr] = false;
                    component.push_back(curr);
                    if (curr == u) break;
                }
                sccs.push_back(std::move(component));
            }
        };

        for (size_t i = 0; i < v_; ++i) {
            if (tin[i] == -1) {
                dfs(dfs, i);
            }
        }

        return sccs;
    }

private:
    size_t v_;
    std::vector<std::vector<size_t>> adj_;
};

int main() {
    TarjanSCC graph(5);
    graph.add_directed_edge(1, 0);
    graph.add_directed_edge(0, 2);
    graph.add_directed_edge(2, 1);
    graph.add_directed_edge(0, 3);
    graph.add_directed_edge(3, 4);

    auto sccs = graph.find_sccs();
    // In this graph: {4} is an SCC, {3} is an SCC, and {0, 1, 2} forms an SCC of size 3.
    assert(sccs.size() == 3);

    bool found_cycle_scc = false;
    for (const auto& comp : sccs) {
        if (comp.size() == 3) {
            found_cycle_scc = true;
        }
    }
    assert(found_cycle_scc);

    std::cout << "All Tarjan SCC tests passed successfully.\n";
    return 0;
}
