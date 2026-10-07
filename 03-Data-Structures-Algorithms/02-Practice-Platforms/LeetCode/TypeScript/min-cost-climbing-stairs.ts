/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 746 - Min Cost Climbing Stairs
 * Language: TypeScript
 *
 * Complexity:
 * - Time: O(N)
 * - Space: O(1)
 */

function minCostClimbingStairs(cost: number[]): number {
    let prev2 = 0;
    let prev1 = 0;

    for (const c of cost) {
        const curr = c + Math.min(prev1, prev2);
        prev2 = prev1;
        prev1 = curr;
    }

    return Math.min(prev1, prev2);
}
