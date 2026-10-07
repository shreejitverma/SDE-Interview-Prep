/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(min(m, n))

function lengthOfLongestSubstring(s: string): number {
    const lookup = new Map<string, number>();
    let left = 0;
    let maxLen = 0;
    for (let right = 0; right < s.length; right++) {
        const c = s[right];
        if (lookup.has(c)) {
            left = Math.max(left, lookup.get(c)! + 1);
        }
        lookup.set(c, right);
        maxLen = Math.max(maxLen, right - left + 1);
    }
    return maxLen;
}
