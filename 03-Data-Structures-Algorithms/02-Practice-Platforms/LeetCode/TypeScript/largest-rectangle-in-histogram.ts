/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 84 - Largest Rectangle in Histogram
 * Language: TypeScript
 *
 * Complexity:
 * - Time: O(N)
 * - Space: O(N)
 */

function largestRectangleArea(heights: number[]): number {
    const n = heights.length;
    const stack: number[] = [];
    let maxArea = 0;

    for (let i = 0; i <= n; i++) {
        const currHeight = (i === n) ? 0 : heights[i];
        while (stack.length > 0 && heights[stack[stack.length - 1]] >= currHeight) {
            const h = heights[stack.pop()!];
            const width = stack.length === 0 ? i : i - 1 - stack[stack.length - 1];
            maxArea = Math.max(maxArea, h * width);
        }
        stack.push(i);
    }

    return maxArea;
}
