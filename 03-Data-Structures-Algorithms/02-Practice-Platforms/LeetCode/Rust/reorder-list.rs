/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(N)

use std::collections::VecDeque;

// Definition for singly-linked list.
#[derive(PartialEq, Eq, Clone, Debug)]
pub struct ListNode {
    pub val: i32,
    pub next: Option<Box<ListNode>>,
}

impl ListNode {
    #[inline]
    pub fn new(val: i32) -> Self {
        ListNode { next: None, val }
    }
}

pub struct Solution;

impl Solution {
    pub fn reorder_list(head: &mut Option<Box<ListNode>>) {
        if head.is_none() {
            return;
        }

        let mut deque = VecDeque::new();
        let mut curr = head.take();
        while let Some(mut node) = curr {
            curr = node.next.take();
            deque.push_back(node);
        }

        let mut dummy = ListNode::new(0);
        let mut tail = &mut dummy;
        let mut take_front = true;

        while !deque.is_empty() {
            let next_node = if take_front {
                deque.pop_front().unwrap()
            } else {
                deque.pop_back().unwrap()
            };
            take_front = !take_front;

            tail.next = Some(next_node);
            tail = tail.next.as_mut().unwrap();
        }

        *head = dummy.next;
    }
}
