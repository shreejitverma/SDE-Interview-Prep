/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 875 - Koko Eating Bananas
 * Language: TypeScript
 *
 * Complexity:
 * - Time: O(N * log(max(piles)))
 * - Space: O(1)
 */

function minEatingSpeed(piles: number[], h: number): number {
    let left = 1;
    let right = Math.max(...piles);

    const canFinish = (k: number): boolean => {
        let hours = 0;
        for (const pile of piles) {
            hours += Math.ceil(pile / k);
        }
        return hours <= h;
    };

    while (left <= right) {
        const mid = Math.floor(left + (right - left) / 2);
        if (canFinish(mid)) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    return left;
}
