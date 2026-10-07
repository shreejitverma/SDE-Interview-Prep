/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(H + k) where H is tree height
// Space: O(H) auxiliary (stack depth)

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

function kthSmallest(root: TreeNode | null, k: number): number {
    const stack: TreeNode[] = [];
    let curr = root;

    while (curr !== null || stack.length > 0) {
        while (curr !== null) {
            stack.push(curr);
            curr = curr.left;
        }

        curr = stack.pop()!;
        if (--k === 0) {
            return curr.val;
        }

        curr = curr.right;
    }

    return -1;
}
