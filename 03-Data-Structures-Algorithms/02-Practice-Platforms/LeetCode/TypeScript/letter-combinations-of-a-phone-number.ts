/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N * 4^N)
// Space: O(N) auxiliary (recursion stack)

function letterCombinations(digits: string): string[] {
    const result: string[] = [];
    if (!digits) {
        return result;
    }

    const mapping: string[] = [
        "", "", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"
    ];

    const path: string[] = [];

    function backtrack(index: number): void {
        if (index === digits.length) {
            result.push(path.join(""));
            return;
        }

        const digitChar = digits.charCodeAt(index) - 48;
        const letters = mapping[digitChar];
        for (let i = 0; i < letters.length; i++) {
            path.push(letters[i]);
            backtrack(index + 1);
            path.pop();
        }
    }

    backtrack(0);
    return result;
}
