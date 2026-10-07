/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Problem: LeetCode 647 - Palindromic Substrings
 * Difficulty: Medium
 * Language: TypeScript
 *
 * Performance Analysis:
 * - Time Complexity: O(N) linear time using Manacher's algorithm; O(N^2) center expansion.
 * - Space Complexity: O(N) auxiliary space for transformed string and radius array.
 */

export function countSubstrings(s: string): number {
    if (!s || s.length === 0) {
        return 0;
    }

    // Preprocess: "^#a#b#c#$"
    const t = '^#' + s.split('').join('#') + '#$';
    const n = t.length;
    const p = new Int32Array(n);
    let center = 0;
    let right = 0;
    let total = 0;

    for (let i = 1; i < n - 1; ++i) {
        const iMirror = 2 * center - i;
        if (right > i) {
            p[i] = Math.min(right - i, p[iMirror]);
        } else {
            p[i] = 0;
        }

        while (t[i + 1 + p[i]] === t[i - 1 - p[i]]) {
            p[i]++;
        }

        if (i + p[i] > right) {
            center = i;
            right = i + p[i];
        }

        total += Math.floor((p[i] + 1) / 2);
    }

    return total;
}

export function countSubstringsExpand(s: string): number {
    let total = 0;
    const n = s.length;

    function expand(l: number, r: number): number {
        let cnt = 0;
        while (l >= 0 && r < n && s[l] === s[r]) {
            cnt++;
            l--;
            r++;
        }
        return cnt;
    }

    for (let i = 0; i < n; ++i) {
        total += expand(i, i);     // Odd-length
        total += expand(i, i + 1); // Even-length
    }

    return total;
}

// Standalone execution test
if (typeof require !== 'undefined' && require.main === module) {
    console.log("countSubstrings('abc'):", countSubstrings('abc')); // 3
    console.log("countSubstrings('aaa'):", countSubstrings('aaa')); // 6
}
