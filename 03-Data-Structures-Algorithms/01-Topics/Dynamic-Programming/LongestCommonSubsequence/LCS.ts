/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time Complexity: O(M * N)
// Space Complexity: O(min(M, N)) with 1D row rolling array

function longestCommonSubsequence(text1: string, text2: string): number {
    if (text1.length < text2.length) {
        [text1, text2] = [text2, text1];
    }

    const m = text1.length;
    const n = text2.length;
    let prev = new Array<number>(n + 1).fill(0);
    let curr = new Array<number>(n + 1).fill(0);

    for (let i = 1; i <= m; i++) {
        for (let j = 1; j <= n; j++) {
            if (text1[i - 1] === text2[j - 1]) {
                curr[j] = prev[j - 1] + 1;
            } else {
                curr[j] = Math.max(prev[j], curr[j - 1]);
            }
        }
        [prev, curr] = [curr, prev];
        curr.fill(0);
    }

    return prev[n];
}

// Example usage
console.log("LCS('abcde', 'ace'):", longestCommonSubsequence("abcde", "ace")); // 3
console.log("LCS('abc', 'abc'):", longestCommonSubsequence("abc", "abc"));     // 3
console.log("LCS('abc', 'def'):", longestCommonSubsequence("abc", "def"));     // 0
