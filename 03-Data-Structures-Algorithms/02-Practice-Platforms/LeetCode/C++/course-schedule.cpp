/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(V + E)
// Space: O(V + E)

#include <vector>
#include <queue>

class Solution {
public:
    bool canFinish(int numCourses, std::vector<std::vector<int>>& prerequisites) {
        std::vector<std::vector<int>> adj(numCourses);
        std::vector<int> in_degree(numCourses, 0);

        for (const auto& pre : prerequisites) {
            int course = pre[0];
            int prerequisite = pre[1];
            adj[prerequisite].push_back(course);
            in_degree[course]++;
        }

        std::queue<int> q;
        for (int i = 0; i < numCourses; ++i) {
            if (in_degree[i] == 0) {
                q.push(i);
            }
        }

        int visited_count = 0;
        while (!q.empty()) {
            int curr = q.front();
            q.pop();
            visited_count++;

            for (int neighbor : adj[curr]) {
                if (--in_degree[neighbor] == 0) {
                    q.push(neighbor);
                }
            }
        }

        return visited_count == numCourses;
    }
};
