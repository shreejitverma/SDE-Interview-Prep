'''
https://www.geeksforgeeks.org/find-maximum-possible-stolen-value-houses/

Find maximum possible stolen value from houses
Difficulty Level : Medium
Last Updated : 03 May, 2021
There are n houses build in a line, each of which contains some value in it. A thief is going to steal the maximal value of these houses, but he can’t steal in two adjacent houses because the owner of the stolen houses will tell his two neighbors left and right side. What is the maximum stolen value?
Examples: 
 

Input: hval[] = {6, 7, 1, 3, 8, 2, 4}
Output: 19

Explanation: The thief will steal 6, 1, 8 and 4 from the house.

Input: hval[] = {5, 3, 4, 11, 2}
Output: 16

Explanation: Thief will steal 5 and 11'''

# Python3 program to find the maximum stolen value

# calculate the maximum stolen value


def maximize_loot(hval, n):
    if n == 0:
        return 0
    if n == 1:
        return hval[0]
    if n == 2:
        return max(hval[0], hval[1])

    # dp[i] represent the maximum value stolen so
    # for after reaching house i.
    dp = [0]*n

    # Initialize the dp[0] and dp[1]
    dp[0] = hval[0]
    dp[1] = max(hval[0], hval[1])

    # Fill remaining positions
    for i in range(2, n):
        dp[i] = max(hval[i]+dp[i-2], dp[i-1])

    return dp[-1]

# Driver to test above code


def main():

    # Value of houses
    hval = [6, 7, 1, 3, 8, 2, 4]

    # number of houses
    n = len(hval)
    print("Maximum loot value : {}".
          format(maximize_loot(hval, n)))


if __name__ == '__main__':
    main()
# Python3 program to find the maximum stolen value

# calculate the maximum stolen value


def maximize_loot(hval, n):
    if n == 0:
        return 0

    value1 = hval[0]
    if n == 1:
        return value1

    value2 = max(hval[0], hval[1])
    if n == 2:
        return value2

    # contains maximum stolen value at the end
    max_val = None

    # Fill remaining positions
    for i in range(2, n):
        max_val = max(hval[i]+value1, value2)
        value1 = value2
        value2 = max_val

    return max_val

# Driver to test above code


def main():

    # Value of houses
    hval = [6, 7, 1, 3, 8, 2, 4]

    # number of houses
    n = len(hval)
    print("Maximum loot value : {}".format(maximize_loot(hval, n)))


if __name__ == '__main__':
    main()
