/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(1)

impl Solution {
    pub fn climb_stairs(n: i32) -> i32 {
        if n <= 2 {
            return n;
        }
        let mut prev2 = 1;
        let mut prev1 = 2;
        for _ in 3..=n {
            let current = prev1 + prev2;
            prev2 = prev1;
            prev1 = current;
        }
        prev1
    }
}
