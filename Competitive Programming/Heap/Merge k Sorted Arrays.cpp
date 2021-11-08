/*
https://practice.geeksforgeeks.org/problems/merge-k-sorted-arrays/1
https://www.geeksforgeeks.org/merge-k-sorted-arrays/

Merge k Sorted Arrays 
Medium Accuracy: 51.19% Submissions: 41264 Points: 4
Given K sorted arrays arranged in the form of a matrix of size K*K. 
The task is to merge them into one sorted array.
Example 1:

Input:
K = 3
arr[][] = {{1,2,3},{4,5,6},{7,8,9}}
Output: 1 2 3 4 5 6 7 8 9
Explanation:Above test case has 3 sorted
arrays of size 3, 3, 3
arr[][] = [[1, 2, 3],[4, 5, 6], 
[7, 8, 9]]
The merged list will be 
[1, 2, 3, 4, 5, 6, 7, 8, 9].
Example 2:

Input:
K = 4
arr[][]={{1,2,3,4}{2,2,3,4},
         {5,5,6,6},{7,8,9,9}}
Output:
1 2 2 2 3 3 4 4 5 5 6 6 7 8 9 9 
Explanation: Above test case has 4 sorted
arrays of size 4, 4, 4, 4
arr[][] = [[1, 2, 2, 2], [3, 3, 4, 4],
[5, 5, 6, 6]  [7, 8, 9, 9 ]]
The merged list will be 
[1, 2, 2, 2, 3, 3, 4, 4, 5, 5, 
6, 6, 7, 8, 9, 9 ].
Your Task:
You do not need to read input or print anything. 
Your task is to complete mergeKArrays() function which takes 2 arguments, 
an arr[K][K] 2D Matrix containing K sorted arrays 
and an integer K denoting the number of sorted arrays, 
as input and returns the merged sorted array 
( as a pointer to the merged sorted arrays in cpp, as an ArrayList in java, and list in python)

Expected Time Complexity: O(K2*Log(K))
Expected Auxiliary Space: O(K)

Constraints:
1 <= K <= 100
*/

class Solution
{
public:
    //Function to merge k sorted arrays.
    vector<int> mergeKArrays(vector<vector<int> > arr, int k)
    {
        //code here

        vector<int> v;
        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < k; j++)
            {
                v.push_back(arr[i][j]);
            }
        }
        sort(v.begin(), v.end());
        return v;
    }
};

/*
https://www.geeksforgeeks.org/merge-k-sorted-arrays-set-2-different-sized-arrays/
Merge k sorted arrays | Set 2 (Different Sized Arrays)
Difficulty Level : Medium
Last Updated : 16 Feb, 2021
Given k sorted arrays of possibly different sizes, merge them and print the sorted output.
Examples: 
 

Input: k = 3 
      arr[][] = { {1, 3},
                  {2, 4, 6},
                  {0, 9, 10, 11}} ;
Output: 0 1 2 3 4 6 9 10 11 

Input: k = 2
      arr[][] = { {1, 3, 20},
                  {2, 4, 6}} ;
Output: 1 2 3 4 6 20 
*/

// C++ program to merge k sorted arrays
// of size n each.
#include <bits/stdc++.h>
using namespace std;

// A pair of pairs, first element is going to
// store value, second element index of array
// and third element index in the array.
typedef pair<int, pair<int, int> > ppi;

// This function takes an array of arrays as an
// argument and all arrays are assumed to be
// sorted. It merges them together and prints
// the final sorted output.
vector<int> mergeKArrays(vector<vector<int> > arr)
{
    vector<int> output;

    // Create a min heap with k heap nodes. Every
    // heap node has first element of an array
    priority_queue<ppi, vector<ppi>, greater<ppi> > pq;

    for (int i = 0; i < arr.size(); i++)
        pq.push({arr[i][0], {i, 0}});

    // Now one by one get the minimum element
    // from min heap and replace it with next
    // element of its array
    while (pq.empty() == false)
    {
        ppi curr = pq.top();
        pq.pop();

        // i ==> Array Number
        // j ==> Index in the array number
        int i = curr.second.first;
        int j = curr.second.second;

        output.push_back(curr.first);

        // The next element belongs to same array as
        // current.
        if (j + 1 < arr[i].size())
            pq.push({arr[i][j + 1], {i, j + 1}});
    }

    return output;
}

// Driver program to test above functions
int main()
{
    // Change n at the top to change number
    // of elements in an array
    vector<vector<int> > arr{{2, 6, 12},
                             {1, 9},
                             {23, 34, 90, 2000}};

    vector<int> output = mergeKArrays(arr);

    cout << "Merged array is " << endl;
    for (auto x : output)
        cout << x << " ";

    return 0;
}