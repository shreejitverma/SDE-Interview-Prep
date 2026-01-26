/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Disjoint Set Union (DSU) / Union-Find
 * Description: Implementation with Path Compression and Union by Rank.
 *           - Time Complexity: O(alpha(N)) ~ O(1) on average.
 *           - Essential for Kruskal's Algorithm and Cycle Detection.
 */

#include <iostream>
#include <vector>
#include <numeric> // for std::iota

class DSU {
    std::vector<int> parent;
    std::vector<int> rank;

public:
    DSU(int n) {
        parent.resize(n);
        rank.resize(n, 0);
        // Initialize parent[i] = i
        std::iota(parent.begin(), parent.end(), 0);
    }

    // Find with Path Compression
    int find(int i) {
        if (parent[i] != i) {
            parent[i] = find(parent[i]); // Path compression
        }
        return parent[i];
    }

    // Union by Rank
    void unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);

        if (root_i != root_j) {
            if (rank[root_i] < rank[root_j]) {
                std::swap(root_i, root_j);
            }
            parent[root_j] = root_i;
            if (rank[root_i] == rank[root_j]) {
                rank[root_i]++;
            }
        }
    }

    // Check if connected
    bool connected(int i, int j) {
        return find(i) == find(j);
    }
};

int main() {
    DSU dsu(5);
    
    dsu.unite(0, 1);
    dsu.unite(2, 3);
    dsu.unite(0, 4);

    // 0 is connected to 1 and 4. 
    // 2 is connected to 3.
    
    std::cout << "0 and 4 connected? " << (dsu.connected(0, 4) ? "Yes" : "No") << "\n"; // Yes
    std::cout << "1 and 3 connected? " << (dsu.connected(1, 3) ? "Yes" : "No") << "\n"; // No
    
    dsu.unite(1, 2); // Connect the two components
    
    std::cout << "1 and 3 connected (after union)? " << (dsu.connected(1, 3) ? "Yes" : "No") << "\n"; // Yes

    return 0;
}
