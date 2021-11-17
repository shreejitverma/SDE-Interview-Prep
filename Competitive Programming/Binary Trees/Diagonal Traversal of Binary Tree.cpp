/*
https://www.geeksforgeeks.org/diagonal-traversal-of-binary-tree/
Consider lines of slope -1 passing between nodes. Given a Binary Tree, print all diagonal elements in a binary tree belonging to the same line.

Input : Root of below tree

Output : 
Diagonal Traversal of binary tree : 
 8 10 14
 3 6 7 13
 1 4
Observation : root and root->right values will be prioritized over all root->left values.
The idea is to use a map. We use different slope distances and use them as key in the map. Value in the map is a vector (or dynamic array) of nodes. We traverse the tree to store values in the map. Once map is built, we print the contents of it. 

Below is the implementation of the above idea.
*/

// C++ program for diagonal
// traversal of Binary Tree
#include <bits/stdc++.h>
using namespace std;

// Tree node
struct Node
{
    int data;
    Node *left, *right;
};

/* root - root of the binary tree
   d -  distance of current line from rightmost
        -topmost slope.
   diagonalPrint - multimap to store Diagonal
                   elements (Passed by Reference) */
void diagonalPrintUtil(Node *root, int d,
                       map<int, vector<int> > &diagonalPrint)
{
    // Base case
    if (!root)
        return;

    // Store all nodes of same
    // line together as a vector
    diagonalPrint[d].push_back(root->data);

    // Increase the vertical
    // distance if left child
    diagonalPrintUtil(root->left,
                      d + 1, diagonalPrint);

    // Vertical distance remains
    // same for right child
    diagonalPrintUtil(root->right,
                      d, diagonalPrint);
}

// Print diagonal traversal
// of given binary tree
void diagonalPrint(Node *root)
{

    // create a map of vectors
    // to store Diagonal elements
    map<int, vector<int> > diagonalPrint;
    diagonalPrintUtil(root, 0, diagonalPrint);

    cout << "Diagonal Traversal of binary tree : \n";
    for (auto it : diagonalPrint)
    {
        vector<int> v = it.second;
        for (auto it : v)
            cout << it << " ";
        cout << endl;
    }
}

// Utility method to create a new node
Node *newNode(int data)
{
    Node *node = new Node;
    node->data = data;
    node->left = node->right = NULL;
    return node;
}

// Driver program
int main()
{
    Node *root = newNode(8);
    root->left = newNode(3);
    root->right = newNode(10);
    root->left->left = newNode(1);
    root->left->right = newNode(6);
    root->right->right = newNode(14);
    root->right->right->left = newNode(13);
    root->left->right->left = newNode(4);
    root->left->right->right = newNode(7);

    /*  Node* root = newNode(1);
        root->left = newNode(2);
        root->right = newNode(3);
        root->left->left = newNode(9);
        root->left->right = newNode(6);
        root->right->left = newNode(4);
        root->right->right = newNode(5);
        root->right->left->right = newNode(7);
        root->right->left->left = newNode(12);
        root->left->right->left = newNode(11);
        root->left->left->right = newNode(10);*/

    diagonalPrint(root);

    return 0;
}

/*
Output
Diagonal Traversal of binary tree : 
8 10 14 
3 6 7 13 
1 4 
The time complexity of this solution is O( N logN ) and the space complexity is O( N )

We can solve the same problem using queue and iterative algorithm. 
*/

#include <bits/stdc++.h>
using namespace std;

// Tree node
struct Node
{
    int data;
    Node *left, *right;
};

vector<int> diagonal(Node *root)
{
    vector<int> diagonalVals;
    if (!root)
        return diagonalVals;

    // The leftQueue will be a queue which will store all
    // left pointers while traversing the tree, and will be
    // utilized when at any point right pointer becomes NULL

    queue<Node *> leftQueue;
    Node *node = root;

    while (node)
    {

        // Add current node to output
        diagonalVals.push_back(node->data);
        // If left child available, add it to queue
        if (node->left)
            leftQueue.push(node->left);

        // if right child, transfer the node to right
        if (node->right)
            node = node->right;

        else
        {
            // If left child Queue is not empty, utilize it
            // to traverse further
            if (!leftQueue.empty())
            {
                node = leftQueue.front();
                leftQueue.pop();
            }
            else
            {
                // All the right childs traversed and no
                // left child left
                node = NULL;
            }
        }
    }
    return diagonalVals;
}

// Utility method to create a new node
Node *newNode(int data)
{
    Node *node = new Node;
    node->data = data;
    node->left = node->right = NULL;
    return node;
}

// Driver program
int main()
{
    Node *root = newNode(8);
    root->left = newNode(3);
    root->right = newNode(10);
    root->left->left = newNode(1);
    root->left->right = newNode(6);
    root->right->right = newNode(14);
    root->right->right->left = newNode(13);
    root->left->right->left = newNode(4);
    root->left->right->right = newNode(7);

    /* Node* root = newNode(1);
            root->left = newNode(2);
            root->right = newNode(3);
            root->left->left = newNode(9);
            root->left->right = newNode(6);
            root->right->left = newNode(4);
            root->right->right = newNode(5);
            root->right->left->right = newNode(7);
            root->right->left->left = newNode(12);
            root->left->right->left = newNode(11);
            root->left->left->right = newNode(10);*/

    vector<int> diagonalValues = diagonal(root);
    for (int i = 0; i < diagonalValues.size(); i++)
    {
        cout << diagonalValues[i] << " ";
    }
    cout << endl;

    return 0;
}
/*
Output
[8, 10, 14, 3, 6, 7, 13, 1, 4]
The time complexity of this solution is O( N logN ) and the space complexity is O( N )

Approach 2 : Using Queue.

Every node will help to generate the next diagonal. We will push the element in the queue only when its left is available. We will process the node and move to its right.
*/

#include <bits/stdc++.h>
using namespace std;

struct Node
{
    int data;
    Node *left, *right;
};

Node *newNode(int data)
{
    Node *node = new Node;
    node->data = data;
    node->left = node->right = NULL;
    return node;
}

vector<vector<int> > result;
void diagonalPrint(Node *root)
{
    if (root == NULL)
        return;

    queue<Node *> q;
    q.push(root);

    while (!q.empty())
    {
        int size = q.size();
        vector<int> answer;

        while (size--)
        {
            Node *temp = q.front();
            q.pop();

            // traversing each component;
            while (temp)
            {
                answer.push_back(temp->data);

                if (temp->left)
                    q.push(temp->left);

                temp = temp->right;
            }
        }
        result.push_back(answer);
    }
}

int main()
{
    Node *root = newNode(8);
    root->left = newNode(3);
    root->right = newNode(10);
    root->left->left = newNode(1);
    root->left->right = newNode(6);
    root->right->right = newNode(14);
    root->right->right->left = newNode(13);
    root->left->right->left = newNode(4);
    root->left->right->right = newNode(7);

    diagonalPrint(root);

    for (int i = 0; i < result.size(); i++)
    {
        for (int j = 0; j < result[i].size(); j++)
            cout << result[i][j] << "  ";
        cout << endl;
    }

    return 0;
}

/*
Output
8  10  14  
3  6  7  13  
1  4  
Time Complexity: O(N), because we are visiting nodes once.

Space Complexity: O(N), because we are using queue.
*/