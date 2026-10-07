/*
 * Problem: LeetCode 150 - Evaluate Reverse Polish Notation
 * Difficulty: Medium
 * Concepts: Stack, Math, Array
 *
 * Time Complexity: O(n)
 * Space Complexity: O(n)
 */

pub struct Solution;

impl Solution {
    pub fn eval_rpn(tokens: Vec<String>) -> i32 {
        let mut stack: Vec<i32> = Vec::with_capacity(tokens.len());

        for token in tokens {
            match token.as_str() {
                "+" => {
                    let b = stack.pop().unwrap();
                    let a = stack.pop().unwrap();
                    stack.push(a + b);
                }
                "-" => {
                    let b = stack.pop().unwrap();
                    let a = stack.pop().unwrap();
                    stack.push(a - b);
                }
                "*" => {
                    let b = stack.pop().unwrap();
                    let a = stack.pop().unwrap();
                    stack.push(a * b);
                }
                "/" => {
                    let b = stack.pop().unwrap();
                    let a = stack.pop().unwrap();
                    stack.push(a / b);
                }
                _ => {
                    stack.push(token.parse::<i32>().unwrap());
                }
            }
        }

        stack[0]
    }
}
