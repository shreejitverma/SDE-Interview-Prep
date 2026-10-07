#include <vector>
#include <deque>

using namespace std;

/*
 * Problem: LeetCode 239 - Sliding Window Maximum
 * Difficulty: Hard
 * Concepts: Monotonic Queue, Sliding Window, Deque
 *
 * Time Complexity: O(n)
 * Space Complexity: O(k)
 */

class Solution {
public:
    vector<int> maxSlidingWindow(vector<int>& nums, int k) {
        int n = static_cast<int>(nums.size());
        vector<int> result;
        result.reserve(n - k + 1);

        deque<int> dq; // Stores indices with values in strictly decreasing order

        for (int i = 0; i < n; ++i) {
            // Remove indices that fall outside the current window
            if (!dq.empty() && dq.front() <= i - k) {
                dq.pop_front();
            }

            // Maintain monotonic decreasing invariant
            while (!dq.empty() && nums[dq.back()] <= nums[i]) {
                dq.pop_back();
            }

            dq.push_back(i);

            // Record maximum once window reaches size k
            if (i >= k - 1) {
                result.push_back(nums[dq.front()]);
            }
        }

        return result;
    }
};
