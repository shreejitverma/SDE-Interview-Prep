/*
 * Problem: LeetCode 134 - Gas Station
 * Difficulty: Medium
 * Concepts: Greedy, Array
 *
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 */

export function canCompleteCircuit(gas: number[], cost: number[]): number {
    let totalTank = 0;
    let currentTank = 0;
    let startIndex = 0;

    for (let i = 0; i < gas.length; i++) {
        const balance = gas[i] - cost[i];
        totalTank += balance;
        currentTank += balance;

        if (currentTank < 0) {
            startIndex = i + 1;
            currentTank = 0;
        }
    }

    return totalTank >= 0 ? startIndex : -1;
}
