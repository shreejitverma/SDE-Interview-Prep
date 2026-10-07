/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 199 - Binary Tree Right Side View
 * Language: Rust
 *
 * Complexity:
 * - Time: O(N)
 * - Space: O(H) where H is tree height
 */

use std::rc::Rc;
use std::cell::RefCell;

#[derive(Debug, PartialEq, Eq)]
pub struct TreeNode {
    pub val: i32,
    pub left: Option<Rc<RefCell<TreeNode>>>,
    pub right: Option<Rc<RefCell<TreeNode>>>,
}

impl TreeNode {
    #[inline]
    pub fn new(val: i32) -> Self {
        TreeNode {
            val,
            left: None,
            right: None,
        }
    }
}

pub struct Solution;

impl Solution {
    pub fn right_side_view(root: Option<Rc<RefCell<TreeNode>>>) -> Vec<i32> {
        let mut result = Vec::new();
        Self::dfs(&root, 0, &mut result);
        result
    }

    fn dfs(node: &Option<Rc<RefCell<TreeNode>>>, depth: usize, result: &mut Vec<i32>) {
        if let Some(n) = node {
            let n_borrow = n.borrow();
            if depth == result.len() {
                result.push(n_borrow.val);
            }
            Self::dfs(&n_borrow.right, depth + 1, result);
            Self::dfs(&n_borrow.left, depth + 1, result);
        }
    }
}
