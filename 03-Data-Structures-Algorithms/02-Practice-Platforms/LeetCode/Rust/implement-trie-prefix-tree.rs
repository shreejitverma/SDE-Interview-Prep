/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(L) per insert / search / startsWith operation where L is string length
// Space: O(N * L) where N is number of words inserted

#[derive(Default)]
pub struct TrieNode {
    children: [Option<Box<TrieNode>>; 26],
    is_end: bool,
}

#[derive(Default)]
pub struct Trie {
    root: TrieNode,
}

impl Trie {
    pub fn new() -> Self {
        Default::default()
    }

    pub fn insert(&mut self, word: String) {
        let mut curr = &mut self.root;
        for &byte in word.as_bytes() {
            let idx = (byte - b'a') as usize;
            curr = curr.children[idx].get_or_insert_with(|| Box::new(TrieNode::default()));
        }
        curr.is_end = true;
    }

    fn find_prefix(&self, prefix: &str) -> Option<&TrieNode> {
        let mut curr = &self.root;
        for &byte in prefix.as_bytes() {
            let idx = (byte - b'a') as usize;
            match curr.children[idx].as_ref() {
                Some(next) => curr = next,
                None => return None,
            }
        }
        Some(curr)
    }

    pub fn search(&self, word: String) -> bool {
        self.find_prefix(&word).map_or(false, |node| node.is_end)
    }

    pub fn starts_with(&self, prefix: String) -> bool {
        self.find_prefix(&prefix).is_some()
    }
}
