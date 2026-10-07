/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time Complexity: Insert O(L), Search O(L), StartsWith O(L)
// Space Complexity: O(ALPHABET_SIZE * L * N)

const ALPHABET_SIZE: usize = 26;

#[derive(Default)]
pub struct TrieNode {
    children: [Option<Box<TrieNode>>; ALPHABET_SIZE],
    is_end_of_word: bool,
}

pub struct Trie {
    root: TrieNode,
}

impl Trie {
    pub fn new() -> Self {
        Trie {
            root: TrieNode::default(),
        }
    }

    pub fn insert(&mut self, key: &str) {
        let mut curr = &mut self.root;
        for &byte in key.as_bytes() {
            let idx = (byte - b'a') as usize;
            if curr.children[idx].is_none() {
                curr.children[idx] = Some(Box::new(TrieNode::default()));
            }
            curr = curr.children[idx].as_mut().unwrap();
        }
        curr.is_end_of_word = true;
    }

    pub fn search(&self, key: &str) -> bool {
        let mut curr = &self.root;
        for &byte in key.as_bytes() {
            let idx = (byte - b'a') as usize;
            match &curr.children[idx] {
                Some(next) => curr = next,
                None => return false,
            }
        }
        curr.is_end_of_word
    }

    pub fn starts_with(&self, prefix: &str) -> bool {
        let mut curr = &self.root;
        for &byte in prefix.as_bytes() {
            let idx = (byte - b'a') as usize;
            match &curr.children[idx] {
                Some(next) => curr = next,
                None => return false,
            }
        }
        true
    }
}

fn main() {
    let keys = vec!["the", "a", "there", "answer", "any", "by", "bye", "their"];
    let mut trie = Trie::new();

    for key in keys {
        trie.insert(key);
    }

    println!("the: {}", if trie.search("the") { "Present" } else { "Not present" });
    println!("these: {}", if trie.search("these") { "Present" } else { "Not present" });
    println!("th (prefix): {}", if trie.starts_with("th") { "Present" } else { "Not present" });
}
