/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(W) where W is the maximum tree width

use std::rc::Rc;
use std::cell::RefCell;
use std::collections::VecDeque;

// Definition for a binary tree node.
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

impl Solution {
    pub fn level_order(root: Option<Rc<RefCell<TreeNode>>>) -> Vec<Vec<i32>> {
        let mut result = Vec::new();
        let Some(root_node) = root else {
            return result;
        };

        let mut queue = VecDeque::new();
        queue.push_back(root_node);

        while !queue.is_empty() {
            let level_size = queue.len();
            let mut current_level = Vec::with_capacity(level_size);

            for _ in 0..level_size {
                if let Some(node) = queue.pop_front() {
                    let n = node.borrow();
                    current_level.push(n.val);

                    if let Some(left) = n.left.clone() {
                        queue.push_back(left);
                    }
                    if let Some(right) = n.right.clone() {
                        queue.push_back(right);
                    }
                }
            }

            result.push(current_level);
        }

        result
    }
}
