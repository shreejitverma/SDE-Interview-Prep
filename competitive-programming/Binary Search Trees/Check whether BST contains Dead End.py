'''https://practice.geeksforgeeks.org/problems/check-whether-bst-contains-dead-end/1
Check whether BST contains Dead End 
Easy Accuracy: 46.96% Submissions: 20899 Points: 2
Given a Binary search Tree that contains positive integer values greater then 0. The task is to complete the function isDeadEnd which returns true if the BST contains a dead end else returns false. Here Dead End means, we are not able to insert any element after that node.

Examples:

Input :   
               8
             /   \ 
           5      9
         /  \     
        2    7 
       /
      1     
          
Output : Yes
Explanation : Node "1" is the dead End because after that 
              we cant insert any element.       

Input :     
              8
            /   \ 
           7     10
         /      /   \
        2      9     13

Output : Yes
Explanation : We can't insert any element at 
              node 9.  

Input:
The first line of the input contains an integer 'T' denoting the number of test cases. Then 'T' test cases follow. Each test case consists of three lines. First line of each test case contains an integer N denoting the no of nodes of the BST . Second line of each test case consists of 'N' space separated integers denoting the elements of the BST. These elements are inserted into BST in the given order.

Output:
The output for each test case will be 1 if the BST contains a dead end else 0.
 

Constraints:
1<=T<=100
1<=N<=200
 

Example(To be used only for expected output):
Input:
2
6
8 5 9 7 2 1
6
8 7 10 9 13 2
Output:
1
1'''

# Your task is to complete this function
# function should return true/false or 1/0


def traverse(node, mi, mx):
    if(node == None):
        return False
    # print(node.data,mi,mx)
    if(mi == node.data and mx == node.data):
        return True

    ans = False
    ans = ans or traverse(node.left, mi, node.data -
                          1) or traverse(node.right, node.data+1, mx)
    return ans


def isdeadEnd(root):
    # Code here
    ans = traverse(root, 1, 999999999)
    return ans

# {
#  Driver Code Starts


class Node:
    def __init__(self, value):
        self.left = None
        self.data = value
        self.right = None


def insert(root, node):
    if root is None:
        root = node
    else:
        if root.data < node.data:
            if root.right is None:
                root.right = node
            else:
                insert(root.right, node)
        elif root.data == node.data:
            return
        else:
            if root.left is None:
                root.left = node
            else:
                insert(root.left, node)


def traverseInorder(root):
    if root is not None:
        traverseInorder(root.left)
        print(root.data, end=" ")
        traverseInorder(root.right)


if __name__ == '__main__':
    t = int(input())
    for i in range(t):
        n = int(input())
        arr = list(map(int, input().strip().split()))
        root = None
        for j in arr:
            if root is None:
                root = Node(j)
            else:
                insert(root, Node(j))
        if isdeadEnd(root):
            print(1)
        else:
            print(0)
# Contributed by: Harshit Sidhwa

# } Driver Code Ends
