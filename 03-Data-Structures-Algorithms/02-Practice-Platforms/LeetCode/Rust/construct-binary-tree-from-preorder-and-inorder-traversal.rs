/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(N) auxiliary

use std::rc::Rc;
use std::cell::RefCell;
use std::collections::HashMap;

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
    pub fn build_tree(preorder: Vec<i32>, inorder: Vec<i32>) -> Option<Rc<RefCell<TreeNode>>> {
        let mut in_map = HashMap::with_capacity(inorder.len());
        for (i, &v) in inorder.iter().enumerate() {
            in_map.insert(v, i);
        }

        let mut pre_index = 0;

        fn build(
            preorder: &[i32],
            in_start: isize,
            in_end: isize,
            pre_index: &mut usize,
            in_map: &HashMap<i32, usize>,
        ) -> Option<Rc<RefCell<TreeNode>>> {
            if in_start > in_end {
                return None;
            }

            let root_val = preorder[*pre_index];
            *pre_index += 1;

            let mid = in_map[&root_val] as isize;

            let left = build(preorder, in_start, mid - 1, pre_index, in_map);
            let right = build(preorder, mid + 1, in_end, pre_index, in_map);

            Some(Rc::new(RefCell::new(TreeNode {
                val: root_val,
                left,
                right,
            })))
        }

        build(&preorder, 0, inorder.len() as isize - 1, &mut pre_index, &in_map)
    }
}
