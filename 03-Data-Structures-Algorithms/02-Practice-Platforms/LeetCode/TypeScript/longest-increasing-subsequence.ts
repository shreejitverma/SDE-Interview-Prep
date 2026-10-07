/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n log n)
// Space: O(n)

function lengthOfLIS(nums: number[]): number {
    if (nums.length === 0) return 0;
    const tails: number[] = [];
    for (const x of nums) {
        let left = 0;
        let right = tails.length;
        while (left < right) {
            const mid = (left + right) >> 1;
            if (tails[mid] < x) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
        if (left === tails.length) {
            tails.push(x);
        } else {
            tails[left] = x;
        }
    }
    return tails.length;
}
