'''
https://www.geeksforgeeks.org/word-break-problem-trie-solution/
Given an input string and a dictionary of words, 
find out if the input string can be segmented into a space-separated sequence of dictionary words. 
See following examples for more details. 
This is a famous Google interview question, also being asked by many other companies now a days.
 

Consider the following dictionary 
{ i, like, sam, sung, samsung, mobile, ice, 
  cream, icecream, man, go, mango}

Input:  ilike
Output: Yes 
The string can be segmented as "i like".

Input:  ilikesamsung
Output: Yes
The string can be segmented as "i like samsung" or 
"i like sam sung".'''


class Solution(object):
    def wordBreak(self, s, wordDict):
        """
        Author : @amitrajitbose
        :type s: str
        :type wordDict: List[str]
        :rtype: bool
        """
        """CREATING THE TRIE CLASS"""

        class TrieNode(object):

            def __init__(self):
                self.children = []  # will be of size = 26
                self.isLeaf = False

            def getNode(self):
                p = TrieNode()  # new trie node
                p.children = []
                for i in range(26):
                    p.children.append(None)
                p.isLeaf = False
                return p

            def insert(self, root, key):
                key = str(key)
                pCrawl = root
                for i in key:
                    index = ord(i)-97
                    if(pCrawl.children[index] == None):
                        # node has to be initialised
                        pCrawl.children[index] = self.getNode()
                    pCrawl = pCrawl.children[index]
                pCrawl.isLeaf = True  # marking end of word

            def search(self, root, key):
                # print("Searching %s" %key) #DEBUG
                pCrawl = root
                for i in key:
                    index = ord(i)-97
                    if(pCrawl.children[index] == None):
                        return False
                    pCrawl = pCrawl.children[index]
                if(pCrawl and pCrawl.isLeaf):
                    return True

        def checkWordBreak(strr, root):
            n = len(strr)
            if(n == 0):
                return True
            for i in range(1, n+1):
                if(root.search(root, strr[:i]) and checkWordBreak(strr[i:], root)):
                    return True
            return False

        """IMPLEMENT SOLUTION"""
        root = TrieNode().getNode()
        for w in wordDict:
            root.insert(root, w)
        out = checkWordBreak(s, root)
        if(out):
            return "Yes"
        else:
            return "No"


print(Solution().wordBreak("thequickbrownfox",
      ["the", "quick", "fox", "brown"]))
print(Solution().wordBreak("bedbathandbeyond", [
      "bed", "bath", "bedbath", "and", "beyond"]))
print(Solution().wordBreak("bedbathandbeyond", [
      "teddy", "bath", "bedbath", "and", "beyond"]))
print(Solution().wordBreak("bedbathandbeyond", [
      "bed", "bath", "bedbath", "and", "away"]))
