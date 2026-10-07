/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 875 - Koko Eating Bananas
 * Language: Java
 *
 * Complexity:
 * - Time: O(N * log(max(piles)))
 * - Space: O(1)
 */

class Solution {
    public int minEatingSpeed(int[] piles, int h) {
        int left = 1;
        int right = 1;
        for (int pile : piles) {
            if (pile > right) {
                right = pile;
            }
        }

        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (canFinish(piles, h, mid)) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }

        return left;
    }

    private boolean canFinish(int[] piles, int h, int k) {
        long hours = 0;
        for (int pile : piles) {
            hours += (pile + k - 1L) / k;
        }
        return hours <= h;
    }
}
