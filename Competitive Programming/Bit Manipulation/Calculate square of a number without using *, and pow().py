'''
https://www.geeksforgeeks.org/calculate-square-of-a-number-without-using-and-pow/
Calculate square of a number without using *, / and pow()
Difficulty Level : Medium
Last Updated : 23 Feb, 2021
Geek Week
Given an integer n, calculate the square of a number without using *, / and pow(). 

Examples : 

Input: n = 5
Output: 25

Input: 7
Output: 49

Input: n = 12
Output: 144
A Simple Solution is to repeatedly add n to result. 
'''

# Simple solution to
# calculate square without
# using * and pow()


def square(n):

    # handle negative input
    if (n < 0):
        n = -n

    # Initialize result
    res = n

    # Add n to res n-1 times
    for i in range(1, n):
        res += n

    return res


# Driver Code
for n in range(1, 6):
    print("n =", n, end=", ")
    print("n^2 =", square(n))
