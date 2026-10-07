/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(h)

function invertTree(root: TreeNode | null): TreeNode | null {
    if (root === null) {
        return null;
    }
    const temp = root.left;
    root.left = invertTree(root.right);
    root.right = invertTree(temp);
    return root;
}
