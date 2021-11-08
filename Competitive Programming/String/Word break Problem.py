'''https://practice.geeksforgeeks.org/problems/word-break1352/1
https://www.geeksforgeeks.org/word-break-problem-dp-32/

Word Break
Medium Accuracy: 50.24% Submissions: 23034 Points: 4
Given a string A and a dictionary of n words B,
find out if A can be segmented into a space-separated sequence of dictionary words.

Note: From the dictionary B each word can be taken any number of times and in any order.
Example 1:

Input:
n = 12
B = { "i", "like", "sam",
"sung", "samsung", "mobile",
"ice","cream", "icecream",
"man", "go", "mango" }
A = "ilike"
Output:
1
Explanation:
The string can be segmented as "i like".

Example 2:

Input:
n = 12
B = { "i", "like", "sam",
"sung", "samsung", "mobile",
"ice","cream", "icecream",
"man", "go", "mango" }
A = "ilikesamsung"
Output:
1
Explanation:
The string can be segmented as
"i like samsung" or "i like sam sung".


Your Task:
Complete wordBreak() function which takes a string and list of strings as a parameter
and returns 1 if it is possible to break words, else return 0. You don't need to read any input or print any output,
it is done by driver code.


Expected time complexity: O(s2)

Expected auxiliary space: O(s) , where s = length of string A



Constraints:
1 ≤ N ≤ 12
1 ≤ s ≤ 1100 , where s = length of string A
 The length of each word is less than 15.'''


def wordBreak(line, dictionary):
    # Complete this function
    d = set(dictionary)

    def _wordBreak(word):
        n = len(word)
        if n == 0:
            return True
        for i in range(1, n+1):
            if word[0:i] in d and _wordBreak(word[i:n]):
                return True
        return False

    return _wordBreak(line)

  def wordBreak(wordList, word):
       if word == '':
            return True
        else:
            wordLen = len(word)
            return any([(word[:i] in wordList) and wordBreak(wordList, word[i:]) for i in range(1, wordLen+1)])
