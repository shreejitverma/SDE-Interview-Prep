'''
https://www.geeksforgeeks.org/minimum-swap-required-convert-binary-tree-binary-search-tree/#:~:text=Given%20the%20array%20representation%20of,it%20into%20Binary%20Search%20Tree.&text=Swap%201%3A%20Swap%20node%208,node%209%20with%20node%2010.
Given the array representation of Complete Binary Tree i.e, if index i is the parent, index 2*i + 1 is the left child and index 2*i + 2 is the right child. The task is to find the minimum number of swap required to convert it into Binary Search Tree.

Examples:  
The idea is to use the fact that inorder traversal of Binary Search Tree is in increasing order of their value. 
So, find the inorder traversal of the Binary Tree and store it in the array and try to sort the array. The minimum number of swap required to get the array sorted will be the answer. Please refer below post to find minimum number of swaps required to get the array sorted.
Minimum number of swaps required to sort an array
Time Complexity: O(n log n).'''

# Python3 program for Minimum swap required
# to convert binary tree to binary search tree

# Inorder Traversal of Binary Tree


def inorder(a, n, index):

    global v

    # If index is greater or equal to
    # vector size
    if (index >= n):
        return

    inorder(a, n, 2 * index + 1)

    # Push elements in vector
    v.append(a[index])
    inorder(a, n, 2 * index + 2)

# Function to find minimum swaps
# to sort an array


def minSwaps():

    global v
    t = [[0, 0] for i in range(len(v))]
    ans = -2

    for i in range(len(v)):
        t[i][0], t[i][1] = v[i], i

    t, i = sorted(t), 0

    while i < len(t):

        # break
        # second element is equal to i
        if (i == t[i][1]):
            i += 1
            continue
        else:

            # Swaping of elements
            t[i][0], t[t[i][1]][0] = t[t[i][1]][0], t[i][0]
            t[i][1], t[t[i][1]][1] = t[t[i][1]][1], t[i][1]

        # Second is not equal to i
        if (i == t[i][1]):
            i -= 1

        i += 1

        ans += 1

    return ans


# Driver Code
if __name__ == '__main__':

    v = []
    a = [5, 6, 7, 8, 9, 10, 11]
    n = len(a)
    inorder(a, n, 0)

    print(minSwaps())
