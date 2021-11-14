/*
https://practice.geeksforgeeks.org/problems/n-meetings-in-one-room-1587115620/1
N meetings in one room 
Easy Accuracy: 43.1% Submissions: 56235 Points: 2
There is one meeting room in a firm. There are N meetings in the form of (start[i], end[i]) where start[i] is start time of meeting i and end[i] is finish time of meeting i.
What is the maximum number of meetings that can be accommodated in the meeting room when only one meeting can be held in the meeting room at a particular time?

Note: Start time of one chosen meeting can't be equal to the end time of the other chosen meeting.


Example 1:

Input:
N = 6
start[] = {1,3,0,5,8,5}
end[] =  {2,4,6,7,9,9}
Output: 
4
Explanation:
Maximum four meetings can be held with
given start and end timings.
The meetings are - (1, 2),(3, 4), (5,7) and (8,9)
Example 2:

Input:
N = 3
start[] = {10, 12, 20}
end[] = {20, 25, 30}
Output: 
1
Explanation:
Only one meetings can be held
with given start and end timings.

Your Task :
You don't need to read inputs or print anything. Complete the function maxMeetings() that takes two arrays start[] and end[] along with their size N as input parameters and returns the maximum number of meetings that can be held in the meeting room.


Expected Time Complexity : O(N*LogN)
Expected Auxilliary Space : O(N)


Constraints:
1 ≤ N ≤ 105
0 ≤ start[i] < end[i] ≤ 105
*/

static bool compare(pair<int, int = ""> a, pair<int, int = ""> b)
{
    return a.second < b.second;
}

int maxMeetings(int start[], int end[], int n)
{
    int ct = 1, i = 0;

    vector<pair<int, int = ""> > vc;

    for (int i = 0; i < n; i++)
        vc.push_back({start[i], end[i]});

    sort(vc.begin(), vc.end(), compare);

    for (int j = 1; j < n; j++)
    {

        if (vc[j].first > vc[i].second)
            ct++, i = j;
    }

    return ct;
}