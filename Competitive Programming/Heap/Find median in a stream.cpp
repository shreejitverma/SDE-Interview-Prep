/*
https://practice.geeksforgeeks.org/problems/find-median-in-a-stream-1587115620/1
Find median in a stream 
Hard Accuracy: 51.38% Submissions: 17203 Points: 8
Given an input stream of N integers. The task is to insert these numbers into a new stream and find the median of the stream formed by each insertion of X to the new stream.

Example 1:

Input:
N = 4
X[] = 5,15,1,3
Output:
5
10
5
4
Explanation:Flow in stream : 5, 15, 1, 3 
5 goes to stream --> median 5 (5) 
15 goes to stream --> median 10 (5,15) 
1 goes to stream --> median 5 (5,15,1) 
3 goes to stream --> median 4 (5,15,1 3) 
 

Example 2:

Input:
N = 3
X[] = 5,10,15
Output:
5
7.5
10
Explanation:Flow in stream : 5, 10, 15
5 goes to stream --> median 5 (5) 
10 goes to stream --> median 7.5 (5,10) 
15 goes to stream --> median 10 (5,10,15) 
Your Task:
You are required to complete the class Solution. 
It should have 2 data members to represent 2 heaps. 
It should have the following member functions:
insertHeap() which takes x as input and inserts it into the heap, the function should then call balanceHeaps() to balance the new heap.
balanceHeaps() does not take any arguments. It is supposed to balance the two heaps.
getMedian() does not take any arguments. It should return the current median of the stream.

Expected Time Complexity : O(nlogn)
Expected Auxilliary Space : O(n)
 
Constraints:
1 <= N <= 106
1 <= x <= 106
*/
// { Driver Code Starts
#include <bits/stdc++.h>
using namespace std;

// } Driver Code Ends
class Solution
{
public:
    priority_queue<int> left;
    priority_queue<int, vector<int>, greater<int> > right;
    //Function to insert heap.
    void insertHeap(int &x)
    {
        if (left.empty() || x < left.top())
        {
            left.push(x);
        }
        else
        {
            right.push(x);
        }
        balanceHeaps();
    }

    //Function to balance heaps.
    void balanceHeaps()
    {
        int l = left.size();
        int r = right.size();
        if (l - r > 1)
        {
            right.push(left.top());
            left.pop();
        }
        else if (r - l > 1)
        {
            left.push(right.top());
            right.pop();
        }
    }

    //Function to return Median.
    double getMedian()
    {
        int l = left.size();
        int r = right.size();
        if (l == r)
            return double(left.top() + right.top()) / 2;
        else if (l > r)
            return left.top();
        else
            return right.top();
    }
};

// { Driver Code Starts.

int main()
{
    int n, x;
    int t;
    cin >> t;
    while (t--)
    {
        Solution ob;
        cin >> n;
        for (int i = 1; i <= n; ++i)
        {
            cin >> x;
            ob.insertHeap(x);
            cout << floor(ob.getMedian()) << endl;
        }
    }
    return 0;
} // } Driver Code Ends