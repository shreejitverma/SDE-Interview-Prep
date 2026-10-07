/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(V + E)
// Space: O(V + E)

package main

func canFinish(numCourses int, prerequisites [][]int) bool {
	adj := make([][]int, numCourses)
	inDegree := make([]int, numCourses)

	for _, pre := range prerequisites {
		course := pre[0]
		prereq := pre[1]
		adj[prereq] = append(adj[prereq], course)
		inDegree[course]++
	}

	queue := make([]int, 0, numCourses)
	for i := 0; i < numCourses; i++ {
		if inDegree[i] == 0 {
			queue = append(queue, i)
		}
	}

	visitedCount := 0
	for len(queue) > 0 {
		curr := queue[0]
		queue = queue[1:]
		visitedCount++

		for _, neighbor := range adj[curr] {
			inDegree[neighbor]--
			if inDegree[neighbor] == 0 {
				queue = append(queue, neighbor)
			}
		}
	}

	return visitedCount == numCourses
}
