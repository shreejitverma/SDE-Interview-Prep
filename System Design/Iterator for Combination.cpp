/*
https://leetcode.com/problems/iterator-for-combination/
1286. Iterator for Combination
Medium

688

53

Add to List

Share
Design the CombinationIterator class:

CombinationIterator(string characters, int combinationLength) Initializes the object with a string characters of sorted distinct lowercase English letters and a number combinationLength as arguments.
next() Returns the next combination of length combinationLength in lexicographical order.
hasNext() Returns true if and only if there exists a next combination.
 

Example 1:

Input
["CombinationIterator", "next", "hasNext", "next", "hasNext", "next", "hasNext"]
[["abc", 2], [], [], [], [], [], []]
Output
[null, "ab", true, "ac", true, "bc", false]

Explanation
CombinationIterator itr = new CombinationIterator("abc", 2);
itr.next();    // return "ab"
itr.hasNext(); // return True
itr.next();    // return "ac"
itr.hasNext(); // return True
itr.next();    // return "bc"
itr.hasNext(); // return False
 

Constraints:

1 <= combinationLength <= characters.length <= 15
All the characters of characters are unique.
At most 104 calls will be made to next and hasNext.
It's guaranteed that all calls of the function next are valid.
*/

// Time:  O(k), per operation
// Space: O(k)

class CombinationIterator
{
public:
    CombinationIterator(string characters, int combinationLength)
        : characters_(characters), combinationLength_(combinationLength), last_(prev(characters.cend(), combinationLength), characters.cend()), stk_{bind(&CombinationIterator::divide, this, 0)}
    {
    }

    string next()
    {
        iterative_backtracking();
        return curr_;
    }

    bool hasNext()
    {
        return curr_ != last_;
    }

private:
    void iterative_backtracking()
    {
        while (!stk_.empty())
        {
            const auto cb = move(stk_.back());
            stk_.pop_back();
            if (cb())
            {
                return;
            }
        }
    }

    bool conquer()
    {
        if (curr_.length() == combinationLength_)
        {
            return true;
        }
        return false;
    }

    bool prev_divide(char c)
    {
        curr_.push_back(c);
        return false;
    }

    bool divide(int i)
    {
        if (curr_.length() != combinationLength_)
        {
            for (int j = int(characters_.length()) - (combinationLength_ - int(curr_.length()) - 1) - 1;
                 j >= i; --j)
            {
                stk_.emplace_back(bind(&CombinationIterator::post_divide, this));
                stk_.emplace_back(bind(&CombinationIterator::divide, this, j + 1));
                stk_.emplace_back(bind(&CombinationIterator::prev_divide, this, characters_[j]));
            }
        }
        stk_.emplace_back(bind(&CombinationIterator::conquer, this));
        return false;
    }

    bool post_divide()
    {
        curr_.pop_back();
        return false;
    }

    const string characters_;
    const int combinationLength_;
    string curr_;
    string last_;
    vector<function<bool()> > stk_;
};

/**
 * Your CombinationIterator object will be instantiated and called as such:
 * CombinationIterator* obj = new CombinationIterator(characters, combinationLength);
 * string param_1 = obj->next();
 * bool param_2 = obj->hasNext();
 */
