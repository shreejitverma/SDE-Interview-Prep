/* URL: https://www.geeksforgeeks.org/detect-cycle-undirected-graph/
 * Solution: Try DFS
 */

package problem_solving;
import java.util.List;
import java.util.Iterator;
import java.util.LinkedList;

class Graph {
	 List<Integer>[] adjacencyList;
	boolean cyclePresent;
	int totalNodes;

	Graph(int nodes) {
		adjacencyList = new LinkedList[nodes+1];
		totalNodes = nodes;
		cyclePresent = false;

		for (int i = 1; i <= totalNodes; i++) {
			adjacencyList[i] = new LinkedList<Integer>();
		}
	}

	void addEdge(int a, int b) {
		adjacencyList[a].add(b);
		adjacencyList[b].add(a);
	}

	/* In order to find a cycle in a graph, we avoid parallel-edges in the graph.
	 * So, we send parentNode too to check that parallel edge.
	 */
	public void startDFS(int startNode, boolean[] visitedNodes, int parentNode) {
		visitedNodes[startNode] = true;
		Iterator it = adjacencyList[startNode].iterator();
		while (it.hasNext()) {
			int currentNode = (Integer)it.next();
			if (visitedNodes[currentNode] == false) {
				startDFS(currentNode, visitedNodes, startNode);
			}
			else {
				if (currentNode != parentNode) {
					cyclePresent = true;
				}
			}
		}
	}
}

public class FindCycle {
	
	private static void findCycle(Graph gp) {
		boolean[] visitedNodes = new boolean[gp.totalNodes+1];
		gp.startDFS(1, visitedNodes, -1);

	}

	/* 1 ---- 2
	 * |  \ -/|
	 * |  /\  |
	 * | /   \|
	 * 3 ---- 4
	 */
	public static void main(String[] args) {
		Graph gp = new Graph(4);
		gp.addEdge(1, 2);
		gp.addEdge(1, 3);
		gp.addEdge(1, 4);
		//gp.addEdge(2, 3);
		//gp.addEdge(2, 4);
		//gp.addEdge(3, 4);

		findCycle(gp);

		if (gp.cyclePresent) {
			System.out.println("Cycle found");
		}
		else {
			System.out.println("Cycle not found");
		}
	}
}