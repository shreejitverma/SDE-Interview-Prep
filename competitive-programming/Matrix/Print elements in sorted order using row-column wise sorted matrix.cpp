/*
https://practice.geeksforgeeks.org/problems/sorted-matrix2333/1
Sorted matrix 
Basic Accuracy: 69.31% Submissions: 13968 Points: 1
Given an NxN matrix Mat. Sort all elements of the matrix.

Example 1:

Input:
N=4
Mat=[[10,20,30,40],
[15,25,35,45] 
[27,29,37,48] 
[32,33,39,50]]
Output:
10 15 20 25 
27 29 30 32
33 35 37 39
40 45 48 50
Explanation:
Sorting the matrix gives this result.
Example 2:

Input:
N=3
Mat=[[1,5,3],[2,8,7],[4,6,9]]
Output:
1 2 3 
4 5 6
7 8 9
Explanation:
Sorting the matrix gives this result.
Your Task:
You don't need to read input or print anything. Your task is to complete the function sortedMatrix() which takes the integer N and the matrix Mat as input parameters and returns the sorted matrix.


Expected Time Complexity:O(N2LogN)
Expected Auxillary Space:O(N2)


Constraints:
1<=N<=1000
1<=Mat[i][j]<=105
*/

class Solution
{
public:
    vector<vector<int> > sortedMatrix(int N, vector<vector<int> > v)
    {
        vector<int> temp;
        for (auto i : v)
            temp.insert(temp.end(), i.begin(), i.end());
        sort(temp.begin(), temp.end());

        int counter = 0;
        for (int i = 0; i < v.size(); i++)
        {
            for (int j = 0; j < v[i].size(); j++)
            {
                v[i][j] = temp[counter++];
            }
        }
        return v;
    }
};