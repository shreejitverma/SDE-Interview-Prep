/*
 * Problem: LeetCode 45 - Jump Game II
 * Difficulty: Medium
 * Concepts: Greedy, BFS, Array
 *
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 */

export function jump(nums: number[]): number {
    const n = nums.length;
    if (n <= 1) {
        return 0;
    }

    let jumps = 0;
    let currentEnd = 0;
    let farthest = 0;

    for (let i = 0; i < n - 1; i++) {
        farthest = Math.max(farthest, i + nums[i]);
        if (i === currentEnd) {
            jumps++;
            currentEnd = farthest;
            if (currentEnd >= n - 1) {
                break;
            }
        }
    }

    return jumps;
}
