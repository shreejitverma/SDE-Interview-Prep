/*
 * Problem: LeetCode 131 - Palindrome Partitioning
 * Difficulty: Medium
 * Concepts: Backtracking, String, Dynamic Programming
 *
 * Time Complexity: O(n * 2^n)
 * Space Complexity: O(n) recursion stack
 */

pub struct Solution;

impl Solution {
    pub fn partition(s: String) -> Vec<Vec<String>> {
        let mut result = Vec::new();
        let mut current = Vec::new();
        let bytes = s.as_bytes();
        Self::backtrack(bytes, 0, &mut current, &mut result);
        result
    }

    fn backtrack(
        bytes: &[u8],
        start: usize,
        current: &mut Vec<String>,
        result: &mut Vec<Vec<String>>,
    ) {
        if start == bytes.len() {
            result.push(current.clone());
            return;
        }

        for end in start..bytes.len() {
            if Self::is_palindrome(bytes, start, end) {
                let s = String::from_utf8_lossy(&bytes[start..=end]).to_string();
                current.push(s);
                Self::backtrack(bytes, end + 1, current, result);
                current.pop();
            }
        }
    }

    fn is_palindrome(bytes: &[u8], mut left: usize, mut right: usize) -> bool {
        while left < right {
            if bytes[left] != bytes[right] {
                return false;
            }
            left += 1;
            right -= 1;
        }
        true
    }
}
