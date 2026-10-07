/*
 * Problem: LeetCode 287 - Find the Duplicate Number
 * Difficulty: Medium
 * Concepts: Two Pointers, Floyd's Cycle Detection, Array
 *
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 */

export function findDuplicate(nums: number[]): number {
    let slow = nums[0];
    let fast = nums[nums[0]];

    // Phase 1: Detect cycle intersection
    while (slow !== fast) {
        slow = nums[slow];
        fast = nums[nums[fast]];
    }

    // Phase 2: Find cycle entrance
    fast = 0;
    while (slow !== fast) {
        slow = nums[slow];
        fast = nums[fast];
    }

    return slow;
}
