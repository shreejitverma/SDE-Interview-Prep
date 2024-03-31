/*
https://practice.geeksforgeeks.org/problems/combination-sum-1587115620/1
Combination Sum 
Medium Accuracy: 50.0% Submissions: 21408 Points: 4
Given an array of integers and a sum B, find all unique combinations in the array where the sum is equal to B. The same number may be chosen from the array any number of times to make B.

Note:
        1. All numbers will be positive integers.
        2. Elements in a combination (a1, a2, …, ak) must be in non-descending order. (ie, a1 ≤ a2 ≤ … ≤ ak).
        3. The combinations themselves must be sorted in ascending order.


Example 1:

Input:
N = 4
arr[] = {7,2,6,5}
B = 16
Output:
(2 2 2 2 2 2 2 2)
(2 2 2 2 2 6)
(2 2 2 5 5)
(2 2 5 7)
(2 2 6 6)
(2 7 7)
(5 5 6)
Example 2:

Input:
N = 11
arr[] = {6,5,7,1,8,2,9,9,7,7,9}
B = 6
Output:
(1 1 1 1 1 1)
(1 1 1 1 2)
(1 1 2 2)
(1 5)
(2 2 2)
(6)

Your Task:
Your task is to complete the function combinationSum() which takes the array A and a sum B as inputs and returns a list of list denoting the required combinations in the order specified in the problem description. The printing is done by the driver's code. If no set can be formed with the given set, then  "Empty" (without quotes) is printed.


Expected Time Complexity: O(X2 * 2N), where X is average of summation B/arri for every number in the array.
Expected Auxiliary Space: O(X * 2N)


Constraints:
1 <= N <= 30
1 <= A[i] <= 20
1 <= B <= 100
*/

class Solution
{
public:
    //Function to return a list of indexes denoting the required
    //combinations whose sum is equal to given number.
    void calSum(vector<int> A, int sum, vector<int> &temp, int index, vector<vector<int> > &res)
    {
        if (sum == 0)
        {
            res.push_back(temp);
            return;
        }

        for (int i = index; i < A.size(); i++)
        {
            if (sum - A[i] >= 0)
            {
                temp.push_back(A[i]);
                sum = sum - A[i];

                calSum(A, sum, temp, i, res);

                sum = sum + A[i];
                temp.pop_back();
            }
        }
    }

    vector<vector<int> > combinationSum(vector<int> &A, int sum)
    {
        sort(A.begin(), A.end());
        A.erase(unique(A.begin(), A.end()), A.end());

        vector<vector<int> > res;
        vector<int> temp;

        calSum(A, sum, temp, 0, res);

        return res;
    }
};