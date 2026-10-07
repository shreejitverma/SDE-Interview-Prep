/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(1)

package main

func maxArea(height []int) int {
    left := 0
    right := len(height) - 1
    maxWater := 0
    for left < right {
        h := height[left]
        if height[right] < h {
            h = height[right]
        }
        area := h * (right - left)
        if area > maxWater {
            maxWater = area
        }
        if height[left] < height[right] {
            left++
        } else {
            right--
        }
    }
    return maxWater
}
