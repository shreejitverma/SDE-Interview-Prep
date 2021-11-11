'''https://leetcode.com/problems/design-a-leaderboard/
1244. Design A Leaderboard
Medium

339

63

Add to List

Share
Design a Leaderboard class, which has 3 functions:

addScore(playerId, score): Update the leaderboard by adding score to the given player's score. If there is no player with such id in the leaderboard, add him to the leaderboard with the given score.
top(K): Return the score sum of the top K players.
reset(playerId): Reset the score of the player with the given id to 0 (in other words erase it from the leaderboard). It is guaranteed that the player was added to the leaderboard before calling this function.
Initially, the leaderboard is empty.

 

Example 1:

Input: 
["Leaderboard","addScore","addScore","addScore","addScore","addScore","top","reset","reset","addScore","top"]
[[],[1,73],[2,56],[3,39],[4,51],[5,4],[1],[1],[2],[2,51],[3]]
Output: 
[null,null,null,null,null,null,73,null,null,null,141]

Explanation: 
Leaderboard leaderboard = new Leaderboard ();
leaderboard.addScore(1,73);   // leaderboard = [[1,73]];
leaderboard.addScore(2,56);   // leaderboard = [[1,73],[2,56]];
leaderboard.addScore(3,39);   // leaderboard = [[1,73],[2,56],[3,39]];
leaderboard.addScore(4,51);   // leaderboard = [[1,73],[2,56],[3,39],[4,51]];
leaderboard.addScore(5,4);    // leaderboard = [[1,73],[2,56],[3,39],[4,51],[5,4]];
leaderboard.top(1);           // returns 73;
leaderboard.reset(1);         // leaderboard = [[2,56],[3,39],[4,51],[5,4]];
leaderboard.reset(2);         // leaderboard = [[3,39],[4,51],[5,4]];
leaderboard.addScore(2,51);   // leaderboard = [[2,51],[3,39],[4,51],[5,4]];
leaderboard.top(3);           // returns 141 = 51 + 51 + 39;
 

Constraints:

1 <= playerId, K <= 10000
It's guaranteed that K is less than or equal to the current number of players.
1 <= score <= 100
There will be at most 1000 function calls.'''


import random
import collections
import bisect


class Leaderboard(object):

    def __init__(self):
        self.dic = {}
        self.scores = []

    def addScore(self, playerId, score):
        """
        :type playerId: int
        :type score: int
        :rtype: None
        """
        if playerId not in self.dic:
            self.dic[playerId] = score
            index = bisect.bisect_left(self.scores, score)
            self.scores = self.scores[:index] + [score] + self.scores[index:]
        else:
            s = self.dic[playerId]
            index = bisect.bisect_left(self.scores, s)
            self.scores = self.scores[:index] + self.scores[index + 1:]

            self.dic[playerId] = score + s
            index = bisect.bisect_left(self.scores, score + s)
            self.scores = self.scores[:index] + \
                [score + s] + self.scores[index:]
            # print(self.scores)
        # print(self.dic)

    def top(self, K):
        """
        :type K: int
        :rtype: int
        """
        # print(self.scores)
        # print(self.scores[::-1][:K])
        # print(self.dic)
        return sum(self.scores[::-1][:K])

    def reset(self, playerId):
        """
        :type playerId: int
        :rtype: None
        """
        s = self.dic[playerId]
        del (self.dic[playerId])

        index = bisect.bisect_left(self.scores, s)
        self.scores = self.scores[:index] + self.scores[index + 1:]
        # print(self.scores)
        # print(self.dic)

# Your Leaderboard object will be instantiated and called as such:
# obj = Leaderboard()
# obj.addScore(playerId,score)
# param_2 = obj.top(K)
# obj.reset(playerId)
# Time:  ctor:  O(1)
#        add:   O(1)
#        top:   O(n)
#        reset: O(1)
# Space: O(n)


class Leaderboard(object):

    def __init__(self):
        self.__lookup = collections.Counter()

    def addScore(self, playerId, score):
        """
        :type playerId: int
        :type score: int
        :rtype: None
        """
        self.__lookup[playerId] += score

    def top(self, K):
        """
        :type K: int
        :rtype: int
        """
        def kthElement(nums, k, compare):
            def PartitionAroundPivot(left, right, pivot_idx, nums, compare):
                new_pivot_idx = left
                nums[pivot_idx], nums[right] = nums[right], nums[pivot_idx]
                for i in xrange(left, right):
                    if compare(nums[i], nums[right]):
                        nums[i], nums[new_pivot_idx] = nums[new_pivot_idx], nums[i]
                        new_pivot_idx += 1

                nums[right], nums[new_pivot_idx] = nums[new_pivot_idx], nums[right]
                return new_pivot_idx

            left, right = 0, len(nums) - 1
            while left <= right:
                pivot_idx = random.randint(left, right)
                new_pivot_idx = PartitionAroundPivot(
                    left, right, pivot_idx, nums, compare)
                if new_pivot_idx == k:
                    return
                elif new_pivot_idx > k:
                    right = new_pivot_idx - 1
                else:  # new_pivot_idx < k.
                    left = new_pivot_idx + 1

        scores = self.__lookup.values()
        kthElement(scores, K, lambda a, b: a > b)
        return sum(scores[:K])

    def reset(self, playerId):
        """
        :type playerId: int
        :rtype: None
        """
        self.__lookup[playerId] = 0
