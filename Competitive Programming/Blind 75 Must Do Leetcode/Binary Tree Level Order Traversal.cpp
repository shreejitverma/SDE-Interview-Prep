/*
https://leetcode.com/problems/binary-tree-level-order-traversal/
102. Binary Tree Level Order Traversal
Medium

6188

127

Add to List

Share
Given the root of a binary tree, return the level order traversal of its nodes' values. (i.e., from left to right, level by level).

 

Example 1:


Input: root = [3,9,20,null,null,15,7]
Output: [[3],[9,20],[15,7]]
Example 2:

Input: root = [1]
Output: [[1]]
Example 3:

Input: root = []
Output: []
 

Constraints:

The number of nodes in the tree is in the range [0, 2000].
-1000 <= Node.val <= 1000
*/

//BFS
//Runtime: 4 ms, faster than 88.94% of C++ online submissions for Binary Tree Level Order Traversal.
//Memory Usage: 13 MB, less than 17.26% of C++ online submissions for Binary Tree Level Order Traversal.
/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
class Solution
{
public:
    vector<vector<int> > levelOrder(TreeNode *root)
    {
        queue<TreeNode *> q;
        vector<vector<int> > ans;

        if (root)
            q.push(root);

        while (!q.empty())
        {
            int level_size = q.size();

            vector<int> level;

            while (level_size-- > 0)
            {
                TreeNode *cur = q.front();
                q.pop();

                level.push_back(cur->val);

                if (cur->left)
                    q.push(cur->left);
                if (cur->right)
                    q.push(cur->right);
            }

            ans.push_back(level);
        }

        return ans;
    }
};