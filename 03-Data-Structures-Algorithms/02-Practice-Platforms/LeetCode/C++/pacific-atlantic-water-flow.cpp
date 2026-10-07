/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(M * N)
// Space: O(M * N)

#include <vector>

using namespace std;

class Solution {
public:
    vector<vector<int>> pacificAtlantic(vector<vector<int>>& heights) {
        if (heights.empty() || heights[0].empty()) {
            return {};
        }

        const int m = heights.size();
        const int n = heights[0].size();
        vector<vector<bool>> pacific(m, vector<bool>(n, false));
        vector<vector<bool>> atlantic(m, vector<bool>(n, false));

        for (int i = 0; i < m; ++i) {
            dfs(heights, i, 0, heights[i][0], pacific);
            dfs(heights, i, n - 1, heights[i][n - 1], atlantic);
        }
        for (int j = 0; j < n; ++j) {
            dfs(heights, 0, j, heights[0][j], pacific);
            dfs(heights, m - 1, j, heights[m - 1][j], atlantic);
        }

        vector<vector<int>> result;
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                if (pacific[i][j] && atlantic[i][j]) {
                    result.push_back({i, j});
                }
            }
        }
        return result;
    }

private:
    void dfs(const vector<vector<int>>& heights, int r, int c, int prev_height, vector<vector<bool>>& reachable) {
        if (r < 0 || r >= static_cast<int>(heights.size()) ||
            c < 0 || c >= static_cast<int>(heights[0].size()) ||
            reachable[r][c] || heights[r][c] < prev_height) {
            return;
        }

        reachable[r][c] = true;
        const int dr[] = {-1, 1, 0, 0};
        const int dc[] = {0, 0, -1, 1};
        for (int i = 0; i < 4; ++i) {
            dfs(heights, r + dr[i], c + dc[i], heights[r][c], reachable);
        }
    }
};
