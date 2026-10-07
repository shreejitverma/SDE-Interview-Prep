/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(H) recursion stack

use std::rc::Rc;
use std::cell::RefCell;

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
    pub fn is_valid_bst(root: Option<Rc<RefCell<TreeNode>>>) -> bool {
        Self::validate(&root, None, None)
    }

    fn validate(
        node: &Option<Rc<RefCell<TreeNode>>>,
        low: Option<i64>,
        high: Option<i64>,
    ) -> bool {
        if let Some(n) = node {
            let n_borrow = n.borrow();
            let val = n_borrow.val as i64;
            if let Some(l) = low {
                if val <= l {
                    return false;
                }
            }
            if let Some(h) = high {
                if val >= h {
                    return false;
                }
            }
            Self::validate(&n_borrow.left, low, Some(val))
                && Self::validate(&n_borrow.right, Some(val), high)
        } else {
            true
        }
    }
}
