/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 543 - Diameter of Binary Tree
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
    pub fn diameter_of_binary_tree(root: Option<Rc<RefCell<TreeNode>>>) -> i32 {
        let mut max_diameter = 0;
        Self::max_depth(&root, &mut max_diameter);
        max_diameter
    }

    fn max_depth(node: &Option<Rc<RefCell<TreeNode>>>, max_diameter: &mut i32) -> i32 {
        if let Some(n) = node {
            let n_borrow = n.borrow();
            let left_depth = Self::max_depth(&n_borrow.left, max_diameter);
            let right_depth = Self::max_depth(&n_borrow.right, max_diameter);

            *max_diameter = (*max_diameter).max(left_depth + right_depth);

            1 + left_depth.max(right_depth)
        } else {
            0
        }
    }
}
