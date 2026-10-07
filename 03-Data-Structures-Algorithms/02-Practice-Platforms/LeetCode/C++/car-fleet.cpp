/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n log n)
// Space: O(n)

#include <vector>
#include <algorithm>

class Solution {
public:
    int carFleet(int target, const std::vector<int>& position, const std::vector<int>& speed) {
        const size_t n = position.size();
        if (n == 0) return 0;

        std::vector<std::pair<int, double>> cars;
        cars.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            const double time = static_cast<double>(target - position[i]) / speed[i];
            cars.emplace_back(position[i], time);
        }

        std::sort(cars.begin(), cars.end());

        int fleets = 0;
        double max_time = 0.0;

        for (int i = static_cast<int>(n) - 1; i >= 0; --i) {
            if (cars[i].second > max_time) {
                max_time = cars[i].second;
                ++fleets;
            }
        }

        return fleets;
    }
};
