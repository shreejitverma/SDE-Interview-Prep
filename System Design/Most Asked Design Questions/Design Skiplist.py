'''https://leetcode.com/problems/design-skiplist/
1206. Design Skiplist
Hard

321

39

Add to List

Share
Design a Skiplist without using any built-in libraries.

A skiplist is a data structure that takes O(log(n)) time to add, erase and search. Comparing with treap and red-black tree which has the same function and performance, the code length of Skiplist can be comparatively short and the idea behind Skiplists is just simple linked lists.

For example, we have a Skiplist containing [30,40,50,60,70,90] and we want to add 80 and 45 into it. The Skiplist works this way:


Artyom Kalinin [CC BY-SA 3.0], via Wikimedia Commons

You can see there are many layers in the Skiplist. Each layer is a sorted linked list. With the help of the top layers, add, erase and search can be faster than O(n). It can be proven that the average time complexity for each operation is O(log(n)) and space complexity is O(n).

See more about Skiplist: https://en.wikipedia.org/wiki/Skip_list

Implement the Skiplist class:

Skiplist() Initializes the object of the skiplist.
bool search(int target) Returns true if the integer target exists in the Skiplist or false otherwise.
void add(int num) Inserts the value num into the SkipList.
bool erase(int num) Removes the value num from the Skiplist and returns true. If num does not exist in the Skiplist, do nothing and return false. If there exist multiple num values, removing any one of them is fine.
Note that duplicates may exist in the Skiplist, your code needs to handle this situation.

 

Example 1:

Input
["Skiplist", "add", "add", "add", "search", "add", "search", "erase", "erase", "search"]
[[], [1], [2], [3], [0], [4], [1], [0], [1], [1]]
Output
[null, null, null, null, false, null, true, false, true, false]

Explanation
Skiplist skiplist = new Skiplist();
skiplist.add(1);
skiplist.add(2);
skiplist.add(3);
skiplist.search(0); // return False
skiplist.add(4);
skiplist.search(1); // return True
skiplist.erase(0);  // return False, 0 is not in skiplist.
skiplist.erase(1);  // return True
skiplist.search(1); // return False, 1 has already been erased.
 

Constraints:

0 <= num, target <= 2 * 104
At most 5 * 104 calls will be made to search, add, and erase.'''

# Time:  O(logn) on average for each operation
# Space: O(n)

# see proof in references:
# 1. https://kunigami.blog/2012/09/25/skip-lists-in-python/
# 2. https://opendatastructures.org/ods-cpp/4_4_Analysis_Skiplists.html
# 3. https://brilliant.org/wiki/skip-lists/
import random


class SkipNode(object):
    def __init__(self, level=0, num=None):
        self.num = num
        self.nexts = [None]*level


class Skiplist(object):
    P_NUMERATOR, P_DENOMINATOR = 1, 2  # P = 1/4 in redis implementation
    MAX_LEVEL = 32  # enough for 2^32 elements

    def __init__(self):
        self.__head = SkipNode()
        self.__len = 0

    def search(self, target):
        """
        :type target: int
        :rtype: bool
        """
        return True if self.__find(target, self.__find_prev_nodes(target)) else False

    def add(self, num):
        """
        :type num: int
        :rtype: None
        """
        node = SkipNode(self.__random_level(), num)
        if len(self.__head.nexts) < len(node.nexts):
            self.__head.nexts.extend(
                [None]*(len(node.nexts)-len(self.__head.nexts)))
        prevs = self.__find_prev_nodes(num)
        for i in range(len(node.nexts)):
            node.nexts[i] = prevs[i].nexts[i]
            prevs[i].nexts[i] = node
        self.__len += 1

    def erase(self, num):
        """
        :type num: int
        :rtype: bool
        """
        prevs = self.__find_prev_nodes(num)
        curr = self.__find(num, prevs)
        if not curr:
            return False
        self.__len -= 1
        for i in reversed(range(len(curr.nexts))):
            prevs[i].nexts[i] = curr.nexts[i]
            if not self.__head.nexts[i]:
                self.__head.nexts.pop()
        return True

    def __find(self, num, prevs):
        if prevs:
            candidate = prevs[0].nexts[0]
            if candidate and candidate.num == num:
                return candidate
        return None

    def __find_prev_nodes(self, num):
        prevs = [None]*len(self.__head.nexts)
        curr = self.__head
        for i in reversed(range(len(self.__head.nexts))):
            while curr.nexts[i] and curr.nexts[i].num < num:
                curr = curr.nexts[i]
            prevs[i] = curr
        return prevs

    def __random_level(self):
        level = 1
        while random.randint(1, Skiplist.P_DENOMINATOR) <= Skiplist.P_NUMERATOR and \
                level < Skiplist.MAX_LEVEL:
            level += 1
        return level

    def __len__(self):
        return self.__len

    def __str__(self):
        result = []
        for i in reversed(range(len(self.__head.nexts))):
            result.append([])
            curr = self.__head.nexts[i]
            while curr:
                result[-1].append(str(curr.num))
                curr = curr.nexts[i]
        return "\n".join(map(lambda x: "->".join(x), result))
