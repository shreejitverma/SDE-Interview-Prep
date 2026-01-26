/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(1)

// hash table
class Solution {
public:
    int minimizedStringLength(string s) {
        return size(unordered_set<char>(cbegin(s), cend(s)));
    }
};
