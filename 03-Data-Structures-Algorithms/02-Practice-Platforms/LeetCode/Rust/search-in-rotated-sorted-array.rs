/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(log n)
// Space: O(1)

impl Solution {
    pub fn search(nums: Vec<i32>, target: i32) -> i32 {
        let mut left = 0;
        let mut right = nums.len() as i32 - 1;
        while left <= right {
            let mid = left + (right - left) / 2;
            let m_val = nums[mid as usize];
            if m_val == target {
                return mid;
            }
            let l_val = nums[left as usize];
            let r_val = nums[right as usize];
            if l_val <= m_val {
                if l_val <= target && target < m_val {
                    right = mid - 1;
                } else {
                    left = mid + 1;
                }
            } else {
                if m_val < target && target <= r_val {
                    left = mid + 1;
                } else {
                    right = mid - 1;
                }
            }
        }
        -1
    }
}
