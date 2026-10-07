/*
 * Problem: LeetCode 72 - Edit Distance
 * Difficulty: Hard
 * Concepts: Dynamic Programming, String
 *
 * Time Complexity: O(m * n)
 * Space Complexity: O(min(m, n))
 */

export function minDistance(word1: string, word2: string): number {
    const m = word1.length;
    const n = word2.length;

    if (m < n) {
        return minDistance(word2, word1);
    }

    const dp = Array.from({ length: n + 1 }, (_, i) => i);

    for (let i = 1; i <= m; i++) {
        let prevDiag = dp[0];
        dp[0] = i;

        for (let j = 1; j <= n; j++) {
            const temp = dp[j];
            if (word1[i - 1] === word2[j - 1]) {
                dp[j] = prevDiag;
            } else {
                dp[j] = 1 + Math.min(prevDiag, dp[j], dp[j - 1]);
            }
            prevDiag = temp;
        }
    }

    return dp[n];
}
