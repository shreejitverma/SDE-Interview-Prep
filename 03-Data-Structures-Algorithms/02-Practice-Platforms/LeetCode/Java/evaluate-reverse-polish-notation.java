/*
 * Problem: LeetCode 150 - Evaluate Reverse Polish Notation
 * Difficulty: Medium
 * Concepts: Stack, Math, Array
 *
 * Time Complexity: O(n)
 * Space Complexity: O(n)
 */

class Solution {
    public int evalRPN(String[] tokens) {
        int[] stack = new int[tokens.length];
        int top = 0;

        for (String token : tokens) {
            if (token.equals("+")) {
                int b = stack[--top];
                int a = stack[--top];
                stack[top++] = a + b;
            } else if (token.equals("-")) {
                int b = stack[--top];
                int a = stack[--top];
                stack[top++] = a - b;
            } else if (token.equals("*")) {
                int b = stack[--top];
                int a = stack[--top];
                stack[top++] = a * b;
            } else if (token.equals("/")) {
                int b = stack[--top];
                int a = stack[--top];
                stack[top++] = a / b;
            } else {
                stack[top++] = Integer.parseInt(token);
            }
        }

        return stack[0];
    }
}
