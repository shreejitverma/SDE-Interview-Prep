/*
https://www.geeksforgeeks.org/next-greater-element/

Given an array, print the Next Greater Element (NGE) for every element. 
The Next greater Element for an element x is the first greater element on the right side of x in the array. 
Elements for which no greater element exist, consider the next greater element as -1. 

Examples: 
For an array, the rightmost element always has the next greater element as -1.
For an array that is sorted in decreasing order, all elements have the next greater element as -1.
For the input array [4, 5, 2, 25], the next greater elements for each element are as follows.
Element       NGE
   4      -->   5
   5      -->   25
   2      -->   25
   25     -->   -1
d) For the input array [13, 7, 6, 12}, the next greater elements for each element are as follows.  



Method 1 (Simple) 
Use two loops: The outer loop picks all the elements one by one. 
The inner loop looks for the first greater element for the element picked by the outer loop. 
If a greater element is found then that element is printed as next, otherwise, -1 is printed.

Below is the implementation of the above approach:


*/
class Solution
{
public:
    //Function to find the next greater element for each element of the array.
    vector<long long> nextLargerElement(vector<long long> arr, int n)
    {
        // Your code here
        vector<long long int> aux;
        stack<long long int> s;
        for (int i = n - 1; i >= 0; i--)
        {
            if (!s.empty())
            {
                while (!s.empty() && s.top() <= arr[i])
                {
                    s.pop();
                }
            }
            (s.empty()) ? aux.push_back(-1) : aux.push_back(s.top());
            s.push(arr[i]);
        }
        for (int i = 0; i < n / 2; i++)
        {
            swap(aux[i], aux[n - i - 1]);
        }
        return aux;
    }
};

// Simple C++ program to print
// next greater elements in a
// given array
#include <iostream>
using namespace std;

/* prints element and NGE pair
for all elements of arr[] of size n */
void printNGE(int arr[], int n)
{
    int next, i, j;
    for (i = 0; i < n; i++)
    {
        next = -1;
        for (j = i + 1; j < n; j++)
        {
            if (arr[i] < arr[j])
            {
                next = arr[j];
                break;
            }
        }
        cout << arr[i] << " -- "
             << next << endl;
    }
}

// Driver Code
int main()
{
    int arr[] = {11, 13, 21, 3};
    int n = sizeof(arr) / sizeof(arr[0]);
    printNGE(arr, n);
    return 0;
}

/*
Time Complexity: O(N2) 
Auxiliary Space: O(1)
 

Method 2 (Using Stack) 

Push the first element to stack.
Pick rest of the elements one by one and follow the following steps in loop. 
Mark the current element as next.
If stack is not empty, compare top element of stack with next.
If next is greater than the top element, Pop element from stack. 
next is the next greater element for the popped element.
Keep popping from the stack while the popped element is smaller than next. 
next becomes the next greater element for all such popped elements.
Finally, push the next in the stack.
After the loop in step 2 is over, pop all the elements from the stack and print -1 as the next element for them.
*/

// A Stack based C++ program to find next
// greater element for all array elements
// in same order as input.
#include <bits/stdc++.h>

using namespace std;

/* prints element and res pair for all
elements of arr[] of size n */
void printNGE(int arr[], int n)
{
    stack<int> s;
    int res[n];
    for (int i = n - 1; i >= 0; i--)
    {
        /* if stack is not empty, then
        pop an element from stack.
        If the popped element is smaller
        than next, then
        a) print the pair
        b) keep popping while elements are
        smaller and stack is not empty */
        if (!s.empty())
        {
            while (!s.empty() && s.top() <= arr[i])
            {
                s.pop();
            }
        }
        res[i] = s.empty() ? -1 : s.top();
        s.push(arr[i]);
    }
    for (int i = 0; i < n; i++)
        cout << arr[i] << " --> " << res[i] << endl;
}
// Driver Code
int main()
{
    int arr[] = {11, 13, 21, 3};
    int n = sizeof(arr) / sizeof(arr[0]);

    // Function call
    printNGE(arr, n);
    return 0;
}