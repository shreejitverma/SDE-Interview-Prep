'''https://practice.geeksforgeeks.org/problems/circular-linked-list/1
Check If Circular Linked List 
Basic Accuracy: 50.74% Submissions: 63180 Points: 1
Given head, the head of a singly linked list, find if the linked list is circular or not. A linked list is called circular if it not NULL terminated and all nodes are connected in the form of a cycle. An empty linked list is considered as circular.

Example 1:

Input:
LinkedList: 1->2->3->4->5
(the first and last node is connected,
i.e. 5 --> 1)
Output: 1
Example 2:

Input:
LinkedList: 2->4->6->7->5->1
Output: 0
Your Task:
The task is to complete the function isCircular() which checks if the given linked list is circular or not. It should return true or false accordingly. (the driver code prints 1 if the returned values is true, otherwise 0)

Expected Time Complexity: O(N).
Expected Auxiliary Space: O(1).

Constraints:
1 <=Number of nodes<= 100'''

# class Node:
#    def __init__(self, data):
#        self.data = data
#        self.next = None


# your task is to complete this function
# function should return true/false or 1/0
def isCircular(head):
    # Code here
    curr = head

    if head.next == head:
        return True

    else:
        while curr != None:
            if curr.next == head:
                return True
            curr = curr.next

        return False


# {
#  Driver Code Starts
# Node class
class Node:
    def __init__(self, data):
        self.data = data
        self.next = None


class LinkedList:
    # Function to initialize head
    def __init__(self):
        self.head = None

    # Function to insert a new node at the beginning
    def push(self, new_data):
        new_node = Node(new_data)
        new_node.next = self.head
        self.head = new_node

    # Utility function to prit the linked LinkedList
    def printList(self, node):
        temp = node
        while (temp):
            print(temp.data, end=" ")
            temp = temp.next

    def getHead(self):
        return self.head

    def createLoop(self, n):
        if n == 0:
            return None
        temp = self.head
        ptr = self.head
        cnt = 1
        while (cnt != n):
            ptr = ptr.next
            cnt += 1
        # print ptr.data
        while (temp.next):
            temp = temp.next
        temp.next = ptr


# Driver program
if __name__ == '__main__':
    t = int(input())
    for i in range(t):
        n, isloop = list(map(int, input().strip().split()))
        arr = list(map(int, input().strip().split()))
        lst = LinkedList()
        for i in arr:
            lst.push(i)
        if(isloop):
            lst.createLoop(1)
        if isCircular(lst.getHead()):
            print(1)
        else:
            print(0)
# Contributed By: Harshit Sidhwa
# } Driver Code Ends
