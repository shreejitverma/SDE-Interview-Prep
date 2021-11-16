'''https://practice.geeksforgeeks.org/problems/delete-nodes-having-greater-value-on-right/1
Delete nodes having greater value on right 
Medium Accuracy: 37.92% Submissions: 50140 Points: 4
Given a singly linked list, remove all the nodes which have a greater value on its following nodes.

Example 1:

Input:
LinkedList = 12->15->10->11->5->6->2->3
Output: 15 11 6 3
Explanation: Since, 12, 10, 5 and 2 are
the elements which have greater elements
on the following nodes. So, after deleting
them, the linked list would like be 15,
11, 6, 3.
Example 2:

Input:
LinkedList = 10->20->30->40->50->60
Output: 60
Your Task:
The task is to complete the function compute() which should modify the list as required and return the head of the modified linked list. The printing is done by the driver code,

Expected Time Complexity: O(N)
Expected Auxiliary Space: O(1)

Constraints:
1 ≤ size of linked list ≤ 1000
1 ≤ element of linked list ≤ 1000
Note: Try to solve the problem without using any extra space.'''

# User function Template for python3

'''
class Node:
    def __init__(self,x):
        self.data=x
        self.next=None

'''


def reverseList(head):
    prev = head
    curr = head.next
    if curr == None:
        return head
    prev.next = None
    while curr:
        temp = curr.next
        curr.next = prev
        prev = curr
        curr = temp
    return prev


class Solution:
    def compute(self, head):
        # Your code here
        # reverse the list

        head = reverseList(head)
        temp = head

        while temp.next:
            if temp.data > temp.next.data:
                temp.next = temp.next.next
            else:
                break
        curr = temp
        nxt = curr.next
        while nxt:
            if curr.data > nxt.data:
                curr.next = nxt.next
                nxt = nxt.next

            else:
                curr = curr.next
                nxt = nxt.next
        head = reverseList(head)
        return head

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
    def append(self, new_value):
        new_node = Node(new_value)
        if self.head is None:
            self.head = new_node
            self.tail = new_node
            return
        self.tail.next = new_node
        self.tail = new_node

    def getNode(self, value):  # return node with given value, if not present return None
        curr_node = self.head
        while(curr_node.next and curr_node.data != value):
            curr_node = curr_node.next
        if(curr_node.data == value):
            return curr_node
        else:
            return None

    # prints the elements of linked list starting with head
    def printList(self):
        if self.head is None:
            print(' ')
            return
        curr_node = self.head
        while curr_node:
            print(curr_node.data, end=" ")
            curr_node = curr_node.next
        print(' ')


if __name__ == "__main__":
    t = int(input())
    while(t > 0):
        n = int(input())
        a = LinkedList()  # create a new linked list 'a'.
        nodes = list(map(int, input().strip().split()))
        for x in nodes:
            a.append(x)

        result = Solution().compute(a.head)
        a.head = result
        a.printList()
        t -= 1


# } Driver Code Ends
