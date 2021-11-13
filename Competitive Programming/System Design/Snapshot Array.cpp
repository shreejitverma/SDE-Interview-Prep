/*
https://leetcode.com/problems/snapshot-array/
1146. Snapshot Array
Medium

1176

198

Add to List

Share
Implement a SnapshotArray that supports the following interface:

SnapshotArray(int length) initializes an array-like data structure with the given length.  Initially, each element equals 0.
void set(index, val) sets the element at the given index to be equal to val.
int snap() takes a snapshot of the array and returns the snap_id: the total number of times we called snap() minus 1.
int get(index, snap_id) returns the value at the given index, at the time we took the snapshot with the given snap_id
 

Example 1:

Input: ["SnapshotArray","set","snap","set","get"]
[[3],[0,5],[],[0,6],[0,0]]
Output: [null,null,0,null,5]
Explanation: 
SnapshotArray snapshotArr = new SnapshotArray(3); // set the length to be 3
snapshotArr.set(0,5);  // Set array[0] = 5
snapshotArr.snap();  // Take a snapshot, return snap_id = 0
snapshotArr.set(0,6);
snapshotArr.get(0,0);  // Get the value of array[0] with snap_id = 0, return 5
 

Constraints:

1 <= length <= 50000
At most 50000 calls will be made to set, snap, and get.
0 <= index < length
0 <= snap_id < (the total number of times we call snap())
0 <= val <= 10^9
*/

// Time:  set: O(1)
//        get: O(logn), n is the total number of set
// Space: O(n)

class SnapshotArray
{
public:
    SnapshotArray(int length)
    {
    }

    void set(int index, int val)
    {
        if (!A_.count(index))
        {
            A_[index].emplace_back(0, 0);
        }
        if (A_[index].back().first == snap_id_)
        {
            A_[index].back().second = val;
        }
        else
        {
            A_[index].emplace_back(snap_id_, val);
        }
    }

    int snap()
    {
        return snap_id_++;
    }

    int get(int index, int snap_id)
    {
        if (!A_.count(index))
        {
            A_[index].emplace_back(0, 0);
        }
        const auto &it = prev(lower_bound(A_[index].cbegin(), A_[index].cend(),
                                          make_pair(snap_id + 1, numeric_limits<int>::min())));
        return it->second;
    }

private:
    unordered_map<int, vector<pair<int, int> > > A_;
    int snap_id_ = 0;
};

// Time:  set: O(logn)
//        get: O(logn), n is the total number of set
// Space: O(n)
class SnapshotArray2
{
public:
    SnapshotArray2(int length)
    {
    }

    void set(int index, int val)
    {
        if (!A_.count(index))
        {
            A_[index][0] = 0;
        }
        A_[index][snap_id_] = val;
    }

    int snap()
    {
        return snap_id_++;
    }

    int get(int index, int snap_id)
    {
        if (!A_.count(index))
        {
            A_[index][0] = 0;
        }
        const auto &it = prev(A_[index].upper_bound(snap_id));
        return it->second;
    }

private:
    unordered_map<int, map<int, int> > A_;
    int snap_id_ = 0;
};

/**
 * Your SnapshotArray object will be instantiated and called as such:
 * SnapshotArray* obj = new SnapshotArray(length);
 * obj->set(index,val);
 * int param_2 = obj->snap();
 * int param_3 = obj->get(index,snap_id);
 */