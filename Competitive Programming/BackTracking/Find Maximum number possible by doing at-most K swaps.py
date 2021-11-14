'''https://practice.geeksforgeeks.org/problems/largest-number-in-k-swaps-1587115620/1
Largest number in K swaps 
Medium Accuracy: 46.92% Submissions: 26080 Points: 4
Given a number K and string str of digits denoting a positive integer, build the largest number possible by performing swap operations on the digits of str at most K times.


Example 1:

Input:
K = 4
str = "1234567"
Output:
7654321
Explanation:
Three swaps can make the
input 1234567 to 7654321, swapping 1
with 7, 2 with 6 and finally 3 with 5
Example 2:

Input:
K = 3
str = "3435335"
Output:
5543333
Explanation:
Three swaps can make the input
3435335 to 5543333, swapping 3 
with 5, 4 with 5 and finally 3 with 4 

Your task:
You don't have to read input or print anything. Your task is to complete the function findMaximumNum() which takes the string and an integer as input and returns a string containing the largest number formed by perfoming the swap operation at most k times.


Expected Time Complexity: O(n!/(n-k)!) , where n = length of input string
Expected Auxiliary Space: O(n)


Constraints:
1 ≤ |str| ≤ 30
1 ≤ K ≤ 10'''


class Solution:

    # Function to find the largest number after k swaps.
    def findMaximumNum(self, s, k):
        # code here
        res = [-2**31]
        s = [int(x) for x in s]

        def solve(curr, s, res, k):
            if k == 0 or curr == len(s):
                return
            maxi = s[curr]
            for i in range(curr+1, len(s)):
                maxi = max(maxi, s[i])
            if maxi != s[curr]:
                k -= 1
            for j in range(len(s)-1, curr-1, -1):
                if s[j] == maxi:
                    t = s[curr]
                    s[curr] = s[j]
                    s[j] = t
                    x = ""
                    for i in s:
                        x += str(i)
                    z = int("".join(x))
                    res[0] = max(res[0], z)
                    solve(curr+1, s, res, k)
                    t = s[curr]
                    s[curr] = s[j]
                    s[j] = t
        solve(0, s, res, k)
        return res[0]


# {
#  Driver Code Starts
# Initial Template for Python 3

if __name__ == "__main__":
    for _ in range(int(input())):
        k = int(input())
        s = input()
        ob = Solution()
        print(ob.findMaximumNum(s, k))

# } Driver Code Ends
