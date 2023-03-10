'''https://leetcode.com/problems/search-suggestions-system/
1268. Search Suggestions System
Medium

1661

110

Add to List

Share
Given an array of strings products and a string searchWord. We want to design a system that suggests at most three product names from products after each character of searchWord is typed. Suggested products should have common prefix with the searchWord. If there are more than three products with a common prefix return the three lexicographically minimums products.

Return list of lists of the suggested products after each character of searchWord is typed. 

 

Example 1:

Input: products = ["mobile","mouse","moneypot","monitor","mousepad"], searchWord = "mouse"
Output: [
["mobile","moneypot","monitor"],
["mobile","moneypot","monitor"],
["mouse","mousepad"],
["mouse","mousepad"],
["mouse","mousepad"]
]
Explanation: products sorted lexicographically = ["mobile","moneypot","monitor","mouse","mousepad"]
After typing m and mo all products match and we show user ["mobile","moneypot","monitor"]
After typing mou, mous and mouse the system suggests ["mouse","mousepad"]
Example 2:

Input: products = ["havana"], searchWord = "havana"
Output: [["havana"],["havana"],["havana"],["havana"],["havana"],["havana"]]
Example 3:

Input: products = ["bags","baggage","banner","box","cloths"], searchWord = "bags"
Output: [["baggage","bags","banner"],["baggage","bags","banner"],["baggage","bags"],["bags"]]
Example 4:

Input: products = ["havana"], searchWord = "tatiana"
Output: [[],[],[],[],[],[],[]]
 

Constraints:

1 <= products.length <= 1000
There are no repeated elements in products.
1 <= Σ products[i].length <= 2 * 10^4
All characters of products[i] are lower-case English letters.
1 <= searchWord.length <= 1000
All characters of searchWord are lower-case English letters.'''

# Time:  ctor: O(n * l), n is the number of products
#                      , l is the average length of product name
#        suggest: O(l^2)
# Space: O(t), t is the number of nodes in trie

import bisect
import collections


class TrieNode(object):

    def __init__(self):
        self.__TOP_COUNT = 3
        self.leaves = collections.defaultdict(TrieNode)
        self.infos = []

    def insert(self, words, i):
        curr = self
        for c in words[i]:
            curr = curr.leaves[c]
            curr.add_info(words, i)

    def add_info(self, words, i):
        self.infos.append(i)
        self.infos.sort(key=lambda x: words[x])
        if len(self.infos) > self.__TOP_COUNT:
            self.infos.pop()


class Solution(object):
    def suggestedProducts(self, products, searchWord):
        """
        :type products: List[str]
        :type searchWord: str
        :rtype: List[List[str]]
        """
        trie = TrieNode()
        for i in range(len(products)):
            trie.insert(products, i)
        result = [[] for _ in range(len(searchWord))]
        for i, c in enumerate(searchWord):
            if c not in trie.leaves:
                break
            trie = trie.leaves[c]
            result[i] = map(lambda x: products[x], trie.infos)
        return result


# Time:  ctor: O(n * l * log(n * l)), n is the number of products
#                                   , l is the average length of product name
#        suggest: O(l^2)
# Space: O(t), t is the number of nodes in trie
class TrieNode2(object):

    def __init__(self):
        self.__TOP_COUNT = 3
        self.leaves = collections.defaultdict(TrieNode2)
        self.infos = []

    def insert(self, words, i):
        curr = self
        for c in words[i]:
            curr = curr.leaves[c]
            curr.add_info(i)

    def add_info(self, i):
        if len(self.infos) == self.__TOP_COUNT:
            return
        self.infos.append(i)


class Solution2(object):
    def suggestedProducts(self, products, searchWord):
        """
        :type products: List[str]
        :type searchWord: str
        :rtype: List[List[str]]
        """
        products.sort()
        trie = TrieNode2()
        for i in range(len(products)):
            trie.insert(products, i)
        result = [[] for _ in range(len(searchWord))]
        for i, c in enumerate(searchWord):
            if c not in trie.leaves:
                break
            trie = trie.leaves[c]
            result[i] = map(lambda x: products[x], trie.infos)
        return result


# Time:  ctor: O(n * l * log(n * l)), n is the number of products
#                                   , l is the average length of product name
#        suggest: O(l^2 * n)
# Space: O(n * l)


class Solution3(object):
    def suggestedProducts(self, products, searchWord):
        """
        :type products: List[str]
        :type searchWord: str
        :rtype: List[List[str]]
        """
        products.sort()  # Time: O(n * l * log(n * l))
        result = []
        prefix = ""
        for i, c in enumerate(searchWord):  # Time: O(l)
            prefix += c
            start = bisect.bisect_left(products, prefix)  # Time: O(log(n * l))
            new_products = []
            for j in range(start, len(products)):  # Time: O(n * l)
                if not (i < len(products[j]) and products[j][i] == c):
                    break
                new_products.append(products[j])
            products = new_products
            result.append(products[:3])
        return result
