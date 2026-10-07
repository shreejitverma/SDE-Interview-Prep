/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(H)

use std::cell::RefCell;
use std::rc::Rc;

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
    pub fn lowest_common_ancestor(
        root: Option<Rc<RefCell<TreeNode>>>,
        p: Option<Rc<RefCell<TreeNode>>>,
        q: Option<Rc<RefCell<TreeNode>>>,
    ) -> Option<Rc<RefCell<TreeNode>>> {
        let p_node = p.as_ref()?;
        let q_node = q.as_ref()?;
        Self::helper(&root, p_node, q_node)
    }

    fn helper(
        root: &Option<Rc<RefCell<TreeNode>>>,
        p: &Rc<RefCell<TreeNode>>,
        q: &Rc<RefCell<TreeNode>>,
    ) -> Option<Rc<RefCell<TreeNode>>> {
        let curr = root.as_ref()?;
        if Rc::ptr_eq(curr, p) || Rc::ptr_eq(curr, q) {
            return Some(Rc::clone(curr));
        }

        let left = Self::helper(&curr.borrow().left, p, q);
        let right = Self::helper(&curr.borrow().right, p, q);

        match (left, right) {
            (Some(_), Some(_)) => Some(Rc::clone(curr)),
            (Some(l), None) => Some(l),
            (None, Some(r)) => Some(r),
            (None, None) => None,
        }
    }
}
