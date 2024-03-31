'''https: // practice.geeksforgeeks.org/problems/permutations-of-a-given-string2041/1
Permutations of a given string
Basic Accuracy: 49.85 % Submissions: 31222 Points: 1
Given a string S. The task is to print all permutations of a given string.


Example 1:

Input: ABC
Output:
ABC ACB BAC BCA CAB CBA
Explanation:
Given string ABC has permutations in 6
forms as ABC, ACB, BAC, BCA, CAB and CBA .
Example 2:

Input: ABSG
Output:
ABGS ABSG AGBS AGSB ASBG ASGB BAGS
BASG BGAS BGSA BSAG BSGA GABS GASB
GBAS GBSA GSAB GSBA SABG SAGB SBAG
SBGA SGAB SGBA
Explanation:
Given string ABSG has 24 permutations.


Your Task:
You don't need to read input or print anything. Your task is to complete the function find_permutaion() which takes the string S as input parameter and returns a vector of string in lexicographical order.


Expected Time Complexity: O(n! * n)

Expected Space Complexity: O(n)


Constraints:
1 <= length of string <= 5
'''


class Solution:

    def permute(self, curr, S, N, words=[]):

        if(len(curr) == N):
            words.append(curr)
            return
        for ch in S:
            self.permute(curr+ch, S.replace(ch, ""), N, words)

    def find_permutation(self, S):
        words = []
        N = len(S)
        self.permute("", ''.join(sorted(S)), N, words)
        return words


class Solution:

    def find_permutation(self, S):
        # Code here
        l = [S[-1]]
        for i in S[0:len(S)-1]:
            l1 = []
            for x in l:
                for j in range(0, len(x)+1):
                    l1.append(x[0:j]+i+x[j:len(x)])
            l = l1
        return sorted(l)
