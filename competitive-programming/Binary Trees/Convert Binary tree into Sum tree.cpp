/*
https://practice.geeksforgeeks.org/problems/transform-to-sum-tree/1
Transform to Sum Tree 
Easy Accuracy: 50.22% Submissions: 41768 Points: 2
Given a Binary Tree of size N , where each node can have positive or negative values. Convert this to a tree where each node contains the sum of the left and right sub trees of the original tree. The values of leaf nodes are changed to 0.

Example 1:

Input:
             10
          /      \
        -2        6
       /   \     /  \
     8     -4   7    5

Output:
            20
          /    \
        4        12
       /  \     /  \
     0     0   0    0

Explanation:

           (4-2+12+6)
          /           \
      (8-4)          (7+5)
       /   \         /  \
      0     0       0    0
 

Your Task:  
You dont need to read input or print anything. Complete the function toSumTree() which takes root node as input parameter and modifies the given tree in-place.

Note: If you click on Compile and Test the output will be the in-order traversal of the modified tree.


Expected Time Complexity: O(N)
Expected Auxiliary Space: O(height of tree)
 

Constraints:
1 ≤ N ≤ 104
*/

// { Driver Code Starts
#include <bits/stdc++.h>
using namespace std;

struct Node
{
    int data;
    struct Node *left;
    struct Node *right;
};
// Utility function to create a new Tree Node
Node *newNode(int val)
{
    Node *temp = new Node;
    temp->data = val;
    temp->left = NULL;
    temp->right = NULL;

    return temp;
}
// Function to Build Tree
Node *buildTree(string str)
{
    // Corner Case
    if (str.length() == 0 || str[0] == 'N')
        return NULL;

    // Creating vector of strings from input
    // string after spliting by space
    vector<string> ip;

    istringstream iss(str);
    for (string str; iss >> str;)
        ip.push_back(str);

    // Create the root of the tree
    Node *root = newNode(stoi(ip[0]));

    // Push the root to the queue
    queue<Node *> queue;
    queue.push(root);

    // Starting from the second element
    int i = 1;
    while (!queue.empty() && i < ip.size())
    {

        // Get and remove the front of the queue
        Node *currNode = queue.front();
        queue.pop();

        // Get the current node's value from the string
        string currVal = ip[i];

        // If the left child is not null
        if (currVal != "N")
        {

            // Create the left child for the current node
            currNode->left = newNode(stoi(currVal));

            // Push it to the queue
            queue.push(currNode->left);
        }

        // For the right child
        i++;
        if (i >= ip.size())
            break;
        currVal = ip[i];

        // If the right child is not null
        if (currVal != "N")
        {

            // Create the right child for the current node
            currNode->right = newNode(stoi(currVal));

            // Push it to the queue
            queue.push(currNode->right);
        }
        i++;
    }

    return root;
}
void inorder(Node *node)
{
    if (node == NULL)
        return;

    inorder(node->left);
    cout << node->data << " ";
    inorder(node->right);
}

// } Driver Code Ends
//User function template for C++

/* A binary tree node
struct Node
{
    int data;
    Node* left, * right;
}; */

class Solution
{
public:
    // Convert a given tree to a tree where every node contains sum of values of
    // nodes in left and right subtrees in the original tree
    int par(Node *root)
    {
        if (root == 0)
            return 0;

        int l = par(root->left);
        int r = par(root->right);

        int t = root->data + l + r;
        root->data = l + r;
        return t;
    }
    void toSumTree(Node *node)
    {
        par(node);
    }
};

// { Driver Code Starts.

int main()
{

    int t;
    scanf("%d ", &t);
    while (t--)
    {
        string s;
        getline(cin, s);
        Node *root = buildTree(s);
        Solution ob;
        ob.toSumTree(root);
        inorder(root);
        cout << endl;
    }
    return 1;
} // } Driver Code Ends