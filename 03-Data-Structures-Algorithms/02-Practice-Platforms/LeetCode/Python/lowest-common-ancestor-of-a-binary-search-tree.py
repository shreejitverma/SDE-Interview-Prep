# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma

# Time:  O(H) where H is tree height
# Space: O(1) auxiliary


class TreeNode:
    def __init__(self, x: int):
        self.val = x
        self.left: TreeNode | None = None
        self.right: TreeNode | None = None


class Solution:
    def lowestCommonAncestor(
        self, root: TreeNode, p: TreeNode, q: TreeNode
    ) -> TreeNode:
        small = min(p.val, q.val)
        large = max(p.val, q.val)

        curr: TreeNode | None = root
        while curr:
            if curr.val > large:
                curr = curr.left
            elif curr.val < small:
                curr = curr.right
            else:
                return curr

        return root
