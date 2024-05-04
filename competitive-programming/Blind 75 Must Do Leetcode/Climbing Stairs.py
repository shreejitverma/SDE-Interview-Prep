'''https://leetcode.com/problems/climbing-stairs/
70. Climbing Stairs
Easy

8813

261

Add to List

Share
You are climbing a staircase. It takes n steps to reach the top.

Each time you can either climb 1 or 2 steps. In how many distinct ways can you climb to the top?

 

Example 1:

Input: n = 2
Output: 2
Explanation: There are two ways to climb to the top.
1. 1 step + 1 step
2. 2 steps
Example 2:

Input: n = 3
Output: 3
Explanation: There are three ways to climb to the top.
1. 1 step + 1 step + 1 step
2. 1 step + 2 steps
3. 2 steps + 1 step
 

Constraints:

1 <= n <= 45'''

# for given n steps stairs
# Each time you can either climb 1 or 2 steps.
# In how many distinct ways can you climb to the top?


class Solution:
    def climbStairs(self, n: int) -> int:
        """
        Find the number of distinct ways to climb to the top of the staircase.

        Args:
        n: int, the number of steps to reach the top.

        Returns:
        int, the number of distinct ways to climb to the top.
        """
        if n == 1:
            return 1
        elif n == 2:
            return 2

        # Initialize variables to store the number of ways to reach the current and previous steps
        prev_step = 1
        curr_step = 2

        # Iterate from step 3 to n, calculating the number of ways to reach each step
        for _ in range(3, n + 1):
            # Calculate the number of ways to reach the current step as the sum of ways to reach the previous two steps
            next_step = prev_step + curr_step
            # Update the values of prev_step and curr_step for the next iteration
            prev_step, curr_step = curr_step, next_step

        return curr_step


def brute_force(n):
    if n == 1 or n == 2:
        return n
    return brute_force(n-1)+brute_force(n-2)


memo = {}


def recursion_memo(n, memo):

    if n == 1 or n == 2:
        return n
    if n in memo.keys():
        return memo[n]
    value = recursion_memo(n-1, memo) + recursion_memo(n-2, memo)
    memo.update({n: value})
    return memo[n]


def dynamic(n):
    if n == 1 or n == 2:
        return n
    result = [0]*(n)
    result[0] = 1
    result[1] = 2
    for i in range(2, n):
        result[i] = result[i-1]+result[i-2]
    return result[n-1]


if __name__ == '__main__':
    print(recursion_memo(10, memo))
