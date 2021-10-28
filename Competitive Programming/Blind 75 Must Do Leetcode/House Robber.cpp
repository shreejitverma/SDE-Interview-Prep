/*
https://leetcode.com/problems/house-robber/
198. House Robber
Medium

8957

221

Add to List

Share
You are a professional robber planning to rob houses along a street. Each house has a certain amount of money stashed, the only constraint stopping you from robbing each of them is that adjacent houses have security systems connected and it will automatically contact the police if two adjacent houses were broken into on the same night.

Given an integer array nums representing the amount of money of each house, return the maximum amount of money you can rob tonight without alerting the police.

 

Example 1:

Input: nums = [1,2,3,1]
Output: 4
Explanation: Rob house 1 (money = 1) and then rob house 3 (money = 3).
Total amount you can rob = 1 + 3 = 4.
Example 2:

Input: nums = [2,7,9,3,1]
Output: 12
Explanation: Rob house 1 (money = 2), rob house 3 (money = 9) and rob house 5 (money = 1).
Total amount you can rob = 2 + 9 + 1 = 12.
 

Constraints:

1 <= nums.length <= 100
0 <= nums[i] <= 400

*/

//recursion
//TLE
/**
class Solution {
public:
    int robStart(vector<int>& nums, int s){
        if(s >= nums.size()) return 0;
        return nums[s] + max(robStart(nums, s+2), robStart(nums, s+3));
    }
    int rob(vector<int>& nums) {
        return max(robStart(nums, 0), robStart(nums, 1));
    }
};
**/

//DP, consider 2nd and 3rd next houses
//Runtime: 4 ms, faster than 100.00% of C++ online submissions for House Robber.
//Memory Usage: 8.6 MB, less than 100.00% of C++ online submissions for House Robber.

class Solution
{
public:
    vector<int> money;
    int rob(vector<int> &nums)
    {
        if (nums.size() == 0)
            return 0;
        else if (nums.size() == 1)
            return nums[0];

        int N = nums.size();
        money = nums;
        //so that we can access money[N]
        money.push_back(0);
        for (int i = N - 3; i >= 0; i--)
        {
            money[i] += max(money[i + 2], money[i + 3]);
        }
        return max(money[0], money[1]);
    }
};

//DP, consider next 2 houses
//Runtime: 0 ms, faster than 100.00% of C++ online submissions for House Robber.
//Memory Usage: 6.4 MB, less than 100.00% of C++ online submissions for House Robber.
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

        vector<int> dp(n);

        dp[0] = nums[0];
        dp[1] = max(nums[0], nums[1]);

        for (int i = 2; i < n; i++)
        {
            //if we rob the ith house, we can add dp[i-2] because there is no alear
            dp[i] = max(dp[i - 1], dp[i - 2] + nums[i]);
        }

        return dp[n - 1];
    }
};