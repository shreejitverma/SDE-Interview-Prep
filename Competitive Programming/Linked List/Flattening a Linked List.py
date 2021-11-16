'''https://practice.geeksforgeeks.org/problems/flattening-a-linked-list/1
Flattening a Linked List 
Medium Accuracy: 33.91% Submissions: 70433 Points: 4
Given a Linked List of size N, where every node represents a sub-linked-list and contains two pointers:
(i) a next pointer to the next node,
(ii) a bottom pointer to a linked list where this node is head.
Each of the sub-linked-list is in sorted order.
Flatten the Link List such that all the nodes appear in a single level while maintaining the sorted order. 
Note: The flattened list will be printed using the bottom pointer instead of next pointer.

 

Example 1:

Input:
5 -> 10 -> 19 -> 28
|     |     |     | 
7     20    22   35
|           |     | 
8          50    40
|                 | 
30               45
Output:  5-> 7-> 8- > 10 -> 19-> 20->
22-> 28-> 30-> 35-> 40-> 45-> 50.
Explanation:
The resultant linked lists has every 
node in a single level.
(Note: | represents the bottom pointer.)
 

Example 2:

Input:
5 -> 10 -> 19 -> 28
|          |                
7          22   
|          |                 
8          50 
|                           
30              
Output: 5->7->8->10->19->20->22->30->50
Explanation:
The resultant linked lists has every
node in a single level.

(Note: | represents the bottom pointer.)
 

Your Task:
You do not need to read input or print anything. Complete the function flatten() that takes the head of the linked list as input parameter and returns the head of flattened link list.

 

Expected Time Complexity: O(N*M)
Expected Auxiliary Space: O(1)

 

Constraints:
0 <= N <= 50
1 <= Mi <= 20
1 <= Element of linked list <= 103'''

# User function Template for python3


'''

class Node:
    def __init__(self, d):
        self.data=d
        self.next=None
        self.bottom=None
        
'''


def flatten(root):
    def merge(l1, l2):
        # print(l1.data,l2.data)
        dummy = Node(0)
        temp = dummy
        while l1 and l2:
            if l1.data <= l2.data:
                temp.bottom = l1
                l1 = l1.bottom
            else:
                temp.bottom = l2
                l2 = l2.bottom
            temp = temp.bottom
        if l1:
            temp.bottom = l1
        if l2:
            temp.bottom = l2
        # print(dummy.next.data)
        return dummy.bottom

    def flatten(head):
        if head.next == None:
            return head
        l1, l2 = head, head.next
        nxtnode = l2.next
        l1.next, l2.next = None, None
        nxt = merge(l1, l2)
        nxt.next = nxtnode
        return flatten(nxt)
    return flatten(root)

# {
#  Driver Code Starts
# Initial Template for Python 3


class Node:
    def __init__(self, d):
        self.data = d
        self.next = None
        self.bottom = None


def printList(node):
    while(node is not None):
        print(node.data, end=" ")
        node = node.bottom

    print()


if __name__ == "__main__":
    t = int(input())
    while(t > 0):
        head = None
        N = int(input())
        arr = []

        arr = [int(x) for x in input().strip().split()]
        temp = None
        tempB = None
        pre = None
        preB = None

        flag = 1
        flag1 = 1
        listo = [int(x) for x in input().strip().split()]
        it = 0
        for i in range(N):
            m = arr[i]
            m -= 1
            a1 = listo[it]
            it += 1
            temp = Node(a1)
            if flag is 1:
                head = temp
                pre = temp
                flag = 0
                flag1 = 1
            else:
                pre.next = temp
                pre = temp
                flag1 = 1

            for j in range(m):
                a = listo[it]
                it += 1
                tempB = Node(a)
                if flag1 is 1:
                    temp.bottom = tempB
                    preB = tempB
                    flag1 = 0
                else:
                    preB.bottom = tempB
                    preB = tempB
        root = flatten(head)
        printList(root)

        t -= 1

# } Driver Code Ends
