/* Create a graph class.
 *
 */

package graphs;
import java.util.List;
import java.util.LinkedList;

public class Graph {
	int totalNodes;
	boolean[] visited;
	List<Integer> adjacencyList[];
	boolean cyclePresent;

	Graph(int totalNodes) {
		this.totalNodes = totalNodes;
		adjacencyList = new LinkedList[totalNodes + 1];
		visited = new boolean[totalNodes + 1];
		cyclePresent = false;

		for (int i = 1; i <= totalNodes; i++) {
			adjacencyList[i] = new LinkedList<Integer>();
		}
	}

	/* Create a redundant/non-redundant adjacency list
	 *
	 */
	void addEdge(int one, int two, boolean redundant) {
		if (redundant == true) {
			adjacencyList[one].add(two);
			adjacencyList[two].add(one);
		}
		else if (one < two) {
			adjacencyList[one].add(two);
		}
		else {
			adjacencyList[two].add(one);
		}
	}

	void removeEdge(int one, int two) {

	}
}