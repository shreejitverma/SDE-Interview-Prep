/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(log(min(M, N)))
// Space: O(1)

use std::cmp;

pub struct Solution;

impl Solution {
    pub fn find_median_sorted_arrays(nums1: Vec<i32>, nums2: Vec<i32>) -> f64 {
        if nums1.len() > nums2.len() {
            return Self::find_median_sorted_arrays(nums2, nums1);
        }

        let m = nums1.len();
        let n = nums2.len();
        let mut low = 0;
        let mut high = m;

        while low <= high {
            let partition_x = (low + high) / 2;
            let partition_y = (m + n + 1) / 2 - partition_x;

            let max_left_x = if partition_x == 0 { i32::MIN } else { nums1[partition_x - 1] };
            let min_right_x = if partition_x == m { i32::MAX } else { nums1[partition_x] };

            let max_left_y = if partition_y == 0 { i32::MIN } else { nums2[partition_y - 1] };
            let min_right_y = if partition_y == n { i32::MAX } else { nums2[partition_y] };

            if max_left_x <= min_right_y && max_left_y <= min_right_x {
                if (m + n) % 2 != 0 {
                    return cmp::max(max_left_x, max_left_y) as f64;
                } else {
                    return (cmp::max(max_left_x, max_left_y) as f64
                        + cmp::min(min_right_x, min_right_y) as f64)
                        / 2.0;
                }
            } else if max_left_x > min_right_y {
                high = partition_x - 1;
            } else {
                low = partition_x + 1;
            }
        }

        0.0
    }
}
