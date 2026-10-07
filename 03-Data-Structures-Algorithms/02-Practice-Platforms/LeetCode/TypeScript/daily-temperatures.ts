/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 739 - Daily Temperatures
 * Language: TypeScript
 *
 * Complexity:
 * - Time: O(N)
 * - Space: O(N)
 */

function dailyTemperatures(temperatures: number[]): number[] {
    const n = temperatures.length;
    const result: number[] = new Array(n).fill(0);
    const stack: number[] = [];

    for (let i = 0; i < n; i++) {
        while (stack.length > 0 && temperatures[stack[stack.length - 1]] < temperatures[i]) {
            const prevIndex = stack.pop()!;
            result[prevIndex] = i - prevIndex;
        }
        stack.push(i);
    }

    return result;
}
