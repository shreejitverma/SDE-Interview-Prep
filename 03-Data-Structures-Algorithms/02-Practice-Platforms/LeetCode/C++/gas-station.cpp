#include <vector>
#include <numeric>

using namespace std;

/*
 * Problem: LeetCode 134 - Gas Station
 * Difficulty: Medium
 * Concepts: Greedy, Array
 *
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 */

class Solution {
public:
    int canCompleteCircuit(vector<int>& gas, vector<int>& cost) {
        int total_tank = 0;
        int current_tank = 0;
        int start_index = 0;

        for (int i = 0; i < static_cast<int>(gas.size()); ++i) {
            int balance = gas[i] - cost[i];
            total_tank += balance;
            current_tank += balance;

            if (current_tank < 0) {
                start_index = i + 1;
                current_tank = 0;
            }
        }

        return total_tank >= 0 ? start_index : -1;
    }
};
