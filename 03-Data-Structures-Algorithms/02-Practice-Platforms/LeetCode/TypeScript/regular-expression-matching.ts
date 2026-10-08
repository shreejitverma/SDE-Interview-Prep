/*
 * Problem: LeetCode 10 - Regular Expression Matching
 * Difficulty: Hard
 * Concepts: Dynamic Programming, String, Recursion
 *
 * Time Complexity: O(m * n)
 * Space Complexity: O(m * n)
 */

export function isMatch(s: string, p: string): boolean {
    const m = s.length;
    const n = p.length;

    const dp: boolean[][] = Array.from({ length: m + 1 }, () =>
        new Array(n + 1).fill(false)
    );
    dp[0][0] = true;

    for (let j = 2; j <= n; j++) {
        if (p[j - 1] === "*") {
            dp[0][j] = dp[0][j - 2];
        }
    }

    for (let i = 1; i <= m; i++) {
        for (let j = 1; j <= n; j++) {
            if (p[j - 1] === "*") {
                dp[i][j] = dp[i][j - 2];
                const prev = p[j - 2];
                if (prev === "." || prev === s[i - 1]) {
                    dp[i][j] = dp[i][j] || dp[i - 1][j];
                }
            } else if (p[j - 1] === "." || p[j - 1] === s[i - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            }
        }
    }

    return dp[m][n];
}
