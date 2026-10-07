/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(N)
// Space: O(N) auxiliary

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

function buildTree(preorder: number[], inorder: number[]): TreeNode | null {
    const inMap: Map<number, number> = new Map();
    for (let i = 0; i < inorder.length; i++) {
        inMap.set(inorder[i], i);
    }

    let preIndex = 0;

    function build(inStart: number, inEnd: number): TreeNode | null {
        if (inStart > inEnd) {
            return null;
        }

        const rootVal = preorder[preIndex++];
        const root = new TreeNode(rootVal);
        const mid = inMap.get(rootVal)!;

        root.left = build(inStart, mid - 1);
        root.right = build(mid + 1, inEnd);

        return root;
    }

    return build(0, inorder.length - 1);
}
