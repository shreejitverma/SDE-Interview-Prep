/*
https://leetcode.com/problems/tweet-counts-per-frequency/

1348. Tweet Counts Per Frequency
Medium

78

150

Add to List

Share
A social media company is trying to monitor activity on their site by analyzing the number of tweets that occur in select periods of time. These periods can be partitioned into smaller time chunks based on a certain frequency (every minute, hour, or day).

For example, the period [10, 10000] (in seconds) would be partitioned into the following time chunks with these frequencies:

Every minute (60-second chunks): [10,69], [70,129], [130,189], ..., [9970,10000]
Every hour (3600-second chunks): [10,3609], [3610,7209], [7210,10000]
Every day (86400-second chunks): [10,10000]
Notice that the last chunk may be shorter than the specified frequency's chunk size and will always end with the end time of the period (10000 in the above example).

Design and implement an API to help the company with their analysis.

Implement the TweetCounts class:

TweetCounts() Initializes the TweetCounts object.
void recordTweet(String tweetName, int time) Stores the tweetName at the recorded time (in seconds).
List<Integer> getTweetCountsPerFrequency(String freq, String tweetName, int startTime, int endTime) Returns a list of integers representing the number of tweets with tweetName in each time chunk for the given period of time [startTime, endTime] (in seconds) and frequency freq.
freq is one of "minute", "hour", or "day" representing a frequency of every minute, hour, or day respectively.
 

Example:

Input
["TweetCounts","recordTweet","recordTweet","recordTweet","getTweetCountsPerFrequency","getTweetCountsPerFrequency","recordTweet","getTweetCountsPerFrequency"]
[[],["tweet3",0],["tweet3",60],["tweet3",10],["minute","tweet3",0,59],["minute","tweet3",0,60],["tweet3",120],["hour","tweet3",0,210]]

Output
[null,null,null,null,[2],[2,1],null,[4]]

Explanation
TweetCounts tweetCounts = new TweetCounts();
tweetCounts.recordTweet("tweet3", 0);                              // New tweet "tweet3" at time 0
tweetCounts.recordTweet("tweet3", 60);                             // New tweet "tweet3" at time 60
tweetCounts.recordTweet("tweet3", 10);                             // New tweet "tweet3" at time 10
tweetCounts.getTweetCountsPerFrequency("minute", "tweet3", 0, 59); // return [2]; chunk [0,59] had 2 tweets
tweetCounts.getTweetCountsPerFrequency("minute", "tweet3", 0, 60); // return [2,1]; chunk [0,59] had 2 tweets, chunk [60,60] had 1 tweet
tweetCounts.recordTweet("tweet3", 120);                            // New tweet "tweet3" at time 120
tweetCounts.getTweetCountsPerFrequency("hour", "tweet3", 0, 210);  // return [4]; chunk [0,210] had 4 tweets
 

Constraints:

0 <= time, startTime, endTime <= 109
0 <= endTime - startTime <= 104
There will be at most 104 calls in total to recordTweet and getTweetCountsPerFrequency.
*/
// Time:  add:   O(logn),
//        query: O(c), c is the total count of matching records
// Space: O(n)

class TweetCounts
{
public:
    TweetCounts()
    {
    }

    void recordTweet(string tweetName, int time)
    {
        records_[tweetName].emplace(time);
    }

    vector<int> getTweetCountsPerFrequency(string freq, string tweetName, int startTime, int endTime)
    {
        static const unordered_map<string, int> lookup = {{"minute", 60}, {"hour", 3600}, {"day", 86400}};
        const auto &delta = lookup.at(freq);
        vector<int> result((endTime - startTime) / delta + 1);
        for (auto it = records_[tweetName].lower_bound(startTime);
             it != records_[tweetName].end() && *it <= endTime; ++it)
        {
            ++result[(*it - startTime) / delta];
        }
        return result;
    }

private:
    unordered_map<string, multiset<int> > records_;
};

// Time:  add:   O(n),
//        query: O(rlogn), r is the size of result
// Space: O(n)
class TweetCounts2
{
public:
    TweetCounts2()
    {
    }

    void recordTweet(string tweetName, int time)
    {
        auto it = lower_bound(records_[tweetName].begin(), records_[tweetName].end(), time);
        records_[tweetName].insert(it, time);
    }

    vector<int> getTweetCountsPerFrequency(string freq, string tweetName, int startTime, int endTime)
    {
        static const unordered_map<string, int> lookup = {{"minute", 60}, {"hour", 3600}, {"day", 86400}};
        const auto &delta = lookup.at(freq);
        vector<int> result;
        for (int i = startTime; i <= endTime; i += delta)
        {
            int j = min(i + delta, endTime + 1);
            result.emplace_back(distance(lower_bound(records_[tweetName].cbegin(), records_[tweetName].cend(), i),
                                         lower_bound(records_[tweetName].cbegin(), records_[tweetName].cend(), j)));
        }
        return result;
    }

private:
    unordered_map<string, vector<int> > records_;
};

// Time:  add:   O(1),
//        query: O(n)
// Space: O(n)
class TweetCounts3
{
public:
    TweetCounts3()
    {
    }

    void recordTweet(string tweetName, int time)
    {
        records_[tweetName].emplace_back(time);
    }

    vector<int> getTweetCountsPerFrequency(string freq, string tweetName, int startTime, int endTime)
    {
        static const unordered_map<string, int> lookup = {{"minute", 60}, {"hour", 3600}, {"day", 86400}};
        const auto &delta = lookup.at(freq);
        vector<int> result((endTime - startTime) / delta + 1);
        for (const auto &t : records_[tweetName])
        {
            if (startTime <= t && t <= endTime)
            {
                ++result[(t - startTime) / delta];
            }
        }
        return result;
    }

private:
    unordered_map<string, vector<int> > records_;
};