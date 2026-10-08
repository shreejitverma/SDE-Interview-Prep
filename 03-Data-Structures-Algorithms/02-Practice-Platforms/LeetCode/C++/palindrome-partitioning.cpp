#include <string>
#include <vector>

using namespace std;

/*
 * Problem: LeetCode 131 - Palindrome Partitioning
 * Difficulty: Medium
 * Concepts: Backtracking, String, Dynamic Programming
 *
 * Time Complexity: O(n * 2^n)
 * Space Complexity: O(n) recursion stack
 */

class Solution {
public:
    vector<vector<string>> partition(string s) {
        vector<vector<string>> result;
        vector<string> current;
        backtrack(s, 0, current, result);
        return result;
    }

private:
    void backtrack(const string& s, int start, vector<string>& current, vector<vector<string>>& result) {
        if (start == static_cast<int>(s.length())) {
            result.push_back(current);
            return;
        }

        for (int end = start; end < static_cast<int>(s.length()); ++end) {
            if (isPalindrome(s, start, end)) {
                current.push_back(s.substr(start, end - start + 1));
                backtrack(s, end + 1, current, result);
                current.pop_back();
            }
        }
    }

    bool isPalindrome(const string& s, int left, int right) {
        while (left < right) {
            if (s[left++] != s[right--]) {
                return false;
            }
        }
        return true;
    }
};
