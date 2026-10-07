/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(H) where H is tree height
// Space: O(1) auxiliary

class TreeNode {
    val: number;
    left: TreeNode | null;
    right: TreeNode | null;
    constructor(val?: number, left?: TreeNode | null, right?: TreeNode | null) {
        this.val = val === undefined ? 0 : val;
        this.left = left === undefined ? null : left;
        this.right = right === undefined ? null : right;
    }
}

function lowestCommonAncestor(root: TreeNode | null, p: TreeNode | null, q: TreeNode | null): TreeNode | null {
    if (!root || !p || !q) {
        return null;
    }

    const small = Math.min(p.val, q.val);
    const large = Math.max(p.val, q.val);

    let curr: TreeNode | null = root;
    while (curr !== null) {
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
