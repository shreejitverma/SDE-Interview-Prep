/*
https://leetcode.com/problems/number-of-1-bits/
191. Number of 1 Bits
Easy

2008

701

Add to List

Share
Write a function that takes an unsigned integer and returns the number of '1' bits it has (also known as the Hamming weight).

Note:

Note that in some languages, such as Java, there is no unsigned integer type. In this case, the input will be given as a signed integer type. It should not affect your implementation, as the integer's internal binary representation is the same, whether it is signed or unsigned.
In Java, the compiler represents the signed integers using 2's complement notation. Therefore, in Example 3, the input represents the signed integer. -3.
 

Example 1:

Input: n = 00000000000000000000000000001011
Output: 3
Explanation: The input binary string 00000000000000000000000000001011 has a total of three '1' bits.
Example 2:

Input: n = 00000000000000000000000010000000
Output: 1
Explanation: The input binary string 00000000000000000000000010000000 has a total of one '1' bit.
Example 3:

Input: n = 11111111111111111111111111111101
Output: 31
Explanation: The input binary string 11111111111111111111111111111101 has a total of thirty one '1' bits.
 

Constraints:

The input must be a binary string of length 32.
 

Follow up: If this function is called many times, how would you optimize it?
*/

//Runtime: 4 ms, faster than 100.00% of C++ online submissions for Number of 1 Bits.
//Memory Usage: 8.1 MB, less than 53.17% of C++ online submissions for Number of 1 Bits.

/**
Approach #1 (Loop and Flip) [Accepted]
**/

/**
Complexity Analysis
The run time depends on the number of bits in nn. Because nn in this piece of code is a 32-bit integer, the time complexity is O(1)O(1).
The space complexity is O(1)O(1), since no additional space is allocated.
**/

class Solution
{
public:
    int hammingWeight(uint32_t n)
    {
        int ans = 0;
        while (n > 0)
        {
            if (n & 1 == 1)
            {
                ans++;
                // cout << ans << endl;
            }
            n >>= 1;
        }
        return ans;
    }
};

/**
Approach #2 (Bit Manipulation Trick) [Accepted]
**/

/**
Complexity Analysis
The run time depends on the number of 11-bits in nn. In the worst case, all bits in nn are 11-bits. In case of a 32-bit integer, the run time is O(1)O(1).
The space complexity is O(1)O(1), since no additional space is allocated.
**/

//Runtime: 4 ms, faster than 100.00% of C++ online submissions for Number of 1 Bits.
//Memory Usage: 8.1 MB, less than 67.61% of C++ online submissions for Number of 1 Bits.

/**
class Solution {
public:
    int hammingWeight(uint32_t n) {
        int ans = 0;
        //the loop will be executed for k times if n has k 1-bit
        while(n > 0){
            ans++;
            //convert the last significant bit of n to 0
            n &= (n-1);
        }
        return ans;
    }
};
**/