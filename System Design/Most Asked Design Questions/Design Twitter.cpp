/*
https://leetcode.com/problems/design-twitter/
355. Design Twitter
Medium

1667

251

Add to List

Share
Design a simplified version of Twitter where users can post tweets, follow/unfollow another user, and is able to see the 10 most recent tweets in the user's news feed.

Implement the Twitter class:

Twitter() Initializes your twitter object.
void postTweet(int userId, int tweetId) Composes a new tweet with ID tweetId by the user userId. Each call to this function will be made with a unique tweetId.
List<Integer> getNewsFeed(int userId) Retrieves the 10 most recent tweet IDs in the user's news feed. Each item in the news feed must be posted by users who the user followed or by the user themself. Tweets must be ordered from most recent to least recent.
void follow(int followerId, int followeeId) The user with ID followerId started following the user with ID followeeId.
void unfollow(int followerId, int followeeId) The user with ID followerId started unfollowing the user with ID followeeId.
 

Example 1:

Input
["Twitter", "postTweet", "getNewsFeed", "follow", "postTweet", "getNewsFeed", "unfollow", "getNewsFeed"]
[[], [1, 5], [1], [1, 2], [2, 6], [1], [1, 2], [1]]
Output
[null, null, [5], null, null, [6, 5], null, [5]]

Explanation
Twitter twitter = new Twitter();
twitter.postTweet(1, 5); // User 1 posts a new tweet (id = 5).
twitter.getNewsFeed(1);  // User 1's news feed should return a list with 1 tweet id -> [5]. return [5]
twitter.follow(1, 2);    // User 1 follows user 2.
twitter.postTweet(2, 6); // User 2 posts a new tweet (id = 6).
twitter.getNewsFeed(1);  // User 1's news feed should return a list with 2 tweet ids -> [6, 5]. Tweet id 6 should precede tweet id 5 because it is posted after tweet id 5.
twitter.unfollow(1, 2);  // User 1 unfollows user 2.
twitter.getNewsFeed(1);  // User 1's news feed should return a list with 1 tweet id -> [5], since user 1 is no longer following user 2.
 

Constraints:

1 <= userId, followerId, followeeId <= 500
0 <= tweetId <= 104
All the tweets have unique IDs.
At most 3 * 104 calls will be made to postTweet, getNewsFeed, follow, and unfollow.
*/

class Tweet
{
public:
    int id, time;
    Tweet *next;
    Tweet(int id, int t)
    {
        this->id = id;
        time = t;
        next = NULL;
    }
};
class User
{
public:
    int id;
    unordered_set<int> followed;
    Tweet *tweets;
    void follow(int id)
    {
        followed.insert(id);
    }
    void unfollow(int id)
    {
        followed.erase(id);
    }
    User(int id)
    {
        this->id = id;
        follow(id);
        tweets = NULL;
    }
    void post(int id, int time)
    {
        Tweet *t = tweets;
        tweets = new Tweet(id, time);
        tweets->next = t;
    }
};
class Twitter
{
private:
    int time = 0;
    unordered_map<int, User *> userMap;
    struct compare
    {
        bool operator()(const Tweet *l, const Tweet *r)
        {
            return l->time < r->time;
        }
    };

public:
    /** Initialize your data structure here. */
    Twitter()
    {
    }

    /** Compose a new tweet. */
    void postTweet(int userId, int tweetId)
    {
        if (!userMap.count(userId))
        {
            User *u = new User(userId);
            userMap[userId] = u;
        }
        userMap[userId]->post(tweetId, time++);
    }

    /** Retrieve the 10 most recent tweet ids in the user's news feed. Each item in the news feed must be posted by users who the user followed or by the user herself. Tweets must be ordered from most recent to least recent. */
    vector<int> getNewsFeed(int userId)
    {
        vector<int> res;
        if (!userMap.count(userId))
            return res;
        unordered_set<int> users = userMap[userId]->followed;
        priority_queue<Tweet *, vector<Tweet *>, compare> pq;
        for (int user : users)
        {
            Tweet *t = userMap[user]->tweets;
            if (t != NULL)
                pq.push(t);
        }
        int cnt = 0;
        while (!pq.empty() && cnt++ < 10)
        {
            Tweet *t = pq.top();
            pq.pop();
            res.push_back(t->id);
            if (t->next != NULL)
                pq.push(t->next);
        }
        return res;
    }

    /** Follower follows a followee. If the operation is invalid, it should be a no-op. */
    void follow(int followerId, int followeeId)
    {
        if (!userMap.count(followerId))
        {
            User *u = new User(followerId);
            userMap[followerId] = u;
        }
        if (!userMap.count(followeeId))
        {
            User *u = new User(followeeId);
            userMap[followeeId] = u;
        }
        userMap[followerId]->follow(followeeId);
    }

    /** Follower unfollows a followee. If the operation is invalid, it should be a no-op. */
    void unfollow(int followerId, int followeeId)
    {
        if (!userMap.count(followerId) || followerId == followeeId)
            return;
        userMap[followerId]->unfollow(followeeId);
    }
};

/**
 * Your Twitter object will be instantiated and called as such:
 * Twitter* obj = new Twitter();
 * obj->postTweet(userId,tweetId);
 * vector<int> param_2 = obj->getNewsFeed(userId);
 * obj->follow(followerId,followeeId);
 * obj->unfollow(followerId,followeeId);
 */