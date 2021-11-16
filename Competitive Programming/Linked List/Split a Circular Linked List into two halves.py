'''https://practice.geeksforgeeks.org/problems/split-a-circular-linked-list-into-two-halves/1
Split a Circular Linked List into two halves 
Easy Accuracy: 50.84% Submissions: 28677 Points: 2
Given a Cirular Linked List of size N, split it into two halves circular lists. If there are odd number of nodes in the given circular linked list then out of the resulting two halved lists, first list should have one node more than the second list. The resultant lists should also be circular lists and not linear lists.

 

Example 1:

Input:
Circular LinkedList: 1->5->7
Output:
1 5
7
 

Example 2:

Input:
Circular LinkedList: 2->6->1->5
Output:
2 6
1 5

Your Task:
Your task is to complete the given function splitList(), which takes 3 input parameters: The address of the head of the linked list, addresses of the head of the first and second halved resultant lists and Set the head1_ref and head2_ref to the first resultant list and second resultant list respectively.


Expected Time Complexity: O(N)
Expected Auxilliary Space: O(1)


Constraints:
1 <= N <= 100'''


# User function Template for python3

'''
class Node:
    def __init__(self):
        self.data = None
        self.next = None
'''


class Solution:
    def splitList(self, head, head1, head2):
        # code here
        if head is None:
            return(None)
        slow = head
        fast = head

        while fast.next != head and fast.next.next != head:
            slow = slow.next
            fast = fast.next.next

        if fast.next != head:
            fast = fast.next

        head1 = head
        head2 = slow.next
        # make first half circular
        slow.next = head1

        # make the second half circular
        fast.next = head2
        # this is to emulate pass by reference in python please don't delete below line.
        return head1, head2

# {
#  Driver Code Starts
# Initial Template for Python 3


class Node:
    def __init__(self):
        self.data = None
        self.next = None


def printCircularLinkedList(head):
    tmp = head
    while True:
        print(tmp.data, end=" ")
        tmp = tmp.next
        if tmp == head:
            break
    print()


if __name__ == "__main__":
    for i in range(int(input())):
        head = Node()
        head1 = Node()
        head2 = Node()
        tmp = head
        n = int(input())
        for i in input().split():
            tmp.next = Node()
            tmp = tmp.next
            tmp.data = int(i)
        head = head.next
        tmp.next = head
        obj = Solution()
        head1, head2 = obj.splitList(head, head1, head2)
        printCircularLinkedList(head1)
        printCircularLinkedList(head2)


# } Driver Code Ends
