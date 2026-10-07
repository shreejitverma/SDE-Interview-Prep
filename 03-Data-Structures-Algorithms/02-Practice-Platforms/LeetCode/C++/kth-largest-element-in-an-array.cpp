#include <vector>
#include <random>
#include <utility>
#include <algorithm>
#include <queue>

using namespace std;

/*
 * Problem: LeetCode 215 - Kth Largest Element in an Array
 * Difficulty: Medium
 * Concepts: QuickSelect, Min-Heap, Divide and Conquer
 *
 * Approach 1 (QuickSelect with 3-way partition):
 * Time Complexity: O(n) average, O(n^2) worst-case (mitigated by randomized pivot)
 * Space Complexity: O(1) iterative auxiliary
 *
 * Approach 2 (Min-Heap):
 * Time Complexity: O(n log k)
 * Space Complexity: O(k)
 */

class Solution {
public:
    int findKthLargest(vector<int>& nums, int k) {
        // Target index in 0-indexed ascending order
        int target_idx = static_cast<int>(nums.size()) - k;
        int left = 0;
        int right = static_cast<int>(nums.size()) - 1;

        default_random_engine gen(random_device{}());

        while (left <= right) {
            uniform_int_distribution<int> dis(left, right);
            int pivot_idx = dis(gen);
            int pivot_val = nums[pivot_idx];

            // 3-way Dutch National Flag partition
            int lt = left;
            int gt = right;
            int i = left;

            while (i <= gt) {
                if (nums[i] < pivot_val) {
                    swap(nums[i++], nums[lt++]);
                } else if (nums[i] > pivot_val) {
                    swap(nums[i], nums[gt--]);
                } else {
                    ++i;
                }
            }

            if (target_idx >= lt && target_idx <= gt) {
                return nums[target_idx];
            } else if (target_idx < lt) {
                right = lt - 1;
            } else {
                left = gt + 1;
            }
        }

        return nums[target_idx];
    }
};

class SolutionMinHeap {
public:
    int findKthLargest(vector<int>& nums, int k) {
        priority_queue<int, vector<int>, greater<int>> min_heap;
        for (int num : nums) {
            min_heap.push(num);
            if (static_cast<int>(min_heap.size()) > k) {
                min_heap.pop();
            }
        }
        return min_heap.top();
    }
};
