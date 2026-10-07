/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(V + E)
// Space: O(V + E)

function canFinish(numCourses: number, prerequisites: number[][]): boolean {
    const adj: number[][] = Array.from({ length: numCourses }, () => []);
    const inDegree: number[] = new Array(numCourses).fill(0);

    for (const [course, prereq] of prerequisites) {
        adj[prereq].push(course);
        inDegree[course]++;
    }

    const queue: number[] = [];
    for (let i = 0; i < numCourses; i++) {
        if (inDegree[i] === 0) {
            queue.push(i);
        }
    }

    let head = 0;
    let visitedCount = 0;

    while (head < queue.length) {
        const curr = queue[head++];
        visitedCount++;

        for (const neighbor of adj[curr]) {
            inDegree[neighbor]--;
            if (inDegree[neighbor] === 0) {
                queue.push(neighbor);
            }
        }
    }

    return visitedCount === numCourses;
}
