/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Monotonic Stack & Monotonic Queue (Sliding Window Maximum)
 * Standard: Modern C++20
 * Description: Idiomatic implementations of Next Greater Element (Monotonic Stack)
 *              and Sliding Window Maximum (Monotonic Double-Ended Queue) in amortized O(N) time.
 * 
 * Complexity:
 * - Next Greater Element: O(N) Time, O(N) Space
 * - Sliding Window Max: O(N) Time, O(K) Space
 */

#include <iostream>
#include <vector>
#include <deque>
#include <span>
#include <cassert>

class MonotonicPatterns {
public:
    // Finds the next greater element for each index; returns -1 if none exists.
    // Time: O(N), Space: O(N)
    template <typename T>
    static std::vector<int> next_greater_element(std::span<const T> nums) {
        size_t n = nums.size();
        std::vector<int> result(n, -1);
        std::vector<size_t> stack; // Stores indices of elements in strictly decreasing order

        for (size_t i = 0; i < n; ++i) {
            while (!stack.empty() && nums[stack.back()] < nums[i]) {
                result[stack.back()] = nums[i];
                stack.pop_back();
            }
            stack.push_back(i);
        }
        return result;
    }

    // Computes the maximum value in every sliding window of width k.
    // Time: O(N), Space: O(K)
    template <typename T>
    static std::vector<T> sliding_window_maximum(std::span<const T> nums, size_t k) {
        if (nums.empty() || k == 0 || k > nums.size()) {
            return {};
        }

        std::vector<T> result;
        result.reserve(nums.size() - k + 1);
        std::deque<size_t> dq; // Stores indices; values at indices are strictly decreasing

        for (size_t i = 0; i < nums.size(); ++i) {
            // Remove indices falling outside the current sliding window [i - k + 1, i]
            while (!dq.empty() && dq.front() + k <= i) {
                dq.pop_front();
            }

            // Maintain monotonic decreasing invariant
            while (!dq.empty() && nums[dq.back()] <= nums[i]) {
                dq.pop_back();
            }

            dq.push_back(i);

            // Append maximum element once the first window is formed
            if (i + 1 >= k) {
                result.push_back(nums[dq.front()]);
            }
        }

        return result;
    }
};

int main() {
    // 1. Next Greater Element Test
    std::vector<int> arr = {2, 1, 2, 4, 3};
    auto nge = MonotonicPatterns::next_greater_element<int>(arr);
    std::vector<int> expected_nge = {4, 2, 4, -1, -1};
    assert(nge == expected_nge);

    // 2. Sliding Window Maximum Test
    std::vector<int> window_arr = {1, 3, -1, -3, 5, 3, 6, 7};
    auto max_windows = MonotonicPatterns::sliding_window_maximum<int>(window_arr, 3);
    std::vector<int> expected_windows = {3, 3, 5, 5, 6, 7};
    assert(max_windows == expected_windows);

    std::cout << "All Monotonic Stack and Monotonic Queue tests passed successfully.\n";
    return 0;
}
