/*
https://practice.geeksforgeeks.org/problems/longest-alternating-subsequence5951/1
https://www.geeksforgeeks.org/longest-alternating-subsequence/
Longest alternating subsequence 
Medium Accuracy: 59.08% Submissions: 8682 Points: 4
A sequence {x1, x2, .. xn} is alternating sequence if its elements satisfy one of the following relations :
x1 < x2 > x3 < x4 > x5..... or  x1 >x2 < x3 > x4 < x5.....
Your task is to find the longest such sequence.

Example 1:

Input: nums = {1,5,4}
Output: 3
Explanation: The entire sequenece is a 
alternating sequence.
Examplae 2:

Input: nums = {1,17,5,10,13,15,10,5,16,8}
Ooutput: 7
Explanation: There are several subsequences
that achieve this length. 
One is {1,17,10,13,10,16,8}.
 

Your Task:
You don't need to read or print anyhting. Your task is to complete the function AlternatingaMaxLength() which takes the sequence  nums as input parameter and returns the maximum length of alternating sequence.

Expected Time Complexity: O(n)
Expected Space Complexity: O(1)

Constraints:
1 <= n <= 105

This problem is an extension of longest increasing subsequence problem, but requires more thinking 
for finding optimal substructure property in this.
We will solve this problem by dynamic Programming method, Let A is given array of length n of integers. 
We define a 2D array las[n][2] such that las[i][0] contains longest alternating subsequence ending at index i 
and last element is greater than its previous element and 
las[i][1] contains longest alternating subsequence ending at index i 
and last element is smaller than its previous element, then we have following recurrence relation between them,  



las[i][0] = Length of the longest alternating subsequence 
          ending at index i and last element is greater
          than its previous element
las[i][1] = Length of the longest alternating subsequence 
          ending at index i and last element is smaller
          than its previous element

Recursive Formulation:
   las[i][0] = max (las[i][0], las[j][1] + 1); 
             for all j < i and A[j] < A[i] 
   las[i][1] = max (las[i][1], las[j][0] + 1); 
             for all j < i and A[j] > A[i]
The first recurrence relation is based on the fact that, If we are at position i 
and this element has to bigger than its previous element then for this sequence (upto i) to be bigger we will 
try to choose an element j ( < i) such that A[j] < A[i] i.e. A[j] can become A[i]’s previous element 
and las[j][1] + 1 is bigger than las[i][0] then we will update las[i][0]. 
Remember we have chosen las[j][1] + 1 not las[j][0] + 1 to satisfy alternate property because 
in las[j][0] last element is bigger than its previous one and A[i] is greater than A[j] which will 
break the alternating property if we update. So above fact derives first recurrence relation, 
similar argument can be made for second recurrence relation also. 
*/

// C++ program to find longest alternating
// subsequence in an array
#include <iostream>
using namespace std;

// Function to return max of two numbers
int max(int a, int b)
{
    return (a > b) ? a : b;
}

// Function to return longest alternating
// subsequence length
int zzis(int arr[], int n)
{

    /*las[i][0] = Length of the longest
        alternating subsequence ending at
        index i and last element is greater
        than its previous element
    las[i][1] = Length of the longest
        alternating subsequence ending
        at index i and last element is
        smaller than its previous element */
    int las[n][2];

    // Initialize all values from 1
    for (int i = 0; i < n; i++)
        las[i][0] = las[i][1] = 1;

    // Initialize result
    int res = 1;

    // Compute values in bottom up manner
    for (int i = 1; i < n; i++)
    {

        // Consider all elements as
        // previous of arr[i]
        for (int j = 0; j < i; j++)
        {

            // If arr[i] is greater, then
            // check with las[j][1]
            if (arr[j] < arr[i] &&
                las[i][0] < las[j][1] + 1)
                las[i][0] = las[j][1] + 1;

            // If arr[i] is smaller, then
            // check with las[j][0]
            if (arr[j] > arr[i] &&
                las[i][1] < las[j][0] + 1)
                las[i][1] = las[j][0] + 1;
        }

        // Pick maximum of both values at index i
        if (res < max(las[i][0], las[i][1]))
            res = max(las[i][0], las[i][1]);
    }
    return res;
}

// Driver code
int main()
{
    int arr[] = {10, 22, 9, 33,
                 49, 50, 31, 60};
    int n = sizeof(arr) / sizeof(arr[0]);

    cout << "Length of Longest alternating "
         << "subsequence is " << zzis(arr, n);

    return 0;
}

/*
Time Complexity: O(n2) 
Auxiliary Space: O(n)

Efficient Solution:
In the above approach, at any moment we are keeping track of two values 
(Length of the longest alternating subsequence ending at index i, 
and last element is smaller than or greater than previous element), 
for every element on array. To optimise space, we only need to store two variables for element at any index i. 

inc = Length of longest alternative subsequence so far with current value being greater than it’s previous value.
dec = Length of longest alternative subsequence so far with current value being smaller than it’s previous value.
The tricky part of this approach is to update these two values. 

“inc” should be increased, if and only if the last element in the alternative sequence was smaller than it’s previous element.
“dec” should be increased, if and only if the last element in the alternative sequence was greater than it’s previous element.
*/

// C++ program for above approach
#include <bits/stdc++.h>
using namespace std;

// Function for finding
// longest alternating
// subsequence
int LAS(int arr[], int n)
{

    // "inc" and "dec" initialized as 1
    // as single element is still LAS
    int inc = 1;
    int dec = 1;

    // Iterate from second element
    for (int i = 1; i < n; i++)
    {

        if (arr[i] > arr[i - 1])
        {

            // "inc" changes iff "dec"
            // changes
            inc = dec + 1;
        }

        else if (arr[i] < arr[i - 1])
        {

            // "dec" changes iff "inc"
            // changes
            dec = inc + 1;
        }
    }

    // Return the maximum length
    return max(inc, dec);
}

// Driver Code
int main()
{
    int arr[] = {10, 22, 9, 33, 49,
                 50, 31, 60};
    int n = sizeof(arr) / sizeof(arr[0]);

    // Function Call
    cout << LAS(arr, n) << endl;
    return 0;
}
// Output:

// 6
// Time Complexity: O(n)
// Auxiliary Space: O(1)