/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(m * n)
// Space: O(m * n)

#include <vector>
#include <queue>
#include <utility>

class Solution {
public:
    int orangesRotting(std::vector<std::vector<int>>& grid) {
        if (grid.empty() || grid[0].empty()) return 0;

        const int m = static_cast<int>(grid.size());
        const int n = static_cast<int>(grid[0].size());
        std::queue<std::pair<int, int>> q;
        int fresh = 0;

        for (int r = 0; r < m; ++r) {
            for (int c = 0; c < n; ++c) {
                if (grid[r][c] == 2) {
                    q.emplace(r, c);
                } else if (grid[r][c] == 1) {
                    ++fresh;
                }
            }
        }

        if (fresh == 0) return 0;

        int minutes = 0;
        static const int dr[4] = {-1, 1, 0, 0};
        static const int dc[4] = {0, 0, -1, 1};

        while (!q.empty() && fresh > 0) {
            const size_t sz = q.size();
            for (size_t i = 0; i < sz; ++i) {
                const auto [r, c] = q.front();
                q.pop();

                for (int d = 0; d < 4; ++d) {
                    const int nr = r + dr[d];
                    const int nc = c + dc[d];

                    if (nr >= 0 && nr < m && nc >= 0 && nc < n && grid[nr][nc] == 1) {
                        grid[nr][nc] = 2;
                        --fresh;
                        q.emplace(nr, nc);
                    }
                }
            }
            ++minutes;
        }

        return (fresh == 0) ? minutes : -1;
    }
};
