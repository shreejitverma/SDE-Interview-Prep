/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Problem: LeetCode 268 - Missing Number
 * Difficulty: Easy
 * Language: Java
 *
 * Performance Analysis:
 * - Time Complexity: O(N) single-pass bitwise XOR or Gauss summation.
 * - Space Complexity: O(1) auxiliary space.
 */

public class Solution {

    /**
     * Bitwise XOR solution: Immune to arithmetic overflow.
     */
    public int missingNumber(int[] nums) {
        int missing = nums.length;
        for (int i = 0; i < nums.length; ++i) {
            missing ^= i ^ nums[i];
        }
        return missing;
    }

    /**
     * Alternative Gauss summation solution:
     * sum = n * (n + 1) / 2 - sum(nums).
     */
    public int missingNumberGauss(int[] nums) {
        int n = nums.length;
        int expectedSum = n * (n + 1) / 2;
        int actualSum = 0;
        for (int x : nums) {
            actualSum += x;
        }
        return expectedSum - actualSum;
    }

    public static void main(String[] args) {
        Solution sol = new Solution();
        int[] nums1 = {3, 0, 1};
        int[] nums2 = {0, 1};
        int[] nums3 = {9, 6, 4, 2, 3, 5, 7, 0, 1};
        System.out.println("Missing 1: " + sol.missingNumber(nums1)); // 2
        System.out.println("Missing 2: " + sol.missingNumber(nums2)); // 2
        System.out.println("Missing 3: " + sol.missingNumber(nums3)); // 8
    }
}
