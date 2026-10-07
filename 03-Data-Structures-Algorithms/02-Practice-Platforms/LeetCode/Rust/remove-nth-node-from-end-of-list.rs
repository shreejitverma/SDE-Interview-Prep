/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(1)

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
    pub fn remove_nth_from_end(head: Option<Box<ListNode>>, n: i32) -> Option<Box<ListNode>> {
        let mut dummy = Some(Box::new(ListNode { val: 0, next: head }));
        let mut len = 0;
        {
            let mut curr = dummy.as_ref().unwrap().next.as_ref();
            while let Some(node) = curr {
                len += 1;
                curr = node.next.as_ref();
            }
        }

        let mut curr = dummy.as_mut();
        for _ in 0..(len - n) {
            curr = curr.unwrap().next.as_mut();
        }

        let next = curr.as_mut().unwrap().next.as_mut().unwrap().next.take();
        curr.unwrap().next = next;

        dummy.unwrap().next
    }
}
