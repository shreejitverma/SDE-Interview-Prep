/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n + m)
// Space: O(1)

impl Solution {
    pub fn merge_two_lists(
        mut list1: Option<Box<ListNode>>,
        mut list2: Option<Box<ListNode>>,
    ) -> Option<Box<ListNode>> {
        let mut dummy = Box::new(ListNode::new(0));
        let mut current = &mut dummy;

        while list1.is_some() && list2.is_some() {
            if list1.as_ref().unwrap().val <= list2.as_ref().unwrap().val {
                let mut next = list1.take().unwrap();
                list1 = next.next.take();
                current.next = Some(next);
            } else {
                let mut next = list2.take().unwrap();
                list2 = next.next.take();
                current.next = Some(next);
            }
            current = current.next.as_mut().unwrap();
        }

        current.next = if list1.is_some() { list1 } else { list2 };
        dummy.next
    }
}
