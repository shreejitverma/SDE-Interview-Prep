/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(1)

class Solution {
public:
    string generateTheString(int n) {
        auto result = string(n - 1, 'a');
        result.push_back(n % 2 ? 'a' : 'b');
        return result;
    }
};
