/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time Complexity: O(N^3)
// Space Complexity: O(N^2)

function matrixChainOrder(p: number[]): number {
    const n = p.length - 1;
    const dp: number[][] = Array.from({ length: n + 1 }, () => new Array(n + 1).fill(0));

    for (let L = 2; L <= n; L++) {
        for (let i = 1; i <= n - L + 1; i++) {
            const j = i + L - 1;
            dp[i][j] = Infinity;
            for (let k = i; k < j; k++) {
                const cost = dp[i][k] + dp[k + 1][j] + p[i - 1] * p[k] * p[j];
                if (cost < dp[i][j]) {
                    dp[i][j] = cost;
                }
            }
        }
    }

    return dp[1][n];
}

// Example driver
const arr = [1, 2, 3, 4, 3];
console.log("Minimum number of multiplications is", matrixChainOrder(arr));
