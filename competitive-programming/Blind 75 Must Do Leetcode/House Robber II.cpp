/*
https://leetcode.com/problems/house-robber-ii/
213. House Robber II
Medium

3920

72

Add to List

Share
You are a professional robber planning to rob houses along a street. Each house has a certain amount of money stashed. All houses at this place are arranged in a circle. That means the first house is the neighbor of the last one. Meanwhile, adjacent houses have a security system connected, and it will automatically contact the police if two adjacent houses were broken into on the same night.

Given an integer array nums representing the amount of money of each house, return the maximum amount of money you can rob tonight without alerting the police.

 

Example 1:

Input: nums = [2,3,2]
Output: 3
Explanation: You cannot rob house 1 (money = 2) and then rob house 3 (money = 2), because they are adjacent houses.
Example 2:

Input: nums = [1,2,3,1]
Output: 4
Explanation: Rob house 1 (money = 1) and then rob house 3 (money = 3).
Total amount you can rob = 1 + 3 = 4.
Example 3:

Input: nums = [1,2,3]
Output: 3
 

Constraints:

1 <= nums.length <= 100
0 <= nums[i] <= 1000
*/

//DP
//Runtime: 0 ms, faster than 100.00% of C++ online submissions for House Robber II.
//Memory Usage: 6.5 MB, less than 100.00% of C++ online submissions for House Robber II.
class Solution
{
public:
    int rob(vector<int> &nums)
    {
        int N = nums.size();
        if (N == 0)
            return 0;
        if (N <= 3)
            return *max_element(nums.begin(), nums.end());
        vector<int> dpA = nums, dpB = nums;

        //0~N-2
        for (int i = N - 3; i >= 0; i--)
        {
            dpA[i] += max(((i + 2 < N - 1) ? dpA[i + 2] : 0),
                          ((i + 3 < N - 1) ? dpA[i + 3] : 0));
        }

        //1~N-1
        for (int i = N - 3; i >= 1; i--)
        {
            dpB[i] += max(((i + 2 < N) ? dpB[i + 2] : 0),
                          ((i + 3 < N) ? dpB[i + 3] : 0));
        }

        return max(max(dpA[0], dpA[1]),
                   max(dpB[1], dpB[2]));

        // return max(dpA[0], dpB[1]);
    }
};

//DP, forward
//Runtime: 0 ms, faster than 100.00% of C++ online submissions for House Robber II.
//Memory Usage: 6.3 MB, less than 100.00% of C++ online submissions for House Robber II.
class Solution
{
public:
    int rob(vector<int> &nums)
    {
        int n = nums.size();

        if (n == 0)
            return 0;
        if (n == 1)
            return nums[0];
        if (n == 2)
            return max(nums[0], nums[1]);

        vector<int> dpNoLast(n - 1);
        vector<int> dpNoFirst(n); //padding 0 first

        dpNoLast[0] = nums[0];
        dpNoLast[1] = max(nums[0], nums[1]);

        for (int i = 2; i < dpNoLast.size(); i++)
        {
            //if we rob the ith house, we can add dp[i-2] because there is no alear
            dpNoLast[i] = max(dpNoLast[i - 1], dpNoLast[i - 2] + nums[i]);
        }

        //start from index 1
        //index 0 is just used for padding
        dpNoFirst[1] = nums[1];
        dpNoFirst[2] = max(nums[1], nums[2]);

        for (int i = 3; i < dpNoFirst.size(); i++)
        {
            //if we rob the ith house, we can add dp[i-2] because there is no alear
            dpNoFirst[i] = max(dpNoFirst[i - 1], dpNoFirst[i - 2] + nums[i]);
        }

        return max(dpNoLast.back(), dpNoFirst.back());
    }
};