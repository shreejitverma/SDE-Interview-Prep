/*
https://www.geeksforgeeks.org/replace-every-element-with-the-least-greater-element-on-its-right/
Given an array of integers, replace every element with the least greater element on its right side in the array. If there are no greater elements on the right side, replace it with -1.

Examples: 
Input: [8, 58, 71, 18, 31, 32, 63, 92, 
         43, 3, 91, 93, 25, 80, 28]
Output: [18, 63, 80, 25, 32, 43, 80, 93, 
         80, 25, 93, -1, 28, -1, -1]
Recommended: Please try your approach on {IDE} first, before moving on to the solution.
A naive method is to run two loops. The outer loop will one by one pick array elements from left to right. The inner loop will find the smallest element greater than the picked element on its right side. Finally, the outer loop will replace the picked element with the element found by inner loop. The time complexity of this method will be O(n2).



A tricky solution would be to use Binary Search Trees. We start scanning the array from right to left and insert each element into the BST. For each inserted element, we replace it in the array by its inorder successor in BST. If the element inserted is the maximum so far (i.e. its inorder successor doesn’t exist), we replace it by -1.

Below is the implementation of the above idea – 
*/

// C++ program to replace every element with the
// least greater element on its right
#include <bits/stdc++.h>
using namespace std;

// A binary Tree node
struct Node
{
    int data;
    Node *left, *right;
};

// A utility function to create a new BST node
Node *newNode(int item)
{
    Node *temp = new Node;
    temp->data = item;
    temp->left = temp->right = NULL;

    return temp;
}

/* A utility function to insert a new node with
   given data in BST and find its successor */
Node *insert(Node *node, int data, Node *&succ)
{

    /* If the tree is empty, return a new node */
    if (node == NULL)
        return node = newNode(data);

    // If key is smaller than root's key, go to left
    // subtree and set successor as current node
    if (data < node->data)
    {
        succ = node;
        node->left = insert(node->left, data, succ);
    }

    // go to right subtree
    else if (data > node->data)
        node->right = insert(node->right, data, succ);
    return node;
}

// Function to replace every element with the
// least greater element on its right
void replace(int arr[], int n)
{
    Node *root = NULL;

    // start from right to left
    for (int i = n - 1; i >= 0; i--)
    {
        Node *succ = NULL;

        // insert current element into BST and
        // find its inorder successor
        root = insert(root, arr[i], succ);

        // replace element by its inorder
        // successor in BST
        if (succ)
            arr[i] = succ->data;
        else // No inorder successor
            arr[i] = -1;
    }
}

// Driver Program to test above functions
int main()
{
    int arr[] = {8, 58, 71, 18, 31, 32, 63, 92,
                 43, 3, 91, 93, 25, 80, 28};
    int n = sizeof(arr) / sizeof(arr[0]);

    replace(arr, n);

    for (int i = 0; i < n; i++)
        cout << arr[i] << " ";

    return 0;
}

/*
Output
18 63 80 25 32 43 80 93 80 25 93 -1 28 -1 -1 
The worst-case time complexity of the above solution is also O(n2) as it uses BST. The worst-case will happen when array is sorted in ascending or descending order. The complexity can easily be reduced to O(nlogn) by using balanced trees like red-black trees.

Another Approach:

We can use the Next Greater Element using stack algorithm to solve this problem in O(Nlog(N)) time and O(N) space.

Algorithm:

First, we take an array of pairs namely temp, and store each element and its index in this array,i.e. temp[i] will be storing {arr[i],i}.
Sort the array according to the array elements.
Now get the next greater index for each and every index of the temp array in an array namely index by using Next Greater Element using stack.
Now index[i] stores the index of the next least greater element of the element temp[i].first and if index[i] is -1, then it means that there is no least greater element of the element temp[i].second at its right side.
Now take a result array where result[i] will be equal to a[indexes[temp[i].second]] if index[i] is not -1 otherwise result[i] will be equal to -1.
Below is the implementation of the above approach
*/
#include <bits/stdc++.h>
using namespace std;
// function to get the next least greater index for each and
// every temp.second of the temp array using stack this
// function is similar to the Next Greater element for each
// and every element of an array using stack difference is
// we are finding the next greater index not value and the
// indexes are stored in the temp[i].second for all i
vector<int> nextGreaterIndex(vector<pair<int, int> > &temp)
{
    int n = temp.size();
    // initially result[i] for all i is -1
    vector<int> res(n, -1);
    stack<int> stack;
    for (int i = 0; i < n; i++)
    {
        // if the stack is empty or this index is smaller
        // than the index stored at top of the stack then we
        // push this index to the stack
        if (stack.empty() || temp[i].second < stack.top())
            stack.push(
                temp[i].second); // notice temp[i].second is
                                 // the index
        // else this index (i.e. temp[i].second) is greater
        // than the index stored at top of the stack we pop
        // all the indexes stored at stack's top and for all
        // these indexes we make this index i.e.
        // temp[i].second as their next greater index
        else
        {
            while (!stack.empty() && temp[i].second > stack.top())
            {
                res[stack.top()] = temp[i].second;
                stack.pop();
            }
            // after that push the current index to the stack
            stack.push(temp[i].second);
        }
    }
    // now res will store the next least greater indexes for
    // each and every indexes stored at temp[i].second for
    // all i
    return res;
}
// now we will be using above function for finding the next
// greater index for each and every indexes stored at
// temp[i].second
vector<int> replaceByLeastGreaterUsingStack(int arr[],
                                            int n)
{
    // first of all in temp we store the pairs of {arr[i].i}
    vector<pair<int, int> > temp;
    for (int i = 0; i < n; i++)
    {
        temp.push_back({arr[i], i});
    }
    // we sort the temp according to the first of the pair
    // i.e value
    sort(temp.begin(), temp.end());
    // now indexes vector will store the next greater index
    // for each temp[i].second index
    vector<int> indexes = nextGreaterIndex(temp);
    // we initialize a result vector with all -1
    vector<int> res(n, -1);
    for (int i = 0; i < n; i++)
    {
        // now if there is no next greater index after the
        // index temp[i].second the result will be -1
        // otherwise the result will be the element of the
        // array arr at index indexes[temp[i].second]
        if (indexes[temp[i].second] != -1)
            res[temp[i].second] = arr[indexes[temp[i].second]];
    }
    // return the res which will store the least greater
    // element of each and every element in the array at its
    // right side
    return res;
}
// driver code
int main()
{
    int arr[] = {8, 58, 71, 18, 31, 32, 63, 92,
                 43, 3, 91, 93, 25, 80, 28};
    int n = sizeof(arr) / sizeof(int);
    auto res = replaceByLeastGreaterUsingStack(arr, n);
    cout << "Least Greater elements on the right side are "
         << endl;
    for (int i : res)
        cout << i << ' ';
    cout << endl;
    return 0;
}