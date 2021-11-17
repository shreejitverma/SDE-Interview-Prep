/*
https://www.geeksforgeeks.org/maximum-sum-nodes-binary-tree-no-two-adjacent/
Given a binary tree with a value associated with each node, we need to choose a subset of these nodes such that the sum of chosen nodes is maximum under a constraint that no two chosen nodes in the subset should be directly connected that is, if we have taken a node in our sum then we can’t take any of its children in consideration and vice versa. 
Method 1 
We can solve this problem by considering the fact that both node and its children can’t be in sum at the same time, so when we take a node into our sum we will call recursively for its grandchildren or if we don’t take this node then we will call for all its children nodes and finally we will choose maximum from both of the results. 
It can be seen easily that the above approach can lead to solving the same subproblem many times, for example in the above diagram node 1 calls node 4 and 5 when its value is chosen and node 3 also calls them when its value is not chosen so these nodes are processed more than once. We can stop solving these nodes more than once by memoizing the result at all nodes. 
In the below code, a map is used for memorizing the result which stores the result of the complete subtree rooted at a node in the map so that if it is called again, the value is not calculated again instead stored value from the map is returned directly. 

Please see the below code for a better understanding.
Method 3(Using dynamic programming)

Store the maximum sum by including a node or excluding the node in a dp array or unordered map. Recursively calls for grandchildren of nodes if the node is included or calls for neighbours if the node is excluded.
*/

// C++ program to find maximum sum in Binary Tree
// such that no two nodes are adjacent.
#include <iostream>
#include <bits/stdc++.h>
using namespace std;

class Node
{
public:
    int data;
    Node *left, *right;
    Node(int data)
    {
        this->data = data;
        left = NULL;
        right = NULL;
    }
};
//declare map /dp array as global
unordered_map<Node *, int> umap;
int maxSum(Node *root)
{
    //base case
    if (!root)
        return 0;

    //if the max sum from the  node is already in map,return the value
    if (umap[root])
        return umap[root];

    //if the current node(root) is included in result
    //then find maximum sum
    int inc = root->data;

    //if left of node exsists, addd their grandchildren
    if (root->left)
    {
        inc += maxSum(root->left->left) + maxSum(root->left->right);
    }
    //if right of node exsist,add their grandchildren
    if (root->right)
    {
        inc += maxSum(root->right->left) + maxSum(root->right->right);
    }

    //if the current node(root) is excluded, find the maximum sum
    int ex = maxSum(root->left) + maxSum(root->right);

    //store the maximum of including & excluding the node in map
    umap[root] = max(inc, ex);
}

// Driver code
int main()
{
    Node *root = new Node(10);
    root->left = new Node(1);
    root->left->left = new Node(2);
    root->left->left->left = new Node(1);
    root->left->right = new Node(3);
    root->left->right->left = new Node(4);
    root->left->right->right = new Node(5);
    cout << maxSum(root);
    return 0;
}

// C++ program to find maximum sum from a subset of
// nodes of binary tree
#include <bits/stdc++.h>
using namespace std;

/* A binary tree node structure */
struct node
{
    int data;
    struct node *left, *right;
};

/* Utility function to create a new Binary Tree node */
struct node *newNode(int data)
{
    struct node *temp = new struct node;
    temp->data = data;
    temp->left = temp->right = NULL;
    return temp;
}

//  Declaration of methods
int sumOfGrandChildren(node *node);
int getMaxSum(node *node);
int getMaxSumUtil(node *node, map<struct node *, int> &mp);

// method returns maximum sum possible from subtrees rooted
// at grandChildrens of node 'node'
int sumOfGrandChildren(node *node, map<struct node *, int> &mp)
{
    int sum = 0;

    //  call for children of left child only if it is not NULL
    if (node->left)
        sum += getMaxSumUtil(node->left->left, mp) +
               getMaxSumUtil(node->left->right, mp);

    //  call for children of right child only if it is not NULL
    if (node->right)
        sum += getMaxSumUtil(node->right->left, mp) +
               getMaxSumUtil(node->right->right, mp);

    return sum;
}

//  Utility method to return maximum sum rooted at node 'node'
int getMaxSumUtil(node *node, map<struct node *, int> &mp)
{
    if (node == NULL)
        return 0;

    // If node is already processed then return calculated
    // value from map
    if (mp.find(node) != mp.end())
        return mp[node];

    //  take current node value and call for all grand children
    int incl = node->data + sumOfGrandChildren(node, mp);

    //  don't take current node value and call for all children
    int excl = getMaxSumUtil(node->left, mp) +
               getMaxSumUtil(node->right, mp);

    //  choose maximum from both above calls and store that in map
    mp[node] = max(incl, excl);

    return mp[node];
}

// Returns maximum sum from subset of nodes
// of binary tree under given constraints
int getMaxSum(node *node)
{
    if (node == NULL)
        return 0;
    map<struct node *, int> mp;
    return getMaxSumUtil(node, mp);
}

//  Driver code to test above methods
int main()
{
    node *root = newNode(1);
    root->left = newNode(2);
    root->right = newNode(3);
    root->right->left = newNode(4);
    root->right->right = newNode(5);
    root->left->left = newNode(1);

    cout << getMaxSum(root) << endl;
    return 0;
}

/*
Method 2 (Using pair) 
Return a pair for each node in the binary tree such that the first of the pair indicates maximum sum when the data of a node is included and the second indicates maximum sum when the data of a particular node is not included. 
*/

// C++ program to find maximum sum in Binary Tree
// such that no two nodes are adjacent.
#include <iostream>
using namespace std;

class Node
{
public:
    int data;
    Node *left, *right;
    Node(int data)
    {
        this->data = data;
        left = NULL;
        right = NULL;
    }
};

pair<int, int> maxSumHelper(Node *root)
{
    if (root == NULL)
    {
        pair<int, int> sum(0, 0);
        return sum;
    }
    pair<int, int> sum1 = maxSumHelper(root->left);
    pair<int, int> sum2 = maxSumHelper(root->right);
    pair<int, int> sum;

    // This node is included (Left and right children
    // are not included)
    sum.first = sum1.second + sum2.second + root->data;

    // This node is excluded (Either left or right
    // child is included)
    sum.second = max(sum1.first, sum1.second) +
                 max(sum2.first, sum2.second);

    return sum;
}

int maxSum(Node *root)
{
    pair<int, int> res = maxSumHelper(root);
    return max(res.first, res.second);
}

// Driver code
int main()
{
    Node *root = new Node(10);
    root->left = new Node(1);
    root->left->left = new Node(2);
    root->left->left->left = new Node(1);
    root->left->right = new Node(3);
    root->left->right->left = new Node(4);
    root->left->right->right = new Node(5);
    cout << maxSum(root);
    return 0;
}