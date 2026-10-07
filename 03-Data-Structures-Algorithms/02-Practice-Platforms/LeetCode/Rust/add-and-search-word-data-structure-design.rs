/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  addWord: O(L), search: O(26^D * L) where D is dot count, L is length
// Space: O(N * L) total heap memory across all nodes

pub struct WordDictionary {
    is_end: bool,
    children: [Option<Box<WordDictionary>>; 26],
}

impl WordDictionary {
    pub fn new() -> Self {
        const INIT: Option<Box<WordDictionary>> = None;
        WordDictionary {
            is_end: false,
            children: [INIT; 26],
        }
    }

    pub fn add_word(&mut self, word: String) {
        let mut curr = self;
        for b in word.bytes() {
            let idx = (b - b'a') as usize;
            curr = curr.children[idx].get_or_insert_with(|| Box::new(WordDictionary::new()));
        }
        curr.is_end = true;
    }

    pub fn search(&self, word: String) -> bool {
        self.search_helper(word.as_bytes(), 0)
    }

    fn search_helper(&self, bytes: &[u8], index: usize) -> bool {
        if index == bytes.len() {
            return self.is_end;
        }

        let b = bytes[index];
        if b == b'.' {
            for child in &self.children {
                if let Some(next_node) = child {
                    if next_node.search_helper(bytes, index + 1) {
                        return true;
                    }
                }
            }
            false
        } else {
            let idx = (b - b'a') as usize;
            if let Some(next_node) = &self.children[idx] {
                next_node.search_helper(bytes, index + 1)
            } else {
                false
            }
        }
    }
}
