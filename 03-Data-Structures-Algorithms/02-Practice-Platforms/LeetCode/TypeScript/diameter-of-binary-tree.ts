/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * Problem: LeetCode 543 - Diameter of Binary Tree
 * Language: TypeScript
 *
 * Complexity:
 * - Time: O(N)
 * - Space: O(H) where H is tree height
 */

class TreeNode {
    val: number;
    left: TreeNode | null;
    right: TreeNode | null;
    constructor(val?: number, left?: TreeNode | null, right?: TreeNode | null) {
        this.val = (val === undefined ? 0 : val);
        this.left = (left === undefined ? null : left);
        this.right = (right === undefined ? null : right);
    }
}

function diameterOfBinaryTree(root: TreeNode | null): number {
    let maxDiameter = 0;

    function maxDepth(node: TreeNode | null): number {
        if (node === null) {
            return 0;
        }

        const leftDepth = maxDepth(node.left);
        const rightDepth = maxDepth(node.right);

        maxDiameter = Math.max(maxDiameter, leftDepth + rightDepth);

        return 1 + Math.max(leftDepth, rightDepth);
    }

    maxDepth(root);
    return maxDiameter;
}
