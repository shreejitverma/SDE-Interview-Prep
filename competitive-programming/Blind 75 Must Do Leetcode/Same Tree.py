'''https://leetcode.com/problems/same-tree/
100. Same Tree
Easy

4237

107

Add to List

Share
Given the roots of two binary trees p and q, write a function to check if they are the same or not.

Two binary trees are considered the same if they are structurally identical, and the nodes have the same value.

 

Example 1:


Input: p = [1,2,3], q = [1,2,3]
Output: true
Example 2:


Input: p = [1,2], q = [1,null,2]
Output: false
Example 3:


Input: p = [1,2,1], q = [1,1,2]
Output: false
 

Constraints:

The number of nodes in both trees is in the range [0, 100].
-104 <= Node.val <= 104'''


class TreeNode:
    def __init__(self, x):
        self.val = x
        self.left = None
        self.right = None


class Solution:
    def isSameTree(self, p: TreeNode, q: TreeNode) -> bool:
        return self.isSameTreeIter(p, q)
       # return p and q and p.val == q.val and all(map(self.isSameTree, (p.left, p.right), (q.left, q.right))) or p is q

    def isSameTreeRecurent(self, p: TreeNode, q: TreeNode) -> bool:
        if not p or not q:
            return True if not q and not p else False
        return p.val == q.val and self.isSameTree(p.left, q.left) and self.isSameTree(p.right, q.right)

    def isSameTreeIter(self, p: TreeNode, q: TreeNode) -> bool:
        if not p or not q:
            return True if not q and not p else False
        stack_p, stack_q = [], []
        while p and q or stack_p and stack_q:
            while p and q:
                stack_p.append(p)
                stack_q.append(q)
                p = p.left
                q = q.left
            if p or q:
                return False
            if stack_p and stack_q:
                p, q = stack_p.pop(), stack_q.pop()
                if p.val != q.val:
                    return False
                p, q = p.right, q.right
        return True
# if __name__ == "__main__":
#     a = TreeNode(1)
#     a.right = TreeNode(2)
#     a.right.right = TreeNode(3)
#     s = Solution()
#     print(s.isSameTreeIter(a,a))

# Time:  O(n)
# Space: O(h), h is height of binary tree


class TreeNode(object):
    def __init__(self, x):
        self.val = x
        self.left = None
        self.right = None


class Solution(object):
    # @param p, a tree node
    # @param q, a tree node
    # @return a boolean
    def isSameTree(self, p, q):
        if p is None and q is None:
            return True

        if p is not None and q is not None:
            return p.val == q.val and self.isSameTree(p.left, q.left) and self.isSameTree(p.right, q.right)

        return False
