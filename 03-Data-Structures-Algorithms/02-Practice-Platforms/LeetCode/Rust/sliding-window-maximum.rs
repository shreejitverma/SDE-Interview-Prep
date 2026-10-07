use std::collections::VecDeque;

/*
 * Problem: LeetCode 239 - Sliding Window Maximum
 * Difficulty: Hard
 * Concepts: Monotonic Queue, Sliding Window, Deque
 *
 * Time Complexity: O(n)
 * Space Complexity: O(k)
 */

pub struct Solution;

impl Solution {
    pub fn max_sliding_window(nums: Vec<i32>, k: i32) -> Vec<i32> {
        let n = nums.len();
        let k = k as usize;
        let mut result = Vec::with_capacity(n.saturating_sub(k) + 1);
        let mut dq: VecDeque<usize> = VecDeque::with_capacity(k);

        for i in 0..n {
            if let Some(&front) = dq.front() {
                if front + k <= i {
                    dq.pop_front();
                }
            }

            while let Some(&back) = dq.back() {
                if nums[back] <= nums[i] {
                    dq.pop_back();
                } else {
                    break;
                }
            }

            dq.push_back(i);

            if i >= k - 1 {
                result.push(nums[*dq.front().unwrap()]);
            }
        }

        result
    }
}
