/*
https://leetcode.com/problems/first-unique-number/
1429. First Unique Number
Medium

334

17

Add to List

Share
You have a queue of integers, you need to retrieve the first unique integer in the queue.

Implement the FirstUnique class:

FirstUnique(int[] nums) Initializes the object with the numbers in the queue.
int showFirstUnique() returns the value of the first unique integer of the queue, and returns -1 if there is no such integer.
void add(int value) insert value to the queue.
 

Example 1:

Input: 
["FirstUnique","showFirstUnique","add","showFirstUnique","add","showFirstUnique","add","showFirstUnique"]
[[[2,3,5]],[],[5],[],[2],[],[3],[]]
Output: 
[null,2,null,2,null,3,null,-1]
Explanation: 
FirstUnique firstUnique = new FirstUnique([2,3,5]);
firstUnique.showFirstUnique(); // return 2
firstUnique.add(5);            // the queue is now [2,3,5,5]
firstUnique.showFirstUnique(); // return 2
firstUnique.add(2);            // the queue is now [2,3,5,5,2]
firstUnique.showFirstUnique(); // return 3
firstUnique.add(3);            // the queue is now [2,3,5,5,2,3]
firstUnique.showFirstUnique(); // return -1
Example 2:

Input: 
["FirstUnique","showFirstUnique","add","add","add","add","add","showFirstUnique"]
[[[7,7,7,7,7,7]],[],[7],[3],[3],[7],[17],[]]
Output: 
[null,-1,null,null,null,null,null,17]
Explanation: 
FirstUnique firstUnique = new FirstUnique([7,7,7,7,7,7]);
firstUnique.showFirstUnique(); // return -1
firstUnique.add(7);            // the queue is now [7,7,7,7,7,7,7]
firstUnique.add(3);            // the queue is now [7,7,7,7,7,7,7,3]
firstUnique.add(3);            // the queue is now [7,7,7,7,7,7,7,3,3]
firstUnique.add(7);            // the queue is now [7,7,7,7,7,7,7,3,3,7]
firstUnique.add(17);           // the queue is now [7,7,7,7,7,7,7,3,3,7,17]
firstUnique.showFirstUnique(); // return 17
Example 3:

Input: 
["FirstUnique","showFirstUnique","add","showFirstUnique"]
[[[809]],[],[809],[]]
Output: 
[null,809,null,-1]
Explanation: 
FirstUnique firstUnique = new FirstUnique([809]);
firstUnique.showFirstUnique(); // return 809
firstUnique.add(809);          // the queue is now [809,809]
firstUnique.showFirstUnique(); // return -1
 

Constraints:

1 <= nums.length <= 10^5
1 <= nums[i] <= 10^8
1 <= value <= 10^8
At most 50000 calls will be made to showFirstUnique and add.
*/

// Time:  ctor: O(k)
//        add: O(1)
//        showFirstUnique: O(1)
// Space: O(n)

class FirstUnique
{
public:
    FirstUnique(vector<int> &nums)
    {
        for (const auto &num : nums)
        {
            add(num);
        }
    }

    int showFirstUnique()
    {
        if (q_.size())
        {
            return q_.front().first;
        }
        return -1;
    }

    void add(int value)
    {
        if (!dup_.count(value) && !q_.count(value))
        {
            q_[value] = true;
            return;
        }
        if (q_.count(value))
        {
            q_.pop(value);
            dup_.emplace(value);
        }
    }

private:
    template <typename K, typename V>
    class OrderedDict
    {
    public:
        bool count(const K &key) const
        {
            return map_.count(key);
        }

        V &operator[](const K &key)
        {
            if (!map_.count(key))
            {
                list_.emplace_front();
                list_.begin()->first = key;
                map_[key] = list_.begin();
            }
            return map_[key]->second;
        }

        void popitem()
        {
            auto del = list_.front();
            list_.pop_front();
            map_.erase(del.first);
        }

        pair<K, V> front()
        {
            return *list_.crbegin();
        }

        void pop(const K &key)
        {
            if (!map_.count(key))
            {
                return;
            }
            list_.erase(map_[key]);
            map_.erase(key);
        }

        vector<K> keys() const
        {
            vector<K> result;
            transform(list_.crbegin(), list_.crend(), back_inserter(result),
                      [](const auto &x)
                      {
                          return x.first;
                      });
            return result;
        }

        int size() const
        {
            return list_.size();
        }

    private:
        list<pair<K, V> > list_;
        unordered_map<K, typename list<pair<K, V> >::iterator> map_;
    };

    OrderedDict<int, bool> q_;
    unordered_set<int> dup_;
};