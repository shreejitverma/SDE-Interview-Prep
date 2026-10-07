/*
 * Problem: LeetCode 621 - Task Scheduler
 * Difficulty: Medium
 * Concepts: Greedy, Counting, Math
 *
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 */

class Solution {
    public int leastInterval(char[] tasks, int n) {
        int[] freq = new int[26];
        int maxFreq = 0;

        for (char task : tasks) {
            freq[task - 'A']++;
            maxFreq = Math.max(maxFreq, freq[task - 'A']);
        }

        int maxCount = 0;
        for (int count : freq) {
            if (count == maxFreq) {
                maxCount++;
            }
        }

        int calculated = (maxFreq - 1) * (n + 1) + maxCount;
        return Math.max(tasks.length, calculated);
    }
}
