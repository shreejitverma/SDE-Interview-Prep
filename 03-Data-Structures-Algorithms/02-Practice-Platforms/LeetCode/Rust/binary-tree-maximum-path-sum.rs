/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(H)

use std::rc::Rc;
use std::cell::RefCell;
use std::cmp;

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

pub struct Solution;

impl Solution {
    pub fn max_path_sum(root: Option<Rc<RefCell<TreeNode>>>) -> i32 {
        let mut max_sum = i32::MIN;
        Self::max_gain(&root, &mut max_sum);
        max_sum
    }

    fn max_gain(node: &Option<Rc<RefCell<TreeNode>>>, max_sum: &mut i32) -> i32 {
        if let Some(n) = node {
            let n_borrow = n.borrow();
            let left_gain = cmp::max(0, Self::max_gain(&n_borrow.left, max_sum));
            let right_gain = cmp::max(0, Self::max_gain(&n_borrow.right, max_sum));

            let current_path = n_borrow.val + left_gain + right_gain;
            *max_sum = cmp::max(*max_sum, current_path);

            n_borrow.val + cmp::max(left_gain, right_gain)
        } else {
            0
        }
    }
}
