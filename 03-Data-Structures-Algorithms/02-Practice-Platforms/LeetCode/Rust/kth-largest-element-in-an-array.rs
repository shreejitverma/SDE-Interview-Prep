use std::cmp::Reverse;
use std::collections::BinaryHeap;

/*
 * Problem: LeetCode 215 - Kth Largest Element in an Array
 * Difficulty: Medium
 * Concepts: QuickSelect, Min-Heap, Divide and Conquer
 *
 * Time Complexity: O(n log k) (Min-Heap)
 * Space Complexity: O(k)
 */

pub struct Solution;

impl Solution {
    pub fn find_kth_largest(nums: Vec<i32>, k: i32) -> i32 {
        let k = k as usize;
        let mut min_heap: BinaryHeap<Reverse<i32>> = BinaryHeap::with_capacity(k + 1);

        for num in nums {
            min_heap.push(Reverse(num));
            if min_heap.len() > k {
                min_heap.pop();
            }
        }

        min_heap.peek().unwrap().0
    }
}
