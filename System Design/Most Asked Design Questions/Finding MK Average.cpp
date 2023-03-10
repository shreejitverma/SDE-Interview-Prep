/*
https://leetcode.com/problems/finding-mk-average/
1825. Finding MK Average
Hard

170

77

Add to List

Share
You are given two integers, m and k, and a stream of integers. You are tasked to implement a data structure that calculates the MKAverage for the stream.

The MKAverage can be calculated using these steps:

If the number of the elements in the stream is less than m you should consider the MKAverage to be -1. Otherwise, copy the last m elements of the stream to a separate container.
Remove the smallest k elements and the largest k elements from the container.
Calculate the average value for the rest of the elements rounded down to the nearest integer.
Implement the MKAverage class:

MKAverage(int m, int k) Initializes the MKAverage object with an empty stream and the two integers m and k.
void addElement(int num) Inserts a new element num into the stream.
int calculateMKAverage() Calculates and returns the MKAverage for the current stream rounded down to the nearest integer.
 

Example 1:

Input
["MKAverage", "addElement", "addElement", "calculateMKAverage", "addElement", "calculateMKAverage", "addElement", "addElement", "addElement", "calculateMKAverage"]
[[3, 1], [3], [1], [], [10], [], [5], [5], [5], []]
Output
[null, null, null, -1, null, 3, null, null, null, 5]

Explanation
MKAverage obj = new MKAverage(3, 1); 
obj.addElement(3);        // current elements are [3]
obj.addElement(1);        // current elements are [3,1]
obj.calculateMKAverage(); // return -1, because m = 3 and only 2 elements exist.
obj.addElement(10);       // current elements are [3,1,10]
obj.calculateMKAverage(); // The last 3 elements are [3,1,10].
                          // After removing smallest and largest 1 element the container will be [3].
                          // The average of [3] equals 3/1 = 3, return 3
obj.addElement(5);        // current elements are [3,1,10,5]
obj.addElement(5);        // current elements are [3,1,10,5,5]
obj.addElement(5);        // current elements are [3,1,10,5,5,5]
obj.calculateMKAverage(); // The last 3 elements are [5,5,5].
                          // After removing smallest and largest 1 element the container will be [5].
                          // The average of [5] equals 5/1 = 5, return 5
 

Constraints:

3 <= m <= 105
1 <= k*2 < m
1 <= num <= 105
At most 105 calls will be made to addElement and calculateMKAverage.
*/

// Time:  ctor:           O(1)
//        add_element:    O(logn)
//        calc_mkaverage: O(1)
// Space: O(m)

class MKAverage
{
public:
    MKAverage(int m, int k)
        : m_{m}, k_{k}
    {
    }

    void addElement(int num)
    {
        if (size(dq_) == m_)
        {
            remove(dq_.front());
            dq_.pop_front();
        }
        dq_.emplace_back(num);
        add(num);
    }

    int calculateMKAverage()
    {
        if (size(dq_) < m_)
        {
            return -1;
        }
        return total_ / (m_ - 2 * k_);
    }

private:
    void add(int num)
    {
        left_.emplace(num);
        if (size(left_) > k_)
        {
            const auto it = prev(end(left_));
            mid_.emplace(*it);
            total_ += *it;
            left_.erase(it);
        }
        if (size(mid_) > (m_ - 2 * k_))
        {
            const auto it = prev(end(mid_));
            total_ -= *it;
            right_.emplace(*it);
            mid_.erase(it);
        }
    }

    void remove(int num)
    {
        if (num <= *rbegin(left_))
        {
            left_.erase(left_.find(num));
        }
        else if (num <= *rbegin(mid_))
        {
            auto it = mid_.find(num);
            total_ -= *it;
            mid_.erase(it);
        }
        else
        {
            right_.erase(right_.find(num));
        }
        if (size(left_) < k_)
        {
            left_.emplace(*begin(mid_));
            total_ -= *begin(mid_);
            mid_.erase(begin(mid_));
        }
        if (size(mid_) < (m_ - 2 * k_))
        {
            mid_.emplace(*begin(right_));
            total_ += *begin(right_);
            right_.erase(begin(right_));
        }
    }

    int m_;
    int k_;
    deque<int> dq_;
    multiset<int> left_, mid_, right_;
    int64_t total_ = 0;
};