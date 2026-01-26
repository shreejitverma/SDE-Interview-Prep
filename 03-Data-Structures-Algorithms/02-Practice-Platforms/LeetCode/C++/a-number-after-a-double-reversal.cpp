/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(1)
// Space: O(1)

class Solution {
public:
    bool isSameAfterReversals(int num) {
        return (num == 0 || num % 10);
    }
};
