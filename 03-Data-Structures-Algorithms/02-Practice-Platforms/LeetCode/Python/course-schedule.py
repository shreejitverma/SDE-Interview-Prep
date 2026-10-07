# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma

# Time:  O(V + E)
# Space: O(V + E)

from collections import deque
from typing import List


class Solution:
    def canFinish(self, numCourses: int, prerequisites: List[List[int]]) -> bool:
        adj: List[List[int]] = [[] for _ in range(numCourses)]
        in_degree: List[int] = [0] * numCourses

        for course, prereq in prerequisites:
            adj[prereq].append(course)
            in_degree[course] += 1

        queue: deque[int] = deque([i for i in range(numCourses) if in_degree[i] == 0])
        visited_count = 0

        while queue:
            curr = queue.popleft()
            visited_count += 1

            for neighbor in adj[curr]:
                in_degree[neighbor] -= 1
                if in_degree[neighbor] == 0:
                    queue.append(neighbor)

        return visited_count == numCourses
