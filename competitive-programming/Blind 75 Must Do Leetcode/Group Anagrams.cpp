/*
https://leetcode.com/problems/group-anagrams/
49. Group Anagrams
Medium

7178

261

Add to List

Share
Given an array of strings strs, group the anagrams together. You can return the answer in any order.

An Anagram is a word or phrase formed by rearranging the letters of a different word or phrase, typically using all the original letters exactly once.

 

Example 1:

Input: strs = ["eat","tea","tan","ate","nat","bat"]
Output: [["bat"],["nat","tan"],["ate","eat","tea"]]
Example 2:

Input: strs = [""]
Output: [[""]]
Example 3:

Input: strs = ["a"]
Output: [["a"]]
 

Constraints:

1 <= strs.length <= 104
0 <= strs[i].length <= 100
strs[i] consists of lowercase English letters.*/

//Runtime: 44 ms, faster than 61.55% of C++ online submissions for Group Anagrams.
//Memory Usage: 19.1 MB, less than 73.13% of C++ online submissions for Group Anagrams.
//time: O(NKlogK), space: O(NK), K is the maximum length of string in "strs"
struct RetrieveValue
{
    template <typename T>
    typename T::second_type operator()(T keyValuePair) const
    {
        return keyValuePair.second;
    }
};

class Solution
{
public:
    vector<vector<string> > groupAnagrams(vector<string> &strs)
    {
        unordered_map<string, vector<string> > groups;

        for (string &str : strs)
        {
            string sorted = str;
            sort(sorted.begin(), sorted.end());
            groups[sorted].push_back(str);
        }

        vector<vector<string> > ans;
        transform(groups.begin(), groups.end(), back_inserter(ans), RetrieveValue());

        return ans;
    }
};

//Approach 2: Categorize by Count(no sorting)
//Runtime: 136 ms, faster than 7.61% of C++ online submissions for Group Anagrams.
//Memory Usage: 29.1 MB, less than 5.97% of C++ online submissions for Group Anagrams.
//time: O(NK), space: O(NK)

struct RetrieveValue
{
    template <typename T>
    typename T::second_type operator()(T keyValuePair) const
    {
        return keyValuePair.second;
    }
};

class Solution
{
public:
    vector<vector<string> > groupAnagrams(vector<string> &strs)
    {
        unordered_map<string, vector<string> > groups;

        for (string &str : strs)
        {
            vector<int> count(26, 0);
            for (char c : str)
            {
                count[c - 'a']++;
            }

            string scount = "";
            for (int &e : count)
            {
                scount += (to_string(e) + "#");
            }

            groups[scount].push_back(str);
        }

        vector<vector<string> > ans;

        transform(groups.begin(), groups.end(), back_inserter(ans), RetrieveValue());

        return ans;
    }
};
