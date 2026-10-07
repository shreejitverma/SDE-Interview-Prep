/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 543 - Diameter of Binary Tree
 * Language: Java
 *
 * Complexity:
 * - Time: O(N)
 * - Space: O(H) where H is tree height
 */

class TreeNode {
    int val;
    TreeNode left;
    TreeNode right;
    TreeNode() {}
    TreeNode(int val) { this.val = val; }
    TreeNode(int val, TreeNode left, TreeNode right) {
        this.val = val;
        this.left = left;
        this.right = right;
    }
}

class Solution {
    private int maxDiameter = 0;

    public int diameterOfBinaryTree(TreeNode root) {
        maxDiameter = 0;
        maxDepth(root);
        return maxDiameter;
    }

    private int maxDepth(TreeNode node) {
        if (node == null) {
            return 0;
        }

        int leftDepth = maxDepth(node.left);
        int rightDepth = maxDepth(node.right);

        maxDiameter = Math.max(maxDiameter, leftDepth + rightDepth);

        return 1 + Math.max(leftDepth, rightDepth);
    }
}
