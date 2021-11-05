/*
https://www.geeksforgeeks.org/count-derangements-permutation-such-that-no-element-appears-in-its-original-position/
A Derangement is a permutation of n elements, such that no element appears in its original position. 
For example, a derangement of {0, 1, 2, 3} is {2, 3, 1, 0}.
Given a number n, find the total number of Derangements of a set of n elements.

Examples : 

Input: n = 2
Output: 1
For two elements say {0, 1}, there is only one 
possible derangement {1, 0}

Input: n = 3
Output: 2
For three elements say {0, 1, 2}, there are two 
possible derangements {2, 0, 1} and {1, 2, 0}

Input: n = 4
Output: 9
For four elements say {0, 1, 2, 3}, there are 9
possible derangements {1, 0, 3, 2} {1, 2, 3, 0}
{1, 3, 0, 2}, {2, 3, 0, 1}, {2, 0, 3, 1}, {2, 3,
1, 0}, {3, 0, 1, 2}, {3, 2, 0, 1} and {3, 2, 1, 0}

Let countDer(n) be count of derangements for n elements. Below is the recursive relation to it.  



countDer(n) = (n - 1) * [countDer(n - 1) + countDer(n - 2)]
How does above recursive relation work? 

There are n – 1 way for element 0 (this explains multiplication with n – 1). 
Let 0 be placed at index i. There are now two possibilities, depending on whether or not element i is placed at 0 in return. 

i is placed at 0: This case is equivalent to solving the problem 
for n-2 elements as two elements have just swapped their positions.
i is not placed at 0: This case is equivalent to solving the problem 
for n-1 elements as now there are n-1 elements, n-1 positions and every element has n-2 choices
Below is the simple solution based on the above recursive formula:
*/

// A Naive Recursive C++ program
// to count derangements
#include <bits/stdc++.h>
using namespace std;

int countDer(int n)
{
    // Base cases
    if (n == 1)
        return 0;
    if (n == 2)
        return 1;

    // countDer(n) = (n-1)[countDer(n-1) + der(n-2)]
    return (n - 1) * (countDer(n - 1) + countDer(n - 2));
}

// Driver Code
int main()
{
    int n = 4;
    cout << "Count of Derangements is "
         << countDer(n);
    return 0;
}

// A Dynamic programming based C++
// program to count derangements
#include <bits/stdc++.h>
using namespace std;

int countDer(int n)
{
    // Create an array to store
    // counts for subproblems
    int der[n + 1] = {0};

    // Base cases
    der[1] = 0;
    der[2] = 1;

    // Fill der[0..n] in bottom up manner
    // using above recursive formula
    for (int i = 3; i <= n; ++i)
        der[i] = (i - 1) * (der[i - 1] +
                            der[i - 2]);

    // Return result for n
    return der[n];
}

// Driver code
int main()
{
    int n = 4;
    cout << "Count of Derangements is "
         << countDer(n);
    return 0;
}

// C++ implementation of the above
// approach

#include <iostream>
using namespace std;

int countDer(int n)
{

    // base case
    if (n == 1 or n == 2)
    {
        return n - 1;
    }

    // Variable for just storing
    // previous values
    int a = 0;
    int b = 1;

    // using above recursive formula
    for (int i = 3; i <= n; ++i)
    {
        int cur = (i - 1) * (a + b);
        a = b;
        b = cur;
    }

    // Return result for n
    return b;
}

// Driver Code
int main()
{

    cout << "Count of Dearrangements is " << countDer(4);
    return 0;
}

// Time Complexity: O(n)
// Auxiliary Space: O(1)