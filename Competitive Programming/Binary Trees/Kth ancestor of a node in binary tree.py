'''https://www.geeksforgeeks.org/kth-ancestor-node-binary-tree-set-2/
https://youtu.be/HeY99dtGi6U
Given a binary tree in which nodes are numbered from 1 to n. Given a node and a positive integer K. We have to print the Kth ancestor of the given node in the binary tree. If there does not exist any such ancestor then print -1.
For example in the below given binary tree, the 2nd ancestor of 5 is 1. 3rd ancestor of node 5 will be -1. 
We have discussed a BFS based solution for this problem in our previous article. If you observe that solution carefully, you will see that the basic approach was to first find the node and then backtrack to the kth parent. The same thing can be done using recursive DFS without using an extra array. 
The idea of using DFS is to first find the given node in the tree and then backtrack k times to reach to the kth ancestor, once we have reached the kth parent, we will simply print the node and return NULL. 
Below is the implementation of above idea: 
Output:  

Kth ancestor is: 1
Time Complexity: O(n), where n is the number of nodes in the binary tree. 
'''
""" Python3 program to calculate Kth
    ancestor of given node """

# A Binary Tree Node
# Utility function to create a new tree node


class newNode:

    # Constructor to create a new node
    def __init__(self, data):
        self.data = data
        self.left = None
        self.right = None

# recursive function to calculate
# Kth ancestor


def kthAncestorDFS(root, node, k):

    # Base case
    if (not root):
        return None

    if (root.data == node or
       (kthAncestorDFS(root.left, node, k)) or
       (kthAncestorDFS(root.right, node, k))):

        if (k[0] > 0):
            k[0] -= 1

        elif (k[0] == 0):

            # print the kth ancestor
            print("Kth ancestor is:", root.data)

            # return None to stop further
            # backtracking
            return None

        # return current node to previous call
        return root


# Driver Code
if __name__ == '__main__':
    root = newNode(1)
    root.left = newNode(2)
    root.right = newNode(3)
    root.left.left = newNode(4)
    root.left.right = newNode(5)

    k = [2]
    node = 5

    # prkth ancestor of given node
    parent = kthAncestorDFS(root, node, k)

    # check if parent is not None, it means
    # there is no Kth ancestor of the node
    if (parent):
        print("-1")
