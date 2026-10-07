/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 167 - Two Sum II - Input Array Is Sorted
 * Language: TypeScript
 *
 * Complexity:
 * - Time: O(N)
 * - Space: O(1)
 */

function twoSum(numbers: number[], target: number): number[] {
    let left = 0;
    let right = numbers.length - 1;

    while (left < right) {
        const sum = numbers[left] + numbers[right];
        if (sum === target) {
            return [left + 1, right + 1];
        } else if (sum < target) {
            left++;
        } else {
            right--;
        }
    }

    return [-1, -1];
}
