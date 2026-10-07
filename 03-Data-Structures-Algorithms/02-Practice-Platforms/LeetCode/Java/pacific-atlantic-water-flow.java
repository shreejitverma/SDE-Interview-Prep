/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(M * N)
// Space: O(M * N)

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

class Solution {
    public List<List<Integer>> pacificAtlantic(int[][] heights) {
        List<List<Integer>> result = new ArrayList<>();
        if (heights == null || heights.length == 0 || heights[0].length == 0) {
            return result;
        }

        int m = heights.length;
        int n = heights[0].length;
        boolean[][] pacific = new boolean[m][n];
        boolean[][] atlantic = new boolean[m][n];

        for (int i = 0; i < m; i++) {
            dfs(heights, i, 0, heights[i][0], pacific);
            dfs(heights, i, n - 1, heights[i][n - 1], atlantic);
        }
        for (int j = 0; j < n; j++) {
            dfs(heights, 0, j, heights[0][j], pacific);
            dfs(heights, m - 1, j, heights[m - 1][j], atlantic);
        }

        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (pacific[i][j] && atlantic[i][j]) {
                    result.add(Arrays.asList(i, j));
                }
            }
        }

        return result;
    }

    private void dfs(int[][] heights, int r, int c, int prevVal, boolean[][] reachable) {
        if (r < 0 || r >= heights.length || c < 0 || c >= heights[0].length ||
            reachable[r][c] || heights[r][c] < prevVal) {
            return;
        }

        reachable[r][c] = true;
        int[] dr = {-1, 1, 0, 0};
        int[] dc = {0, 0, -1, 1};

        for (int i = 0; i < 4; i++) {
            dfs(heights, r + dr[i], c + dc[i], heights[r][c], reachable);
        }
    }
}
