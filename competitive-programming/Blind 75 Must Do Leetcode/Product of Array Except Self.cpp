/*
https://leetcode.com/problems/product-of-array-except-self/
238. Product of Array Except Self
Medium

9332

618

Add to List

Share
Given an integer array nums, return an array answer such that answer[i] is equal to the product of all the elements of nums except nums[i].

The product of any prefix or suffix of nums is guaranteed to fit in a 32-bit integer.

You must write an algorithm that runs in O(n) time and without using the division operation.

 

Example 1:

Input: nums = [1,2,3,4]
Output: [24,12,8,6]
Example 2:

Input: nums = [-1,1,0,-3,3]
Output: [0,0,9,0,0]
 

Constraints:

2 <= nums.length <= 105
-30 <= nums[i] <= 30
The product of any prefix or suffix of nums is guaranteed to fit in a 32-bit integer.
 

Follow up: Can you solve the problem in O(1) extra space complexity? 
(The output array does not count as extra space for space complexity analysis.)
*/

//WA
class Solution
{
public:
    vector<int> productExceptSelf(vector<int> &nums)
    {
        int product = accumulate(nums.begin(), nums.end(), 1, multiplies<int>());
        int N = nums.size();
        vector<int> output(N, 0);

        for (int i = 0; i < N; i++)
        {
            output[i] = product / nums[i];
        }

        return output;
    }
};

//Approach 1: Left and Right product lists
//Runtime: 44 ms, faster than 31.25% of C++ online submissions for Product of Array Except Self.
//Memory Usage: 10.7 MB, less than 100.00% of C++ online submissions for Product of Array Except Self.
//time: O(N), space: O(N)
class Solution
{
public:
    vector<int> productExceptSelf(vector<int> &nums)
    {
        int N = nums.size();
        vector<int> L(N, 1), R(N, 1), output(N, 1);

        for (int i = 1; i < N; i++)
        {
            L[i] = L[i - 1] * nums[i - 1];
        }

        for (int i = N - 2; i >= 0; i--)
        {
            R[i] = R[i + 1] * nums[i + 1];
        }

        for (int i = 0; i < N; i++)
        {
            output[i] = L[i] * R[i];
        }

        return output;
    }
};

//Optimize the array R
//Runtime: 40 ms, faster than 75.01% of C++ online submissions for Product of Array Except Self.
//Memory Usage: 10.4 MB, less than 100.00% of C++ online submissions for Product of Array Except Self.
//time: O(N), space O(1)
class Solution
{
public:
    vector<int> productExceptSelf(vector<int> &nums)
    {
        int N = nums.size();
        int R = 1;
        vector<int> ans(N, 1);

        for (int i = 1; i < N; i++)
        {
            ans[i] = ans[i - 1] * nums[i - 1];
        }

        for (int i = N - 2; i >= 0; i--)
        {
            R *= nums[i + 1];
            ans[i] *= R;
        }

        return ans;
    }
};