/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(H) where H is tree height
// Space: O(1) auxiliary

class TreeNode {
    int val;
    TreeNode left;
    TreeNode right;
    TreeNode(int x) { val = x; }
}

class Solution {
    public TreeNode lowestCommonAncestor(TreeNode root, TreeNode p, TreeNode q) {
        int small = Math.min(p.val, q.val);
        int large = Math.max(p.val, q.val);

        TreeNode curr = root;
        while (curr != null) {
            if (curr.val > large) {
                curr = curr.left;
            } else if (curr.val < small) {
                curr = curr.right;
            } else {
                return curr;
            }
        }

        return null;
    }
}
