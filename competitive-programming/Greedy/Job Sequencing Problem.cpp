/*
https://practice.geeksforgeeks.org/problems/job-sequencing-problem-1587115620/1
Job Sequencing Problem 
Medium Accuracy: 48.94% Submissions: 47973 Points: 4
Given a set of N jobs where each jobi has a deadline and profit associated with it. Each job takes 1 unit of time to complete and only one job can be scheduled at a time. We earn the profit if and only if the job is completed by its deadline. The task is to find the number of jobs done and the maximum profit.

Note: Jobs will be given in the form (Jobid, Deadline, Profit) associated with that Job.


Example 1:

Input:
N = 4
Jobs = {(1,4,20),(2,1,10),(3,1,40),(4,1,30)}
Output:
2 60
Explanation:
Job1 and Job3 can be done with
maximum profit of 60 (20+40).
Example 2:

Input:
N = 5
Jobs = {(1,2,100),(2,1,19),(3,2,27),
        (4,1,25),(5,1,15)}
Output:
2 127
Explanation:
2 jobs can be done with
maximum profit of 127 (100+27).

Your Task :
You don't need to read input or print anything. Your task is to complete the function JobScheduling() which takes an integer N and an array of Jobs(Job id, Deadline, Profit) as input and returns the count of jobs and maximum profit.


Expected Time Complexity: O(NlogN)
Expected Auxilliary Space: O(N)


Constraints:
1 <= N <= 105
1 <= Deadline <= 100
1 <= Profit <= 500
*/
/*
struct Job 
{ 
    int id;	 // Job Id 
    int dead; // Deadline of job 
    int profit; // Profit if job is over before or on deadline 
};
*/

class Solution
{
public:
    //Function to find the maximum profit and the number of jobs done.
    vector<int> JobScheduling(Job arr[], int n)
    {
        unordered_map<int, priority_queue<int> > m;
        int max1 = 0;
        for (int i = 0; i < n; i++)
        {
            m[arr[i].dead].push(arr[i].profit);
            max1 = max(max1, arr[i].dead);
        }
        priority_queue<int, vector<int>, greater<int> > p;
        for (int i = 1; i <= max1; i++)
        {
            int r = m[i].size();
            for (int j = 0; j < min(i, r); j++)
            {
                int k = m[i].top();
                p.push(k);
                m[i].pop();
            }
            while (p.size() > i)
            {
                p.pop();
            }
        }
        int ans = 0, c = p.size();
        while (p.size())
        {
            ans = ans + p.top();
            //cout << p.top();
            p.pop();
        }
        vector<int> ans1;
        ans1.push_back(c);
        ans1.push_back(ans);
        return ans1;
    }
};

// Another One
bool com(Job a, Job b)
{
    return a.profit > b.profit;
}

class Solution
{
public:
    vector<int> JobScheduling(Job arr[], int n){
        {//Sort the jobs according to maximum profit
         sort(arr, arr + n, com);

    //we will keep a deadline check array, at which deadline the jobs had done
    bool check[n + 1] = {0};

    int profit = 0;
    int noOfJobs = 0;

    //IMP* any job can be done on or before its deadline
    //So for every job (starting with jobs of maximum profit, since we have to
    //find maximum porfit) it can be done on or before its deadling

    //So we will check on check array if that deadline is free or not
    //If free/false then we will do that job in that deadline
    //If not free/true then we will traverse backward to the check array as
    //Jobs can only be done on its deadline or "before it".

    //Iterate throught the jobs array
    for (int i = 0; i < n; i++)
    {

        //Iterate to the check array backwards
        //why j=min() because maybe the deadline is more than n but we can do it
        //before n also.
        for (int j = min(n, arr[i].dead); j >= 1; j--)
        {
            if (check[j] == 0)
            {
                profit += arr[i].profit;
                noOfJobs++;
                check[j] = 1;
                break;
            }
        }
    }

    return {noOfJobs, profit};
}
}
;
// Program to find the maximum profit job sequence from a given array
// of jobs with deadlines and profits
#include <iostream>
#include <algorithm>
using namespace std;

// A structure to represent a job
struct Job
{
    char id;    // Job Id
    int dead;   // Deadline of job
    int profit; // Profit if job is over before or on deadline
};

// This function is used for sorting all jobs according to profit
bool comparison(Job a, Job b)
{
    return (a.profit > b.profit);
}

// Returns minimum number of platforms required
void printJobScheduling(Job arr[], int n)
{
    // Sort all jobs according to decreasing order of profit
    sort(arr, arr + n, comparison);

    int result[n]; // To store result (Sequence of jobs)
    bool slot[n];  // To keep track of free time slots

    // Initialize all slots to be free
    for (int i = 0; i < n; i++)
        slot[i] = false;

    // Iterate through all given jobs
    for (int i = 0; i < n; i++)
    {
        // Find a free slot for this job (Note that we start
        // from the last possible slot)
        for (int j = min(n, arr[i].dead) - 1; j >= 0; j--)
        {
            // Free slot found
            if (slot[j] == false)
            {
                result[j] = i;  // Add this job to result
                slot[j] = true; // Make this slot occupied
                break;
            }
        }
    }

    // Print the result
    for (int i = 0; i < n; i++)
        if (slot[i])
            cout << arr[result[i]].id << " ";
}

// Driver code
int main()
{
    Job arr[] = {{'a', 2, 100}, {'b', 1, 19}, {'c', 2, 27}, {'d', 1, 25}, {'e', 3, 15}};
    int n = sizeof(arr) / sizeof(arr[0]);
    cout << "Following is maximum profit sequence of jobs \n";

    // Function call
    printJobScheduling(arr, n);
    return 0;
}