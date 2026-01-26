/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Dijkstra's Algorithm (Shortest Path)
 * Description: Idiomatic C++ implementation using std::priority_queue.
 *           - Time Complexity: O(E * log(V))
 *           - Space Complexity: O(V + E)
 */

#include <iostream>
#include <vector>
#include <queue>
#include <limits>

// Pair: {weight, node_index}
using pii = std::pair<int, int>;

class Graph {
    int V;
    std::vector<std::vector<pii>> adj;

public:
    Graph(int V) : V(V), adj(V) {}

    // Add directed edge
    void addEdge(int u, int v, int w) {
        adj[u].push_back({w, v});
        // adj[v].push_back({w, u}); // Uncomment for undirected
    }

    std::vector<int> dijkstra(int src) {
        // Min-Heap: Stores {distance_from_src, node_index}
        std::priority_queue<pii, std::vector<pii>, std::greater<pii>> pq;
        
        // Distance vector initialized to Infinity
        std::vector<int> dist(V, std::numeric_limits<int>::max());

        dist[src] = 0;
        pq.push({0, src});

        while (!pq.empty()) {
            int u = pq.top().second;
            int d = pq.top().first;
            pq.pop();

            // Optimization: If current distance is greater than already found shortest distance, skip
            if (d > dist[u]) continue;

            for (auto& edge : adj[u]) {
                int weight = edge.first;
                int v = edge.second;

                // Relaxation Step
                if (dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                    pq.push({dist[v], v});
                }
            }
        }
        return dist;
    }
};

int main() {
    int V = 5;
    Graph g(V);
    
    // 0 -> 1 (10)
    // 0 -> 4 (5)
    // ...
    g.addEdge(0, 1, 10);
    g.addEdge(0, 4, 5);
    g.addEdge(1, 2, 1);
    g.addEdge(4, 1, 3);
    g.addEdge(4, 2, 9);
    g.addEdge(4, 3, 2);
    g.addEdge(2, 3, 4);
    g.addEdge(3, 0, 7);

    std::vector<int> dist = g.dijkstra(0);

    std::cout << "Vertex\tDistance from Source (0)\n";
    for (int i = 0; i < V; ++i) {
        std::cout << i << "\t" << dist[i] << "\n";
    }

    return 0;
}
