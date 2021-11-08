'''https://practice.geeksforgeeks.org/problems/reverse-a-linked-list-in-groups-of-given-size/1
https://www.geeksforgeeks.org/reverse-a-list-in-groups-of-given-size/
Reverse a Linked List in groups of given size. 
Medium Accuracy: 45.83% Submissions: 100k+ Points: 4
Given a linked list of size N. The task is to reverse every k nodes 
(where k is an input to the function) in the linked list. 
If the number of nodes is not a multiple of k then left-out nodes, 
in the end, should be considered as a group and must be reversed (See Example 2 for clarification).

Example 1:

Input:
LinkedList: 1->2->2->4->5->6->7->8
K = 4
Output: 4 2 2 1 8 7 6 5 
Explanation: 
The first 4 elements 1,2,2,4 are reversed first 
and then the next 4 elements 5,6,7,8. Hence, the 
resultant linked list is 4->2->2->1->8->7->6->5.
Example 2:

Input:
LinkedList: 1->2->3->4->5
K = 3
Output: 3 2 1 5 4 
Explanation: 
The first 3 elements are 1,2,3 are reversed 
first and then elements 4,5 are reversed.Hence, 
the resultant linked list is 3->2->1->5->4.
Your Task:
You don't need to read input or print anything. Your task is to complete the function reverse() 
which should reverse the linked list in group of size k and return the head of the modified linked list.

Expected Time Complexity : O(N)
Expected Auxilliary Space : O(1)

Constraints:
1 <= N <= 104
1 <= k <= N'''

"""Return reference of new head of the reverse linked list
  The input list will have at least one element
  Node is defined as

class Node:
    def __init__(self, data):
		self.data = data
		self.next = None
  This is method only submission.
  You only need to complete the method.
"""


class Solution:
    def reverse(self, head, k):
        # Code here
        # Code here
        front = head
        last = front
        for i in range(k-1):
            if last.next != None:
                last = last.next
        if last != None:
            newfront = last.next
        if front != last:
            self.reversesublist(front, last)
        prev = front
        front = newfront
        head = last
        while front != None:
            last = front
            for j in range(k-1):
                if last.next != None:
                    last = last.next
            if last != None:
                newfront = last.next
            if front != last:
                self.reversesublist(front, last)
            else:
                return head
            prev.next = last
            prev = front
            front = newfront
        return head

    def reversesublist(self, start, end):
        head = start.next
        temp = head
        start.next = end.next
        if head != None and head.next != None:
            while head != end:
                head = head.next
                temp.next = start
                start = temp
                temp = head
        head.next = start


# Python program to reverse a
# linked list in group of given size

# Node class


class Node:

    # Constructor to initialize the node object
    def __init__(self, data):
        self.data = data
        self.next = None


class LinkedList:

    # Function to initialize head
    def __init__(self):
        self.head = None

    def reverse(self, head, k):

        if head == None:
            return None
        current = head
        next = None
        prev = None
        count = 0

        # Reverse first k nodes of the linked list
        while(current is not None and count < k):
            next = current.next
            current.next = prev
            prev = current
            current = next
            count += 1

        # next is now a pointer to (k+1)th node
        # recursively call for the list starting
        # from current. And make rest of the list as
        # next of first node
        if next is not None:
            head.next = self.reverse(next, k)

        # prev is new head of the input list
        return prev

    # Function to insert a new node at the beginning
    def push(self, new_data):
        new_node = Node(new_data)
        new_node.next = self.head
        self.head = new_node

    # Utility function to print the linked LinkedList
    def printList(self):
        temp = self.head
        while(temp):
            print temp.data,
            temp = temp.next


# Driver program
llist = LinkedList()
llist.push(9)
llist.push(8)
llist.push(7)
llist.push(6)
llist.push(5)
llist.push(4)
llist.push(3)
llist.push(2)
llist.push(1)

print "Given linked list"
llist.printList()
llist.head = llist.reverse(llist.head, 3)

print "\nReversed Linked list"
llist.printList()
