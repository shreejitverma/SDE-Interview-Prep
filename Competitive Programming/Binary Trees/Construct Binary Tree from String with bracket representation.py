'''https://www.geeksforgeeks.org/construct-binary-tree-string-bracket-representation/
Construct a binary tree from a string consisting of parenthesis and integers. The whole input represents a binary tree. It contains an integer followed by zero, one or two pairs of parenthesis. The integer represents the root’s value and a pair of parenthesis contains a child binary tree with the same structure. Always start to construct the left child node of the parent first if it exists.

Examples: 
Input : "1(2)(3)" 
Output : 1 2 3
Explanation :
           1
          / \
         2   3
Explanation: first pair of parenthesis contains 
left subtree and second one contains the right 
subtree. Preorder of above tree is "1 2 3".  

Input : "4(2(3)(1))(6(5))"
Output : 4 2 3 1 6 5
Explanation :
           4
         /   \
        2     6
       / \   / 
      3   1 5   
We know first character in string is root. Substring inside the first adjacent pair of parenthesis is for left subtree and substring inside second pair of parenthesis is for right subtree as in the below diagram. 





We need to find the substring corresponding to left subtree and substring corresponding to right subtree and then recursively call on both of the substrings. 

For this first find the index of starting index and end index of each substring. 
To find the index of closing parenthesis of left subtree substring, use a stack. Let the found index be stored in index variable. '''

# Python3 program to conStruct a
# binary tree from the given String

# Helper class that allocates a new node


class newNode:
    def __init__(self, data):
        self.data = data
        self.left = self.right = None

# This function is here just to test


def preOrder(node):
    if (node == None):
        return
    print(node.data, end=" ")
    preOrder(node.left)
    preOrder(node.right)

# function to return the index of
# close parenthesis


def findIndex(Str, si, ei):
    if (si > ei):
        return -1

    # Inbuilt stack
    s = []
    for i in range(si, ei + 1):

        # if open parenthesis, push it
        if (Str[i] == '('):
            s.append(Str[i])

        # if close parenthesis
        elif (Str[i] == ')'):
            if (s[-1] == '('):
                s.pop(-1)

                # if stack is empty, this is
                # the required index
                if len(s) == 0:
                    return i
    # if not found return -1
    return -1

# function to conStruct tree from String


def treeFromString(Str, si, ei):

    # Base case
    if (si > ei):
        return None

    # new root
    root = newNode(ord(Str[si]) - ord('0'))
    index = -1

    # if next char is '(' find the
    # index of its complement ')'
    if (si + 1 <= ei and Str[si + 1] == '('):
        index = findIndex(Str, si + 1, ei)

    # if index found
    if (index != -1):

        # call for left subtree
        root.left = treeFromString(Str, si + 2,
                                   index - 1)

        # call for right subtree
        root.right = treeFromString(Str, index + 2,
                                    ei - 1)
    return root


# Driver Code
if __name__ == '__main__':
    Str = "4(2(3)(1))(6(5))"
    root = treeFromString(Str, 0, len(Str) - 1)
    preOrder(root)


'''Output
4 2 3 1 6 5 
Time Complexity: O(N2)
Auxiliary Space: O(N)

Another recursive approach:

Algorithm:

The very first element of the string is the root.
If the next two consecutive elements are “(” and “)”, this means there is no left child otherwise we will create and add the left child to the parent node recursively.
Once the left child is added recursively, we will look for consecutive “(” and add the right child to the parent node.
Encountering “)” means the end of either left or right node and we will increment the start index
The recursion ends when the start index is greater than equal to the end index'''


class newNode:
    def __init__(self, data):
        self.data = data
        self.left = self.right = None


def preOrder(node):
    if (node == None):
        return
    print(node.data, end=" ")
    preOrder(node.left)
    preOrder(node.right)


def treeFromStringHelper(si, ei, arr, root):

    if si[0] >= ei:
        return None

    if arr[si[0]] == "(":

        if arr[si[0]+1] != ")":
            if root.left is None:
                if si[0] >= ei:
                    return
                new_root = newNode(arr[si[0]+1])
                root.left = new_root
                si[0] += 2
                treeFromStringHelper(si, ei, arr, new_root)

        else:
            si[0] += 2

        if root.right is None:
            if si[0] >= ei:
                return

            if arr[si[0]] != "(":
                si[0] += 1
                return

            new_root = newNode(arr[si[0]+1])
            root.right = new_root
            si[0] += 2
            treeFromStringHelper(si, ei, arr, new_root)
        else:
            return

    if arr[si[0]] == ")":
        if si[0] >= ei:
            return
        si[0] += 1
        return

    return


def treeFromString(string):

    root = newNode(string[0])

    if len(string) > 1:
        si = [1]
        ei = len(string)-1

        treeFromStringHelper(si, ei, string, root)

    return root


# Driver Code
if __name__ == '__main__':
    Str = "4(2(3)(1))(6(5))"
    root = treeFromString(Str)
    preOrder(root)
