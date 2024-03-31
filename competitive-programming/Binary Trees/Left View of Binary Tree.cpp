/*https: // practice.geeksforgeeks.org/problems/left-view-of-binary-tree/1
https://www.geeksforgeeks.org/print-left-view-binary-tree/
https://www.geeksforgeeks.org/iterative-method-to-print-left-view-of-a-binary-tree/
Given a Binary Tree, print Left view of it. Left view of a Binary Tree is set of nodes visible when tree is visited from Left side. The task is to complete the function leftView(), which accepts root of the tree as argument.

Left view of following tree is 1 2 4 8.

          1
       /     \
     2        3
   /     \    /    \
  4     5   6    7
   \
     8   

Example 1:

Input:
   1
 /  \
3    2
Output: 1 3

Example 2:

Input:

Output: 10 20 40
Your Task:
You just have to complete the function leftView() that prints the left view. The newline is automatically appended by the driver code.
Expected Time Complexity: O(N).
Expected Auxiliary Space: O(Height of the Tree).

Constraints:
0 <= Number of nodes <= 100
1 <= Data of a node <= 1000*/
// C++ program to print left view of Binary Tree
#include <bits/stdc++.h>
using namespace std;

struct Node
{
    int data;
    struct Node *left, *right;
};

// A utility function to
// create a new Binary Tree Node
struct Node *newNode(int item)
{
    struct Node *temp = (struct Node *)malloc(
        sizeof(struct Node));
    temp->data = item;
    temp->left = temp->right = NULL;
    return temp;
}

// Recursive function to print
// left view of a binary tree.
void leftViewUtil(struct Node *root,
                  int level, int *max_level)
{
    // Base Case
    if (root == NULL)
        return;

    // If this is the first Node of its level
    if (*max_level < level)
    {
        cout << root->data << " ";
        *max_level = level;
    }

    // Recur for left subtree first,
    // then right subtree
    leftViewUtil(root->left, level + 1, max_level);
    leftViewUtil(root->right, level + 1, max_level);
}

// A wrapper over leftViewUtil()
void leftView(struct Node *root)
{
    int max_level = 0;
    leftViewUtil(root, 1, &max_level);
}

// Driver Code
int main()
{
    Node *root = newNode(10);
    root->left = newNode(2);
    root->right = newNode(3);
    root->left->left = newNode(7);
    root->left->right = newNode(8);
    root->right->right = newNode(15);
    root->right->left = newNode(12);
    root->right->right->left = newNode(14);

    leftView(root);

    return 0;
}

// Iterative Approach
// C++ program to print left view of
// Binary Tree

#include <bits/stdc++.h>
using namespace std;

// A Binary Tree Node
struct Node
{
    int data;
    struct Node *left, *right;
};

// Utility function to create a new tree node
Node *newNode(int data)
{
    Node *temp = new Node;
    temp->data = data;
    temp->left = temp->right = NULL;
    return temp;
}

// function to print left view of
// binary tree
void printLeftView(Node *root)
{
    if (!root)
        return;

    queue<Node *> q;
    q.push(root);

    while (!q.empty())
    {
        // number of nodes at current level
        int n = q.size();

        // Traverse all nodes of current level
        for (int i = 1; i <= n; i++)
        {
            Node *temp = q.front();
            q.pop();

            // Print the left most element
            // at the level
            if (i == 1)
                cout << temp->data << " ";

            // Add left node to queue
            if (temp->left != NULL)
                q.push(temp->left);

            // Add right node to queue
            if (temp->right != NULL)
                q.push(temp->right);
        }
    }
}

// Driver code
int main()
{
    // Let's construct the tree as
    // shown in example

    Node *root = newNode(10);
    root->left = newNode(2);
    root->right = newNode(3);
    root->left->left = newNode(7);
    root->left->right = newNode(8);
    root->right->right = newNode(15);
    root->right->left = newNode(12);
    root->right->right->left = newNode(14);

    printLeftView(root);
}
// 2nd Iterative Approach
// C++ program to print the
// left view of Binary Tree

#include <bits/stdc++.h>

using namespace std;

// A Binary Tree Node
struct node
{
    int data;
    struct node *left, *right;
};

// A utility function to create a new
// Binary Tree node
struct node *newNode(int item)
{
    struct node *temp = new node;
    temp->data = item;
    temp->left = NULL;
    temp->right = NULL;
    return temp;
}

// Utility function to print the left view of
// the binary tree
void leftViewUtil(struct node *root, queue<node *> &q)
{
    if (root == NULL)
        return;

    // Push root
    q.push(root);

    // Delimiter
    q.push(NULL);

    while (!q.empty())
    {
        node *temp = q.front();

        if (temp)
        {

            // Prints first node
            // of each level
            cout << temp->data << " ";

            // Push children of all nodes at
            // current level
            while (q.front() != NULL)
            {

                // If left child is present
                // push into queue
                if (temp->left)
                    q.push(temp->left);

                // If right child is present
                // push into queue
                if (temp->right)
                    q.push(temp->right);

                // Pop the current node
                q.pop();

                temp = q.front();
            }

            // Push delimiter
            // for the next level
            q.push(NULL);
        }

        // Pop the delimiter of
        // the previous level
        q.pop();
    }
}

// Function to print the leftView
// of Binary Tree
void leftView(struct node *root)
{
    // Queue to store all
    // the nodes of the tree
    queue<node *> q;

    leftViewUtil(root, q);
}

// Driver Code
int main()
{
    struct node *root = newNode(10);
    root->left = newNode(12);
    root->right = newNode(3);
    root->left->right = newNode(4);
    root->right->left = newNode(5);
    root->right->left->right = newNode(6);
    root->right->left->right->left = newNode(18);
    root->right->left->right->right = newNode(7);

    leftView(root);

    return 0;
}