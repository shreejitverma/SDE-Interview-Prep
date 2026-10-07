/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Problem: LeetCode 338 - Counting Bits
 * Difficulty: Easy
 * Language: TypeScript
 *
 * Performance Analysis:
 * - Time Complexity: O(N) single-pass dynamic programming.
 * - Space Complexity: O(1) auxiliary space (O(N) return array).
 */

export function countBits(n: number): number[] {
    const ans = new Int32Array(n + 1);
    for (let i = 1; i <= n; ++i) {
        ans[i] = ans[i >> 1] + (i & 1);
    }
    return Array.from(ans);
}

export function countBitsKernighan(n: number): number[] {
    const ans = new Int32Array(n + 1);
    for (let i = 1; i <= n; ++i) {
        ans[i] = ans[i & (i - 1)] + 1;
    }
    return Array.from(ans);
}

// Standalone execution test
if (typeof require !== 'undefined' && require.main === module) {
    console.log('countBits(2):', countBits(2)); // [0, 1, 1]
    console.log('countBits(5):', countBits(5)); // [0, 1, 1, 2, 1, 2]
}
