/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(4^N / sqrt(N))
// Space: O(N) auxiliary (recursion stack)

pub struct Solution;

impl Solution {
    pub fn generate_parenthesis(n: i32) -> Vec<String> {
        let mut result = Vec::new();
        let mut path = String::with_capacity((2 * n) as usize);
        Self::backtrack(n, 0, 0, &mut path, &mut result);
        result
    }

    fn backtrack(n: i32, open_count: i32, close_count: i32, path: &mut String, result: &mut Vec<String>) {
        if open_count == n && close_count == n {
            result.push(path.clone());
            return;
        }

        if open_count < n {
            path.push('(');
            Self::backtrack(n, open_count + 1, close_count, path, result);
            path.pop();
        }

        if close_count < open_count {
            path.push(')');
            Self::backtrack(n, open_count, close_count + 1, path, result);
            path.pop();
        }
    }
}
