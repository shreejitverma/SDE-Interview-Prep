/*https://practice.geeksforgeeks.org/problems/top-view-of-binary-tree/1
https://www.geeksforgeeks.org/print-nodes-top-view-binary-tree/#:~:text=Top%20view%20of%20a%20binary,node%20at%20its%20horizontal%20distance.
Top View of Binary Tree 
Medium Accuracy: 32.3% Submissions: 100k+ Points: 4
Given below is a binary tree. The task is to print the top view of binary tree. Top view of a binary tree is the set of nodes visible when the tree is viewed from the top. For the given below tree

       1
    /     \
   2       3
  /  \    /   \
4    5  6   7

Top view will be: 4 2 1 3 7
Note: Return nodes from leftmost node to rightmost node.

Example 1:

Input:
      1
   /    \
  2      3
Output: 2 1 3
Example 2:

Input:
       10
    /      \
  20        30
 /   \    /    \
40   60  90    100
Output: 40 20 10 30 100
Your Task:
Since this is a function problem. You don't have to take input. Just complete the function topView() that takes root node as parameter and returns a list of nodes visible from the top view from left to right.

Expected Time Complexity: O(N)
Expected Auxiliary Space: O(N).

Constraints:
1 ≤ N ≤ 105
1 ≤ Node Data ≤ 105*/

// C++ program to print top
// view of binary tree

#include <bits/stdc++.h>
using namespace std;

// Structure of binary tree
struct Node
{
    Node *left;
    Node *right;
    int hd;
    int data;
};

// function to create a new node
Node *newNode(int key)
{
    Node *node = new Node();
    node->left = node->right = NULL;
    node->data = key;
    return node;
}

// function should print the topView of
// the binary tree
void topview(Node *root)
{
    if (root == NULL)
        return;
    queue<Node *> q;
    map<int, int> m;
    int hd = 0;
    root->hd = hd;

    // push node and horizontal distance to queue
    q.push(root);

    cout << "The top view of the tree is : \n";

    while (q.size())
    {
        hd = root->hd;

        // count function returns 1 if the container
        // contains an element whose key is equivalent
        // to hd, or returns zero otherwise.
        if (m.count(hd) == 0)
            m[hd] = root->data;
        if (root->left)
        {
            root->left->hd = hd - 1;
            q.push(root->left);
        }
        if (root->right)
        {
            root->right->hd = hd + 1;
            q.push(root->right);
        }
        q.pop();
        root = q.front();
    }

    for (auto i = m.begin(); i != m.end(); i++)
    {
        cout << i->second << " ";
    }
}

// Driver Program to test above functions
int main()
{
    /* Create following Binary Tree
            1
        / \
        2 3
        \
            4
            \
            5
            \
                6*/
    Node *root = newNode(1);
    root->left = newNode(2);
    root->right = newNode(3);
    root->left->right = newNode(4);
    root->left->right->right = newNode(5);
    root->left->right->right->right = newNode(6);
    cout << "Following are nodes in top view of Binary "
            "Tree\n";
    topview(root);
    return 0;
}

vector<int> topView(Node *root)
{
    //Your code here
    map<int, int> m;
    queue<pair<Node *, int> > q;
    q.push({root, 0});
    while (!q.empty())
    {
        auto it = q.front();
        q.pop();
        Node *curr = it.first;
        int pos = it.second;

        //We will add only top of any position to map
        /*If map already contains element of this pos then this node will not be visible in top view*/

        if (m.find(pos) == m.end())
            m[pos] = curr->data;

        if (curr->left != NULL)
            q.push({curr->left, pos - 1});

        if (curr->right != NULL)
            q.push({curr->right, pos + 1});
    }
    vector<int> ans;
    for (auto i : m)
        ans.push_back(i.second);
    return ans;
}