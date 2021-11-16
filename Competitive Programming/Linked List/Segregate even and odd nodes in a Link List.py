'''https://practice.geeksforgeeks.org/problems/segregate-even-and-odd-nodes-in-a-linked-list5035/1
Segregate even and odd nodes in a Link List 
Easy Accuracy: 64.41% Submissions: 21212 Points: 2
Given a link list of size N, modify the list such that all the even numbers appear before all the odd numbers in the modified list. The order of appearance of numbers within each segregation should be same as that in the original list.

NOTE: Don't create a new linked list, instead rearrange the provided one.


Example 1:

Input: 
N = 7
Link List:
17 -> 15 -> 8 -> 9 -> 2 -> 4 -> 6 -> NULL

Output: 8 2 4 6 17 15 9

Explaination: 8,2,4,6 are the even numbers 
so they appear first and 17,15,9 are odd 
numbers that appear later.

Example 2:

Input:
N = 4
Link List:
1 -> 3 -> 5 -> 7

Output: 1 3 5 7

Explaination: There is no even number. 
So ne need for modification.

Your Task:
You do not need to read input or print anything. Your task is to complete the function divide() which takes N and head of Link List as input parameters and returns the head of modified link list. Don't create a new linked list, instead rearrange the provided one.


Expected Time Complexity: O(N)
Expected Auxiliary Space: O(1)


Constraints:
1 ≤ N ≤ 105
1 ≤ Each element of the list ≤ 105'''

# User function Template for Python3

# Following is the structure of node
# class Node:
#     def __init__(self):
#         self.data = None
#         self.next = None


class Solution:
    def divide(self, N, head):
        # code here
        temp = head
        temp1 = None
        flag = 0
        while temp is not None:
            if temp.data % 2 == 0:
                temp1 = temp
                temp = temp.next
                flag = 1
            else:
                break
        while temp != None and temp.next is not None:
            if temp.next.data % 2 == 0 and flag == 0:
                temp2 = temp.next
                temp.next = temp.next.next
                temp2.next = head
                head = temp2
                temp1 = temp2
                flag = 1
            elif temp.next.data % 2 == 0 and flag == 1:
                temp2 = temp.next
                temp.next = temp.next.next
                temp2.next = temp1.next
                temp1.next = temp2
                temp1 = temp2
            else:
                temp = temp.next
        return head
# {
#  Driver Code Starts
# Initial Template for Python3

# Node Class


class node:
    def __init__(self):
        self.data = None
        self.next = None

# Linked List Class


class Linked_List:
    def __init__(self):
        self.head = None
        self.tail = None

    def insert(self, data):
        if self.head == None:
            self.head = node()
            self.tail = self.head
            self.head.data = data
        else:
            new_node = node()
            new_node.data = data
            new_node.next = None
            self.tail.next = new_node
            self.tail = self.tail.next


def printlist(head):
    temp = head
    while temp is not None:
        print(temp.data, end=" ")
        temp = temp.next
    print('')


# Driver Program
if __name__ == '__main__':
    t = int(input())
    for i in range(t):
        list1 = Linked_List()
        n = int(input())
        values = list(map(int, input().strip().split()))
        for i in values:
            list1.insert(i)
        ob = Solution()
        newhead = ob.divide(n, list1.head)
        printlist(newhead)


# } Driver Code Ends
