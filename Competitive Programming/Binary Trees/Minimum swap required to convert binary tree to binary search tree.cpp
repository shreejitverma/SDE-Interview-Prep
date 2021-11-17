/*
https://www.geeksforgeeks.org/minimum-swap-required-convert-binary-tree-binary-search-tree/#:~:text=Given%20the%20array%20representation%20of,it%20into%20Binary%20Search%20Tree.&text=Swap%201%3A%20Swap%20node%208,node%209%20with%20node%2010.
Given the array representation of Complete Binary Tree i.e, if index i is the parent, index 2*i + 1 is the left child and index 2*i + 2 is the right child. The task is to find the minimum number of swap required to convert it into Binary Search Tree.

Examples:  
The idea is to use the fact that inorder traversal of Binary Search Tree is in increasing order of their value. 
So, find the inorder traversal of the Binary Tree and store it in the array and try to sort the array. The minimum number of swap required to get the array sorted will be the answer. Please refer below post to find minimum number of swaps required to get the array sorted.
Minimum number of swaps required to sort an array
Time Complexity: O(n log n).
*/

// C++ program for Minimum swap required
// to convert binary tree to binary search tree
#include <bits/stdc++.h>
using namespace std;

// Inorder Traversal of Binary Tree
void inorder(int a[], std::vector<int> &v,
             int n, int index)
{
    // if index is greater or equal to vector size
    if (index >= n)
        return;
    inorder(a, v, n, 2 * index + 1);

    // push elements in vector
    v.push_back(a[index]);
    inorder(a, v, n, 2 * index + 2);
}

// Function to find minimum swaps to sort an array
int minSwaps(std::vector<int> &v)
{
    std::vector<pair<int, int> > t(v.size());
    int ans = 0;
    for (int i = 0; i < v.size(); i++)
        t[i].first = v[i], t[i].second = i;

    sort(t.begin(), t.end());
    for (int i = 0; i < t.size(); i++)
    {
        // second element is equal to i
        if (i == t[i].second)
            continue;
        else
        {
            // swaping of elements
            swap(t[i].first, t[t[i].second].first);
            swap(t[i].second, t[t[i].second].second);
        }

        // Second is not equal to i
        if (i != t[i].second)
            --i;
        ans++;
    }
    return ans;
}

// Driver code
int main()
{
    int a[] = {5, 6, 7, 8, 9, 10, 11};
    int n = sizeof(a) / sizeof(a[0]);
    std::vector<int> v;
    inorder(a, v, n, 0);
    cout << minSwaps(v) << endl;
}