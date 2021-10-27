/*
https://leetcode.com/problems/missing-number/
268. Missing Number
Easy

4055

2698

Add to List

Share
Given an array nums containing n distinct numbers in the range [0, n], return the only number in the range that is missing from the array.

 

Example 1:

Input: nums = [3,0,1]
Output: 2
Explanation: n = 3 since there are 3 numbers, so all numbers are in the range [0,3]. 2 is the missing number in the range since it does not appear in nums.
Example 2:

Input: nums = [0,1]
Output: 2
Explanation: n = 2 since there are 2 numbers, so all numbers are in the range [0,2]. 2 is the missing number in the range since it does not appear in nums.
Example 3:

Input: nums = [9,6,4,2,3,5,7,0,1]
Output: 8
Explanation: n = 9 since there are 9 numbers, so all numbers are in the range [0,9]. 8 is the missing number in the range since it does not appear in nums.
Example 4:

Input: nums = [0]
Output: 1
Explanation: n = 1 since there is 1 number, so all numbers are in the range [0,1]. 1 is the missing number in the range since it does not appear in nums.
 

Constraints:

n == nums.length
1 <= n <= 104
0 <= nums[i] <= n
All the numbers of nums are unique.
 

Follow up: 
Could you implement a solution using only O(1) extra space complexity and O(n) runtime complexity?
*/

//Runtime: 1080 ms, faster than 6.59% of C++ online submissions for Missing Number.
//Memory Usage: 10.3 MB, less than 9.80% of C++ online submissions for Missing Number.

class Solution
{
public:
    int missingNumber(vector<int> &nums)
    {
        vector<int> full(nums.size() + 1);
        iota(full.begin(), full.end(), 0);

        for (int num : nums)
        {
            full.erase(remove(full.begin(), full.end(), num), full.end());
        }

        return full[0];
    }
};

//https://leetcode.com/problems/missing-number/discuss/69777/C%2B%2B-solution-using-bit-manipulation
//Runtime: 24 ms, faster than 80.72% of C++ online submissions for Missing Number.
//Memory Usage: 9.9 MB, less than 70.59% of C++ online submissions for Missing Number.
class Solution
{
public:
    int missingNumber(vector<int> &nums)
    {
        int result = 0;
        //exclusive-or from (a) [0 to n] and (b) [all number in "nums"]
        //same numbers in (a) and (b) will counteract each other
        //so it will remain 0 ^ the missing number = the missing nuber
        for (int i = 0; i < nums.size(); i++)
        {
            result ^= nums[i];
            result ^= i;
        }
        result ^= nums.size();
        return result;
    }
};