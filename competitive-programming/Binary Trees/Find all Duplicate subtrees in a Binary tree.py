'''https://practice.geeksforgeeks.org/problems/duplicate-subtrees/1
Duplicate Subtrees 
Medium Accuracy: 43.37% Submissions: 15751 Points: 4
Given a binary tree of size N, your task is to that find all duplicate subtrees from the given binary tree.

Example:

Input : 

Output : 2 4
         4
Explanation: Above Trees are two 
duplicate subtrees.i.e  and 
Therefore,you need to return above trees 
root in the form of a list.
Your Task:
You don't need to take input. Just complete the function printAllDups() that takes the root node as a parameter and returns an array of Node*, which contains all the duplicate subtree.
Note: Here the Output of every Node printed in the Pre-Order tree traversal format.


Constraints:
1<=T<=100
1<=N<=100'''

# User function Template for python3

'''
class Node:
    def __init__(self,val):
        self.data=val
        self.left=None
        self.right=None
'''




from collections import deque
class Solution:
    def printUtil(self, root, d, ans):
        if root is None:
            return ''
        l = self.printUtil(root.left, d, ans)
        r = self.printUtil(root.right, d, ans)
        p = l+'-'+str(root.data)+'-'+r
        # print(d)
        if d.get(p) is not None:
           # print(p,d[p])
            if d[p] == 1:
                ans.append(root)
                d[p] += 1
            else:
                d[p] += 1
        else:
            d[p] = 1
        return p

    def printAllDups(self, root):
        # code here
        ans = []
        d = {}
        self.printUtil(root, d, ans)

        return ans

# {
#  Driver Code Starts
# Initial Template for Python 3


class Node:
    def __init__(self, val):
        self.data = val
        self.left = None
        self.right = None


def buildTree(s):
    # Corner Case
    if(len(s) == 0 or s[0] == "N"):
        return None

    # Creating list of strings from input
    # string after spliting by space
    ip = list(map(str, s.split()))

    # Create the root of the tree
    root = Node(int(ip[0]))
    size = 0
    q = deque()

    # Push the root to the queue
    q.append(root)
    size = size+1

    # Starting from the second element
    i = 1
    while(size > 0 and i < len(ip)):
        # Get and remove the front of the queue
        currNode = q[0]
        q.popleft()
        size = size-1

        # Get the current node's value from the string
        currVal = ip[i]

        # If the left child is not null
        if(currVal != "N"):

            # Create the left child for the current node
            currNode.left = Node(int(currVal))

            # Push it to the queue
            q.append(currNode.left)
            size = size+1
        # For the right child
        i = i+1
        if(i >= len(ip)):
            break
        currVal = ip[i]

        # If the right child is not null
        if(currVal != "N"):

            # Create the right child for the current node
            currNode.right = Node(int(currVal))

            # Push it to the queue
            q.append(currNode.right)
            size = size+1
        i = i+1
    return root


def preord(root):
    if not root:
        return
    print(root.data, end=' ')
    preord(root.left)
    preord(root.right)


if __name__ == '__main__':
    test_cases = int(input())
    for cases in range(test_cases):
        s = input()
        root = buildTree(s)
        ob = Solution()
        res = ob.printAllDups(root)
        for i in res:
            preord(i)
            print()
# } Driver Code Ends
