/*
https://www.geeksforgeeks.org/sliding-window-maximum-maximum-of-all-subarrays-of-size-k/
Given an array and an integer K, find the maximum for each and every contiguous subarray of size k.

Input: arr[] = {1, 2, 3, 1, 4, 5, 2, 3, 6}, K = 3 
Output: 3 3 4 5 5 5 6
Explanation: 
Maximum of 1, 2, 3 is 3
Maximum of 2, 3, 1 is 3
Maximum of 3, 1, 4 is 4
Maximum of 1, 4, 5 is 5
Maximum of 4, 5, 2 is 5 
Maximum of 5, 2, 3 is 5
Maximum of 2, 3, 6 is 6

Input: arr[] = {8, 5, 10, 7, 9, 4, 15, 12, 90, 13}, K = 4 
Output: 10 10 10 15 15 90 90
Explanation:
Maximum of first 4 elements is 10, similarly for next 4 
elements (i.e from index 1 to 4) is 10, So the sequence 
generated is 10 10 10 15 15 90 90


Method 1: This is a simple method to solve the above problem.



Approach: 
The idea is very basic run a nested loop, 
the outer loop which will mark the starting point of the subarray of length k, 
the inner loop will run from the starting index to index+k, 
k elements from starting index and print the maximum element among these k elements. 

Algorithm: 

Create a nested loop, the outer loop from starting index to n – k th elements. 
The inner loop will run for k iterations.
Create a variable to store the maximum of k elements traversed by the inner loop.
Find the maximum of k elements traversed by the inner loop.
Print the maximum element in every iteration of outer loop
Implementation: 

*/

// C++ Program to find the maximum for
// each and every contiguous subarray of size k.
#include <bits/stdc++.h>
using namespace std;

// Method to find the maximum for each
// and every contiguous subarray of size k.
void printKMax(int arr[], int n, int k)
{
    int j, max;

    for (int i = 0; i <= n - k; i++)
    {
        max = arr[i];

        for (j = 1; j < k; j++)
        {
            if (arr[i + j] > max)
                max = arr[i + j];
        }
        cout << max << " ";
    }
}

// Driver code
int main()
{
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int n = sizeof(arr) / sizeof(arr[0]);
    int k = 3;
    printKMax(arr, n, k);
    return 0;
}

/*
Output
3 4 5 6 7 8 9 10 
Complexity Analysis: 

Time Complexity: O(N * K). 
The outer loop runs n-k+1 times and the inner loop runs k times for every iteration of outer loop. 
So time complexity is O((n-k+1)*k) which can also be written as O(N * K).
Space Complexity: O(1). 
No extra space is required.
Method 2: This method uses the Self-Balancing BST to solve the given problem.

Approach: 
To find maximum among k elements of the subarray the previous method uses a loop traversing through the elements. 
To reduce that time the idea is to use an AVL tree which returns the maximum element in log n time. 
So, traverse through the array and keep k elements in the BST and print the maximum in every iteration. 
AVL tree is a suitable data structure as lookup, insertion, 
and deletion all take O(log n) time in both the average and worst cases, 
where n is the number of nodes in the tree prior to the operation. 

Algorithm: 



Create a Self-balancing BST (AVL tree) to store and find the maximum element.
Traverse through the array from start to end.
Insert the element in the AVL tree.
If the loop counter is greater than or equal to k then delete i-k th element from the BST
Print the maximum element of the BST.
Implementation: 
*/

// C++ program to delete a node from AVL Tree
#include <bits/stdc++.h>
using namespace std;

// An AVL tree node
class Node
{
public:
    int key;
    Node *left;
    Node *right;
    int height;
};

// A utility function to get maximum
// of two integers
int max(int a, int b);

// A utility function to get height
// of the tree
int height(Node *N)
{
    if (N == NULL)
        return 0;
    return N->height;
}

// A utility function to get maximum
// of two integers
int max(int a, int b)
{
    return (a > b) ? a : b;
}

/* Helper function that allocates a
new node with the given key and
NULL left and right pointers. */
Node *newNode(int key)
{
    Node *node = new Node();
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    node->height = 1; // new node is initially
                      // added at leaf
    return (node);
}

// A utility function to right
// rotate subtree rooted with y
// See the diagram given above.
Node *rightRotate(Node *y)
{
    Node *x = y->left;
    Node *T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = max(height(y->left),
                    height(y->right)) +
                1;
    x->height = max(height(x->left),
                    height(x->right)) +
                1;

    // Return new root
    return x;
}

