/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n log(max_pile))
// Space: O(1)

#include <vector>
#include <algorithm>

class Solution {
public:
    int minEatingSpeed(const std::vector<int>& piles, int h) {
        int left = 1;
        int right = *std::max_element(piles.begin(), piles.end());

        while (left <= right) {
            const int mid = left + (right - left) / 2;
            if (canFinish(piles, h, mid)) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }
        return left;
    }

private:
    bool canFinish(const std::vector<int>& piles, int h, int k) {
        long long hours = 0;
        for (const int pile : piles) {
            hours += (static_cast<long long>(pile) + k - 1) / k;
        }
        return hours <= h;
    }
};
