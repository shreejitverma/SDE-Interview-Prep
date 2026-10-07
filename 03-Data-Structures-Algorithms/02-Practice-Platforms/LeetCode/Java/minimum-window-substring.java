/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(m + n)
// Space: O(1) auxiliary (128 ASCII array)

class Solution {
    public String minWindow(String s, String t) {
        if (s == null || t == null || s.length() < t.length()) {
            return "";
        }

        int[] count = new int[128];
        for (char c : t.toCharArray()) {
            count[c]++;
        }

        int remain = t.length();
        int left = 0;
        int minStart = 0;
        int minLen = Integer.MAX_VALUE;

        for (int right = 0; right < s.length(); right++) {
            if (count[s.charAt(right)] > 0) {
                remain--;
            }
            count[s.charAt(right)]--;

            while (remain == 0) {
                if (right - left + 1 < minLen) {
                    minLen = right - left + 1;
                    minStart = left;
                }

                count[s.charAt(left)]++;
                if (count[s.charAt(left)] > 0) {
                    remain++;
                }
                left++;
            }
        }

        return minLen == Integer.MAX_VALUE ? "" : s.substring(minStart, minStart + minLen);
    }
}
