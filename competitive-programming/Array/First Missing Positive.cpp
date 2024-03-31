/*
https://leetcode.com/problems/first-missing-positive/
41. First Missing Positive
Hard

7625

1170

Add to List

Share
Given an unsorted integer array nums, return the smallest missing positive integer.

You must implement an algorithm that runs in O(n) time and uses constant extra space.

 

Example 1:

Input: nums = [1,2,0]
Output: 3
Example 2:

Input: nums = [3,4,-1,1]
Output: 2
Example 3:

Input: nums = [7,8,9,11,12]
Output: 1
 

Constraints:

1 <= nums.length <= 5 * 105
-231 <= nums[i] <= 231 - 1
*/

class Solution
{
public:
    long long int firstMissingPositive(vector<int> &nums)
    {
        long long int i = 0;
        while (i < nums.size())
        {
            long long int correct = nums[i];
            correct--;
            if (nums[i] > 0 && nums[i] <= nums.size() && nums[i] != nums[correct])
            {
                swap(nums, i, correct);
            }
            else
            {
                i++;
            }
        }

        // search for first missing number
        for (long long int index = 0; index < nums.size(); index++)
        {
            if (nums[index] != index + 1)
            {
                return index + 1;
            }
        }

        // case 2
        return nums.size() + 1;
    }

    void swap(vector<int> &arr, int first, int second)
    {
        long long temp = arr[first];
        arr[first] = arr[second];
        arr[second] = temp;
    }
};