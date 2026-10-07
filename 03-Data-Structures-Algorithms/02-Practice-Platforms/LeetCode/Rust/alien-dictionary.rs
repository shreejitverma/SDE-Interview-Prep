//! Author: Shreejit Verma
//! GitHub: https://github.com/shreejitverma
//!
//! Problem: LeetCode 269 - Alien Dictionary
//! Difficulty: Hard
//! Language: Rust
//!
//! Performance Analysis:
//! - Time Complexity: O(C) where C is the total number of characters across all words.
//! - Space Complexity: O(1) auxiliary space bounded by alphabet size 26.

use std::collections::VecDeque;

pub struct Solution;

impl Solution {
    pub fn alien_order(words: Vec<String>) -> String {
        if words.is_empty() {
            return String::new();
        }

        let mut present = [false; 26];
        let mut in_degree = [0usize; 26];
        let mut adj: Vec<Vec<usize>> = vec![Vec::new(); 26];
        let mut unique_count = 0;

        for w in &words {
            for &b in w.as_bytes() {
                let idx = (b - b'a') as usize;
                if !present[idx] {
                    present[idx] = true;
                    unique_count += 1;
                }
            }
        }

        for i in 0..words.len().saturating_sub(1) {
            let w1 = words[i].as_bytes();
            let w2 = words[i + 1].as_bytes();

            // Prefix validation check
            if w1.len() > w2.len() && w1.starts_with(w2) {
                return String::new();
            }

            let min_len = w1.len().min(w2.len());
            for j in 0..min_len {
                if w1[j] != w2[j] {
                    let u = (w1[j] - b'a') as usize;
                    let v = (w2[j] - b'a') as usize;
                    adj[u].push(v);
                    in_degree[v] += 1;
                    break;
                }
            }
        }

        let mut queue = VecDeque::new();
        for i in 0..26 {
            if present[i] && in_degree[i] == 0 {
                queue.push_back(i);
            }
        }

        let mut result = Vec::new();
        while let Some(u) = queue.pop_front() {
            result.push((b'a' + u as u8) as char);

            for &v in &adj[u] {
                in_degree[v] -= 1;
                if in_degree[v] == 0 {
                    queue.push_back(v);
                }
            }
        }

        if result.len() < unique_count {
            return String::new();
        }

        result.into_iter().collect()
    }
}

fn main() {
    let words1 = vec![
        "wrt".to_string(),
        "wrf".to_string(),
        "er".to_string(),
        "ett".to_string(),
        "rftt".to_string(),
    ];
    println!("Order 1: {}", Solution::alien_order(words1)); // "wertf"

    let words2 = vec!["z".to_string(), "x".to_string()];
    println!("Order 2: {}", Solution::alien_order(words2)); // "zx"

    let words3 = vec!["z".to_string(), "x".to_string(), "z".to_string()];
    println!("Order 3: {}", Solution::alien_order(words3)); // ""
}
