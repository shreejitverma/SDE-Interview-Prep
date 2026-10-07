/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 875 - Koko Eating Bananas
 * Language: Rust
 *
 * Complexity:
 * - Time: O(N * log(max(piles)))
 * - Space: O(1)
 */

pub struct Solution;

impl Solution {
    pub fn min_eating_speed(piles: Vec<i32>, h: i32) -> i32 {
        let mut left: i32 = 1;
        let mut right: i32 = *piles.iter().max().unwrap_or(&1);

        let can_finish = |k: i32| -> bool {
            let mut hours: i64 = 0;
            for &pile in &piles {
                hours += ((pile as i64) + (k as i64) - 1) / (k as i64);
            }
            hours <= (h as i64)
        };

        while left <= right {
            let mid = left + (right - left) / 2;
            if can_finish(mid) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }

        left
    }
}
