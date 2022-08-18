/*
https://leetcode.com/problems/design-skiplist/
1206. Design Skiplist
Hard

321

39

Add to List

Share
Design a Skiplist without using any built-in libraries.

A skiplist is a data structure that takes O(log(n)) time to add, erase and search. Comparing with treap and red-black tree which has the same function and performance, the code length of Skiplist can be comparatively short and the idea behind Skiplists is just simple linked lists.

For example, we have a Skiplist containing [30,40,50,60,70,90] and we want to add 80 and 45 into it. The Skiplist works this way:


Artyom Kalinin [CC BY-SA 3.0], via Wikimedia Commons

You can see there are many layers in the Skiplist. Each layer is a sorted linked list. With the help of the top layers, add, erase and search can be faster than O(n). It can be proven that the average time complexity for each operation is O(log(n)) and space complexity is O(n).

See more about Skiplist: https://en.wikipedia.org/wiki/Skip_list

Implement the Skiplist class:

Skiplist() Initializes the object of the skiplist.
bool search(int target) Returns true if the integer target exists in the Skiplist or false otherwise.
void add(int num) Inserts the value num into the SkipList.
bool erase(int num) Removes the value num from the Skiplist and returns true. If num does not exist in the Skiplist, do nothing and return false. If there exist multiple num values, removing any one of them is fine.
Note that duplicates may exist in the Skiplist, your code needs to handle this situation.

 

Example 1:

Input
["Skiplist", "add", "add", "add", "search", "add", "search", "erase", "erase", "search"]
[[], [1], [2], [3], [0], [4], [1], [0], [1], [1]]
Output
[null, null, null, null, false, null, true, false, true, false]

Explanation
Skiplist skiplist = new Skiplist();
skiplist.add(1);
skiplist.add(2);
skiplist.add(3);
skiplist.search(0); // return False
skiplist.add(4);
skiplist.search(1); // return True
skiplist.erase(0);  // return False, 0 is not in skiplist.
skiplist.erase(1);  // return True
skiplist.search(1); // return False, 1 has already been erased.
 

Constraints:

0 <= num, target <= 2 * 104
At most 5 * 104 calls will be made to search, add, and erase.
*/

// Time:  O(logn) on average for each operation
// Space: O(n)

// see proof in references:
// 1. https://kunigami.blog/2012/09/25/skip-lists-in-python/
// 2. https://opendatastructures.org/ods-cpp/4_4_Analysis_Skiplists.html
// 3. https://brilliant.org/wiki/skip-lists/
class Skiplist
{
private:
    class SkipNode
    {
    public:
        SkipNode() : SkipNode(0, -1)
        {
        }

        SkipNode(int level, int num)
            : num(num), nexts(level)
        {
        }

        int num;
        vector<SkipNode *> nexts;
    };

public:
    Skiplist()
        : gen_((random_device())()), len_(0), head_(new SkipNode())
    {
    }

    ~Skiplist()
    {
        if (head_->nexts.empty())
        {
            return;
        }
        auto curr = head_->nexts[0];
        while (curr)
        {
            auto next = curr->nexts[0];
            delete curr;
            curr = next;
        }
    }

    bool search(int target) const
    {
        return find(target, find_prev_nodes(target)) != nullptr;
    }

    void add(int num)
    {
        auto node = new SkipNode(random_level(), num);
        if (head_->nexts.size() < node->nexts.size())
        {
            head_->nexts.resize(node->nexts.size());
        }
        auto prevs = find_prev_nodes(num);
        for (int i = 0; i < node->nexts.size(); ++i)
        {
            node->nexts[i] = prevs[i]->nexts[i];
            prevs[i]->nexts[i] = node;
        }
        ++len_;
    }

    bool erase(int num)
    {
        auto prevs = find_prev_nodes(num);
        auto curr = find(num, prevs);
        if (!curr)
        {
            return false;
        }
        --len_;
        for (int i = curr->nexts.size() - 1; i >= 0; --i)
        {
            prevs[i]->nexts[i] = curr->nexts[i];
            if (!head_->nexts[i])
            {
                head_->nexts.pop_back();
            }
        }
        delete curr;
        return true;
    }

    int size() const
    {
        return len_;
    }

private:
    SkipNode *find(int num, const vector<SkipNode *> &prevs) const
    {
        if (!prevs.empty())
        {
            auto candidate = prevs[0]->nexts[0];
            if (candidate && candidate->num == num)
            {
                return candidate;
            }
        }
        return nullptr;
    }

