/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 746 - Min Cost Climbing Stairs
 * Language: Java
 *
 * Complexity:
 * - Time: O(N)
 * - Space: O(1)
 */

class Solution {
    public int minCostClimbingStairs(int[] cost) {
        int prev2 = 0;
        int prev1 = 0;

        for (int c : cost) {
            int curr = c + Math.min(prev1, prev2);
            prev2 = prev1;
            prev1 = curr;
        }

        return Math.min(prev1, prev2);
    }
}
