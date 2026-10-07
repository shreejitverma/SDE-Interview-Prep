/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(H + k) where H is tree height
// Space: O(H) auxiliary (stack depth)

use std::rc::Rc;
use std::cell::RefCell;

#[derive(Debug, PartialEq, Eq)]
pub struct TreeNode {
    pub val: i32,
    pub left: Option<Rc<RefCell<TreeNode>>>,
    pub right: Option<Rc<RefCell<TreeNode>>>,
}

pub struct Solution;

impl Solution {
    pub fn kth_smallest(root: Option<Rc<RefCell<TreeNode>>>, mut k: i32) -> i32 {
        let mut stack = Vec::new();
        let mut curr = root;

        while curr.is_some() || !stack.is_empty() {
            while let Some(node) = curr {
                curr = node.borrow().left.clone();
                stack.push(node);
            }

            if let Some(node) = stack.pop() {
                k -= 1;
                if k == 0 {
                    return node.borrow().val;
                }
                curr = node.borrow().right.clone();
            }
        }

        -1
    }
}
