'''https://practice.geeksforgeeks.org/problems/top-view-of-binary-tree/1
https://www.geeksforgeeks.org/print-nodes-top-view-binary-tree/#:~:text=Top%20view%20of%20a%20binary,node%20at%20its%20horizontal%20distance.
Top View of Binary Tree 
Medium Accuracy: 32.3% Submissions: 100k+ Points: 4
Given below is a binary tree. The task is to print the top view of binary tree. Top view of a binary tree is the set of nodes visible when the tree is viewed from the top. For the given below tree

       1
    /     \
   2       3
  /  \    /   \
4    5  6   7

Top view will be: 4 2 1 3 7
Note: Return nodes from leftmost node to rightmost node.

Example 1:

Input:
      1
   /    \
  2      3
Output: 2 1 3
Example 2:

Input:
       10
    /      \
  20        30
 /   \    /    \
40   60  90    100
Output: 40 20 10 30 100
Your Task:
Since this is a function problem. You don't have to take input. Just complete the function topView() that takes root node as parameter and returns a list of nodes visible from the top view from left to right.

Expected Time Complexity: O(N)
Expected Auxiliary Space: O(N).

Constraints:
1 ≤ N ≤ 105
1 ≤ Node Data ≤ 105'''


# Python3 program to print top
# view of binary tree

# Binary Tree Node
""" utility that allocates a newNode
with the given key """


class newNode:

    # Construct to create a newNode
    def __init__(self, key):
        self.data = key
        self.left = None
        self.right = None
        self.hd = 0

# function should print the topView
# of the binary tree


def topview(root):

    if(root == None):
        return
    q = []
    m = dict()
    hd = 0
    root.hd = hd

    # push node and horizontal
    # distance to queue
    q.append(root)

    while(len(q)):
        root = q[0]
        hd = root.hd

        # count function returns 1 if the
        # container contains an element
        # whose key is equivalent to hd,
        # or returns zero otherwise.
        if hd not in m:
            m[hd] = root.data
        if(root.left):
            root.left.hd = hd - 1
            q.append(root.left)

        if(root.right):
            root.right.hd = hd + 1
            q.append(root.right)

        q.pop(0)
    for i in sorted(m):
        print(m[i], end="")


# Driver Code
if __name__ == '__main__':

    """ Create following Binary Tree
            1
        / \
        2 3
        \
            4
            \
            5
            \
                6*"""
    root = newNode(1)
    root.left = newNode(2)
    root.right = newNode(3)
    root.left.right = newNode(4)
    root.left.right.right = newNode(5)
    root.left.right.right.right = newNode(6)
    print("Following are nodes in top",
          "view of Binary Tree")
    topview(root)


def topView(self, root):
    dic = {}

    def inner(root, dic, index, height):
        if index not in dic:
            dic[index] = (root.data, height)
        else:
            d, h = dic[index]
            if height < h:
                dic[index] = (root.data, height)
        if root.left:
            inner(root.left, dic, index-1, height+1)
        if root.right:
            inner(root.right, dic, index+1, height+1)
    inner(root, dic, 0, 0)
    maxi = max(dic)
    mini = min(dic)
    res = []
    for i in range(mini, maxi+1):
        res.append(dic[i][0])
    return res
