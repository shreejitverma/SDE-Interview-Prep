/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Space-Optimized 0/1 Knapsack & Subset Reconstruction
 * Standard: Modern C++20
 * Description: Demonstrates strict O(W) space optimization for 0/1 Knapsack via reverse
 *              inner loop iteration, and shows exact item reconstruction using bitset tracking.
 * 
 * Complexity:
 * - Time Complexity: O(N * W)
 * - Space Complexity: O(W) for value computation, O(N * W / 64) for bitset path reconstruction.
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

struct Item {
    int weight;
    int value;
    int id;
};

class KnapsackSolver {
public:
    // Pure O(W) space calculation when only maximum profit is needed
    static int solve_max_value(int capacity, const std::vector<Item>& items) {
        std::vector<int> dp(capacity + 1, 0);

        for (const auto& item : items) {
            // Reverse iteration ensures each item is used at most once
            for (int w = capacity; w >= item.weight; --w) {
                dp[w] = std::max(dp[w], dp[w - item.weight] + item.value);
            }
        }
        return dp[capacity];
    }

    // Space-efficient item subset reconstruction using a flat bit vector
    static std::pair<int, std::vector<int>> solve_with_reconstruction(int capacity, const std::vector<Item>& items) {
        int n = items.size();
        std::vector<int> dp(capacity + 1, 0);
        // keep[i][w] is true if item i was included in the optimal solution for capacity w
        std::vector<std::vector<bool>> keep(n, std::vector<bool>(capacity + 1, false));

        for (int i = 0; i < n; ++i) {
            for (int w = capacity; w >= items[i].weight; --w) {
                if (dp[w - items[i].weight] + items[i].value > dp[w]) {
                    dp[w] = dp[w - items[i].weight] + items[i].value;
                    keep[i][w] = true;
                }
            }
        }

        // Backtrack to recover chosen item IDs
        std::vector<int> chosen_ids;
        int rem_w = capacity;
        for (int i = n - 1; i >= 0; --i) {
            if (keep[i][rem_w]) {
                chosen_ids.push_back(items[i].id);
                rem_w -= items[i].weight;
            }
        }
        std::reverse(chosen_ids.begin(), chosen_ids.end());

        return {dp[capacity], chosen_ids};
    }
};

int main() {
    std::vector<Item> items = {
        {10, 60, 1},
        {20, 100, 2},
        {30, 120, 3}
    };
    int capacity = 50;

    int max_val = KnapsackSolver::solve_max_value(capacity, items);
    assert(max_val == 220);

    auto [val, chosen] = KnapsackSolver::solve_with_reconstruction(capacity, items);
    assert(val == 220);
    // Best subset is items 2 and 3 (weights 20 + 30 = 50, values 100 + 120 = 220)
    std::vector<int> expected = {2, 3};
    assert(chosen == expected);

    std::cout << "All Space-Optimized Knapsack tests passed successfully.\n";
    return 0;
}
