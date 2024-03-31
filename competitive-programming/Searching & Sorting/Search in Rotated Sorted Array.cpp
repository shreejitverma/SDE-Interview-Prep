/*
https://leetcode.com/problems/search-in-rotated-sorted-array/

33. Search in Rotated Sorted Array
Medium

10975

786

Add to List

Share
There is an integer array nums sorted in ascending order (with distinct values).

Prior to being passed to your function, nums is possibly rotated at an unknown pivot index k (1 <= k < nums.length) such that the resulting array is [nums[k], nums[k+1], ..., nums[n-1], nums[0], nums[1], ..., nums[k-1]] (0-indexed). For example, [0,1,2,4,5,6,7] might be rotated at pivot index 3 and become [4,5,6,7,0,1,2].

Given the array nums after the possible rotation and an integer target, return the index of target if it is in nums, or -1 if it is not in nums.

You must write an algorithm with O(log n) runtime complexity.

 

Example 1:

Input: nums = [4,5,6,7,0,1,2], target = 0
Output: 4
Example 2:

Input: nums = [4,5,6,7,0,1,2], target = 3
Output: -1
Example 3:

Input: nums = [1], target = 0
Output: -1
 

Constraints:

1 <= nums.length <= 5000
-104 <= nums[i] <= 104
All values of nums are unique.
nums is an ascending array that is possibly rotated.
-104 <= target <= 104
*/

class Solution
{
public:
    int findMinIdx(vector<int> &nums)
    {
        int l = 0, r = nums.size() - 1;

        while (l <= r)
        {
            int mid = l + (r - l) / 2;
            if (nums[mid] > nums.back())
            {
                //ex: [4,5,6,7,0,1,2], 7 > 2
                //so search in right part
                l = mid + 1;
            }
            else
            {
                //right part is monotonic,
                //so search in left part
                r = mid - 1;
            }
        }

        return l;
    };

    int search(vector<int> &nums, int target)
    {
        int n = nums.size();
        if (n == 0)
            return -1;
        int minIdx = findMinIdx(nums);
        if (nums[minIdx] == target)
            return minIdx;

        // cout << "minIdx: " << minIdx << endl;

        //this is the important part!!
        int l, r;
        if (nums[n - 1] >= target)
        {
            //we can find target in left part
            l = minIdx + 1;
            r = n - 1;
        }
        else
        {
            //we can find target in left part
            l = 0;
            r = minIdx - 1;
        }

        while (l <= r)
        {
            int mid = l + (r - l) / 2;

            // cout << l << " " << mid << " " << r << endl;

            if (nums[mid] == target)
            {
                return mid;
            }
            else if (nums[mid] > target)
            {
                r = mid - 1;
            }
            else
            {
                l = mid + 1;
            }
        }

        return -1;
    }
};