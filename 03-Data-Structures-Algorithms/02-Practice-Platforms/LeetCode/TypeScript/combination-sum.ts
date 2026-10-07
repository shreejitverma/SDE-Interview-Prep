/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N^(T/M + 1)) where N = candidates count, T = target, M = min(candidates)
// Space: O(T/M) auxiliary (recursion stack depth)

function combinationSum(candidates: number[], target: number): number[][] {
    candidates.sort((a, b) => a - b);
    const result: number[][] = [];
    const path: number[] = [];

    function backtrack(remain: number, start: number): void {
        if (remain === 0) {
            result.push([...path]);
            return;
        }

        for (let i = start; i < candidates.length; i++) {
            if (candidates[i] > remain) {
                break;
            }
            path.push(candidates[i]);
            backtrack(remain - candidates[i], i);
            path.pop();
        }
    }

    backtrack(target, 0);
    return result;
}
