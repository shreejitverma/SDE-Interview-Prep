/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 853 - Car Fleet
 * Language: Rust
 *
 * Complexity:
 * - Time: O(N log N)
 * - Space: O(N)
 */

pub struct Solution;

impl Solution {
    pub fn car_fleet(target: i32, position: Vec<i32>, speed: Vec<i32>) -> i32 {
        let n = position.len();
        if n == 0 {
            return 0;
        }

        let mut cars: Vec<(i32, f64)> = position
            .into_iter()
            .zip(speed.into_iter())
            .map(|(p, s)| (p, (target - p) as f64 / s as f64))
            .collect();

        cars.sort_unstable_by_key(|&(p, _)| p);

        let mut fleets = 0;
        let mut max_time = 0.0;

        for &(_, time) in cars.iter().rev() {
            if time > max_time {
                max_time = time;
                fleets += 1;
            }
        }

        fleets
    }
}
