/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n log n)
// Space: O(1) auxiliary

impl Solution {
    pub fn merge(mut intervals: Vec<Vec<i32>>) -> Vec<Vec<i32>> {
        if intervals.len() <= 1 {
            return intervals;
        }

        intervals.sort_unstable_by_key(|item| item[0]);
        let mut result = Vec::with_capacity(intervals.len());
        result.push(intervals[0].clone());

        for i in 1..intervals.len() {
            let last = result.last_mut().unwrap();
            let curr = &intervals[i];

            if curr[0] <= last[1] {
                last[1] = last[1].max(curr[1]);
            } else {
                result.push(curr.clone());
            }
        }

        result
    }
}
