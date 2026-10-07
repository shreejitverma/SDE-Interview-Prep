/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(1)

impl Solution {
    pub fn max_area(height: Vec<i32>) -> i32 {
        let mut left = 0;
        let mut right = height.len() - 1;
        let mut max_water = 0;
        while left < right {
            let h = height[left].min(height[right]);
            let area = h * (right - left) as i32;
            if area > max_water {
                max_water = area;
            }
            if height[left] < height[right] {
                left += 1;
            } else {
                right -= 1;
            }
        }
        max_water
    }
}
