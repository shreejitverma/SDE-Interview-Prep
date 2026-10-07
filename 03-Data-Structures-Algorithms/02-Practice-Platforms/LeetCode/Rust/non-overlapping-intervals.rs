/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N log N)
// Space: O(1) auxiliary

pub struct Solution;

impl Solution {
    pub fn erase_overlap_intervals(mut intervals: Vec<Vec<i32>>) -> i32 {
        if intervals.is_empty() {
            return 0;
        }

        intervals.sort_unstable_by_key(|k| k[1]);

        let mut removals = 0;
        let mut prev_end = intervals[0][1];

        for interval in intervals.iter().skip(1) {
            if interval[0] < prev_end {
                removals += 1;
            } else {
                prev_end = interval[1];
            }
        }

        removals
    }
}
