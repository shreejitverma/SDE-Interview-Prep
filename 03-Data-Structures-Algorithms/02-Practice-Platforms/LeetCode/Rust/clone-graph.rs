/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(V + E)
// Space: O(V) auxiliary

use std::rc::Rc;
use std::cell::RefCell;
use std::collections::{HashMap, VecDeque};

// Definition for a Node.
#[derive(Debug, PartialEq, Eq)]
pub struct Node {
    pub val: i32,
    pub neighbors: Vec<Rc<RefCell<Node>>>,
}

impl Node {
    #[inline]
    pub fn new(val: i32) -> Self {
        Node {
            val,
            neighbors: Vec::new(),
        }
    }
}

impl Solution {
    pub fn clone_graph(node: Option<Rc<RefCell<Node>>>) -> Option<Rc<RefCell<Node>>> {
        let Some(start_node) = node else {
            return None;
        };

        let mut visited: HashMap<i32, Rc<RefCell<Node>>> = HashMap::new();
        let mut queue = VecDeque::new();

        let clone_start = Rc::new(RefCell::new(Node::new(start_node.borrow().val)));
        visited.insert(start_node.borrow().val, clone_start.clone());
        queue.push_back(start_node);

        while let Some(curr) = queue.pop_front() {
            let curr_borrow = curr.borrow();
            let curr_clone = visited.get(&curr_borrow.val).unwrap().clone();

            for neighbor in &curr_borrow.neighbors {
                let n_val = neighbor.borrow().val;
                if !visited.contains_key(&n_val) {
                    let neighbor_clone = Rc::new(RefCell::new(Node::new(n_val)));
                    visited.insert(n_val, neighbor_clone);
                    queue.push_back(neighbor.clone());
                }
                let neighbor_clone = visited.get(&n_val).unwrap().clone();
                curr_clone.borrow_mut().neighbors.push(neighbor_clone);
            }
        }

        Some(clone_start)
    }
}
