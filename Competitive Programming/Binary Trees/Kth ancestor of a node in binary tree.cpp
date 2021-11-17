/*
https://www.geeksforgeeks.org/kth-ancestor-node-binary-tree-set-2/
https://youtu.be/HeY99dtGi6U
Given a binary tree in which nodes are numbered from 1 to n. Given a node and a positive integer K. We have to print the Kth ancestor of the given node in the binary tree. If there does not exist any such ancestor then print -1.
For example in the below given binary tree, the 2nd ancestor of 5 is 1. 3rd ancestor of node 5 will be -1. 
We have discussed a BFS based solution for this problem in our previous article. If you observe that solution carefully, you will see that the basic approach was to first find the node and then backtrack to the kth parent. The same thing can be done using recursive DFS without using an extra array. 
The idea of using DFS is to first find the given node in the tree and then backtrack k times to reach to the kth ancestor, once we have reached the kth parent, we will simply print the node and return NULL. 
Below is the implementation of above idea: 

Output:  

Kth ancestor is: 1
Time Complexity: O(n), where n is the number of nodes in the binary tree. 
*/

/* C++ program to calculate Kth ancestor of given node */
#include <bits/stdc++.h>
using namespace std;

// A Binary Tree Node
struct Node
{
    int data;
    struct Node *left, *right;
};

// temporary node to keep track of Node returned
// from previous recursive call during backtrack
Node *temp = NULL;

// recursive function to calculate Kth ancestor
Node *kthAncestorDFS(Node *root, int node, int &k)
{
    // Base case
    if (!root)
        return NULL;

    if (root->data == node ||
        (temp = kthAncestorDFS(root->left, node, k)) ||
        (temp = kthAncestorDFS(root->right, node, k)))
    {
        if (k > 0)
            k--;

        else if (k == 0)
        {
            // print the kth ancestor
            cout << "Kth ancestor is: " << root->data;

            // return NULL to stop further backtracking
            return NULL;
        }

        // return current node to previous call
        return root;
    }
}

// Utility function to create a new tree node
Node *newNode(int data)
{
    Node *temp = new Node;
    temp->data = data;
    temp->left = temp->right = NULL;
    return temp;
}

// Driver program to test above functions
int main()
{
    // Let us create binary tree shown in above diagram
    Node *root = newNode(1);
    root->left = newNode(2);
    root->right = newNode(3);
    root->left->left = newNode(4);
    root->left->right = newNode(5);

    int k = 2;
    int node = 5;

    // print kth ancestor of given node
    Node *parent = kthAncestorDFS(root, node, k);

    // check if parent is not NULL, it means
    // there is no Kth ancestor of the node
    if (parent)
        cout << "-1";

    return 0;
}