    vector<SkipNode *> find_prev_nodes(int num) const
    {
        vector<SkipNode *> prevs(head_->nexts.size());
        auto curr = head_;
        for (int i = head_->nexts.size() - 1; i >= 0; --i)
        {
            while (curr->nexts[i] && curr->nexts[i]->num < num)
            {
                curr = curr->nexts[i];
            }
            prevs[i] = curr;
        }
        return prevs;
    }

    int random_level()
    {
        static const int P_NUMERATOR = 1;
        static const int P_DENOMINATOR = 2; // P = 1/4 in redis implementation
        static const int MAX_LEVEL = 32;    // enough for 2^32 elements
        int level = 1;
        while (uniform_int_distribution<int>{1, P_DENOMINATOR}(gen_) <= P_NUMERATOR &&
               level < MAX_LEVEL)
        {
            ++level;
        }
        return level;
    }

    void print_list() const
    {
        for (int i = head_->nexts.size() - 1; i >= 0; --i)
        {
            auto curr = head_->nexts[i];
            cout << curr->num;
            curr = curr->nexts[i];
            while (curr)
            {
                cout << "->" << curr->num;
                curr = curr->nexts[i];
            }
            cout << endl;
        }
    }

    default_random_engine gen_;
    int len_;
    SkipNode *head_;
};

// Time:  O(logn) on average for each operation
// Space: O(n)
// smart pointer version (a little bit slower)
class Skiplist2
{
private:
    class SkipNode
    {
    public:
        SkipNode() : SkipNode(0, -1)
        {
        }

        SkipNode(int level, int num)
            : num(num), nexts(level)
        {
        }

        int num;
        vector<shared_ptr<SkipNode> > nexts;
    };

public:
    Skiplist2()
        : gen_((random_device())()), len_(0), head_(make_shared<SkipNode>())
    {
    }

    bool search(int target) const
    {
        return find(target, find_prev_nodes(target)) != nullptr;
    }

    void add(int num)
    {
        auto node = make_shared<SkipNode>(random_level(), num);
        if (head_->nexts.size() < node->nexts.size())
        {
            head_->nexts.resize(node->nexts.size());
        }
        auto prevs = find_prev_nodes(num);
        for (int i = 0; i < node->nexts.size(); ++i)
        {
            node->nexts[i] = prevs[i]->nexts[i];
            prevs[i]->nexts[i] = node;
        }
        ++len_;
    }

    bool erase(int num)
    {
        auto prevs = find_prev_nodes(num);
        auto curr = find(num, prevs);
        if (!curr)
        {
            return false;
        }
        --len_;
        for (int i = curr->nexts.size() - 1; i >= 0; --i)
        {
            prevs[i]->nexts[i] = curr->nexts[i];
            if (!head_->nexts[i])
            {
                head_->nexts.pop_back();
            }
        }
        return true;
    }

    int size() const
    {
        return len_;
    }

private:
    shared_ptr<SkipNode> find(int num, const vector<shared_ptr<SkipNode> > &prevs) const
    {
        if (!prevs.empty())
        {
            auto candidate = prevs[0]->nexts[0];
            if (candidate && candidate->num == num)
            {
                return candidate;
            }
        }
        return nullptr;
    }

    vector<shared_ptr<SkipNode> > find_prev_nodes(int num) const
    {
        vector<shared_ptr<SkipNode> > prevs(head_->nexts.size());
        auto curr = head_;
        for (int i = head_->nexts.size() - 1; i >= 0; --i)
        {
            while (curr->nexts[i] && curr->nexts[i]->num < num)
            {
                curr = curr->nexts[i];
            }
            prevs[i] = curr;
        }
        return prevs;
    }

    int random_level()
    {
        static const int P_NUMERATOR = 1;
        static const int P_DENOMINATOR = 2; // P = 1/4 in redis implementation
        static const int MAX_LEVEL = 32;    // enough for 2^32 elements
        int level = 1;
        while (uniform_int_distribution<int>{1, P_DENOMINATOR}(gen_) <= P_NUMERATOR &&
               level < MAX_LEVEL)
        {
            ++level;
        }
        return level;
    }

    void print_list() const
    {
        for (int i = head_->nexts.size() - 1; i >= 0; --i)
        {
            auto curr = head_->nexts[i];
            cout << curr->num;
            curr = curr->nexts[i];
            while (curr)
            {
                cout << "->" << curr->num;
                curr = curr->nexts[i];
            }
            cout << endl;
        }
    }

    default_random_engine gen_;
    int len_;
    shared_ptr<SkipNode> head_;
};