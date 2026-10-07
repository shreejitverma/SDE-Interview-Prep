/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 994 - Rotting Oranges
 * Language: TypeScript
 *
 * Complexity:
 * - Time: O(M * N)
 * - Space: O(M * N)
 */

function orangesRotting(grid: number[][]): number {
    if (grid.length === 0 || grid[0].length === 0) return 0;

    const m = grid.length;
    const n = grid[0].length;
    const queue: [number, number][] = [];
    let fresh = 0;

    for (let r = 0; r < m; r++) {
        for (let c = 0; c < n; c++) {
            if (grid[r][c] === 2) {
                queue.push([r, c]);
            } else if (grid[r][c] === 1) {
                fresh++;
            }
        }
    }

    if (fresh === 0) return 0;

    let minutes = 0;
    const dr = [-1, 1, 0, 0];
    const dc = [0, 0, -1, 1];
    let head = 0;

    while (head < queue.length && fresh > 0) {
        const size = queue.length - head;
        for (let i = 0; i < size; i++) {
            const [r, c] = queue[head++];

            for (let d = 0; d < 4; d++) {
                const nr = r + dr[d];
                const nc = c + dc[d];

                if (nr >= 0 && nr < m && nc >= 0 && nc < n && grid[nr][nc] === 1) {
                    grid[nr][nc] = 2;
                    fresh--;
                    queue.push([nr, nc]);
                }
            }
        }
        minutes++;
    }

    return fresh === 0 ? minutes : -1;
}
