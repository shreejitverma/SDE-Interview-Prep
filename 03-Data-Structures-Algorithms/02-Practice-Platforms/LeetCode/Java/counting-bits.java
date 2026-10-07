/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Problem: LeetCode 338 - Counting Bits
 * Difficulty: Easy
 * Language: Java
 *
 * Performance Analysis:
 * - Time Complexity: O(N) linear single-pass dynamic programming.
 * - Space Complexity: O(1) auxiliary space (excluding the output array of size N + 1).
 */

public class Solution {

    /**
     * Tier 1: DP via Least Significant Bit (Right Shift)
     * ans[i] = ans[i >> 1] + (i & 1)
     */
    public int[] countBits(int n) {
        int[] ans = new int[n + 1];
        for (int i = 1; i <= n; ++i) {
            ans[i] = ans[i >> 1] + (i & 1);
        }
        return ans;
    }

    /**
     * Tier 2: DP via Lowest Set Bit (Brian Kernighan Invariant)
     * ans[i] = ans[i & (i - 1)] + 1
     */
    public int[] countBitsKernighan(int n) {
        int[] ans = new int[n + 1];
        for (int i = 1; i <= n; ++i) {
            ans[i] = ans[i & (i - 1)] + 1;
        }
        return ans;
    }

    public static void main(String[] args) {
        Solution sol = new Solution();
        int[] res2 = sol.countBits(2);
        int[] res5 = sol.countBits(5);
        System.out.print("Bits for 2: ");
        for (int x : res2) System.out.print(x + " ");
        System.out.println(); // 0 1 1

        System.out.print("Bits for 5: ");
        for (int x : res5) System.out.print(x + " ");
        System.out.println(); // 0 1 1 2 1 2
    }
}
