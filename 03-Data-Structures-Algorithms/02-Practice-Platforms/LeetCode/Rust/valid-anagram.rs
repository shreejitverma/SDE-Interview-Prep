/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(1)

impl Solution {
    pub fn is_anagram(s: String, t: String) -> bool {
        if s.len() != t.len() {
            return false;
        }
        let mut counts = [0i32; 26];
        for (sb, tb) in s.bytes().zip(t.bytes()) {
            counts[(sb - b'a') as usize] += 1;
            counts[(tb - b'a') as usize] -= 1;
        }
        counts.iter().all(|&c| c == 0)
    }
}
