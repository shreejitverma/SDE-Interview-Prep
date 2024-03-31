/*
https://practice.geeksforgeeks.org/problems/egg-dropping-puzzle-1587115620/1
https://www.geeksforgeeks.org/egg-dropping-puzzle-dp-11/
Egg Dropping Puzzle 
Medium Accuracy: 54.38% Submissions: 30023 Points: 4
You are given N identical eggs and you have access to a K-floored building from 1 to K.

There exists a floor f where 0 <= f <= K such that any egg dropped at a floor higher than f will break, and any egg dropped at or below floor f will not break. There are few rules given below. 

An egg that survives a fall can be used again.
A broken egg must be discarded.
The effect of a fall is the same for all eggs.
If the egg doesn't break at a certain floor, it will not break at any floor below.
If the eggs breaks at a certain floor, it will break at any floor above.
Return the minimum number of moves that you need to determine with certainty what the value of f is.

For more description on this problem see wiki page

Example 1:

Input:
N = 1, K = 2
Output: 2
Explanation: 
1. Drop the egg from floor 1. If it 
   breaks, we know that f = 0.
2. Otherwise, drop the egg from floor 2.
   If it breaks, we know that f = 1.
3. If it does not break, then we know f = 2.
4. Hence, we need at minimum 2 moves to
   determine with certainty what the value of f is.
Example 2:

Input:
N = 2, K = 10
Output: 4
Your Task:
Complete the function eggDrop() which takes two positive integer N and K as input parameters and returns the minimum number of attempts you need in order to find the critical floor.

Expected Time Complexity : O(N*K)
Expected Auxiliary Space: O(N*K)

Constraints:
1<=N<=200
1<=K<=200
*/

// A Dynamic Programming based for
// the Egg Dropping Puzzle
#include <bits/stdc++.h>
using namespace std;

// A utility function to get
// maximum of two integers
int max(int a, int b)
{
    return (a > b) ? a : b;
}

/* Function to get minimum
number of trials needed in worst
case with n eggs and k floors */
int eggDrop(int n, int k)
{
    /* A 2D table where entry
    eggFloor[i][j] will represent
    minimum number of trials needed for
    i eggs and j floors. */
    int eggFloor[n + 1][k + 1];
    int res;
    int i, j, x;

    // We need one trial for one floor and 0
    // trials for 0 floors
    for (i = 1; i <= n; i++)
    {
        eggFloor[i][1] = 1;
        eggFloor[i][0] = 0;
    }

    // We always need j trials for one egg
    // and j floors.
    for (j = 1; j <= k; j++)
        eggFloor[1][j] = j;

    // Fill rest of the entries in table using
    // optimal substructure property
    for (i = 2; i <= n; i++)
    {
        for (j = 2; j <= k; j++)
        {
            eggFloor[i][j] = INT_MAX;
            for (x = 1; x <= j; x++)
            {
                res = 1 + max(
                              eggFloor[i - 1][x - 1],
                              eggFloor[i][j - x]);
                if (res < eggFloor[i][j])
                    eggFloor[i][j] = res;
            }
        }
    }

    // eggFloor[n][k] holds the result
    return eggFloor[n][k];
}

/* Driver program to test to pront printDups*/
int main()
{
    int n = 2, k = 36;
    cout << "\nMinimum number of trials "
            "in worst case with "
         << n << " eggs and " << k << " floors is " << eggDrop(n, k);
    return 0;
}

// C++ program to find minimum
// number of trials in worst case.
#include <bits/stdc++.h>

using namespace std;

// Find sum of binomial coefficients xCi
// (where i varies from 1 to n).
int binomialCoeff(int x, int n, int k)
{
    int sum = 0, term = 1;
    for (int i = 1; i <= n; ++i)
    {
        term *= x - i + 1;
        term /= i;
        sum += term;
        if (sum > k)
            return sum;
    }
    return sum;
}
/*
How many floors we can cover with x trials? 
When we drop an egg, two cases arise. 

If egg breaks, then we are left with x-1 trials and n-1 eggs.
If egg does not break, then we are left with x-1 trials and n eggs
Let maxFloors(x, n) be the maximum number of floors 
that we can cover with x trials and n eggs. From above 
two cases, we can write.

maxFloors(x, n) = maxFloors(x-1, n-1) + maxFloors(x-1, n) + 1
For all x >= 1 and n >= 1

Base cases : 
We can't cover any floor with 0 trials or 0 eggs
maxFloors(0, n) = 0
maxFloors(x, 0) = 0

Since we need to cover k floors, 
maxFloors(x, n) >= k           ----------(1)

The above recurrence simplifies to following,
Refer this for proof.

maxFloors(x, n) = &Sum;xCi
                  1 <= i <= n   ----------(2)
Here C represents Binomial Coefficient.

From above two equations, we can say.
&Sum;xCj  >= k
1 <= i <= n
Basically we need to find minimum value of x
that satisfies above inequality. We can find
such x using Binary Search.*/
// Do binary search to find minimum
// number of trials in worst case.
int minTrials(int n, int k)
{
    // Initialize low and high as 1st
    // and last floors
    int low = 1, high = k;

    // Do binary search, for every mid,
    // find sum of binomial coefficients and
    // check if the sum is greater than k or not.
    while (low < high)
    {
        int mid = (low + high) / 2;
        if (binomialCoeff(mid, n, k) < k)
            low = mid + 1;
        else
            high = mid;
    }

    return low;
}

/* Driver code*/
int main()
{
    cout << minTrials(2, 10);
    return 0;
}