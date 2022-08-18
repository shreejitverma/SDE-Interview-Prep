/*
https://leetcode.com/problems/design-an-expression-tree-with-evaluate-function/
1628. Design an Expression Tree With Evaluate Function
Medium

215

40

Add to List

Share
Given the postfix tokens of an arithmetic expression, build and return the binary expression tree that represents this expression.

Postfix notation is a notation for writing arithmetic expressions in which the operands (numbers) appear before their operators. For example, the postfix tokens of the expression 4*(5-(7+2)) are represented in the array postfix = ["4","5","7","2","+","-","*"].

The class Node is an interface you should use to implement the binary expression tree. The returned tree will be tested using the evaluate function, which is supposed to evaluate the tree's value. You should not remove the Node class; however, you can modify it as you wish, and you can define other classes to implement it if needed.

A binary expression tree is a kind of binary tree used to represent arithmetic expressions. Each node of a binary expression tree has either zero or two children. Leaf nodes (nodes with 0 children) correspond to operands (numbers), and internal nodes (nodes with two children) correspond to the operators '+' (addition), '-' (subtraction), '*' (multiplication), and '/' (division).

It's guaranteed that no subtree will yield a value that exceeds 109 in absolute value, and all the operations are valid (i.e., no division by zero).

Follow up: Could you design the expression tree such that it is more modular? For example, is your design able to support additional operators without making changes to your existing evaluate implementation?

 

Example 1:



Input: s = ["3","4","+","2","*","7","/"]
Output: 2
Explanation: this expression evaluates to the above binary tree with expression ((3+4)*2)/7) = 14/7 = 2.
Example 2:



Input: s = ["4","5","2","7","+","-","*"]
Output: -16
Explanation: this expression evaluates to the above binary tree with expression 4*(5-(2+7)) = 4*(-4) = -16.
Example 3:

Input: s = ["4","2","+","3","5","1","-","*","+"]
Output: 18
Example 4:

Input: s = ["100","200","+","2","/","5","*","7","+"]
Output: 757
 

Constraints:

1 <= s.length < 100
s.length is odd.
s consists of numbers and the characters '+', '-', '*', and '/'.
If s[i] is a number, its integer representation is no more than 105.
It is guaranteed that s is a valid expression.
The absolute value of the result and intermediate values will not exceed 109.
It is guaranteed that no expression will include division by zero.
*/

/**
 * This is the interface for the expression tree Node.
 * You should not remove it, and you can define some classes to implement it.
 */

#define ITER

class Node
{
public:
    virtual ~Node(){};
    virtual int evaluate() const = 0;

protected:
};

/**
 * This is the TreeBuilder class.
 * You can treat it as the driver code that takes the postinfix input 
 * and returns the expression tree represnting it as a Node.
 */
// Time:  O(n)
// Space: O(h)

class NodeIter : public Node
{
public:
    NodeIter(int val) : val(val){};
    NodeIter(char op, NodeIter *left, NodeIter *right) : op(op), left(left), right(right){};
    virtual ~NodeIter(){};
    virtual int evaluate() const
    {
        int result = 0;
        vector<tuple<int, NodeIter *, unique_ptr<int>, unique_ptr<int>, int *> > stk;
        stk.emplace_back(1, const_cast<NodeIter *>(this), nullptr, nullptr, &result);
        while (!empty(stk))
        {
            const auto [step, node, ret1, ret2, ret] = move(stk.back());
            stk.pop_back();
            if (step == 1)
            {
                if (node->op == 0)
                {
                    *ret = node->val;
                    continue;
                }
                auto ret1 = make_unique<int>(), ret2 = make_unique<int>();
                auto p1 = ret1.get(), p2 = ret2.get();
                stk.emplace_back(2, node, move(ret1), move(ret2), ret);
                stk.emplace_back(1, node->right, nullptr, nullptr, p2);
                stk.emplace_back(1, node->left, nullptr, nullptr, p1);
            }
            else if (step == 2)
            {
                int l = *ret1, r = *ret2;
                *ret = (node->op == '+') ? l + r : (node->op == '-') ? l - r
                                               : (node->op == '*')   ? l * r
                                               : (node->op == '/')   ? l / r
                                                                     : 0;
            }
        }
        return result;
    }

public:
    int val;
    char op = 0;
    NodeIter *left = nullptr;
    NodeIter *right = nullptr;
};

class NodeRecu : public Node
{
public:
    NodeRecu(int val) : val(val){};
    NodeRecu(char op, NodeRecu *left, NodeRecu *right) : op(op), left(left), right(right){};
    virtual ~NodeRecu(){};
    virtual int evaluate() const
    {
        if (op == 0)
        {
            return val;
        }
        int l = left->evaluate(), r = right->evaluate();
        return (op == '+') ? l + r : (op == '-') ? l - r
                                 : (op == '*')   ? l * r
                                 : (op == '/')   ? l / r
                                                 : 0;
    }

public:
    int val;
    char op = 0;
    NodeRecu *left = nullptr;
    NodeRecu *right = nullptr;
};

class TreeBuilder
{
public:
    Node *buildTree(vector<string> &postfix)
    {
#ifdef ITER
        using TreeNode = NodeIter;
#else
        using TreeNode = NodeRecu;
#endif
        vector<Node *> stk;
        for (const auto &s : postfix)
        {
            if (isdigit(s[0]))
            {
                stk.emplace_back(new TreeNode(stoi(s)));
            }
            else
            {
                auto right = dynamic_cast<TreeNode *>(stk.back());
                stk.pop_back();
                auto left = dynamic_cast<TreeNode *>(stk.back());
                stk.pop_back();
                stk.emplace_back(new TreeNode(s[0], left, right));
            }
        }
        return stk.back();
    }
};
/**
 * Your TreeBuilder object will be instantiated and called as such:
 * TreeBuilder* obj = new TreeBuilder();
 * Node* expTree = obj->buildTree(postfix);
 * int ans = expTree->evaluate();
 */