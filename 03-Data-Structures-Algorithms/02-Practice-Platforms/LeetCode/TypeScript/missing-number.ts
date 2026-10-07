/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Problem: LeetCode 268 - Missing Number
 * Difficulty: Easy
 * Language: TypeScript
 *
 * Performance Analysis:
 * - Time Complexity: O(N) single-pass bitwise XOR traversal.
 * - Space Complexity: O(1) auxiliary space.
 */

export function missingNumber(nums: number[]): number {
    let missing = nums.length;
    for (let i = 0; i < nums.length; ++i) {
        missing ^= i ^ nums[i];
    }
    return missing;
}

export function missingNumberGauss(nums: number[]): number {
    const n = nums.length;
    let expectedSum = (n * (n + 1)) / 2;
    for (let i = 0; i < n; ++i) {
        expectedSum -= nums[i];
    }
    return expectedSum;
}

// Standalone execution test
if (typeof require !== 'undefined' && require.main === module) {
    console.log('Missing 1:', missingNumber([3, 0, 1])); // 2
    console.log('Missing 2:', missingNumber([0, 1])); // 2
    console.log('Missing 3:', missingNumber([9, 6, 4, 2, 3, 5, 7, 0, 1])); // 8
}
