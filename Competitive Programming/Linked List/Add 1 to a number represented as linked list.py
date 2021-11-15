'''https://practice.geeksforgeeks.org/problems/add-1-to-a-number-represented-as-linked-list/1
Add 1 to a number represented as linked list 
Easy Accuracy: 51.33% Submissions: 74755 Points: 2
A number N is represented in Linked List such that each digit corresponds to a node in linked list. You need to add 1 to it.

Example 1:

Input:
LinkedList: 4->5->6
Output: 457 
Example 2:

Input:
LinkedList: 1->2->3
Output: 124 
Your Task:
Your task is to complete the function addOne() which takes the head of the linked list as the only argument and returns the head of the modified linked list. The driver code prints the number.
Note: The head represents the left-most digit of the number.

Expected Time Complexity: O(N).
Expected Auxiliary Space: O(1).

Constraints:
1 <= N <= 101000'''

# User function Template for python3

'''

class Node:
    def __init__(self, data):   # data -> value stored in node
        self.data = data
        self.next = None
'''


class Solution:
    def addOne(self, head):
        temp = head
        # reverse ll
        prev = None
        temp1 = head
        while temp1:
            nxt = temp1.next
            temp1.next = prev
            prev = temp1
            temp1 = nxt
        it = prev
        lst = None
        while it and it.data == 9:
            lst = it
            it.data = 0
            it = it.next
        if not it:
            new = Node(1)
            lst.next = new
        else:
            it.data += 1

        curr = prev
        prev = None
        while curr:
            nxt = curr.next
            curr.next = prev
            prev = curr
            curr = nxt
        return prev

# {
#  Driver Code Starts
# Initial Template for Python 3

# Node Class


class Node:
    def __init__(self, data):   # data -> value stored in node
        self.data = data
        self.next = None

# Linked List Class


class LinkedList:
    def __init__(self):
        self.head = None
        self.tail = None

    # creates a new node with given value and appends it at the end of the linked list
    def insert(self, value):
        if self.head is None:
            self.head = Node(value)
            self.tail = self.head
        else:
            self.tail.next = Node(value)
            self.tail = self.tail.next


def PrintList(head):
    while head:
        print(head.data, end='')
        head = head.next


if __name__ == '__main__':
    t = int(input())
    for _ in range(t):

        num = input()
        ll = LinkedList()  # create a new linked list 'll1'.
        for digit in num:
            ll.insert(int(digit))  # add to the end of the list

        resHead = Solution().addOne(ll.head)
        PrintList(resHead)
        print()


# } Driver Code Ends
