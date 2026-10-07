/*
 * Problem: LeetCode 134 - Gas Station
 * Difficulty: Medium
 * Concepts: Greedy, Array
 *
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 */

class Solution {
    public int canCompleteCircuit(int[] gas, int[] cost) {
        int totalTank = 0;
        int currentTank = 0;
        int startIndex = 0;

        for (int i = 0; i < gas.length; i++) {
            int balance = gas[i] - cost[i];
            totalTank += balance;
            currentTank += balance;

            if (currentTank < 0) {
                startIndex = i + 1;
                currentTank = 0;
            }
        }

        return totalTank >= 0 ? startIndex : -1;
    }
}
