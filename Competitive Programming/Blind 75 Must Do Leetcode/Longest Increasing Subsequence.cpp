/*
https://leetcode.com/problems/longest-increasing-subsequence/
300. Longest Increasing Subsequence
Medium

9299

190

Add to List

Share
Given an integer array nums, return the length of the longest strictly increasing subsequence.

A subsequence is a sequence that can be derived from an array by deleting some or no elements without changing the order of the remaining elements. For example, [3,6,2,7] is a subsequence of the array [0,3,1,6,2,2,7].

 

Example 1:

Input: nums = [10,9,2,5,3,7,101,18]
Output: 4
Explanation: The longest increasing subsequence is [2,3,7,101], therefore the length is 4.
Example 2:

Input: nums = [0,1,0,3,2,3]
Output: 4
Example 3:

Input: nums = [7,7,7,7,7,7,7]
Output: 1
 

Constraints:

1 <= nums.length <= 2500
-104 <= nums[i] <= 104*/

//DP
//Runtime: 80 ms, faster than 27.26% of C++ online submissions for Longest Increasing Subsequence.
//Memory Usage: 7.9 MB, less than 100.00% of C++ online submissions for Longest Increasing Subsequence.
//time: O(N^2), space: O(N)
class Solution
{
public:
    int lengthOfLIS(vector<int> &nums)
    {
        int n = nums.size();
        if (n == 0)
            return 0;

        //note that the shortest possible increasing sequence's length is 1!
        //dp[i]: length of longest increasing subsequence ending at i
        vector<int> dp(n, 1);
        int ans = 1;

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < i; j++)
            {
                /*
                before nums[i], add the increasing subsequence ending at j
                */
                if (nums[i] > nums[j])
                {
                    dp[i] = max(dp[i], dp[j] + 1);
                }
            }
            // cout << dp[i] << " ";
            ans = max(ans, dp[i]);
        }
        // cout << endl;

        return ans;
    }
};

//DP + binary search
//Runtime: 4 ms, faster than 89.41% of C++ online submissions for Longest Increasing Subsequence.
//Memory Usage: 7.9 MB, less than 100.00% of C++ online submissions for Longest Increasing Subsequence.
//https://algorithmsandme.com/longest-increasing-subsequence-in-onlogn/
//https://github.com/keineahnung2345/fucking-algorithm/blob/note/%E5%8A%A8%E6%80%81%E8%A7%84%E5%88%92%E7%B3%BB%E5%88%97/%E5%8A%A8%E6%80%81%E8%A7%84%E5%88%92%E8%AE%BE%E8%AE%A1%EF%BC%9A%E6%9C%80%E9%95%BF%E9%80%92%E5%A2%9E%E5%AD%90%E5%BA%8F%E5%88%97.md
//time: O(NlogN), space: O(N)
class Solution
{
public:
    int lengthOfLIS(vector<int> &nums)
    {
        int n = nums.size();
        if (n == 0)
            return 0;

        vector<int> top;

        for (int poker : nums)
        {
            int left = 0, right = top.size() - 1;
            /*
            find the smallest top poker that is greater than current poker,
            so we are finding lower bound,
            that means we should focus on "left"
            */
            while (left <= right)
            {
                int mid = left + (right - left) / 2;
                if (poker < top[mid])
                {
                    right = mid - 1;
                }
                else if (poker == top[mid])
                {
                    right = mid - 1;
                }
                else if (poker > top[mid])
                {
                    left = mid + 1;
                }
            }

            if (left == top.size())
            {
                //poker is larger than all cards in top
                top.push_back(poker);
            }
            else
            {
                //overlay current poker to the specific pile
                top[left] = poker;
            }
        }

        return top.size();
    }
};

//Approach 1: Brute Force, Recursion
//TLE
//21 / 24 test cases passed.
//time: O(2^N), because for each element, there are 2 cases: taken or nottaken
class Solution
{
public:
    int lengthOfLIS(vector<int> &nums, int prev = -1, int cur = 0)
    {
        /*
        nums[cur] is the element we consider to append,
        it cannot be out of the range of nums
        */
        if (cur == nums.size())
            return 0;
        int taken = 0;
        //we may append current element into sequence, take
        /*
        initial state: prev is -1, and cur = 0,
        that means we are considering the 0th element,
        and the 0th element is always ok to be appended to the empty sequence
        */
        if (prev < 0 || nums[cur] > nums[prev])
        {
            taken = 1 + lengthOfLIS(nums, cur, cur + 1);
        }
        //skip current element, not take
        int nottaken = lengthOfLIS(nums, prev, cur + 1);
        // cout << prev << ", " << cur << " : " << taken << ", " << nottaken << endl;
        return max(taken, nottaken);
    }
};

//Approach 2: Recursion with Memoization
//Runtime: 748 ms, faster than 5.13% of C++ online submissions for Longest Increasing Subsequence.
//Memory Usage: 110.3 MB, less than 6.25% of C++ online submissions for Longest Increasing Subsequence.
//time: O(N^2), space: O(N^2)
class Solution
{
public:
    vector<vector<int> > memo;

    int lengthOfLIS(vector<int> &nums, int prev, int cur)
    {
        /*
        nums[cur] is the element we consider to append,
        it cannot be out of the range of nums
        */
        if (cur == nums.size())
            return 0;
        if (memo[prev + 1][cur] != -1)
            return memo[prev + 1][cur];
        int taken = 0;
        //we may append current element into sequence
        /*
        initial state: prev is -1, and cur = 0,
        that means we are considering the 0th element,
        and the 0th element is always ok to be appended to the empty sequence
        */
        if (prev < 0 || nums[cur] > nums[prev])
        {
            taken = 1 + lengthOfLIS(nums, cur, cur + 1);
        }
        //skip current element
        int nottaken = lengthOfLIS(nums, prev, cur + 1);
        // cout << prev << ", " << cur << " : " << taken << ", " << nottaken << endl;
        memo[prev + 1][cur] = max(taken, nottaken);
        return memo[prev + 1][cur];
    }

    int lengthOfLIS(vector<int> &nums)
    {
        /*
        padding ahead,
        because prev may be -1
        */
        int n = nums.size();
        memo = vector<vector<int> >(n + 1, vector(n, -1));
        return lengthOfLIS(nums, -1, 0);
    }
};