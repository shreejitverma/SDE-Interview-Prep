/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(1)

function characterReplacement(s: string, k: number): number {
    const count = new Array(26).fill(0);
    let maxCount = 0;
    let left = 0;
    let maxLen = 0;

    for (let right = 0; right < s.length; right++) {
        const code = s.charCodeAt(right) - 65;
        count[code]++;
        maxCount = Math.max(maxCount, count[code]);

        if (right - left + 1 - maxCount > k) {
            count[s.charCodeAt(left) - 65]--;
            left++;
        }
        maxLen = Math.max(maxLen, right - left + 1);
    }
    return maxLen;
}
