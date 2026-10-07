/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(1)

function numDecodings(s: string): number {
    if (!s || s[0] === '0') {
        return 0;
    }

    let prev2 = 1;
    let prev1 = 1;

    for (let i = 1; i < s.length; i++) {
        let current = 0;

        // Single digit decode
        if (s[i] !== '0') {
            current += prev1;
        }

        // Two digit decode
        const twoDigit = Number(s.substring(i - 1, i + 1));
        if (twoDigit >= 10 && twoDigit <= 26) {
            current += prev2;
        }

        prev2 = prev1;
        prev1 = current;
    }

    return prev1;
}
