/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Problem: LeetCode 647 - Palindromic Substrings
 * Difficulty: Medium
 * Language: Java
 *
 * Performance Analysis:
 * - Time Complexity: O(N) using Manacher's algorithm; O(N^2) using expand-around-center.
 * - Space Complexity: O(N) for Manacher's transformed array; O(1) auxiliary for expand-around-center.
 */

public class Solution {

    /**
     * Tier 1: Manacher's Algorithm - Strictly O(N) Time
     */
    public int countSubstrings(String s) {
        if (s == null || s.length() == 0) {
            return 0;
        }

        // Preprocess string into "^#a#b#c#$" format
        StringBuilder sb = new StringBuilder("^");
        for (int i = 0; i < s.length(); ++i) {
            sb.append("#").append(s.charAt(i));
        }
        sb.append("#$");
        char[] t = sb.toString().toCharArray();
        int n = t.length;

        int[] p = new int[n];
        int center = 0;
        int right = 0;
        int count = 0;

        for (int i = 1; i < n - 1; ++i) {
            int iMirror = 2 * center - i;
            if (right > i) {
                p[i] = Math.min(right - i, p[iMirror]);
            } else {
                p[i] = 0;
            }

            // Expand around center i
            while (t[i + 1 + p[i]] == t[i - 1 - p[i]]) {
                p[i]++;
            }

            // Update center and right boundary
            if (i + p[i] > right) {
                center = i;
                right = i + p[i];
            }

            count += (p[i] + 1) / 2;
        }

        return count;
    }

    /**
     * Tier 2: Expand Around Center - O(N^2) Time, O(1) Space
     */
    public int countSubstringsExpand(String s) {
        int count = 0;
        for (int i = 0; i < s.length(); ++i) {
            count += expand(s, i, i);     // Odd-length centers
            count += expand(s, i, i + 1); // Even-length centers
        }
        return count;
    }

    private int expand(String s, int left, int right) {
        int matches = 0;
        while (left >= 0 && right < s.length() && s.charAt(left) == s.charAt(right)) {
            matches++;
            left--;
            right++;
        }
        return matches;
    }

    public static void main(String[] args) {
        Solution sol = new Solution();
        System.out.println("Palindromes in 'abc': " + sol.countSubstrings("abc")); // 3 ("a", "b", "c")
        System.out.println("Palindromes in 'aaa': " + sol.countSubstrings("aaa")); // 6 ("a", "a", "a", "aa", "aa", "aaa")
    }
}
