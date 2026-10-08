/*
 * Problem: LeetCode 131 - Palindrome Partitioning
 * Difficulty: Medium
 * Concepts: Backtracking, String, Dynamic Programming
 *
 * Time Complexity: O(n * 2^n)
 * Space Complexity: O(n) recursion stack
 */

export function partition(s: string): string[][] {
    const result: string[][] = [];
    const current: string[] = [];

    function isPalindrome(left: number, right: number): boolean {
        while (left < right) {
            if (s[left++] !== s[right--]) {
                return false;
            }
        }
        return true;
    }

    function backtrack(start: number): void {
        if (start === s.length) {
            result.push([...current]);
            return;
        }

        for (let end = start; end < s.length; end++) {
            if (isPalindrome(start, end)) {
                current.push(s.slice(start, end + 1));
                backtrack(end + 1);
                current.pop();
            }
        }
    }

    backtrack(0);
    return result;
}
