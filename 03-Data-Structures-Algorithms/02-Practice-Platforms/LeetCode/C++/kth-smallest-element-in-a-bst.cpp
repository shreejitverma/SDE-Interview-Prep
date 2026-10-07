/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(H + k) where H is tree height
// Space: O(H) auxiliary (stack depth)

#include <vector>

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

class Solution {
public:
    int kthSmallest(TreeNode* root, int k) {
        std::vector<TreeNode*> stack;
        TreeNode* curr = root;

        while (curr != nullptr || !stack.empty()) {
            while (curr != nullptr) {
                stack.push_back(curr);
                curr = curr->left;
            }

            curr = stack.back();
            stack.pop_back();

            if (--k == 0) {
                return curr->val;
            }

            curr = curr->right;
        }

        return -1;
    }
};
