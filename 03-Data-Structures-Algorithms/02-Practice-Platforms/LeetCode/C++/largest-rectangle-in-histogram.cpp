/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(n)

#include <vector>
#include <stack>
#include <algorithm>

class Solution {
public:
    int largestRectangleArea(const std::vector<int>& heights) {
        std::stack<int> stk;
        int max_area = 0;
        const int n = static_cast<int>(heights.size());

        for (int i = 0; i <= n; ++i) {
            const int curr_height = (i == n) ? 0 : heights[i];
            while (!stk.empty() && heights[stk.top()] >= curr_height) {
                const int h = heights[stk.top()];
                stk.pop();
                const int width = stk.empty() ? i : i - 1 - stk.top();
                max_area = std::max(max_area, h * width);
            }
            stk.push(i);
        }

        return max_area;
    }
};
