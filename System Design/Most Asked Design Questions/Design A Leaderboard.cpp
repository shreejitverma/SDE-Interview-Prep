/*
https://leetcode.com/problems/design-a-leaderboard/
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
There will be at most 1000 function calls.
*/

// Time:  ctor:  O(1)
//        add:   O(1)
//        top:   O(n)
//        reset: O(1)
// Space: O(n)

class Leaderboard
{
public:
    Leaderboard()
    {
    }

    void addScore(int playerId, int score)
    {
        lookup_[playerId] += score;
    }

    int top(int K)
    {
        vector<int> scores;
        for (const auto &[k, v] : lookup_)
        {
            scores.emplace_back(v);
        }
        nth_element(scores.begin(), scores.begin() + K, scores.end(), greater<int>());
        return accumulate(scores.cbegin(), scores.cbegin() + K, 0);
    }

    void reset(int playerId)
    {
        lookup_[playerId] = 0;
    }

private:
    unordered_map<int, int> lookup_;
};