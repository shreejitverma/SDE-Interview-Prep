/*
 * Problem: LeetCode 45 - Jump Game II
 * Difficulty: Medium
 * Concepts: Greedy, BFS, Array
 *
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 */

class Solution {
    public int jump(int[] nums) {
        int n = nums.length;
        if (n <= 1) {
            return 0;
        }

        int jumps = 0;
        int currentEnd = 0;
        int farthest = 0;

        for (int i = 0; i < n - 1; i++) {
            farthest = Math.max(farthest, i + nums[i]);
            if (i == currentEnd) {
                jumps++;
                currentEnd = farthest;
                if (currentEnd >= n - 1) {
                    break;
                }
            }
        }

        return jumps;
    }
}
