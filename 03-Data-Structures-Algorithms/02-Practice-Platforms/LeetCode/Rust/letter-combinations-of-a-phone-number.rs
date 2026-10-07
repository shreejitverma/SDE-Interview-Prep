/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N * 4^N)
// Space: O(N) auxiliary (recursion stack)

pub struct Solution;

const MAPPING: [&str; 10] = [
    "", "", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz",
];

impl Solution {
    pub fn letter_combinations(digits: String) -> Vec<String> {
        let mut result = Vec::new();
        if digits.is_empty() {
            return result;
        }

        let digits_bytes = digits.as_bytes();
        let mut path = String::with_capacity(digits_bytes.len());
        Self::backtrack(digits_bytes, 0, &mut path, &mut result);
        result
    }

    fn backtrack(digits: &[u8], index: usize, path: &mut String, result: &mut Vec<String>) {
        if index == digits.len() {
            result.push(path.clone());
            return;
        }

        let digit_idx = (digits[index] - b'0') as usize;
        let letters = MAPPING[digit_idx];
        for ch in letters.chars() {
            path.push(ch);
            Self::backtrack(digits, index + 1, path, result);
            path.pop();
        }
    }
}
