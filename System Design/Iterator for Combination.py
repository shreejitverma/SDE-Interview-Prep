'''https://leetcode.com/problems/iterator-for-combination/
1286. Iterator for Combination
Medium

688

53

Add to List

Share
Design the CombinationIterator class:

CombinationIterator(string characters, int combinationLength) Initializes the object with a string characters of sorted distinct lowercase English letters and a number combinationLength as arguments.
next() Returns the next combination of length combinationLength in lexicographical order.
hasNext() Returns true if and only if there exists a next combination.
 

Example 1:

Input
["CombinationIterator", "next", "hasNext", "next", "hasNext", "next", "hasNext"]
[["abc", 2], [], [], [], [], [], []]
Output
[null, "ab", true, "ac", true, "bc", false]

Explanation
CombinationIterator itr = new CombinationIterator("abc", 2);
itr.next();    // return "ab"
itr.hasNext(); // return True
itr.next();    // return "ac"
itr.hasNext(); // return True
itr.next();    // return "bc"
itr.hasNext(); // return False
 

Constraints:

1 <= combinationLength <= characters.length <= 15
All the characters of characters are unique.
At most 104 calls will be made to next and hasNext.
It's guaranteed that all calls of the function next are valid.'''

# Time:  O(k), per operation
# Space: O(k)

import functools
import itertools


class CombinationIterator(object):

    def __init__(self, characters, combinationLength):
        """
        :type characters: str
        :type combinationLength: int
        """
        self.__it = itertools.combinations(characters, combinationLength)
        self.__curr = None
        self.__last = characters[-combinationLength:]

    def next(self):
        """
        :rtype: str
        """
        self.__curr = "".join(self.__it.next())
        return self.__curr

    def hasNext(self):
        """
        :rtype: bool
        """
        return self.__curr != self.__last


# Time:  O(k), per operation
# Space: O(k)


class CombinationIterator2(object):

    def __init__(self, characters, combinationLength):
        """
        :type characters: str
        :type combinationLength: int
        """
        self.__characters = characters
        self.__combinationLength = combinationLength
        self.__it = self.__iterative_backtracking()
        self.__curr = None
        self.__last = characters[-combinationLength:]

    def __iterative_backtracking(self):
        def conquer():
            if len(curr) == self.__combinationLength:
                return curr

        def prev_divide(c):
            curr.append(c)

        def divide(i):
            if len(curr) != self.__combinationLength:
                for j in reversed(range(i, len(self.__characters)-(self.__combinationLength-len(curr)-1))):
                    stk.append(functools.partial(post_divide))
                    stk.append(functools.partial(divide, j+1))
                    stk.append(functools.partial(
                        prev_divide, self.__characters[j]))
            stk.append(functools.partial(conquer))

        def post_divide():
            curr.pop()

        curr = []
        stk = [functools.partial(divide, 0)]
        while stk:
            result = stk.pop()()
            if result is not None:
                yield result

    def next(self):
        """
        :rtype: str
        """
        self.__curr = "".join(next(self.__it))
        return self.__curr

    def hasNext(self):
        """
        :rtype: bool
        """
        return self.__curr != self.__last


# Your CombinationIterator object will be instantiated and called as such:
# obj = CombinationIterator(characters, combinationLength)
# param_1 = obj.next()
# param_2 = obj.hasNext()
