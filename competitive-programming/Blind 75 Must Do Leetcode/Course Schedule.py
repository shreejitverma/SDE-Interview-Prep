'''https://leetcode.com/problems/course-schedule/
207. Course Schedule
Medium

7445

303

Add to List

Share
There are a total of numCourses courses you have to take, labeled from 0 to numCourses - 1. You are given an array prerequisites where prerequisites[i] = [ai, bi] indicates that you must take course bi first if you want to take course ai.

For example, the pair [0, 1], indicates that to take course 0 you have to first take course 1.
Return true if you can finish all courses. Otherwise, return false.

 

Example 1:

Input: numCourses = 2, prerequisites = [[1,0]]
Output: true
Explanation: There are a total of 2 courses to take. 
To take course 1 you should have finished course 0. So it is possible.
Example 2:

Input: numCourses = 2, prerequisites = [[1,0],[0,1]]
Output: false
Explanation: There are a total of 2 courses to take. 
To take course 1 you should have finished course 0, and to take course 0 you should also have finished course 1. So it is impossible.
 

Constraints:

1 <= numCourses <= 105
0 <= prerequisites.length <= 5000
prerequisites[i].length == 2
0 <= ai, bi < numCourses
All the pairs prerequisites[i] are unique.'''

# [207] Course Schedule
#


class Solution:
    def canFinish(self, numCourses: int, prerequisites: List[List[int]]) -> bool:
        indegree = [0 for i in range(numCourses)]
        connection = {i: [] for i in range(numCourses)}
        for link in prerequisites:
            connection[link[1]].append(link[0])
            indegree[link[0]] += 1
        zero_indegree = []
        for i in range(numCourses):
            if indegree[i] == 0:
                zero_indegree.append(i)
        i = 0
        while i < len(zero_indegree):
            for node in connection[zero_indegree[i]]:
                indegree[node] -= 1
                if indegree[node] == 0:
                    zero_indegree.append(node)
            i += 1
        if len(zero_indegree) == numCourses:
            return True
        else:
            return False

    def DFS(self):
        record = [[0 for _ in range(numCourses)]
                  for _ in range(numCourses)]

        visited = [False for _ in range(numCourses)]

        for item in prerequisites:
            record[item[1]][item[0]] = 1
        # sort

        def dfs(num):
            for i in range(numCourses):
                if record[num][i] == 1:
                    visited[num] = True
                    if visited[i]:
                        return False
                    else:
                        if not dfs(i):
                            return False
                        visited[num] = False
            return True

        for i in range(numCourses):
            if not dfs(i):
                return False
        return True
