/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(min(M, N))
// Space: O(1)

function uniquePaths(m: number, n: number): number {
    const totalSteps = m + n - 2;
    const k = Math.min(m - 1, n - 1);
    let result = 1;

    for (let i = 1; i <= k; i++) {
        result = (result * (totalSteps - k + i)) / i;
    }

    return Math.round(result);
}
