/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n * k), where k is the max string length
// Space: O(n * k)

use std::collections::HashMap;

impl Solution {
    pub fn group_anagrams(strs: Vec<String>) -> Vec<Vec<String>> {
        let mut map: HashMap<[u8; 26], Vec<String>> = HashMap::new();

        for s in strs {
            let mut count = [0u8; 26];
            for &b in s.as_bytes() {
                count[(b - b'a') as usize] += 1;
            }
            map.entry(count).or_default().push(s);
        }

        map.into_values().collect()
    }
}
