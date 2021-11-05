'''
https://practice.geeksforgeeks.org/problems/reach-a-given-score-1587115621/1
https://www.geeksforgeeks.org/count-number-ways-reach-given-score-game/
Reach a given score 
Easy Accuracy: 67.49% Submissions: 11561 Points: 2
Consider a game where a player can score 3 or 5 or 10 points in a move. Given a total score n, find number of distinct combinations to reach the given score.

Example:
Input
3
8
20
13
Output
1
4
2
Explanation
For 1st example when n = 8
{ 3, 5 } and {5, 3} are the 
two possible permutations but 
these represent the same 
cobmination. Hence output is 1.
Your Task:  
You don't need to read input or print anything. Your task is to complete the function count( ) which takes N as input parameter and returns the answer to the problem.

Constraints:
1 ≤ T ≤ 100
1 ≤ n ≤ 1000'''

# Python program to count number of possible ways to a given
# score can be reached in a game where a move can earn 3 or
# 5 or 10.

# Returns number of ways to reach score n.


def count(n):

    # table[i] will store count of solutions for value i.
    # Initialize all table values as 0.
    table = [0 for i in range(n+1)]

    # Base case (If given value is 0)
    table[0] = 1

    # One by one consider given 3 moves and update the
    # table[] values after the index greater than or equal
    # to the value of the picked move.
    for i in range(3, n+1):
        table[i] += table[i-3]
    for i in range(5, n+1):
        table[i] += table[i-5]
    for i in range(10, n+1):
        table[i] += table[i-10]

    return table[n]


# Driver Program
n = 20
print('Count for', n, 'is', count(n))

n = 13
print('Count for', n, 'is', count(n))
