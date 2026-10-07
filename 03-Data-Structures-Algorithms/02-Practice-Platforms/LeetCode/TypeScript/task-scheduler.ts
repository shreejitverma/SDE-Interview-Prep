/*
 * Problem: LeetCode 621 - Task Scheduler
 * Difficulty: Medium
 * Concepts: Greedy, Counting, Math
 *
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 */

export function leastInterval(tasks: string[], n: number): number {
    const freq = new Array(26).fill(0);
    const codeA = "A".charCodeAt(0);
    let maxFreq = 0;

    for (const task of tasks) {
        const idx = task.charCodeAt(0) - codeA;
        freq[idx]++;
        if (freq[idx] > maxFreq) {
            maxFreq = freq[idx];
        }
    }

    let maxCount = 0;
    for (const count of freq) {
        if (count === maxFreq) {
            maxCount++;
        }
    }

    const calculated = (maxFreq - 1) * (n + 1) + maxCount;
    return Math.max(tasks.length, calculated);
}
