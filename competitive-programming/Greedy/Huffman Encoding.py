'''
https://practice.geeksforgeeks.org/problems/huffman-encoding3345/1
https://youtu.be/co4_ahEDCho
Huffman Encoding 
Medium Accuracy: 37.28% Submissions: 5850 Points: 4
Given a string S of distinct character of size N and their corresponding frequency f[ ] i.e. character S[i] has f[i] frequency. Your task is to build the Huffman tree print all the huffman codes in preorder traversal of the tree.
Note: If two elements have same frequency, then the element which occur at first will be taken on the left of Binary Tree and other one to the right.

Example 1:

S = "abcdef"
f[] = {5, 9, 12, 13, 16, 45}
Output: 
0 100 101 1100 1101 111
Explanation:
HuffmanCodes will be:
f : 0
c : 100
d : 101
a : 1100
b : 1101
e : 111
Hence printing them in the PreOrder of Binary 
Tree.
Your Task:
You don't need to read or print anything. Your task is to complete the function huffmanCodes() which takes the given string S, frequency array f[ ] and number of characters N as input parameters and returns a vector of strings containing all huffman codes in order of preorder traversal of the tree.

Expected Time complexity: O(N * LogN) 
Expected Space complexity: O(N) 

Constraints:
1 ≤ N ≤ 26'''

from heapq import heappop, heappush
import heapq


class Node:
    def __init__(self, ch, freq, left=None, right=None):
        self.ch = ch
        self.freq = freq
        self.left = left
        self.right = right

    def __lt__(self, other):
        return self.freq < other.freq


class Solution:
    def huffmanCodes(self, S, f, N):
        # Code here
        pq = [Node(S[i], f[i]) for i in range(N)]
        heapq.heapify(pq)
        while len(pq) != 1:
            left = heappop(pq)
            right = heappop(pq)
            total = left.freq+right.freq
            heappush(pq, Node(None, total, left, right))

        def preorder(n, root):
            nonlocal ans
            if root.left is None and root.right is None:
                ans.append(n)
            if root.left:
                preorder(n+'0', root.left)
            if root.right:
                preorder(n+'1', root.right)
            return
        root = pq[0]
        ans = []
        preorder('', root)
        return ans
