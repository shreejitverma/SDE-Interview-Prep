/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(H) where H is tree height
// Space: O(1) auxiliary

#include <algorithm>

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

class Solution {
public:
    TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
        int small = std::min(p->val, q->val);
        int large = std::max(p->val, q->val);

        while (root != nullptr) {
            if (root->val > large) {
                root = root->left;
            } else if (root->val < small) {
                root = root->right;
            } else {
                return root;
            }
        }

        return nullptr;
    }
};
