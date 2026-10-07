/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(1)

function rob(nums: number[]): number {
    if (!nums || nums.length === 0) {
        return 0;
    }
    if (nums.length === 1) {
        return nums[0];
    }

    function robRange(start: number, end: number): number {
        let prev2 = 0;
        let prev1 = 0;

        for (let i = start; i < end; i++) {
            const current = Math.max(prev1, prev2 + nums[i]);
            prev2 = prev1;
            prev1 = current;
        }

        return prev1;
    }

    return Math.max(robRange(0, nums.length - 1), robRange(1, nums.length));
}
