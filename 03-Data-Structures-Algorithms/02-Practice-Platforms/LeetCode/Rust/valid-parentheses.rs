/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(n)

impl Solution {
    pub fn is_valid(s: String) -> bool {
        if s.len() % 2 != 0 {
            return false;
        }
        let mut stack = Vec::with_capacity(s.len());
        for b in s.bytes() {
            match b {
                b'(' => stack.push(b')'),
                b'{' => stack.push(b'}'),
                b'[' => stack.push(b']'),
                expected => {
                    if stack.pop() != Some(expected) {
                        return false;
                    }
                }
            }
        }
        stack.is_empty()
    }
}
