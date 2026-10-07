/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(V + E)
// Space: O(V) auxiliary

class Node {
    val: number;
    neighbors: Node[];
    constructor(val?: number, neighbors?: Node[]) {
        this.val = (val === undefined ? 0 : val);
        this.neighbors = (neighbors === undefined ? [] : neighbors);
    }
}

function cloneGraph(node: Node | null): Node | null {
    if (!node) {
        return null;
    }

    const visited: Map<Node, Node> = new Map();
    const queue: Node[] = [node];

    visited.set(node, new Node(node.val));

    while (queue.length > 0) {
        const curr = queue.shift()!;

        for (const neighbor of curr.neighbors) {
            if (!visited.has(neighbor)) {
                visited.set(neighbor, new Node(neighbor.val));
                queue.push(neighbor);
            }
            visited.get(curr)!.neighbors.push(visited.get(neighbor)!);
        }
    }

    return visited.get(node)!;
}
