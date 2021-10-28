/*
https://leetcode.com/problems/binary-tree-maximum-path-sum/
124. Binary Tree Maximum Path Sum
Hard

7430

459

Add to List

Share
A path in a binary tree is a sequence of nodes where each pair of adjacent nodes in the sequence has an edge connecting them. A node can only appear in the sequence at most once. Note that the path does not need to pass through the root.

The path sum of a path is the sum of the node's values in the path.

Given the root of a binary tree, return the maximum path sum of any path.

 

Example 1:


Input: root = [1,2,3]
Output: 6
Explanation: The optimal path is 2 -> 1 -> 3 with a path sum of 2 + 1 + 3 = 6.
Example 2:


Input: root = [-10,9,20,null,null,15,7]
Output: 42
Explanation: The optimal path is 15 -> 20 -> 7 with a path sum of 15 + 20 + 7 = 42.
 

Constraints:

The number of nodes in the tree is in the range [1, 3 * 104].
-1000 <= Node.val <= 1000

*/

//https://leetcode.com/problems/binary-tree-maximum-path-sum/discuss/39775/Accepted-short-solution-in-Java
//Runtime: 36 ms, faster than 33.76% of C++ online submissions for Binary Tree Maximum Path Sum.
//Memory Usage: 28.8 MB, less than 6.06% of C++ online submissions for Binary Tree Maximum Path Sum.
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
    int ans;

    int maxPathDown(TreeNode *node)
    {
        if (!node)
            return 0;
        //max sum of path rooted at node->left, going down
        int left = max(maxPathDown(node->left), 0);
        //max sum of path rooted at node->right, going down
        int right = max(maxPathDown(node->right), 0);
        ans = max(ans, left + right + node->val);
        //max sum of path that goes down
        return node->val + max(left, right);
    };

    int maxPathSum(TreeNode *root)
    {
        ans = INT_MIN;

        maxPathDown(root);

        return ans;
    }
};