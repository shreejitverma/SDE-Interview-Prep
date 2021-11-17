/*
https://www.geeksforgeeks.org/construct-binary-tree-string-bracket-representation/
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
To find the index of closing parenthesis of left subtree substring, use a stack. Let the found index be stored in index variable. 
*/

/* C++ program to construct a binary tree from
   the given string */
#include <bits/stdc++.h>
using namespace std;

/* A binary tree node has data, pointer to left
   child and a pointer to right child */
struct Node
{
    int data;
    Node *left, *right;
};
/* Helper function that allocates a new node */
Node *newNode(int data)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->data = data;
    node->left = node->right = NULL;
    return (node);
}

/* This function is here just to test  */
void preOrder(Node *node)
{
    if (node == NULL)
        return;
    printf("%d ", node->data);
    preOrder(node->left);
    preOrder(node->right);
}

// function to return the index of close parenthesis
int findIndex(string str, int si, int ei)
{
    if (si > ei)
        return -1;

    // Inbuilt stack
    stack<char> s;

    for (int i = si; i <= ei; i++)
    {

        // if open parenthesis, push it
        if (str[i] == '(')
            s.push(str[i]);

        // if close parenthesis
        else if (str[i] == ')')
        {
            if (s.top() == '(')
            {
                s.pop();

                // if stack is empty, this is
                // the required index
                if (s.empty())
                    return i;
            }
        }
    }
    // if not found return -1
    return -1;
}

// function to construct tree from string
Node *treeFromString(string str, int si, int ei)
{
    // Base case
    if (si > ei)
        return NULL;

    // new root
    Node *root = newNode(str[si] - '0');
    int index = -1;

    // if next char is '(' find the index of
    // its complement ')'
    if (si + 1 <= ei && str[si + 1] == '(')
        index = findIndex(str, si + 1, ei);

    // if index found
    if (index != -1)
    {

        // call for left subtree
        root->left = treeFromString(str, si + 2, index - 1);

        // call for right subtree
        root->right = treeFromString(str, index + 2, ei - 1);
    }
    return root;
}

// Driver Code
int main()
{
    string str = "4(2(3)(1))(6(5))";
    Node *root = treeFromString(str, 0, str.length() - 1);
    preOrder(root);
}

/*
Output
4 2 3 1 6 5 
Time Complexity: O(N2)
Auxiliary Space: O(N)

Another recursive approach:

Algorithm:

The very first element of the string is the root.
If the next two consecutive elements are “(” and “)”, this means there is no left child otherwise we will create and add the left child to the parent node recursively.
Once the left child is added recursively, we will look for consecutive “(” and add the right child to the parent node.
Encountering “)” means the end of either left or right node and we will increment the start index
The recursion ends when the start index is greater than equal to the end index
*/

#include <bits/stdc++.h>
using namespace std;

// custom data type for tree building
struct Node
{
    int data;
    struct Node *left;
    struct Node *right;
    Node(int val)
    {
        data = val;
        left = right = NULL;
    }
};

// Below function accepts string and a pointer variable as
// an argument
// and draw the tree. Returns the root of the tree
Node *constructtree(string s, int *start)
{
    // Assuming there is/are no negative
    // character/characters in the string
    if (s.size() == 0 || *start >= s.size())
        return NULL;

    // constructing a number from the continuous digits
    int num = 0;
    while (*start < s.size() && s[*start] != '(' && s[*start] != ')')
    {
        int num_here = (int)(s[*start] - '0');
        num = num * 10 + num_here;
        *start = *start + 1;
    }

    // creating a node from the constructed number from
    // above loop
    struct Node *root = new Node(num);

    // check if start has reached the end of the string
    if (*start >= s.size())
        return root;

    // As soon as we see first right parenthesis from the
    // current node we start to construct the tree in the
    // left
    if (*start < s.size() && s[*start] == '(')
    {
        *start = *start + 1;
        root->left = constructtree(s, start);
    }
    if (*start < s.size() && s[*start] == ')')
        *start = *start + 1;

    // As soon as we see second right parenthesis from the
    // current node we start to construct the tree in the
    // right
    if (*start < s.size() && s[*start] == '(')
    {
        *start = *start + 1;
        root->right = constructtree(s, start);
    }
    if (*start < s.size() && s[*start] == ')')
        *start = *start + 1;
    return root;
}
void preorder(Node *root)
{
    if (root == NULL)
        return;
    cout << root->data << " ";
    preorder(root->left);
    preorder(root->right);
}
int main()
{
    string s = "4(2(3)(1))(6(5))";
    // cin>>s;
    int start = 0;
    Node *root = constructtree(s, &start);
    preorder(root);
    return 0;
}