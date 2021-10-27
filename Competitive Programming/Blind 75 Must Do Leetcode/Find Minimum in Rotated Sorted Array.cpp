/*
https://leetcode.com/problems/find-minimum-in-rotated-sorted-array/
153. Find Minimum in Rotated Sorted Array
Medium

4973

348

Add to List

Share
Suppose an array of length n sorted in ascending order is rotated between 1 and n times. For example, the array nums = [0,1,2,4,5,6,7] might become:

[4,5,6,7,0,1,2] if it was rotated 4 times.
[0,1,2,4,5,6,7] if it was rotated 7 times.
Notice that rotating an array [a[0], a[1], a[2], ..., a[n-1]] 1 time results in the array [a[n-1], a[0], a[1], a[2], ..., a[n-2]].

Given the sorted rotated array nums of unique elements, return the minimum element of this array.

You must write an algorithm that runs in O(log n) time.

 

Example 1:

Input: nums = [3,4,5,1,2]
Output: 1
Explanation: The original array was [1,2,3,4,5] rotated 3 times.
Example 2:

Input: nums = [4,5,6,7,0,1,2]
Output: 0
Explanation: The original array was [0,1,2,4,5,6,7] and it was rotated 4 times.
Example 3:

Input: nums = [11,13,15,17]
Output: 11
Explanation: The original array was [11,13,15,17] and it was rotated 4 times. 
 

Constraints:

n == nums.length
1 <= n <= 5000
-5000 <= nums[i] <= 5000
All the integers of nums are unique.
nums is sorted and rotated between 1 and n times.
*/

//Binary search
//Runtime: 8 ms, faster than 61.21% of C++ online submissions for Find Minimum in Rotated Sorted Array.
//Memory Usage: 10.1 MB, less than 84.69% of C++ online submissions for Find Minimum in Rotated Sorted Array.
class Solution
{
public:
    int findMin(vector<int> &nums)
    {
        int n = nums.size();

        int mid;
        /*
        choose nums[0] as pivot, 
        so we don't need to put nums[0] in the search range
        */
        int ans = nums[0];
        int l = 1, r = n - 1;

        while (l <= r)
        {
            mid = (l + r) >> 1;
            // cout << mid << endl;

            if (nums[mid] < nums[0])
            {
                ans = min(ans, nums[mid]);
                r = mid - 1;
            }
            else
            {
                //nums[mid] > nums[0]
                l = mid + 1;
            }
        }

        return ans;
    }
};

//Binary search, find inflection point
//Runtime: 8 ms, faster than 61.21% of C++ online submissions for Find Minimum in Rotated Sorted Array.
//Memory Usage: 10.3 MB, less than 34.15% of C++ online submissions for Find Minimum in Rotated Sorted Array.
//time: O(logN)
//space: O(1)
class Solution
{
public:
    int findMin(vector<int> &nums)
    {
        int n = nums.size();

        if (n == 1)
            return nums[0];

        /*
        choose nums[0] as pivot, 
        so we don't need to put nums[0] in the search range
        */
        int l = 0, r = n - 1;

        //array is sorted, not rotated
        if (nums[l] < nums[r])
            return nums[l];

        int mid;

        while (l <= r)
        {
            mid = (l + r) >> 1;
            // cout << mid << endl;

            /*
            found the inflection point: 
            the position where right element < left element
            */
            if (nums[mid + 1] < nums[mid])
                return nums[mid + 1];
            if (nums[mid] < nums[mid - 1])
                return nums[mid];

            if (nums[mid] < nums[0])
            {
                r = mid - 1;
            }
            else
            {
                //nums[mid] > nums[0]
                l = mid + 1;
            }
        }

        return -1;
    }
};