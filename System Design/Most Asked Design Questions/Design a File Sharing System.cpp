/*
https://leetcode.com/problems/design-a-file-sharing-system/

1500. Design a File Sharing System
Medium

32

77

Add to List

Share
We will use a file-sharing system to share a very large file which consists of m small chunks with IDs from 1 to m.

When users join the system, the system should assign a unique ID to them. The unique ID should be used once for each user, but when a user leaves the system, the ID can be reused again.

Users can request a certain chunk of the file, the system should return a list of IDs of all the users who own this chunk. If the user receives a non-empty list of IDs, they receive the requested chunk successfully.


Implement the FileSharing class:

FileSharing(int m) Initializes the object with a file of m chunks.
int join(int[] ownedChunks): A new user joined the system owning some chunks of the file, the system should assign an id to the user which is the smallest positive integer not taken by any other user. Return the assigned id.
void leave(int userID): The user with userID will leave the system, you cannot take file chunks from them anymore.
int[] request(int userID, int chunkID): The user userID requested the file chunk with chunkID. Return a list of the IDs of all users that own this chunk sorted in ascending order.
 

Example:

Input:
["FileSharing","join","join","join","request","request","leave","request","leave","join"]
[[4],[[1,2]],[[2,3]],[[4]],[1,3],[2,2],[1],[2,1],[2],[[]]]
Output:
[null,1,2,3,[2],[1,2],null,[],null,1]
Explanation:
FileSharing fileSharing = new FileSharing(4); // We use the system to share a file of 4 chunks.

fileSharing.join([1, 2]);    // A user who has chunks [1,2] joined the system, assign id = 1 to them and return 1.

fileSharing.join([2, 3]);    // A user who has chunks [2,3] joined the system, assign id = 2 to them and return 2.

fileSharing.join([4]);       // A user who has chunk [4] joined the system, assign id = 3 to them and return 3.

fileSharing.request(1, 3);   // The user with id = 1 requested the third file chunk, as only the user with id = 2 has the file, return [2] . Notice that user 1 now has chunks [1,2,3].

fileSharing.request(2, 2);   // The user with id = 2 requested the second file chunk, users with ids [1,2] have this chunk, thus we return [1,2].

fileSharing.leave(1);        // The user with id = 1 left the system, all the file chunks with them are no longer available for other users.

fileSharing.request(2, 1);   // The user with id = 2 requested the first file chunk, no one in the system has this chunk, we return empty list [].

fileSharing.leave(2);        // The user with id = 2 left the system.

fileSharing.join([]);        // A user who doesn't have any chunks joined the system, assign id = 1 to them and return 1. Notice that ids 1 and 2 are free and we can reuse them.
 

Constraints:

1 <= m <= 105
0 <= ownedChunks.length <= min(100, m)
1 <= ownedChunks[i] <= m
Values of ownedChunks are unique.
1 <= chunkID <= m
userID is guaranteed to be a user in the system if you assign the IDs correctly.
At most 104 calls will be made to join, leave and request.
Each call to leave will have a matching call for join.
 

Follow-up:

What happens if the system identifies the user by their IP address instead of their unique ID and users disconnect and connect from the system with the same IP?
If the users in the system join and leave the system frequently without requesting any chunks, will your solution still be efficient?
If all users join the system one time, request all files, and then leave, will your solution still be efficient?
If the system will be used to share n files where the ith file consists of m[i], what are the changes you have to make?
*/

// Time:  ctor:    O(1)
//        join:    O(logu + c), u is the number of total joined users
//        leave:   O(logu + c), c is the number of chunks
//        request: O(u)
// Space: O(u * c)

// "u ~= n" solution, n is the average number of users who own the chunk
class FileSharing
{
public:
    FileSharing(int m)
    {
    }

    int join(vector<int> ownedChunks)
    {
        int userID = users_.size() + 1;
        if (!min_heap_.empty())
        {
            userID = min_heap_.top();
            min_heap_.pop();
        }
        else
        {
            users_.emplace_back();
        }
        for (const auto &chunk : ownedChunks)
        {
            users_[userID - 1].emplace(chunk);
        }
        lookup_.emplace(userID);
        return userID;
    }

    void leave(int userID)
    {
        if (!lookup_.count(userID))
        {
            return;
        }
        lookup_.erase(userID);
        users_[userID - 1].clear();
        min_heap_.emplace(userID);
    }

    vector<int> request(int userID, int chunkID)
    {
        vector<int> result;
        for (int i = 0; i < users_.size(); ++i)
        {
            if (users_[i].count(chunkID))
            {
                result.emplace_back(i + 1);
            }
        }
        if (!result.empty())
        {
            users_[userID - 1].emplace(chunkID);
        }
        return result;
    }

private:
    vector<unordered_set<int> > users_;
    unordered_set<int> lookup_;
    priority_queue<int, vector<int>, greater<int> > min_heap_;
};

// Time:  ctor:    O(1)
//        join:    O(logu + c), u is the number of total joined users
//        leave:   O(logu + c), c is the number of chunks
//        request: O(nlogn)   , n is the average number of users who own the chunk
// Space: O(u * c + m)
// "u >> n" solution
class FileSharing2
{
public:
    FileSharing2(int m)
    {
    }

    int join(vector<int> ownedChunks)
    {
        int userID = users_.size() + 1;
        if (!min_heap_.empty())
        {
            userID = min_heap_.top();
            min_heap_.pop();
        }
        else
        {
            users_.emplace_back();
        }
        for (const auto &chunk : ownedChunks)
        {
            users_[userID - 1].emplace(chunk);
            chunks_[chunk].emplace(userID);
        }
        lookup_.emplace(userID);
        return userID;
    }

    void leave(int userID)
    {
        if (!lookup_.count(userID))
        {
            return;
        }
        lookup_.erase(userID);
        for (const auto &chunk : users_[userID - 1])
        {
            chunks_[chunk].erase(userID);
        }
        users_[userID - 1].clear();
        min_heap_.emplace(userID);
    }

    vector<int> request(int userID, int chunkID)
    {
        vector<int> result(cbegin(chunks_[chunkID]), cend(chunks_[chunkID]));
        sort(begin(result), end(result));
        if (!result.empty())
        {
            users_[userID - 1].emplace(chunkID);
            chunks_[chunkID].emplace(userID);
        }
        return result;
    }

private:
    vector<unordered_set<int> > users_;
    unordered_set<int> lookup_;
    unordered_map<int, unordered_set<int> > chunks_;
    priority_queue<int, vector<int>, greater<int> > min_heap_;
};