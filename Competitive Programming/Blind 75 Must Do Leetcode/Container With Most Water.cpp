/*
https://leetcode.com/problems/container-with-most-water/
11. Container With Most Water
Medium

12436

802

Add to List

Share
Given n non-negative integers a1, a2, ..., an , where each represents a point at coordinate (i, ai). n vertical lines are drawn such that the two endpoints of the line i is at (i, ai) and (i, 0). Find two lines, which, together with the x-axis forms a container, such that the container contains the most water.

Notice that you may not slant the container.

 

Example 1:


Input: height = [1,8,6,2,5,4,8,3,7]
Output: 49
Explanation: The above vertical lines are represented by array [1,8,6,2,5,4,8,3,7]. In this case, the max area of water (blue section) the container can contain is 49.
Example 2:

Input: height = [1,1]
Output: 1
Example 3:

Input: height = [4,3,2,1,4]
Output: 16
Example 4:

Input: height = [1,2,1]
Output: 2
 

Constraints:

n == height.length
2 <= n <= 105
0 <= height[i] <= 104
*/

//WA
class Solution
{
public:
    int maxArea(vector<int> &height)
    {
        int N = height.size();
        int left = 0, right = N - 1;
        int ans = 0;
        bool leftTurn = true;

        while (left < right)
        {
            int lh = height[left], rh = height[right];
            // cout << left << " " << right << endl;
            ans = max(ans, min(lh, rh) * (right - left));

            if (leftTurn)
            {
                //move left so heigh[left] is larger than old lh
                do
                {
                    left++;
                    // cout << "left: " << left << endl;
                } while (left < N && height[left] <= lh);
            }
            else
            {
                //move right so height[right] is larger than old rh
                do
                {
                    right--;
                    // cout << "right: " << right << endl;
                } while (right >= 0 && height[right] <= rh);
            }

            leftTurn = !leftTurn;
        }

        return ans;
    }
};

//Approach 1: Brute Force
//TLE
//time: O(n^2), space: O(1)
class Solution
{
public:
    int maxArea(vector<int> &height)
    {
        int N = height.size();
        int ans = 0;
        for (int i = 0; i < N - 1; i++)
        {
            for (int j = i + 1; j < N; j++)
            {
                ans = max(ans, min(height[i], height[j]) * (j - i));
            }
        }

        return ans;
    }
};

//Approach 2: Two Pointer Approach
//Runtime: 16 ms, faster than 96.00% of C++ online submissions for Container With Most Water.
//Memory Usage: 9 MB, less than 100.00% of C++ online submissions for Container With Most Water.
//time: O(n), space: O(1)
class Solution
{
public:
    int maxArea(vector<int> &height)
    {
        int N = height.size();
        int left = 0, right = N - 1, ans = 0;
        while (left < right)
        {
            ans = max(ans, min(height[left], height[right]) * (right - left));
            //move the pointer pointing to lower bar forward,
            //because if we move the pointer pointing to higher bar forward,
            //the area can only be shrinked
            if (height[left] < height[right])
            {
                left++;
            }
            else
            {
                right--;
            }
        }
        return ans;
    }
};