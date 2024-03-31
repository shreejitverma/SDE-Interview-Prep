'''https://practice.geeksforgeeks.org/problems/remove-loop-in-linked-list/1
Remove loop in Linked List 
Medium Accuracy: 47.96% Submissions: 100k+ Points: 4
Given a linked list of N nodes such that it may contain a loop.

A loop here means that the last node of the link list is connected to the node at position X. If the link list does not have any loop, X=0.
Remove the loop from the linked list, if it is present.  


Example 1:

Input:
N = 3
value[] = {1,3,4}
X = 2
Output: 1
Explanation: The link list looks like
1 -> 3 -> 4
     ^    |
     |____|    
A loop is present. If you remove it 
successfully, the answer will be 1. 

Example 2:

Input:
N = 4
value[] = {1,8,3,4}
X = 0
Output: 1
Explanation: The Linked list does not 
contains any loop. 

Example 3:

Input:
N = 4
value[] = {1,2,3,4}
X = 1
Output: 1
Explanation: The link list looks like 
1 -> 2 -> 3 -> 4
|______________|
A loop is present. 
If you remove it successfully, 
the answer will be 1. 

Your Task:
You don't need to read input or print anything. Your task is to complete the function removeLoop() which takes the head of the linked list as the input parameter. Simply remove the loop in the list (if present) without disconnecting any nodes from the list.
Note: The generated output will be 1 if your submitted code is correct.


Expected time complexity: O(N)
Expected auxiliary space: O(1)


Constraints:
1 ≤ N ≤ 104'''

'''
# node class:

class Node:
    def __init__(self,val):
        self.next=None
        self.data=val

'''


class Solution:
    # Function to remove a loop in the linked list.
    def removeLoop(self, head):
        # code here
        # remove the loop without losing any nodes
        d = {}
        prev = None
        while(head):
            # if(head.next==None):
            #     return head
            if(head in d):
                prev.next = None
                return True
            else:
                d[head] = 1
                prev = head
                head = head.next
            # head.next
        return False


# {
#  Driver Code Starts
# driver code:

class Node:
    def __init__(self, val):
        self.next = None
        self.data = val


class linkedList:
    def __init__(self):
        self.head = None
        self.tail = None

    def add(self, num):
        if self.head is None:
            self.head = Node(num)
            self.tail = self.head
        else:
            self.tail.next = Node(num)
            self.tail = self.tail.next

    def isLoop(self):
        if self.head is None:
            return False

        fast = self.head.next
        slow = self.head

        while slow != fast:
            if fast is None or fast.next is None:
                return False
            fast = fast.next.next
            slow = slow.next

        return True

    def loopHere(self, position):
        if position == 0:
            return

        walk = self.head
        for _ in range(1, position):
            walk = walk.next
        self.tail.next = walk

    def length(self):
        walk = self.head
        ret = 0
        while walk:
            ret += 1
            walk = walk.next
        return ret


if __name__ == "__main__":
    t = int(input())
    for _ in range(t):
        n = int(input())
        arr = tuple(int(x) for x in input().split())
        pos = int(input())

        ll = linkedList()
        for i in arr:
            ll.add(i)
        ll.loopHere(pos)

        Solution().removeLoop(ll.head)

        if ll.isLoop() or ll.length() != n:
            print(0)
            continue
        else:
            print(1)

# } Driver Code Ends
