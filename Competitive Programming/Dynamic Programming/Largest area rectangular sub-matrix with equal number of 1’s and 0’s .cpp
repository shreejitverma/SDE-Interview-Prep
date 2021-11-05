/*
https: // www.geeksforgeeks.org/largest-area-rectangular-sub-matrix-equal-number-1s-0s/
Given a binary matrix.
The problem is to find the largest area rectangular sub-matrix with equal number of 1’s and 0’s.

Examples:


Input: mat[][] = {{0, 0, 1, 1},
                  {0, 1, 1, 0},
                  {1, 1, 1, 0},
                  {1, 0, 0, 1}}
Output: 8 sq. units
(Top, left): (0, 0)
(Bottom, right): (3, 1)

Input: mat[][] = {{0, 0, 1, 1},
                  {0, 1, 1, 1}}
Output: 6 sq. units


Recommended: Please try your approach on {IDE} first, before moving on to the solution.
The naive solution for this problem is to check every possible rectangle in given 2D array
by counting the total number of 1’s and 0’s in that rectangle. This solution requires 4 nested loops and
time complexity of this solution would be O(n ^ 4).

An efficient solution is based on Largest rectangular sub-matrix whose sum is 0
which reduces the time complexity to O(n ^ 3). First of all consider every ‘0’ in the matrix as ‘-1’.
Now, the idea is to reduce the problem to 1-D array. We fix the left and right columns one by one and
find the largest sub-array with 0 sum contiguous rows for every left and right column pair.
We basically find top and bottom row numbers(which have sum zero) for every fixed left and right column pair.
To find the top and bottom row numbers, calculate sum of elements in every row from left to right and
store these sums in an array say temp[]. So temp[i] indicates sum of elements from left to right in row i.
If we find largest subarray with 0 sum in temp[], we can get the index positions of rectangular sub-matrix with
sum equal to 0 (i.e. having equal number of 1’s and 0’s).
With this process we can find the largest area rectangular sub-matrix with sum equal to 0
(i.e. having equal number of 1’s and 0’s).
We can use Hashing technique to find maximum length sub-array with sum equal to 0 in 1-D array in O(n) time.

*/

// C++ implementation to find largest area rectangular
// submatrix with equal number of 1's and 0's
#include <bits/stdc++.h>

using namespace std;

#define MAX_ROW 10
#define MAX_COL 10

// This function basically finds largest 0
// sum subarray in arr[0..n-1]. If 0 sum
// does't exist, then it returns false. Else
// it returns true and sets starting and
// ending indexes as start and end.
bool subArrWithSumZero(int arr[], int &start,
                       int &end, int n)
{
    // to store cumulative sum
    int sum[n];

    // Initialize all elements of sum[] to 0
    memset(sum, 0, sizeof(sum));

    // map to store the indexes of sum
    unordered_map<int, int> um;

    // build up the cumulative sum[] array
    sum[0] = arr[0];
    for (int i = 1; i < n; i++)
        sum[i] = sum[i - 1] + arr[i];

    // to store the maximum length subarray
    // with sum equal to 0
    int maxLen = 0;

    // traverse to the sum[] array
    for (int i = 0; i < n; i++)
    {
        // if true, then there is a subarray
        // with sum equal to 0 from the
        // beginning up to index 'i'
        if (sum[i] == 0)
        {
            // update the required variables
            start = 0;
            end = i;
            maxLen = (i + 1);
        }

        // else if true, then sum[i] has not
        // seen before in 'um'
        else if (um.find(sum[i]) == um.end())
            um[sum[i]] = i;

        // sum[i] has been seen before in the
        // unordered_map 'um'
        else
        {
            // if previous subarray length is smaller
            // than the current subarray length, then
            // update the required variables
            if (maxLen < (i - um[sum[i]]))
            {
                maxLen = (i - um[sum[i]]);
                start = um[sum[i]] + 1;
                end = i;
            }
        }
    }

    // if true, then there is no
    // subarray with sum equal to 0
    if (maxLen == 0)
        return false;

    // else return true
    return true;
}

// function to find largest area rectangular
// submatrix with equal number of 1's and 0's
void maxAreaRectWithSumZero(int mat[MAX_ROW][MAX_COL],
                            int row, int col)
{
    // to store intermediate values
    int temp[row], startRow, endRow;

    // to store the final outputs
    int finalLeft, finalRight, finalTop, finalBottom;
    finalLeft = finalRight = finalTop = finalBottom = -1;
    int maxArea = 0;

    // Set the left column
    for (int left = 0; left < col; left++)
    {
        // Initialize all elements of temp as 0
        memset(temp, 0, sizeof(temp));

        // Set the right column for the left column
        // set by outer loop
        for (int right = left; right < col; right++)
        {
            // Calculate sum between current left
            // and right for every row 'i'
            // consider value '1' as '1' and
            // value '0' as '-1'
            for (int i = 0; i < row; i++)
                temp[i] += mat[i][right] ? 1 : -1;

            // Find largest subarray with 0 sum in
            // temp[]. The subArrWithSumZero() function
            // also sets values of finalTop, finalBottom,
            // finalLeft and finalRight if there exists
            // a subarray with sum 0 in temp
            if (subArrWithSumZero(temp, startRow, endRow, row))
            {
                int area = (right - left + 1) *
                           (endRow - startRow + 1);

                // Compare current 'area' with previous area
                // and accodingly update final values
                if (maxArea < area)
                {
                    finalTop = startRow;
                    finalBottom = endRow;
                    finalLeft = left;
                    finalRight = right;
                    maxArea = area;
                }
            }
        }
    }

    // if true then there is no rectangular submatrix
    // with equal number of 1's and 0's
    if (maxArea == 0)
        cout << "No such rectangular submatrix exists:";

    // displaying the top left and bottom right boundaries
    // with the area of the rectangular submatrix
    else
    {
        cout << "(Top, Left): "
             << "(" << finalTop << ", " << finalLeft
             << ")" << endl;

        cout << "(Bottom, Right): "
             << "(" << finalBottom << ", " << finalRight
             << ")" << endl;

        cout << "Area: " << maxArea << " sq.units";
    }
}

// Driver program to test above
int main()
{
    int mat[MAX_ROW][MAX_COL] = {{0, 0, 1, 1},
                                 {0, 1, 1, 0},
                                 {1, 1, 1, 0},
                                 {1, 0, 0, 1}};
    int row = 4, col = 4;
    maxAreaRectWithSumZero(mat, row, col);
    return 0;
}