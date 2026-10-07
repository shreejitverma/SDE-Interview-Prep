//! Author: Shreejit Verma
//! GitHub: https://github.com/shreejitverma
//!
//! Problem: LeetCode 212 - Word Search II
//! Difficulty: Hard
//! Language: Rust
//!
//! Performance Analysis:
//! - Time Complexity: O(M * N * 4 * 3^(L - 1)) with proactive subtree pruning.
//! - Space Complexity: O(Sum(len(words))) for Trie node storage, O(L) call stack.

#[derive(Default)]
struct TrieNode {
    children: [Option<Box<TrieNode>>; 26],
    word: Option<String>,
    word_count: usize,
}

impl TrieNode {
    fn insert(&mut self, word: String) {
        let mut curr = self;
        for &b in word.as_bytes() {
            let idx = (b - b'a') as usize;
            curr = curr.children[idx].get_or_insert_with(Box::default);
            curr.word_count += 1;
        }
        curr.word = Some(word);
    }
}

pub struct Solution;

impl Solution {
    pub fn find_words(mut board: Vec<Vec<char>>, words: Vec<String>) -> Vec<String> {
        let mut result = Vec::new();
        if board.is_empty() || board[0].is_empty() || words.is_empty() {
            return result;
        }

        let mut root = TrieNode::default();
        for w in words {
            root.insert(w);
        }

        let m = board.len();
        let n = board[0].len();

        for r in 0..m {
            for c in 0..n {
                let idx = (board[r][c] as u8 - b'a') as usize;
                if idx < 26 && root.children[idx].is_some() {
                    Self::dfs(&mut board, r, c, &mut root, idx, &mut result);
                }
            }
        }

        result
    }

    fn dfs(
        board: &mut Vec<Vec<char>>,
        r: usize,
        c: usize,
        parent: &mut TrieNode,
        idx: usize,
        result: &mut Vec<String>,
    ) {
        let curr = match parent.children[idx].as_mut() {
            Some(node) if node.word_count > 0 => node,
            _ => return,
        };

        if let Some(w) = curr.word.take() {
            result.push(w);
            curr.word_count = curr.word_count.saturating_sub(1);
            if curr.word_count == 0 {
                parent.children[idx] = None;
                return;
            }
        }

        let original = board[r][c];
        board[r][c] = '#';

        let m = board.len();
        let n = board[0].len();
        let directions: [(isize, isize); 4] = [(-1, 0), (1, 0), (0, -1), (0, 1)];

        for (dr, dc) in directions {
            let nr = r as isize + dr;
            let nc = c as isize + dc;
            if nr >= 0 && nr < m as isize && nc >= 0 && nc < n as isize {
                let (ur, uc) = (nr as usize, nc as usize);
                let next_char = board[ur][uc];
                if next_char != '#' {
                    let next_idx = (next_char as u8 - b'a') as usize;
                    if next_idx < 26 && curr.children[next_idx].is_some() {
                        Self::dfs(board, ur, uc, curr, next_idx, result);
                    }
                }
            }
        }

        board[r][c] = original;
    }
}

fn main() {
    let board = vec![
        vec!['o', 'a', 'a', 'n'],
        vec!['e', 't', 'a', 'e'],
        vec!['i', 'h', 'k', 'r'],
        vec!['i', 'f', 'l', 'v'],
    ];
    let words = vec![
        "oath".to_string(),
        "pea".to_string(),
        "eat".to_string(),
        "rain".to_string(),
    ];
    let res = Solution::find_words(board, words);
    println!("Found words: {:?}", res);
}
