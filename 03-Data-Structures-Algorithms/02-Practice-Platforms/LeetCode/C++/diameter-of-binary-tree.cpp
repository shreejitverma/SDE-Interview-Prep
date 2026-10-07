/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(h)

#include <algorithm>

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    explicit TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

class Solution {
public:
    int diameterOfBinaryTree(TreeNode* root) {
        int max_diameter = 0;
        maxDepth(root, max_diameter);
        return max_diameter;
    }

private:
    int maxDepth(TreeNode* node, int& max_diameter) {
        if (!node) return 0;
        const int left = maxDepth(node->left, max_diameter);
        const int right = maxDepth(node->right, max_diameter);
        max_diameter = std::max(max_diameter, left + right);
        return 1 + std::max(left, right);
    }
};
