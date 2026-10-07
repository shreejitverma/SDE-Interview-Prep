/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(H)

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

function maxPathSum(root: TreeNode | null): number {
    let maxSum = -Infinity;

    function maxGain(node: TreeNode | null): number {
        if (!node) {
            return 0;
        }

        const leftGain = Math.max(0, maxGain(node.left));
        const rightGain = Math.max(0, maxGain(node.right));

        const currentPathSum = node.val + leftGain + rightGain;
        if (currentPathSum > maxSum) {
            maxSum = currentPathSum;
        }

        return node.val + Math.max(leftGain, rightGain);
    }

    maxGain(root);
    return maxSum;
}
