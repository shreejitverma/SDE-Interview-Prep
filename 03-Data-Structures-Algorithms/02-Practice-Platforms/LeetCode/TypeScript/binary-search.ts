/*
 * Problem: LeetCode 704 - Binary Search
 * Difficulty: Easy
 * Concepts: Binary Search, Array
 *
 * Time Complexity: O(log n)
 * Space Complexity: O(1)
 */

export function search(nums: number[], target: number): number {
    let left = 0;
    let right = nums.length - 1;

    while (left <= right) {
        const mid = left + Math.floor((right - left) / 2);
        if (nums[mid] === target) {
            return mid;
        } else if (nums[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return -1;
}
