/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(1)

class Solution {
public:
    int minPartitions(string n) {
        return *max_element(cbegin(n), cend(n)) - '0';
    }
};
