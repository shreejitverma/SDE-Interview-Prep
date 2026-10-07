/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N log K)
// Space: O(1) auxiliary

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

impl Solution {
    pub fn merge_k_lists(mut lists: Vec<Option<Box<ListNode>>>) -> Option<Box<ListNode>> {
        if lists.is_empty() {
            return None;
        }

        let mut interval = 1;
        while interval < lists.len() {
            let mut i = 0;
            while i + interval < lists.len() {
                let l1 = lists[i].take();
                let l2 = lists[i + interval].take();
                lists[i] = Self::merge_two_lists(l1, l2);
                i += interval * 2;
            }
            interval *= 2;
        }

        lists[0].take()
    }

    fn merge_two_lists(
        mut l1: Option<Box<ListNode>>,
        mut l2: Option<Box<ListNode>>,
    ) -> Option<Box<ListNode>> {
        let mut dummy = ListNode::new(0);
        let mut curr = &mut dummy;

        while l1.is_some() && l2.is_some() {
            if l1.as_ref().unwrap().val <= l2.as_ref().unwrap().val {
                let next = l1.as_mut().unwrap().next.take();
                curr.next = l1;
                l1 = next;
            } else {
                let next = l2.as_mut().unwrap().next.take();
                curr.next = l2;
                l2 = next;
            }
            curr = curr.next.as_mut().unwrap();
        }

        curr.next = if l1.is_some() { l1 } else { l2 };
        dummy.next
    }
}
