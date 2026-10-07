/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(N)

use std::cell::RefCell;
use std::collections::HashMap;
use std::rc::Rc;

pub struct Node {
    pub val: i32,
    pub next: Option<Rc<RefCell<Node>>>,
    pub random: Option<Rc<RefCell<Node>>>,
}

impl Node {
    pub fn new(val: i32) -> Rc<RefCell<Self>> {
        Rc::new(RefCell::new(Node {
            val,
            next: None,
            random: None,
        }))
    }
}

pub struct Solution;

impl Solution {
    pub fn copy_random_list(head: Option<Rc<RefCell<Node>>>) -> Option<Rc<RefCell<Node>>> {
        let head_ref = head.as_ref()?;
        let mut visited: HashMap<usize, Rc<RefCell<Node>>> = HashMap::new();

        let mut curr = Some(Rc::clone(head_ref));
        while let Some(node) = curr {
            let addr = Rc::as_ptr(&node) as usize;
            let val = node.borrow().val;
            visited.insert(addr, Node::new(val));
            curr = node.borrow().next.clone();
        }

        curr = Some(Rc::clone(head_ref));
        while let Some(node) = curr {
            let addr = Rc::as_ptr(&node) as usize;
            let clone_node = visited.get(&addr).unwrap();

            if let Some(next_node) = &node.borrow().next {
                let next_addr = Rc::as_ptr(next_node) as usize;
                clone_node.borrow_mut().next = visited.get(&next_addr).cloned();
            }

            if let Some(random_node) = &node.borrow().random {
                let random_addr = Rc::as_ptr(random_node) as usize;
                clone_node.borrow_mut().random = visited.get(&random_addr).cloned();
            }

            curr = node.borrow().next.clone();
        }

        let head_addr = Rc::as_ptr(head_ref) as usize;
        visited.get(&head_addr).cloned()
    }
}
