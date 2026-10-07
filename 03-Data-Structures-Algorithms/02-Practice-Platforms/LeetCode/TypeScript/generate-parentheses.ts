/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(4^N / sqrt(N))
// Space: O(N) auxiliary (recursion stack)

function generateParenthesis(n: number): string[] {
    const result: string[] = [];
    const path: string[] = [];

    function backtrack(openCount: number, closeCount: number): void {
        if (openCount === n && closeCount === n) {
            result.push(path.join(""));
            return;
        }

        if (openCount < n) {
            path.push("(");
            backtrack(openCount + 1, closeCount);
            path.pop();
        }

        if (closeCount < openCount) {
            path.push(")");
            backtrack(openCount, closeCount + 1);
            path.pop();
        }
    }

    backtrack(0, 0);
    return result;
}
