'''https://leetcode.com/problems/snapshot-array/
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
0 <= val <= 10^9'''

# Time:  set: O(1)
#        get: O(logn), n is the total number of set
# Space: O(n)

import collections
import bisect


class SnapshotArray(object):

    def __init__(self, length):
        """
        :type length: int
        """
        self.__A = collections.defaultdict(lambda: [[0, 0]])
        self.__snap_id = 0

    def set(self, index, val):
        """
        :type index: int
        :type val: int
        :rtype: None
        """
        if self.__A[index][-1][0] == self.__snap_id:
            self.__A[index][-1][1] = val
        else:
            self.__A[index].append([self.__snap_id, val])

    def snap(self):
        """
        :rtype: int
        """
        self.__snap_id += 1
        return self.__snap_id - 1

    def get(self, index, snap_id):
        """
        :type index: int
        :type snap_id: int
        :rtype: int
        """
        i = bisect.bisect_left(self.__A[index], [snap_id+1, float("-inf")]) - 1
        return self.__A[index][i][1]
