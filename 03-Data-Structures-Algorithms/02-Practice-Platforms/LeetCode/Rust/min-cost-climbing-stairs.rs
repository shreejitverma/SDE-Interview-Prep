/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 746 - Min Cost Climbing Stairs
 * Language: Rust
 *
 * Complexity:
 * - Time: O(N)
 * - Space: O(1)
 */

pub struct Solution;

impl Solution {
    pub fn min_cost_climbing_stairs(cost: Vec<i32>) -> i32 {
        let mut prev2 = 0;
        let mut prev1 = 0;

        for c in cost {
            let curr = c + prev1.min(prev2);
            prev2 = prev1;
            prev1 = curr;
        }

        prev1.min(prev2)
    }
}
