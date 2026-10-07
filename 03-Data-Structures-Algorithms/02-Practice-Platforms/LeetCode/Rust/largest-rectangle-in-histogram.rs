/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 84 - Largest Rectangle in Histogram
 * Language: Rust
 *
 * Complexity:
 * - Time: O(N)
 * - Space: O(N)
 */

pub struct Solution;

impl Solution {
    pub fn largest_rectangle_area(heights: Vec<i32>) -> i32 {
        let n = heights.len();
        let mut stack: Vec<usize> = Vec::with_capacity(n + 1);
        let mut max_area: i32 = 0;

        for i in 0..=n {
            let curr_height = if i < n { heights[i] } else { 0 };

            while let Some(&top_idx) = stack.last() {
                if heights[top_idx] >= curr_height {
                    stack.pop();
                    let h = heights[top_idx];
                    let width = match stack.last() {
                        Some(&prev_idx) => (i - 1 - prev_idx) as i32,
                        None => i as i32,
                    };
                    max_area = max_area.max(h * width);
                } else {
                    break;
                }
            }
            stack.push(i);
        }

        max_area
    }
}
