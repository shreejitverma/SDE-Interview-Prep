/*
 * Problem: LeetCode 150 - Evaluate Reverse Polish Notation
 * Difficulty: Medium
 * Concepts: Stack, Math, Array
 *
 * Time Complexity: O(n)
 * Space Complexity: O(n)
 */

export function evalRPN(tokens: string[]): number {
    const stack: number[] = [];

    for (const token of tokens) {
        if (token === "+" || token === "-" || token === "*" || token === "/") {
            const b = stack.pop()!;
            const a = stack.pop()!;

            if (token === "+") {
                stack.push(a + b);
            } else if (token === "-") {
                stack.push(a - b);
            } else if (token === "*") {
                stack.push(a * b);
            } else {
                stack.push(Math.trunc(a / b));
            }
        } else {
            stack.push(parseInt(token, 10));
        }
    }

    return stack[0];
}
