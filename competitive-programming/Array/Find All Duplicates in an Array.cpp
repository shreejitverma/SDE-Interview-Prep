/*
https://leetcode.com/problems/find-all-duplicates-in-an-array/
442. Find All Duplicates in an Array
Medium

5087

225

Add to List

Share
Given an integer array nums of length n where all the integers of nums are in the range [1, n] and each integer appears once or twice, return an array of all the integers that appears twice.

You must write an algorithm that runs in O(n) time and uses only constant extra space.

 

Example 1:

Input: nums = [4,3,2,7,8,2,3,1]
Output: [2,3]
Example 2:

Input: nums = [1,1,2]
Output: [1]
Example 3:

Input: nums = [1]
Output: []
 

Constraints:

n == nums.length
1 <= n <= 105
1 <= nums[i] <= n
Each element in nums appears once or twice.
*/

class Solution
{
    vector<int> findDuplicates(vector<int> &arr)
    {
        int i = 0;
        while (i < arr.size())
        {
            int correct = arr[i] - 1;
            if (arr[i] != arr[correct])
            {
                swap(arr, i, correct);
            }
            else
            {
                i++;
            }
        }

        vector<int> ans;
        for (int index = 0; index < arr.size(); index++)
        {
            if (arr[index] != index + 1)
            {
                ans.add(arr[index]);
            }
        }

        return ans;
    }

    void swap(int[] arr, int first, int second)
    {
        int temp = arr[first];
        arr[first] = arr[second];
        arr[second] = temp;
    }
}