// A utility function to left
// rotate subtree rooted with x
// See the diagram given above.
Node *leftRotate(Node *x)
{
    Node *y = x->right;
    Node *T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    x->height = max(height(x->left),
                    height(x->right)) +
                1;
    y->height = max(height(y->left),
                    height(y->right)) +
                1;

    // Return new root
    return y;
}

// Get Balance factor of node N
int getBalance(Node *N)
{
    if (N == NULL)
        return 0;
    return height(N->left) -
           height(N->right);
}

Node *insert(Node *node, int key)
{
    /* 1. Perform the normal BST rotation */
    if (node == NULL)
        return (newNode(key));

    if (key < node->key)
        node->left = insert(node->left, key);
    else if (key > node->key)
        node->right = insert(node->right, key);
    else // Equal keys not allowed
        return node;

    /* 2. Update height of this ancestor node */
    node->height = 1 + max(height(node->left),
                           height(node->right));

    /* 3. Get the balance factor of this
        ancestor node to check whether
        this node became unbalanced */
    int balance = getBalance(node);

    // If this node becomes unbalanced,
    // then there are 4 cases

    // Left Left Case
    if (balance > 1 && key < node->left->key)
        return rightRotate(node);

    // Right Right Case
    if (balance < -1 && key > node->right->key)
        return leftRotate(node);

    // Left Right Case
    if (balance > 1 && key > node->left->key)
    {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && key < node->right->key)
    {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    /* return the (unchanged) node pointer */
    return node;
}

/* Given a non-empty binary search tree,
return the node with minimum key value
found in that tree. Note that the entire
tree does not need to be searched. */
Node *minValueNode(Node *node)
{
    Node *current = node;

    /* loop down to find the leftmost leaf */
    while (current->left != NULL)
        current = current->left;

    return current;
}

// Recursive function to delete a node
// with given key from subtree with
// given root. It returns root of the
// modified subtree.
Node *deleteNode(Node *root, int key)
{

    // STEP 1: PERFORM STANDARD BST DELETE
    if (root == NULL)
        return root;

    // If the key to be deleted is smaller
    // than the root's key, then it lies
    // in left subtree
    if (key < root->key)
        root->left = deleteNode(root->left, key);

    // If the key to be deleted is greater
    // than the root's key, then it lies
    // in right subtree
    else if (key > root->key)
        root->right = deleteNode(root->right, key);

    // if key is same as root's key, then
    // This is the node to be deleted
    else
    {
        // node with only one child or no child
        if ((root->left == NULL) ||
            (root->right == NULL))
        {
            Node *temp = root->left ? root->left : root->right;

            // No child case
            if (temp == NULL)
            {
                temp = root;
                root = NULL;
            }
            else               // One child case
                *root = *temp; // Copy the contents of
                               // the non-empty child
            free(temp);
        }
        else
        {
            // node with two children: Get the inorder
            // successor (smallest in the right subtree)
            Node *temp = minValueNode(root->right);

            // Copy the inorder successor's
            // data to this node
            root->key = temp->key;

            // Delete the inorder successor
            root->right = deleteNode(root->right,
                                     temp->key);
        }
    }

    // If the tree had only one node
    // then return
    if (root == NULL)
        return root;

    // STEP 2: UPDATE HEIGHT OF THE CURRENT NODE
    root->height = 1 + max(height(root->left),
                           height(root->right));

    // STEP 3: GET THE BALANCE FACTOR OF
    // THIS NODE (to check whether this
    // node became unbalanced)
    int balance = getBalance(root);

    // If this node becomes unbalanced,
    // then there are 4 cases

    // Left Left Case
    if (balance > 1 &&
        getBalance(root->left) >= 0)
        return rightRotate(root);

    // Left Right Case
    if (balance > 1 &&
        getBalance(root->left) < 0)
    {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    // Right Right Case
    if (balance < -1 &&
        getBalance(root->right) <= 0)
        return leftRotate(root);

    // Right Left Case
    if (balance < -1 &&
        getBalance(root->right) > 0)
    {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

// A utility function to print preorder
// traversal of the tree.
// The function also prints height
// of every node
void preOrder(Node *root)
{
    if (root != NULL)
    {
        cout << root->key << " ";
        preOrder(root->left);
        preOrder(root->right);
    }
}

// Returns maximum value in a given
// Binary Tree
int findMax(Node *root)
{
    // Base case
    if (root == NULL)
        return INT_MIN;

    // Return maximum of 3 values:
    // 1) Root's data 2) Max in Left Subtree
    // 3) Max in right subtree
    int res = root->key;
    int lres = findMax(root->left);
    int rres = findMax(root->right);
    if (lres > res)
        res = lres;
    if (rres > res)
        res = rres;
    return res;
}

// Method to find the maximum for each
// and every contiguous subarray of size k.
void printKMax(int arr[], int n, int k)
{
    int c = 0, l = 0;
    Node *root = NULL;

    //traverse the array ;
    for (int i = 0; i < n; i++)
    {
        c++;
        //insert the element in BST
        root = insert(root, arr[i]);

        //size of subarray greater than k
        if (c > k)
        {
            root = deleteNode(root, arr[l++]);
            c--;
        }

        //size of subarray equal to k
        if (c == k)
        {
            cout << findMax(root) << " ";
        }
    }
}
// Driver code
int main()
{
    int arr[] = {8, 5, 10, 7, 9, 4, 15, 12, 90, 13}, k = 4;
    int n = sizeof(arr) / sizeof(arr[0]);
    printKMax(arr, n, k);
    return 0;
}

/*
Output
10 10 10 15 15 90 90 
Complexity Analysis:  

Time Complexity: O(N * Log k). 
Insertion, deletion and search takes log k time in a AVL tree. So the overall time Complexity is O(N * log k)
Space Complexity: O(k). 
The space required to store k elements in a BST is O(k).
Method 3: This method uses Deque to solve the above problem

Approach: 
Create a Deque, Qi of capacity k, 
that stores only useful elements of current window of k elements. 
An element is useful if it is in current window and 
is greater than all other elements on right side of it in current window. 
Process all array elements one by one and maintain Qi 
to contain useful elements of current window and these useful elements are maintained in sorted order. 
The element at front of the Qi is the largest and element at rear/back of Qi is the smallest of current window. 
Thanks to Aashish for suggesting this method.

Dry run of the above approach: 



Algorithm:  

Create a deque to store k elements.
Run a loop and insert first k elements in the deque. Before inserting the element, 
check if the element at the back of the queue is smaller than the current element, 
if it is so remove the element from the back of the deque, 
until all elements left in the deque are greater than the current element. 
Then insert the current element, at the back of the deque.
Now, run a loop from k to end of the array.
Print the front element of the deque.
Remove the element from the front of the queue if they are out of the current window.
Insert the next element in the deque. Before inserting the element, 
check if the element at the back of the queue is smaller than the current element, 
if it is so remove the element from the back of the deque, 
until all elements left in the deque are greater than the current element. 
Then insert the current element, at the back of the deque.
Print the maximum element of the last window.
Implementation: 
*/

// CPP program for the above approach
#include <bits/stdc++.h>
using namespace std;

// A Dequeue (Double ended queue) based
// method for printing maximum element of
// all subarrays of size k
void printKMax(int arr[], int n, int k)
{

    // Create a Double Ended Queue,
    // Qi that will store indexes
    // of array elements
    // The queue will store indexes
    // of useful elements in every
    // window and it will
    // maintain decreasing order of
    // values from front to rear in Qi, i.e.,
    // arr[Qi.front[]] to arr[Qi.rear()]
    // are sorted in decreasing order
    std::deque<int> Qi(k);

    /* Process first k (or first window)
     elements of array */
    int i;
    for (i = 0; i < k; ++i)
    {

        // For every element, the previous
        // smaller elements are useless so
        // remove them from Qi
        while ((!Qi.empty()) && arr[i] >=
                                    arr[Qi.back()])

            // Remove from rear
            Qi.pop_back();

        // Add new element at rear of queue
        Qi.push_back(i);
    }

    // Process rest of the elements,
    // i.e., from arr[k] to arr[n-1]
    for (; i < n; ++i)
    {

        // The element at the front of
        // the queue is the largest element of
        // previous window, so print it
        cout << arr[Qi.front()] << " ";

        // Remove the elements which
        // are out of this window
        while ((!Qi.empty()) && Qi.front() <=
                                    i - k)

            // Remove from front of queue
            Qi.pop_front();

        // Remove all elements
        // smaller than the currently
        // being added element (remove
        // useless elements)
        while ((!Qi.empty()) && arr[i] >=
                                    arr[Qi.back()])
            Qi.pop_back();

        // Add current element at the rear of Qi
        Qi.push_back(i);
    }

    // Print the maximum element
    // of last window
    cout << arr[Qi.front()];
}

// Driver code
int main()
{
    int arr[] = {12, 1, 78, 90, 57, 89, 56};
    int n = sizeof(arr) / sizeof(arr[0]);
    int k = 3;
    printKMax(arr, n, k);
    return 0;
}

/*
Output

78 90 90 90 89
Complexity Analysis: 

Time Complexity: O(n). 
It seems more than O(n) at first look. 
It can be observed that every element of array is added and removed at most once. 
So there are total 2n operations.
Auxiliary Space: O(k). 
Elements stored in the dequeue take O(k) space.
Below is an extension of this problem: 
Sum of minimum and maximum elements of all subarrays of size k.

Method 4: This method is modification in queue implementation of two stack:

Algorithm :

While pushing the element, we constantly push in stack 2. 
The maximum of stack 2 will always be the maximum of the top element of stack 2.
While popping, we will always pop from stack 1, 
and if stack 1 is empty then we shall push every element of stack 2 to stack 1 and updating the maximum
The above two-step are followed in queue implementation of stack
Now to find the maximum of the whole queue (Same as both stack), 
we will take the top element of both stack maximum; hence this is the maximum of the whole queue.
Now, this technique can be used to slide window and get maximum.
while sliding window by 1 index delete the last one, insert the new one and then take a maximum of both stack
Implementation
*/

#include <bits/stdc++.h>
using namespace std;

struct node
{
    int data;
    int maximum;
};

// it is a modification  in the way of implementation of queue using two stack

void insert(stack<node> &s2, int val)
{
    //inserting the element in s2
    node other;
    other.data = val;

    if (s2.empty())
        other.maximum = val;
    else
    {
        node front = s2.top();
        //updating maximum in that stack push it
        other.maximum = max(val, front.maximum);
    }
    s2.push(other);
    return;
}

void delet(stack<node> &s1, stack<node> &s2)
{
    //if s1 is not empty directly pop
    //else we have to push all element from s2 and thatn pop from s1
    //while pushing from s2 to s1 update maximum variable in s1
    if (s1.size())
        s1.pop();
    else
    {
        while (!s2.empty())
        {
            node val = s2.top();
            insert(s1, val.data);
            s2.pop();
        }
        s1.pop();
    }
}

int get_max(stack<node> &s1, stack<node> &s2)
{
    // the maximum of both stack will be the maximum of overall window
    int ans = -1;
    if (s1.size())
        ans = max(ans, s1.top().maximum);
    if (s2.size())
        ans = max(ans, s2.top().maximum);
    return ans;
}

vector<int> slidingMaximum(int a[], int b, int n)
{
    //s2 for push
    //s1 for pop
    vector<int> ans;
    stack<node> s1, s2;

    //shifting all value except the last one if first window
    for (int i = 0; i < b - 1; i++)
        insert(s2, a[i]);

    for (int i = 0; i <= n - b; i++)
    {
        //removing the last element of previous window as window has shift by one
        if (i - 1 >= 0)
            delet(s1, s2);

        //adding the new element to the window as the window is shift by one
        insert(s2, a[i + b - 1]);

        ans.push_back(get_max(s1, s2));
    }
    return ans;
}

int main()
{
    int arr[] = {8, 5, 10, 7, 9, 4, 15, 12, 90, 13};
    int n = sizeof(arr) / sizeof(arr[0]);
    int k = 4;
    vector<int> ans = slidingMaximum(arr, k, n);
    for (auto x : ans)
        cout << x << " ";
    return 0;
}

/*
Output
78 90 90 90 89 
Complexity Analysis: 

Time Complexity: O(N): This is because every element will just two types push and pop; hence time complexity is linear.
Space Complexity: O(K): This is because at any moment, 
the sum of stack size of both stacks will exactly equal to K, 
As every time we pop exactly one element and push exactly One.
Method 5: This method uses Max-Heap to solve the above problem.

Approach: 
In the above-mentioned methods, one of them was using AVL tree. 
This approach is very similar to that approach. 
The difference is that instead of using the AVL tree, 
Max-Heap will be used in this approach. 
The elements of the current window will be stored in the Max-Heap 
and the maximum element or the root will be printed in each iteration. 
Max-heap is a suitable data structure as it provides constant-time retrieval 
and logarithmic time removal of both the minimum and maximum elements in it, i.e. 
it takes constant time to find the maximum element, and insertion and deletion takes log n time.  

Algorithm:

Pick first k elements and create a max heap of size k.
Perform heapify and print the root element.
Store the next and last element from the array
Run a loop from k – 1 to n 
Replace the value of element which is got out of the window with new element which came inside the window.
Perform heapify.
Print the root of the Heap.
*/