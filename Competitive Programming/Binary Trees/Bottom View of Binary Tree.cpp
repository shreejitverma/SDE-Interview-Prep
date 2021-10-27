/*https://practice.geeksforgeeks.org/problems/bottom-view-of-binary-tree/1
https://www.geeksforgeeks.org/bottom-view-binary-tree/
Bottom View of Binary Tree 
Medium Accuracy: 45.32% Submissions: 90429 Points: 4
Given a binary tree, print the bottom view from left to right.
A node is included in bottom view if it can be seen when we look at the tree from bottom.

                      20
                    /    \
                  8       22
                /   \        \
              5      3       25
                    /   \      
                  10    14

For the above tree, the bottom view is 5 10 3 14 25.
If there are multiple bottom-most nodes for a horizontal distance from root, then print the later one in level traversal. For example, in the below diagram, 3 and 4 are both the bottommost nodes at horizontal distance 0, we need to print 4.

                      20
                    /    \
                  8       22
                /   \     /   \
              5      3 4     25
                     /    \      
                 10       14

For the above tree the output should be 5 10 4 14 25.
 

Example 1:

Input:
       1
     /   \
    3     2
Output: 3 1 2
Explanation:
First case represents a tree with 3 nodes
and 2 edges where root is 1, left child of
1 is 3 and right child of 1 is 2.

Thus nodes of the binary tree will be
printed as such 3 1 2.
Example 2:

Input:
         10
       /    \
      20    30
     /  \
    40   60
Output: 40 20 60 30
Your Task:
This is a functional problem, you don't need to care about input, just complete the function bottomView() which takes the root node of the tree as input and returns an array containing the bottom view of the given tree.

Expected Time Complexity: O(N).
Expected Auxiliary Space: O(N).

Constraints:
1 <= Number of nodes <= 105
1 <= Data of a node <= 105
*/

// C++ Program to print Bottom View of Binary Tree
#include <bits/stdc++.h>
using namespace std;

// Tree node class
struct Node
{
  int data;           //data of the node
  int hd;             //horizontal distance of the node
  Node *left, *right; //left and right references

  // Constructor of tree node
  Node(int key)
  {
    data = key;
    hd = INT_MAX;
    left = right = NULL;
  }
};

// Method that prints the bottom view.
void bottomView(Node *root)
{
  if (root == NULL)
    return;

  // Initialize a variable 'hd' with 0
  // for the root element.
  int hd = 0;

  // TreeMap which stores key value pair
  // sorted on key value
  map<int, int> m;

  // Queue to store tree nodes in level
  // order traversal
  queue<Node *> q;

  // Assign initialized horizontal distance
  // value to root node and add it to the queue.
  root->hd = hd;
  q.push(root); // In STL, push() is used enqueue an item

  // Loop until the queue is empty (standard
  // level order loop)
  while (!q.empty())
  {
    Node *temp = q.front();
    q.pop(); // In STL, pop() is used dequeue an item

    // Extract the horizontal distance value
    // from the dequeued tree node.
    hd = temp->hd;

    // Put the dequeued tree node to TreeMap
    // having key as horizontal distance. Every
    // time we find a node having same horizontal
    // distance we need to replace the data in
    // the map.
    m[hd] = temp->data;

    // If the dequeued node has a left child, add
    // it to the queue with a horizontal distance hd-1.
    if (temp->left != NULL)
    {
      temp->left->hd = hd - 1;
      q.push(temp->left);
    }

    // If the dequeued node has a right child, add
    // it to the queue with a horizontal distance
    // hd+1.
    if (temp->right != NULL)
    {
      temp->right->hd = hd + 1;
      q.push(temp->right);
    }
  }

  // Traverse the map elements using the iterator.
  for (auto i = m.begin(); i != m.end(); ++i)
    cout << i->second << " ";
}

// Driver Code
int main()
{
  Node *root = new Node(20);
  root->left = new Node(8);
  root->right = new Node(22);
  root->left->left = new Node(5);
  root->left->right = new Node(3);
  root->right->left = new Node(4);
  root->right->right = new Node(25);
  root->left->right->left = new Node(10);
  root->left->right->right = new Node(14);
  cout << "Bottom view of the given binary tree :\n" bottomView(root);
  return 0;
}

// C++ Program to print Bottom View of Binary Tree
#include <bits/stdc++.h>
#include <map>
using namespace std;

// Tree node class
struct Node
{
  // data of the node
  int data;

  // horizontal distance of the node
  int hd;

  //left and right references
  Node *left, *right;

  // Constructor of tree node
  Node(int key)
  {
    data = key;
    hd = INT_MAX;
    left = right = NULL;
  }
};

void printBottomViewUtil(Node *root, int curr, int hd, map<int, pair<int, int> > &m)
{
  // Base case
  if (root == NULL)
    return;

  // If node for a particular
  // horizontal distance is not
  // present, add to the map.
  if (m.find(hd) == m.end())
  {
    m[hd] = make_pair(root->data, curr);
  }
  // Compare height for already
  // present node at similar horizontal
  // distance
  else
  {
    pair<int, int> p = m[hd];
    if (p.second <= curr)
    {
      m[hd].second = curr;
      m[hd].first = root->data;
    }
  }

  // Recur for left subtree
  printBottomViewUtil(root->left, curr + 1, hd - 1, m);

  // Recur for right subtree
  printBottomViewUtil(root->right, curr + 1, hd + 1, m);
}

void printBottomView(Node *root)
{

  // Map to store Horizontal Distance,
  // Height and Data.
  map<int, pair<int, int> > m;

  printBottomViewUtil(root, 0, 0, m);

  // Prints the values stored by printBottomViewUtil()
  map<int, pair<int, int> >::iterator it;
  for (it = m.begin(); it != m.end(); ++it)
  {
    pair<int, int> p = it->second;
    cout << p.first << " ";
  }
}

int main()
{
  Node *root = new Node(20);
  root->left = new Node(8);
  root->right = new Node(22);
  root->left->left = new Node(5);
  root->left->right = new Node(3);
  root->right->left = new Node(4);
  root->right->right = new Node(25);
  root->left->right->left = new Node(10);
  root->left->right->right = new Node(14);
  cout << "Bottom view of the given binary tree :\n";
  printBottomView(root);
  return 0;
}

vector<int> bottomView(Node *root)
{
  vector<int> ans;
  map<int, vector<int> > m;
  queue<pair<Node *, int> > q;
  int dist = 0;
  q.push(make_pair(root, 0));
  while (!q.empty())
  {
    pair<Node *, int> p = q.front();
    Node *curr = p.first;
    dist = p.second;
    m[dist].push_back(curr->data);
    q.pop();
    if (curr->left)
    {
      q.push(make_pair(curr->left, dist - 1));
    }
    if (curr->right)
    {
      q.push(make_pair(curr->right, dist + 1));
    }
  }
  map<int, vector<int> >::iterator itr;
  for (itr = m.begin(); itr != m.end(); itr++)
  {
    ans.push_back(itr->second.back());
  }
  return ans;
}