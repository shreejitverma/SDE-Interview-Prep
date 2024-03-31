'''https://leetcode.com/problems/fancy-sequence/
1622. Fancy Sequence
Hard

217

79

Add to List

Share
Write an API that generates fancy sequences using the append, addAll, and multAll operations.

Implement the Fancy class:

Fancy() Initializes the object with an empty sequence.
void append(val) Appends an integer val to the end of the sequence.
void addAll(inc) Increments all existing values in the sequence by an integer inc.
void multAll(m) Multiplies all existing values in the sequence by an integer m.
int getIndex(idx) Gets the current value at index idx (0-indexed) of the sequence modulo 109 + 7. If the index is greater or equal than the length of the sequence, return -1.
 

Example 1:

Input
["Fancy", "append", "addAll", "append", "multAll", "getIndex", "addAll", "append", "multAll", "getIndex", "getIndex", "getIndex"]
[[], [2], [3], [7], [2], [0], [3], [10], [2], [0], [1], [2]]
Output
[null, null, null, null, null, 10, null, null, null, 26, 34, 20]

Explanation
Fancy fancy = new Fancy();
fancy.append(2);   // fancy sequence: [2]
fancy.addAll(3);   // fancy sequence: [2+3] -> [5]
fancy.append(7);   // fancy sequence: [5, 7]
fancy.multAll(2);  // fancy sequence: [5*2, 7*2] -> [10, 14]
fancy.getIndex(0); // return 10
fancy.addAll(3);   // fancy sequence: [10+3, 14+3] -> [13, 17]
fancy.append(10);  // fancy sequence: [13, 17, 10]
fancy.multAll(2);  // fancy sequence: [13*2, 17*2, 10*2] -> [26, 34, 20]
fancy.getIndex(0); // return 26
fancy.getIndex(1); // return 34
fancy.getIndex(2); // return 20
 

Constraints:

1 <= val, inc, m <= 100
0 <= idx <= 105
At most 105 calls total will be made to append, addAll, multAll, and getIndex.'''

# Time:  O(1)
# Space: O(n)

MOD = 10**9+7


class Fancy(object):

    def __init__(self):
        self.__arr = []
        self.__ops = [[1, 0]]

    def append(self, val):
        """
        :type val: int
        :rtype: None
        """
        self.__arr.append(val)
        self.__ops.append(self.__ops[-1][:])

    def addAll(self, inc):
        """
        :type inc: int
        :rtype: None
        """
        self.__ops[-1][1] = (self.__ops[-1][1]+inc) % MOD

    def multAll(self, m):
        """
        :type m: int
        :rtype: None
        """
        self.__ops[-1] = [(self.__ops[-1][0]*m) %
                          MOD, (self.__ops[-1][1]*m) % MOD]

    def getIndex(self, idx):
        """
        :type idx: int
        :rtype: int
        """
        if idx >= len(self.__arr):
            return -1
        a1, b1 = self.__ops[idx]
        a2, b2 = self.__ops[-1]
        a = a2*pow(a1, MOD-2, MOD) % MOD  # O(logMOD), we treat it as O(1) here
        b = (b2 - b1*a) % MOD
        return (self.__arr[idx]*a + b) % MOD


# Time:  O(1)
# Space: O(n)
class Fancy2(object):

    def __init__(self):
        self.__arr = []
        self.__op = [1, 0]

    def append(self, val):
        """
        :type val: int
        :rtype: None
        """
        self.__arr.append((val-self.__op[1])*pow(self.__op[0], MOD-2, MOD) %
                          MOD)  # O(logMOD), we treat it as O(1) here

    def addAll(self, inc):
        """
        :type inc: int
        :rtype: None
        """
        self.__op[1] = (self.__op[1]+inc) % MOD

    def multAll(self, m):
        """
        :type m: int
        :rtype: None
        """
        self.__op = [(self.__op[0]*m) % MOD, (self.__op[1]*m) % MOD]

    def getIndex(self, idx):
        """
        :type idx: int
        :rtype: int
        """
        if idx >= len(self.__arr):
            return -1
        a, b = self.__op
        return (self.__arr[idx]*a + b) % MOD
