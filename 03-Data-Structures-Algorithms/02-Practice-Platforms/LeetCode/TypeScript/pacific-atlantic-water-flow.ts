/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(M * N)
// Space: O(M * N)

function pacificAtlantic(heights: number[][]): number[][] {
    if (!heights || heights.length === 0 || heights[0].length === 0) {
        return [];
    }

    const m = heights.length;
    const n = heights[0].length;
    const pacific: boolean[][] = Array.from({ length: m }, () => new Array(n).fill(false));
    const atlantic: boolean[][] = Array.from({ length: m }, () => new Array(n).fill(false));

    function dfs(r: number, c: number, reachable: boolean[][], prevVal: number): void {
        if (
            r < 0 || r >= m ||
            c < 0 || c >= n ||
            reachable[r][c] ||
            heights[r][c] < prevVal
        ) {
            return;
        }

        reachable[r][c] = true;
        const dr = [-1, 1, 0, 0];
        const dc = [0, 0, -1, 1];

        for (let i = 0; i < 4; i++) {
            dfs(r + dr[i], c + dc[i], reachable, heights[r][c]);
        }
    }

    for (let i = 0; i < m; i++) {
        dfs(i, 0, pacific, heights[i][0]);
        dfs(i, n - 1, atlantic, heights[i][n - 1]);
    }
    for (let j = 0; j < n; j++) {
        dfs(0, j, pacific, heights[0][j]);
        dfs(m - 1, j, atlantic, heights[m - 1][j]);
    }

    const result: number[][] = [];
    for (let i = 0; i < m; i++) {
        for (let j = 0; j < n; j++) {
            if (pacific[i][j] && atlantic[i][j]) {
                result.push([i, j]);
            }
        }
    }

    return result;
}
