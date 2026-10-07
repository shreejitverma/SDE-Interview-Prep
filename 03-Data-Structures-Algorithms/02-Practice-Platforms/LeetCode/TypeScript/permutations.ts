/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N * N!)
// Space: O(N) auxiliary (recursion stack depth)

function permute(nums: number[]): number[][] {
    const result: number[][] = [];
    const arr = [...nums];

    function backtrack(first: number): void {
        if (first === arr.length) {
            result.push([...arr]);
            return;
        }

        for (let i = first; i < arr.length; i++) {
            [arr[first], arr[i]] = [arr[i], arr[first]];
            backtrack(first + 1);
            [arr[first], arr[i]] = [arr[i], arr[first]];
        }
    }

    backtrack(0);
    return result;
}
