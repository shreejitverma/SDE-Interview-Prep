/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(h)

use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn invert_tree(root: Option<Rc<RefCell<TreeNode>>>) -> Option<Rc<RefCell<TreeNode>>> {
        if let Some(node) = root.clone() {
            let mut n = node.borrow_mut();
            let left = n.left.take();
            let right = n.right.take();
            n.left = Self::invert_tree(right);
            n.right = Self::invert_tree(left);
        }
        root
    }
}
