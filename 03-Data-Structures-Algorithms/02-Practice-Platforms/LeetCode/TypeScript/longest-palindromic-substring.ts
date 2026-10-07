/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(n)

function longestPalindrome(s: string): string {
    if (s.length <= 1) return s;

    let t = "^";
    for (let i = 0; i < s.length; i++) {
        t += "#" + s[i];
    }
    t += "#$";

    const n = t.length;
    const p = new Int32Array(n);
    let c = 0;
    let r = 0;

    for (let i = 1; i < n - 1; i++) {
        const iMirror = 2 * c - i;
        if (r > i) {
            p[i] = Math.min(r - i, p[iMirror]);
        }
        while (t[i + 1 + p[i]] === t[i - 1 - p[i]]) {
            p[i]++;
        }
        if (i + p[i] > r) {
            c = i;
            r = i + p[i];
        }
    }

    let maxLen = 0;
    let centerIndex = 0;
    for (let i = 1; i < n - 1; i++) {
        if (p[i] > maxLen) {
            maxLen = p[i];
            centerIndex = i;
        }
    }

    const start = Math.floor((centerIndex - maxLen) / 2);
    return s.substring(start, start + maxLen);
}
