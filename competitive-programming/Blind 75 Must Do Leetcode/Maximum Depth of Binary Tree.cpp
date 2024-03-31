/*
https://leetcode.com/problems/maximum-depth-of-binary-tree/
104. Maximum Depth of Binary Tree
Easy

5057

106

Add to List

Share
Given the root of a binary tree, return its maximum depth.

A binary tree's maximum depth is the number of nodes along the longest path from the root node down to the farthest leaf node.

 

Example 1:


Input: root = [3,9,20,null,null,15,7]
Output: 3
Example 2:

Input: root = [1,null,2]
Output: 2
Example 3:

Input: root = []
Output: 0
Example 4:

Input: root = [0]
Output: 1
 

Constraints:

The number of nodes in the tree is in the range [0, 104].
-100 <= Node.val <= 100
*/
/**
 * Definition for binary tree
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
class Solution
{
public:
    int maxDepth(TreeNode *root)
    {
        // Start typing your C/C++ solution below
        // DO NOT write int main() function
        if (!root)
            return 0;
        int nLeftDeep = 1;
        int nRightDeep = 1;
        if (root->left)
            nLeftDeep = nLeftDeep + maxDepth(root->left);
        if (root->right)
            nRightDeep = nRightDeep + maxDepth(root->right);
        if (nLeftDeep > nRightDeep)
            return nLeftDeep;
        else
            return nRightDeep;
    }
};
}
}
;