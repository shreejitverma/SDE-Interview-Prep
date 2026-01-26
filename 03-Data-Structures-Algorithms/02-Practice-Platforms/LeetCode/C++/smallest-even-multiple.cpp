/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(1)
// Space: O(1)

// math, bit manipulation
class Solution {
public:
    int smallestEvenMultiple(int n) {
        return n << (n & 1);
    }
};
