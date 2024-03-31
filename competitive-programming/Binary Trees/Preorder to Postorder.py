'''https://practice.geeksforgeeks.org/problems/preorder-to-postorder4423/1
Preorder to Postorder 
Medium Accuracy: 56.79% Submissions: 13376 Points: 4
Given an array arr[] of N nodes representing preorder traversal of BST. The task is to print its postorder traversal.

Example 1:

Input:
N = 5
arr[]  = {40,30,35,80,100}
Output: 35 30 100 80 40
Explanation: PreOrder: 40 30 35 80 100
InOrder: 30 35 40 80 100
Therefore, the BST will be:
              40
           /      \
         30       80
           \        \   
           35      100
Hence, the postOrder traversal will
be: 35 30 100 80 40
Example 2:

Input:
N = 8
arr[]  = {40,30,32,35,80,90,100,120}
Output: 35 32 30 120 100 90 80 40
Your Task:
You need to complete the given function and return the root of the tree. The driver code will then use this root to print the post order traversal.

Expected Time Complexity: O(N).
Expected Auxiliary Space: O(N).

Constraints:
1 <= N <= 103
1 <= arr[i] <= 104'''

# User function Template for python3

# Function that constructs BST from its preorder traversal.


def post_order(pre, size):
    # code here
    root = Node(pre[0])
    i = 1
    s = []
    s.append(root)
    while(i < size):
        temp = None
        while(len(s) > 0 and pre[i] > s[-1].data):
            temp = s.pop()
        if temp != None:
            temp.right = Node(pre[i])
            s.append(temp.right)
        else:
            temp = s[-1]
            temp.left = Node(pre[i])
            s.append(temp.left)
        i += 1
    return root

# {
#  Driver Code Starts
# Initial Template for Python 3

# contributed by RavinderSinghPB


class Node:
    def __init__(self, data=0):
        self.data = data
        self.left = None
        self.right = None


def postOrd(root):
    if not root:
        return
    postOrd(root.left)
    postOrd(root.right)
    print(root.data, end=' ')


if __name__ == '__main__':
    t = int(input())

    for _tcs in range(t):
        size = int(input())
        pre = [int(x)for x in input().split()]

        root = post_order(pre, size)

        postOrd(root)
        print()
# } Driver Code Ends
