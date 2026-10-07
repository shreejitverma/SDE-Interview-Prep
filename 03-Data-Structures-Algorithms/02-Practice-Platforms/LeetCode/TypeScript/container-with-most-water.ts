/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(1)

function maxArea(height: number[]): number {
    let left = 0;
    let right = height.length - 1;
    let maxWater = 0;
    while (left < right) {
        const h = Math.min(height[left], height[right]);
        const area = h * (right - left);
        if (area > maxWater) {
            maxWater = area;
        }
        if (height[left] < height[right]) {
            left++;
        } else {
            right--;
        }
    }
    return maxWater;
}
