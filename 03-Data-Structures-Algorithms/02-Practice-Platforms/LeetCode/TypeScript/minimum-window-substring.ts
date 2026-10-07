/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(m + n)
// Space: O(1) auxiliary

function minWindow(s: string, t: string): string {
    if (s.length < t.length) return "";

    const count = new Int32Array(128);
    for (let i = 0; i < t.length; i++) {
        count[t.charCodeAt(i)]++;
    }

    let remain = t.length;
    let left = 0;
    let minStart = 0;
    let minLen = Infinity;

    for (let right = 0; right < s.length; right++) {
        const rightChar = s.charCodeAt(right);
        if (count[rightChar] > 0) {
            remain--;
        }
        count[rightChar]--;

        while (remain === 0) {
            if (right - left + 1 < minLen) {
                minLen = right - left + 1;
                minStart = left;
            }

            const leftChar = s.charCodeAt(left);
            count[leftChar]++;
            if (count[leftChar] > 0) {
                remain++;
            }
            left++;
        }
    }

    return minLen === Infinity ? "" : s.substring(minStart, minStart + minLen);
}
