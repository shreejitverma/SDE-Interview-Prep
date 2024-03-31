/*
https://www.geeksforgeeks.org/k-th-largest-sum-contiguous-subarray/
Given an array of integers. Write a program to find the K-th largest sum of contiguous subarray within the array of numbers which has negative and positive numbers.

Examples: 
Input: a[] = {20, -5, -1} 
         k = 3
Output: 14
Explanation: All sum of contiguous 
subarrays are (20, 15, 14, -5, -6, -1) 
so the 3rd largest sum is 14.

Input: a[] = {10, -10, 20, -40} 
         k = 6
Output: -10 
Explanation: The 6th largest sum among 
sum of all contiguous subarrays is -10.
A brute force approach is to store all the contiguous sums in another array and sort it and print the k-th largest. But in the case of the number of elements being large, the array in which we store the contiguous sums will run out of memory as the number of contiguous subarrays will be large (quadratic order)



An efficient approach is to store the pre-sum of the array in a sum[] array. We can find sum of contiguous subarray from index i to j as sum[j]-sum[i-1] 

Now for storing the Kth largest sum, use a min heap (priority queue) in which we push the contiguous sums till we get K elements, once we have our K elements, check if the element is greater than the Kth element it is inserted to the min heap with popping out the top element in the min-heap, else not inserted. In the end, the top element in the min-heap will be your answer.

Below is the implementation of the above approach.  

C++JavaPython3C#

*/

// CPP program to find the k-th largest sum
// of subarray
#include <bits/stdc++.h>
using namespace std;

// function to calculate kth largest element
// in contiguous subarray sum
int kthLargestSum(int arr[], int n, int k)
{
    // array to store predix sums
    int sum[n + 1];
    sum[0] = 0;
    sum[1] = arr[0];
    for (int i = 2; i <= n; i++)
        sum[i] = sum[i - 1] + arr[i - 1];

    // priority_queue of min heap
    priority_queue<int, vector<int>, greater<int> > Q;

    // loop to calculate the contiguous subarray
    // sum position-wise
    for (int i = 1; i <= n; i++)
    {

        // loop to traverse all positions that
        // form contiguous subarray
        for (int j = i; j <= n; j++)
        {
            // calculates the contiguous subarray
            // sum from j to i index
            int x = sum[j] - sum[i - 1];

            // if queue has less then k elements,
            // then simply push it
            if (Q.size() < k)
                Q.push(x);

            else
            {
                // it the min heap has equal to
                // k elements then just check
                // if the largest kth element is
                // smaller than x then insert
                // else its of no use
                if (Q.top() < x)
                {
                    Q.pop();
                    Q.push(x);
                }
            }
        }
    }

    // the top element will be then kth
    // largest element
    return Q.top();
}

// Driver program to test above function
int main()
{
    int a[] = {10, -10, 20, -40};
    int n = sizeof(a) / sizeof(a[0]);
    int k = 6;

    // calls the function to find out the
    // k-th largest sum
    cout << kthLargestSum(a, n, k);
    return 0;
}
/*
Output:

-10
Time complexity: O(n^2 log (k)) 
Auxiliary Space : O(k) for min-heap and we can store the sum array in the array itself as it is of no use.
*/