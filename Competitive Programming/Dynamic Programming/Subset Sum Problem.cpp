/*
https://practice.geeksforgeeks.org/problems/subset-sum-problem2014/1
https://www.geeksforgeeks.org/partition-problem-dp-18/
Partition Equal Subset Sum 
Medium Accuracy: 38.0% Submissions: 75383 Points: 4
Given an array arr[] of size N, check if it can be partitioned into two parts such that the sum of elements in both parts is the same.

Example 1:

Input: N = 4
arr = {1, 5, 11, 5}
Output: YES
Explaination: 
The two parts are {1, 5, 5} and {11}.
Example 2:

Input: N = 3
arr = {1, 3, 5}
Output: NO
Explaination: This array can never be 
partitioned into two such parts.

Your Task:
You do not need to read input or print anything. Your task is to complete the function equalPartition() which takes the value N and the array as input parameters and returns 1 if the partition is possible. Otherwise, returns 0.


Expected Time Complexity: O(N*sum of elements)
Expected Auxiliary Space: O(N*sum of elements)


Constraints:
1 ≤ N ≤ 100
1 ≤ arr[i] ≤ 1000


Recursive Solution 
Following is the recursive property of the second step mentioned above. 

Let isSubsetSum(arr, n, sum/2) be the function that returns true if 
there is a subset of arr[0..n-1] with sum equal to sum/2

The isSubsetSum problem can be divided into two subproblems
 a) isSubsetSum() without considering last element 
    (reducing n to n-1)
 b) isSubsetSum considering the last element 
    (reducing sum/2 by arr[n-1] and n to n-1)
If any of the above subproblems return true, then return true. 
isSubsetSum (arr, n, sum/2) = isSubsetSum (arr, n-1, sum/2) ||
                              isSubsetSum (arr, n-1, sum/2 - arr[n-1])
Below is the implementation of the above code: 
*/

// A recursive C++ program for partition problem
#include <bits/stdc++.h>
using namespace std;

// A utility function that returns true if there is
// a subset of arr[] with sun equal to given sum
bool isSubsetSum(int arr[], int n, int sum)
{
    // Base Cases
    if (sum == 0)
        return true;
    if (n == 0 && sum != 0)
        return false;

    // If last element is greater than sum, then
    // ignore it
    if (arr[n - 1] > sum)
        return isSubsetSum(arr, n - 1, sum);

    /* else, check if sum can be obtained by any of
        the following
        (a) including the last element
        (b) excluding the last element
    */
    return isSubsetSum(arr, n - 1, sum) || isSubsetSum(arr, n - 1, sum - arr[n - 1]);
}

// Returns true if arr[] can be partitioned in two
// subsets of equal sum, otherwise false
bool findPartiion(int arr[], int n)
{
    // Calculate sum of the elements in array
    int sum = 0;
    for (int i = 0; i < n; i++)
        sum += arr[i];

    // If sum is odd, there cannot be two subsets
    // with equal sum
    if (sum % 2 != 0)
        return false;

    // Find if there is subset with sum equal to
    // half of total sum
    return isSubsetSum(arr, n, sum / 2);
}

// Driver code
int main()
{
    int arr[] = {3, 1, 5, 9, 12};
    int n = sizeof(arr) / sizeof(arr[0]);

    // Function call
    if (findPartiion(arr, n) == true)
        cout << "Can be divided into two subsets "
                "of equal sum";
    else
        cout << "Can not be divided into two subsets"
                " of equal sum";
    return 0;
}

/*
Time Complexity: O(2^n) In the worst case, this solution tries two possibilities (whether to include or exclude) for every element.

Dynamic Programming Solution 
The problem can be solved using dynamic programming when the sum of the elements is not too big. We can create a 2D array part[][] of size (sum/2 + 1)*(n+1). And we can construct the solution in a bottom-up manner such that every filled entry has the following property  

part[i][j] = true if a subset of {arr[0], arr[1], ..arr[j-1]} has sum 
             equal to i, otherwise false
*/

// A Dynamic Programming based
// C++ program to partition problem
#include <bits/stdc++.h>
using namespace std;

// Returns true if arr[] can be partitioned
// in two subsets of equal sum, otherwise false
bool findPartiion(int arr[], int n)
{
    int sum = 0;
    int i, j;

    // Calculate sum of all elements
    for (i = 0; i < n; i++)
        sum += arr[i];

    if (sum % 2 != 0)
        return false;

    bool part[sum / 2 + 1][n + 1];

    // initialize top row as true
    for (i = 0; i <= n; i++)
        part[0][i] = true;

    // initialize leftmost column,
    // except part[0][0], as 0
    for (i = 1; i <= sum / 2; i++)
        part[i][0] = false;

    // Fill the partition table in bottom up manner
    for (i = 1; i <= sum / 2; i++)
    {
        for (j = 1; j <= n; j++)
        {
            part[i][j] = part[i][j - 1];
            if (i >= arr[j - 1])
                part[i][j] = part[i][j] || part[i - arr[j - 1]][j - 1];
        }
    }

    /* // uncomment this part to print table
    for (i = 0; i <= sum/2; i++)
    {
    for (j = 0; j <= n; j++)
        cout<<part[i][j];
    cout<<endl;
    } */

    return part[sum / 2][n];
}

// Driver Code
int main()
{
    int arr[] = {3, 1, 1, 2, 2, 1};
    int n = sizeof(arr) / sizeof(arr[0]);

    // Function call
    if (findPartiion(arr, n) == true)
        cout << "Can be divided into two subsets of equal "
                "sum";
    else
        cout << "Can not be divided into"
             << " two subsets of equal sum";
    return 0;
}
/*
Time Complexity: O(sum*n) 
Auxiliary Space: O(sum*n) 

Please note that this solution will not be feasible for arrays with big sum.

Dynamic Programming Solution (Space Complexity Optimized)

Instead of creating a 2-D array of size (sum/2 + 1)*(n  + 1), we can solve this problem using an array of size (sum/2 + 1 ) only. 

part[j] = true if there is a subset with sum equal to j, otherwise false.

Below is the implementation of the above approach: 
*/

// A Dynamic Programming based
// C++ program to partition problem
#include <bits/stdc++.h>
using namespace std;

// Returns true if arr[] can be partitioned
// in two subsets of equal sum, otherwise false
bool findPartiion(int arr[], int n)
{
    int sum = 0;
    int i, j;

    // Calculate sum of all elements
    for (i = 0; i < n; i++)
        sum += arr[i];

    if (sum % 2 != 0)
        return false;

    bool part[sum / 2 + 1];

    // Initialize the part array
    // as 0
    for (i = 0; i <= sum / 2; i++)
    {
        part[i] = 0;
    }

    // Fill the partition table in bottom up manner

    for (i = 0; i < n; i++)
    {
        // the element to be included
        // in the sum cannot be
        // greater than the sum
        for (j = sum / 2; j >= arr[i]; j--)
        { // check if sum - arr[i]
            // could be formed
            // from a subset
            // using elements
            // before index i
            if (part[j - arr[i]] == 1 || j == arr[i])
                part[j] = 1;
        }
    }

    return part[sum / 2];
}

// Driver Code
int main()
{
    int arr[] = {1, 3, 3, 2, 3, 2};
    int n = sizeof(arr) / sizeof(arr[0]);

    // Function call
    if (findPartiion(arr, n) == true)
        cout << "Can be divided into two subsets of equal "
                "sum";
    else
        cout << "Can not be divided into"
             << " two subsets of equal sum";
    return 0;
}
// Time Complexity: O(sum * n)
// Auxiliary Space: O(sum)