/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(n)

#include <vector>
#include <stack>

class Solution {
public:
    std::vector<int> dailyTemperatures(const std::vector<int>& temperatures) {
        const size_t n = temperatures.size();
        std::vector<int> result(n, 0);
        std::stack<int> stk;

        for (size_t i = 0; i < n; ++i) {
            while (!stk.empty() && temperatures[stk.top()] < temperatures[i]) {
                const int prev = stk.top();
                stk.pop();
                result[prev] = static_cast<int>(i) - prev;
            }
            stk.push(static_cast<int>(i));
        }
        return result; 
    }
};
