/*
 * Problem: LeetCode 239 - Sliding Window Maximum
 * Difficulty: Hard
 * Concepts: Monotonic Queue, Sliding Window, Deque
 *
 * Time Complexity: O(n)
 * Space Complexity: O(k)
 */

export function maxSlidingWindow(nums: number[], k: number): number[] {
    const n = nums.length;
    const result: number[] = [];
    const dq: number[] = []; // Stores indices

    let head = 0; // Pointer for O(1) shift simulation without array copy overhead

    for (let i = 0; i < n; i++) {
        // Remove indices outside the window
        while (head < dq.length && dq[head] <= i - k) {
            head++;
        }

        // Maintain monotonic decreasing order
        while (dq.length > head && nums[dq[dq.length - 1]] <= nums[i]) {
            dq.pop();
        }

        dq.push(i);

        if (i >= k - 1) {
            result.push(nums[dq[head]]);
        }
    }

    return result;
}
