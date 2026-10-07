/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(H) where H is tree height
// Space: O(1) auxiliary

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
    pub fn lowest_common_ancestor(
        root: Option<Rc<RefCell<TreeNode>>>,
        p: Option<Rc<RefCell<TreeNode>>>,
        q: Option<Rc<RefCell<TreeNode>>>,
    ) -> Option<Rc<RefCell<TreeNode>>> {
        let p_val = p.as_ref()?.borrow().val;
        let q_val = q.as_ref()?.borrow().val;
        let small = p_val.min(q_val);
        let large = p_val.max(q_val);

        let mut curr = root;
        while let Some(node) = curr {
            let val = node.borrow().val;
            if val > large {
                curr = node.borrow().left.clone();
            } else if val < small {
                curr = node.borrow().right.clone();
            } else {
                return Some(node);
            }
        }

        None
    }
}
