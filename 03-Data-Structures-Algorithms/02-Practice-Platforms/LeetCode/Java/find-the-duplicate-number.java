/*
 * Problem: LeetCode 287 - Find the Duplicate Number
 * Difficulty: Medium
 * Concepts: Two Pointers, Floyd's Cycle Detection, Array
 *
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 */

class Solution {
    public int findDuplicate(int[] nums) {
        int slow = nums[0];
        int fast = nums[nums[0]];

        // Phase 1: Detect cycle
        while (slow != fast) {
            slow = nums[slow];
            fast = nums[nums[fast]];
        }

        // Phase 2: Locate cycle entrance
        fast = 0;
        while (slow != fast) {
            slow = nums[slow];
            fast = nums[fast];
        }

        return slow;
    }
}
