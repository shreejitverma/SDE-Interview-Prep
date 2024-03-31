'''https://leetcode.com/problems/merge-two-sorted-lists/
21. Merge Two Sorted Lists
Easy

8853

885

Add to List

Share
Merge two sorted linked lists and return it as a sorted list. The list should be made by splicing together the nodes of the first two lists.

 

Example 1:


Input: l1 = [1,2,4], l2 = [1,3,4]
Output: [1,1,2,3,4,4]
Example 2:

Input: l1 = [], l2 = []
Output: []
Example 3:

Input: l1 = [], l2 = [0]
Output: [0]
 

Constraints:

The number of nodes in both lists is in the range [0, 50].
-100 <= Node.val <= 100
Both l1 and l2 are sorted in non-decreasing order.'''

# Definition for singly-linked list.
# class ListNode:
#     def __init__(self, x):
#         self.val = x
#         self.next = None


class Solution:
    def mergeTwoLists(self, l1: ListNode, l2: ListNode) -> ListNode:
        newList = ListNode(0)
        p = newList
        p1, p2 = l1, l2
        while p1 and p2:
            if p1.val < p2.val:
                p.next = ListNode(p1.val)
                p1 = p1.next
            else:
                p.next = ListNode(p2.val)
                p2 = p2.next
            p = p.next
        p.next = p1 if p1 else p2
        return newList.next

# if __name__ == "__main__":
#     a =  ListNode(1)
#     a.next = ListNode(2)
#     a.next.next = ListNode(4)
#     b =  ListNode(1)
#     b.next = ListNode(3)
#     b.next.next = ListNode(4)

#     s= Solution()
#     res = s.mergeTwoLists(a,b)
#     while res:
#         print(res.val,end='->')
#         res = res.next
