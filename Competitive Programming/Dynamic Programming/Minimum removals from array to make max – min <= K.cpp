/*
https://www.geeksforgeeks.org/minimum-removals-array-make-max-min-k/
Given N integers and K, find the minimum number of elements that should be removed, such that Amax-Amin<=K. 
After the removal of elements, Amax and Amin is considered among the remaining elements. 

Input : a[] = {1, 3, 4, 9, 10, 11, 12, 17, 20} 
          k = 4 
Output : 5 
Explanation: Remove 1, 3, 4 from beginning 
and 17, 20 from the end.

Input : a[] = {1, 5, 6, 2, 8}  K=2
Output : 3
Explanation: There are multiple ways to 
remove elements in this case.
One among them is to remove 5, 6, 8.
The other is to remove 1, 2, 5
*/

/*
Approach:

The solution could be further optimized. 
The idea is to sort the array in increasing order and traverse through all the elements (let’s say index j) 
and find the minimum element on its left (index i) such that arr[j] – arr[i] <= k and store it in dp[j].
Thus, the number of elements to be removed becomes n-(j-i+1). 
The minimum number of elements can be found by taking the minimum of n-(j-i-1) overall j. 
The value of index i can be found through its previous dp array element value.
Below is the implementation of the approach:
*/

// C++ program for the above approach
#include <bits/stdc++.h>
using namespace std;

// To sort the array and return the answer
int removals(int arr[], int n, int k)
{

    // sort the array
    sort(arr, arr + n);
    int dp[n];

    // fill all stated with -1
    // when only one element
    for (int i = 0; i < n; i++)
        dp[i] = -1;

    // as dp[0] = 0 (base case) so min
    // no of elements to be removed are
    // n-1 elements
    int ans = n - 1;
    dp[0] = 0;
    for (int i = 1; i < n; i++)
    {
        dp[i] = i;
        int j = dp[i - 1];
        while (j != i && arr[i] - arr[j] > k)
        {
            j++;
        }
        dp[i] = min(dp[i], j);
        ans = min(ans, (n - (i - j + 1)));
    }
    return ans;
}

// Driver code
int main()
{
    int a[] = {1, 3, 4, 9, 10, 11, 12, 17, 20};
    int n = sizeof(a) / sizeof(a[0]);
    int k = 4;
    cout << removals(a, n, k);
    return 0;
}
/*
Time Complexity: O(nlog n). As outer loop is going to make n iterations. 
And the inner loop iterates at max n times for all outer iterations. 
Because we start value of j from dp[i-1] and loops it until it reaches i 
and then for the next element we again start from the previous dp[i] value. 
So the total time complexity is O(n) if we don’t consider the complexity of the sorting 
as it is not considered in the above solution as well.

Auxiliary Space: O(n)
*/

/*
Approach: Sort the given elements.
Using the greedy approach, the best way is to remove the minimum element or the maximum element
and then check if Amax-Amin <= K. There are various combinations of removals that have to be considered.
We write a recurrence relation to try every possible combination.
There will be two possible ways of removal, either we remove the minimum or we remove the maximum.
Let(i…j) be the index of elements left after removal of elements. Initially,
we start with i=0 and j=n-1 and the number of elements removed is 0 at the beginning.
We only remove an element if a[j]-a[i]>k, the two possible ways of removal are (i+1…j) or (i…j-1).
The minimum of the two is considered.
Let DPi, j be the number of elements that need to be removed so that after removal a[j]-a[i]<=k. 


Recurrence relation for sorted array:

DPi, j = 1 + (min(countRemovals(i+1, j), countRemovals(i, j-1))
Below is the implementation of the above idea:
*/
// CPP program to find minimum removals
// to make max-min <= K
#include <bits/stdc++.h>
using namespace std;

#define MAX 100
int dp[MAX][MAX];

// function to check all possible combinations
// of removal and return the minimum one
int countRemovals(int a[], int i, int j, int k)
{
    // base case when all elements are removed
    if (i >= j)
        return 0;

    // if condition is satisfied, no more
    // removals are required
    else if ((a[j] - a[i]) <= k)
        return 0;

    // if the state has already been visited
    else if (dp[i][j] != -1)
        return dp[i][j];

    // when Amax-Amin>d
    else if ((a[j] - a[i]) > k)
    {

        // minimum is taken of the removal
        // of minimum element or removal
        // of the maximum element
        dp[i][j] = 1 + min(countRemovals(a, i + 1, j, k),
                           countRemovals(a, i, j - 1, k));
    }
    return dp[i][j];
}

// To sort the array and return the answer
int removals(int a[], int n, int k)
{
    // sort the array
    sort(a, a + n);

    // fill all stated with -1
    // when only one element
    memset(dp, -1, sizeof(dp));
    if (n == 1)
        return 0;
    else
        return countRemovals(a, 0, n - 1, k);
}

// Driver Code
int main()
{
    int a[] = {1, 3, 4, 9, 10, 11, 12, 17, 20};
    int n = sizeof(a) / sizeof(a[0]);
    int k = 4;
    cout << removals(a, n, k);
    return 0;
}

/*
Time Complexity :O(n2)
Auxiliary Space: O(n2)

The solution could be further optimized.
The idea is to sort the array in increasing order and traverse through all the elements
(let’s say index i) and find the maximum element on its right (index j) such that arr[j] – arr[i] <= k.
Thus, the number of elements to be removed becomes n-(j-i+1).
The minimum number of elements can be found by taking the minimum of n-(j-i-1) overall i.
The value of index j can be found through a binary search between start = i+1 and end = n-1;
*/