'''https://www.geeksforgeeks.org/maximum-sum-nodes-binary-tree-no-two-adjacent/
Given a binary tree with a value associated with each node, we need to choose a subset of these nodes such that the sum of chosen nodes is maximum under a constraint that no two chosen nodes in the subset should be directly connected that is, if we have taken a node in our sum then we can’t take any of its children in consideration and vice versa. 
Method 1 
We can solve this problem by considering the fact that both node and its children can’t be in sum at the same time, so when we take a node into our sum we will call recursively for its grandchildren or if we don’t take this node then we will call for all its children nodes and finally we will choose maximum from both of the results. 
It can be seen easily that the above approach can lead to solving the same subproblem many times, for example in the above diagram node 1 calls node 4 and 5 when its value is chosen and node 3 also calls them when its value is not chosen so these nodes are processed more than once. We can stop solving these nodes more than once by memoizing the result at all nodes. 
In the below code, a map is used for memorizing the result which stores the result of the complete subtree rooted at a node in the map so that if it is called again, the value is not calculated again instead stored value from the map is returned directly. 

Please see the below code for a better understanding.'''

# Python3 program to find
# maximum sum from a subset
# of nodes of binary tree

# A binary tree node structure


class Node:

    def __init__(self, data):

        self.data = data
        self.left = None
        self.right = None

# Utility function to create
# a new Binary Tree node


def newNode(data):

    temp = Node(data)
    return temp

# method returns maximum sum
# possible from subtrees rooted
# at grandChildrens of node 'node'


def sumOfGrandChildren(node, mp):

    sum = 0

    # call for children of left
    # child only if it is not NULL
    if (node.left):
        sum += (getMaxSumUtil(node.left.left, mp) +
                getMaxSumUtil(node.left.right, mp))

    # call for children of right
    # child only if it is not NULL
    if (node.right):
        sum += (getMaxSumUtil(node.right.left, mp) +
                getMaxSumUtil(node.right.right, mp))

    return sum

# Utility method to return
# maximum sum rooted at node
# 'node'


def getMaxSumUtil(node, mp):

    if (node == None):
        return 0

    # If node is already processed
    # then return calculated
    # value from map
    if node in mp:
        return mp[node]

    # take current node value
    # and call for all grand children
    incl = (node.data +
            sumOfGrandChildren(node, mp))

    # don't take current node
    # value and call for all children
    excl = (getMaxSumUtil(node.left, mp) +
            getMaxSumUtil(node.right, mp))

    # choose maximum from both
    # above calls and store that
    # in map
    mp[node] = max(incl, excl)

    return mp[node]

# Returns maximum sum from
# subset of nodes of binary
# tree under given constraints


def getMaxSum(node):

    if (node == None):
        return 0

    mp = dict()
    return getMaxSumUtil(node, mp)


# Driver code
if __name__ == "__main__":

    root = newNode(1)
    root.left = newNode(2)
    root.right = newNode(3)
    root.right.left = newNode(4)
    root.right.right = newNode(5)
    root.left.left = newNode(1)

    print(getMaxSum(root))
'''Method 2 (Using pair) 
Return a pair for each node in the binary tree such that the first of the pair indicates maximum sum when the data of a node is included and the second indicates maximum sum when the data of a particular node is not included. '''

# Python3 program to find maximum sum in Binary
# Tree such that no two nodes are adjacent.

# Binary Tree Node

""" utility that allocates a newNode
with the given key """


class newNode:

    # Construct to create a newNode
    def __init__(self, key):
        self.data = key
        self.left = None
        self.right = None


def maxSumHelper(root):

    if (root == None):

        sum = [0, 0]
        return sum

    sum1 = maxSumHelper(root.left)
    sum2 = maxSumHelper(root.right)
    sum = [0, 0]

    # This node is included (Left and right
    # children are not included)
    sum[0] = sum1[1] + sum2[1] + root.data

    # This node is excluded (Either left or
    # right child is included)
    sum[1] = (max(sum1[0], sum1[1]) +
              max(sum2[0], sum2[1]))

    return sum


def maxSum(root):

    res = maxSumHelper(root)
    return max(res[0], res[1])


# Driver Code
if __name__ == '__main__':
    root = newNode(10)
    root.left = newNode(1)
    root.left.left = newNode(2)
    root.left.left.left = newNode(1)
    root.left.right = newNode(3)
    root.left.right.left = newNode(4)
    root.left.right.right = newNode(5)
    print(maxSum(root))
