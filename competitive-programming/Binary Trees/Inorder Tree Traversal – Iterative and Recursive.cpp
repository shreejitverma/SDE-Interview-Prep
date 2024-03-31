// https://www.techiedelight.com/inorder-tree-traversal-iterative-recursive/
#include <iostream>
using namespace std;

// Data structure to store a binary tree node
struct Node
{
    int data;
    Node *left, *right;

    Node(int data)
    {
        this->data = data;
        this->left = this->right = nullptr;
    }
};

// Recursive function to perform inorder traversal on the tree
void inorder(Node *root)
{
    // return if the current node is empty
    if (root == nullptr)
    {
        return;
    }

    // Traverse the left subtree
    inorder(root->left);

    // Display the data part of the root (or current node)
    cout << root->data << " ";

    // Traverse the right subtree
    inorder(root->right);
}

int main()
{
    /* Construct the following tree
               4
             /   \
            /     \
           2       8
          / \     /  \
         /   \   /    \
        1     3 6      9
               / \
              /   \
             5     7
    */

    Node *root = new Node(4);
    root->left = new Node(2);
    root->right = new Node(8);
    root->left->left = new Node(1);
    root->left->right = new Node(3);
    root->right->left = new Node(6);
    root->right->right = new Node(9);
    root->right->left->left = new Node(5);
    root->right->left->right = new Node(7);

    inorder(root);

    return 0;
}

#include <iostream>
#include <stack>
using namespace std;

// Data structure to store a binary tree node
struct Node
{
    int data;
    Node *left, *right;

    Node(int data)
    {
        this->data = data;
        this->left = this->right = nullptr;
    }
};

// Iterative function to perform inorder traversal on the tree
void inorderIterative(Node *root)
{
    // create an empty stack
    stack<Node *> stack;

    // start from the root node (set current node to the root node)
    Node *curr = root;

    // if the current node is null and the stack is also empty, we are done
    while (!stack.empty() || curr != nullptr)
    {
        // if the current node exists, push it into the stack (defer it)
        // and move to its left child
        if (curr != nullptr)
        {
            stack.push(curr);
            curr = curr->left;
        }
        else
        {
            // otherwise, if the current node is null, pop an element from the stack,
            // print it, and finally set the current node to its right child
            curr = stack.top();
            stack.pop();
            cout << curr->data << " ";

            curr = curr->right;
        }
    }
}

int main()
{
    /* Construct the following tree
               4
             /   \
            /     \
           2       8
          / \     /  \
         /   \   /    \
        1     3 6      9
               / \
              /   \
             5     7
    */

    Node *root = new Node(4);
    root->left = new Node(2);
    root->right = new Node(8);
    root->left->left = new Node(1);
    root->left->right = new Node(3);
    root->right->left = new Node(6);
    root->right->right = new Node(9);
    root->right->left->left = new Node(5);
    root->right->left->right = new Node(7);

    inorderIterative(root);

    return 0;
}