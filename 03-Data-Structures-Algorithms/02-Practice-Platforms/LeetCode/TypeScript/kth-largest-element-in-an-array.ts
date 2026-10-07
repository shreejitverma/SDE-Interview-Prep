/*
 * Problem: LeetCode 215 - Kth Largest Element in an Array
 * Difficulty: Medium
 * Concepts: QuickSelect, Min-Heap, Divide and Conquer
 *
 * Time Complexity: O(n) average (QuickSelect)
 * Space Complexity: O(1) iterative
 */

export function findKthLargest(nums: number[], k: number): number {
    const targetIdx = nums.length - k;
    let left = 0;
    let right = nums.length - 1;

    while (left <= right) {
        const pivotIdx = Math.floor(Math.random() * (right - left + 1)) + left;
        const pivotVal = nums[pivotIdx];

        let lt = left;
        let gt = right;
        let i = left;

        while (i <= gt) {
            if (nums[i] < pivotVal) {
                const temp = nums[i];
                nums[i] = nums[lt];
                nums[lt] = temp;
                i++;
                lt++;
            } else if (nums[i] > pivotVal) {
                const temp = nums[i];
                nums[i] = nums[gt];
                nums[gt] = temp;
                gt--;
            } else {
                i++;
            }
        }

        if (targetIdx >= lt && targetIdx <= gt) {
            return nums[targetIdx];
        } else if (targetIdx < lt) {
            right = lt - 1;
        } else {
            left = gt + 1;
        }
    }

    return nums[targetIdx];
}
