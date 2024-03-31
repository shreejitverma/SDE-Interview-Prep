'''https://www.geeksforgeeks.org/largest-independent-set-problem-dp-26/
Given a Binary Tree, find size of the Largest Independent Set(LIS) in it. 
A subset of all tree nodes is an independent set if there is no edge between any two nodes of the subset. 

For example, consider the following binary tree. 
The largest independent set(LIS) is {10, 40, 60, 70, 80} and size of the LIS is 5.

A Dynamic Programming solution solves a given problem using solutions of subproblems in bottom up manner. 
Can the given problem be solved using solutions to subproblems? If yes, then what are the subproblems? 
Can we find largest independent set size (LISS) for a node X if we know LISS for all descendants of X? 
If a node is considered as part of LIS, then its children cannot be part of LIS, but its grandchildren can be. 
Following is optimal substructure property.

1) Optimal Substructure: 
Let LISS(X) indicates size of largest independent set of a tree with root X. 

     LISS(X) = MAX { (1 + sum of LISS for all grandchildren of X),
                     (sum of LISS for all children of X) }
The idea is simple, there are two possibilities 
for every node X, either X is a member of the set or not a member. 
If X is a member, then the value of LISS(X) is 1 plus LISS of all grandchildren. 
If X is not a member, then the value is sum of LISS of all children.

2) Overlapping Subproblems 
Following is recursive implementation that simply follows the recursive structure mentioned above. '''

# A naive recursive implementation of
# Largest Independent Set problem

# A utility function to find
# max of two integers


def max(x, y):
    if(x > y):
        return x
    else:
        return y

# A binary tree node has data,
# pointer to left child and a
# pointer to right child


class node:
    def __init__(self):
        self.data = 0
        self.left = self.right = None

# The function returns size of the
# largest independent set in a given
# binary tree


def LISS(root):

    if (root == None):
        return 0

    # Calculate size excluding the current node
    size_excl = LISS(root.left) + LISS(root.right)

    # Calculate size including the current node
    size_incl = 1
    if (root.left != None):
        size_incl += LISS(root.left.left) + \
            LISS(root.left.right)
    if (root.right != None):
        size_incl += LISS(root.right.left) + \
            LISS(root.right.right)

    # Return the maximum of two sizes
    return max(size_incl, size_excl)

# A utility function to create a node


def newNode(data):

    temp = node()
    temp.data = data
    temp.left = temp.right = None
    return temp

# Driver Code


# Let us construct the tree
# given in the above diagram
root = newNode(20)
root.left = newNode(8)
root.left.left = newNode(4)
root.left.right = newNode(12)
root.left.right.left = newNode(10)
root.left.right.right = newNode(14)
root.right = newNode(22)
root.right.right = newNode(25)

print("Size of the Largest", " Independent Set is ", LISS(root))

'''Time complexity of the above naive recursive approach is exponential. 
It should be noted that the above function computes the same subproblems again and again. 
For example, LISS of node with value 50 is evaluated 
for node with values 10 and 20 as 50 is grandchild of 10 and child of 20. 

Since same subproblems are called again, 
this problem has Overlapping Subproblems property. 
So LISS problem has both properties (see this and this) of a dynamic programming problem. 
Like other typical Dynamic Programming(DP) problems, 
recomputations of same subproblems can be avoided by storing the solutions to subproblems 
and solving problems in bottom up manner.

Following are implementation of Dynamic Programming based solution. 
In the following solution, an additional field ‘liss’ is added to tree nodes. 
The initial value of ‘liss’ is set as 0 for all nodes. 
The recursive function LISS() calculates ‘liss’ for a node only if it is not already set. '''
# Python3 program for calculating LISS
# using dynamic programming

# A binary tree node has data,
# pointer to left child and a
# pointer to right child


class node:
    def __init__(self, data):

        self.data = data
        self.left = self.right = None
        self.liss = 0

# A memoization function returns size
# of the largest independent set in
# a given binary tree


def liss(root):

    if root == None:
        return 0

    if root.liss != 0:
        return root.liss

    if (root.left == None and
            root.right == None):
        root.liss = 1
        return root.liss

    # Calculate size excluding the
    # current node
    liss_excl = (liss(root.left) +
                 liss(root.right))

    # Calculate size including the
    # current node
    liss_incl = 1
    if root.left != None:
        liss_incl += (liss(root.left.left) +
                      liss(root.left.right))

    if root.right != None:
        liss_incl += (liss(root.right.left) +
                      liss(root.right.right))

    # Maximum of two sizes is LISS,
    # store it for future uses.
    root.liss = max(liss_excl, liss_incl)

    return root.liss

# Driver Code


# Let us construct the tree given
# in the above diagram
root = node(20)
root.left = node(8)
root.left.left = node(4)
root.left.right = node(12)
root.left.right.left = node(10)
root.left.right.right = node(14)
root.right = node(22)
root.right.right = node(25)

print("Size of the Largest Independent "
      "Set is ", liss(root))

'''Time Complexity: O(n) where n is the number of nodes in given Binary tree. 
Following extensions to above solution can be tried as an exercise. 
1) Extend the above solution for n-ary tree. 
2) The above solution modifies the given tree structure by adding an additional field ‘liss’ to tree nodes. 
Extend the solution so that it doesn’t modify the tree structure.
3) The above solution only returns size of LIS, it doesn’t print elements of LIS. 
Extend the solution to print all nodes that are part of LIS.'''
