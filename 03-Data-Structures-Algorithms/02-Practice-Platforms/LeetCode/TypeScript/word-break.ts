/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N * L_max) where L_max is maximum word length
// Space: O(N + M) auxiliary

function wordBreak(s: string, wordDict: string[]): boolean {
    const dict = new Set<string>(wordDict);
    let maxLen = 0;
    for (const w of wordDict) {
        maxLen = Math.max(maxLen, w.length);
    }

    const n = s.length;
    const dp = new Array<boolean>(n + 1).fill(false);
    dp[0] = true;

    for (let i = 1; i <= n; i++) {
        const start = Math.max(0, i - maxLen);
        for (let j = i - 1; j >= start; j--) {
            if (dp[j] && dict.has(s.substring(j, i))) {
                dp[i] = true;
                break;
            }
        }
    }

    return dp[n];
}
