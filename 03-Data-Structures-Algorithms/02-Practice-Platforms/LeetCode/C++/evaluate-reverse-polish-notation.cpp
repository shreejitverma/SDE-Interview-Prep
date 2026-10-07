#include <vector>
#include <string>

using namespace std;

/*
 * Problem: LeetCode 150 - Evaluate Reverse Polish Notation
 * Difficulty: Medium
 * Concepts: Stack, Math, Array
 *
 * Time Complexity: O(n)
 * Space Complexity: O(n)
 */

class Solution {
public:
    int evalRPN(vector<string>& tokens) {
        vector<int> st;

        for (const string& token : tokens) {
            if (token == "+" || token == "-" || token == "*" || token == "/") {
                int b = st.back();
                st.pop_back();
                int a = st.back();
                st.pop_back();

                if (token == "+") {
                    st.push_back(a + b);
                } else if (token == "-") {
                    st.push_back(a - b);
                } else if (token == "*") {
                    st.push_back(a * b);
                } else {
                    st.push_back(a / b);
                }
            } else {
                st.push_back(stoi(token));
            }
        }

        return st.back();
    }
};
