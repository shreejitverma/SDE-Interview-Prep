'''
https://practice.geeksforgeeks.org/problems/print-anagrams-together/1
https://www.geeksforgeeks.org/given-a-sequence-of-words-print-all-anagrams-together/
Print Anagrams Together 
Medium Accuracy: 56.1% Submissions: 20996 Points: 4
Given an array of strings, return all groups of strings that are anagrams. 
The groups must be created in order of their appearance in the original array. 
Look at the sample case for clarification.


Example 1:

Input:
N = 5
words[] = {act,god,cat,dog,tac}
Output: 
god dog
act cat tac
Explanation:
There are 2 groups of
anagrams "god", "dog" make group 1.
"act", "cat", "tac" make group 2.
Example 2:

Input:
N = 3
words[] = {no,on,is}
Output: 
no on
is
Explanation:
There are 2 groups of
anagrams "no", "on" make group 1.
"is" makes group 2. 

Your Task:
The task is to complete the function Anagrams() that takes a list of strings as input 
and returns a list of groups such that each group consists of all the strings that are anagrams.


Expected Time Complexity: O(N*|S|*log|S|), where |S| is the length of the strings.
Expected Auxiliary Space: O(N*|S|), where |S| is the length of the strings.


Constraints:
1<=N<=100


A simple method is to create a Hash Table. 
Calculate the hash value of each word in such a way that all anagrams have the same hash value. 
Populate the Hash Table with these hash values. Finally, print those words together with same hash values. 
A simple hashing mechanism can be modulo sum of all characters. 
With modulo sum, two non-anagram words may have same hash value. 
This can be handled by matching individual characters.

Following is another method to print all anagrams together. 
Take two auxiliary arrays, index array and word array. 
Populate the word array with the given sequence of words. 
Sort each individual word of the word array. 
Finally, sort the word array and keep track of the corresponding indices. 
After sorting, all the anagrams cluster together. 
Use the index array to print the strings from the original array of strings.



Let us understand the steps with following input Sequence of Words: 

"cat", "dog", "tac", "god", "act"
1) Create two auxiliary arrays index[] and words[]. 
Copy all given words to words[] and store the original indexes in index[] 

index[]:  0   1   2   3   4
words[]: cat dog tac god act
2) Sort individual words in words[]. Index array doesn’t change.

index[]:   0    1    2    3    4
words[]:  act  dgo  act  dgo  act
3) Sort the words array. Compare individual words using strcmp() to sort

index:     0    2    4    1    3
words[]:  act  act  act  dgo  dgo
4) All anagrams come together. But words are changed in words array. 
To print the original words, take index from the index array and use it in the original array. 
We get "cat tac act dog god"
Following are the implementations of the above algorithm. 
In the following program, an array of structure “Word” is used to store both index and word arrays. 
DupArray is another structure that stores array of structure “Word”. '''

# A Python program to print all anagrams together

# structure for each word of duplicate array
from collections import Counter, defaultdict


class Solution:
    def Anagrams(self, words, size):
        '''
        words: list of word
        n:      no of words
        return : list of group of anagram {list will be sorted in driver code (not word in grp)}
        '''

        m = defaultdict(list)

        # loop over all the words
        for word in words:
            # Counter('cat') :
            #    counts the frequency of the characters present in a string
            #    >>> Counter({'c': 1, 'a': 1, 't': 1})

            # frozenset(dict(Counter('cat')).items()) :
            #    frozenset takes an iterable object as input and makes them immutable.
            #    So that hash(frozenset(Counter('cat'))) is equal to
            #   hash of other 'cat' anagrams
            #    >>> frozenset({('c', 1), ('a', 1), ('t', 1)})
            m[frozenset(dict(Counter(word)).items())].append(word)
        return [v for k, v in m.items()]


class Word(object):
    def __init__(self, string, index):
        self.string = string
        self.index = index

# Create a DupArray object that contains an array
# of Words


def createDupArray(string, size):
    dupArray = []

    # One by one copy words from the given wordArray
    # to dupArray
    for i in xrange(size):
        dupArray.append(Word(string[i], i))

    return dupArray

# Given a list of words in wordArr[]


def printAnagramsTogether(wordArr, size):
    # Step 1: Create a copy of all words present in
    # given wordArr.
    # The copy will also have original indexes of words
    dupArray = createDupArray(wordArr, size)

    # Step 2: Iterate through all words in dupArray and sort
    # individual words.
    for i in xrange(size):
        dupArray[i].string = ''.join(sorted(dupArray[i].string))

    # Step 3: Now sort the array of words in dupArray
    dupArray = sorted(dupArray, key=lambda k: k.string)

    # Step 4: Now all words in dupArray are together, but
    # these words are changed. Use the index member of word
    # struct to get the corresponding original word
    for word in dupArray:
        print wordArr[word.index],


# Driver program
wordArr = ["cat", "dog", "tac", "god", "act"]
size = len(wordArr)
printAnagramsTogether(wordArr, size)
