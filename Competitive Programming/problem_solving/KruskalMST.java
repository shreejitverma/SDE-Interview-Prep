package problem_solving;
import problem_solving.Graph;
import java.io.File;
import java.util.Scanner;
import java.io.FileNotFoundException;
import java.util.Iterator;

public class KruskalMST {

	private static void readFileAndCreateAdjacencyList(String filename, Graph g) throws FileNotFoundException {
		File f = new File(filename);
		Scanner sc = new Scanner(f);
		while (sc.hasNext()) {
			String edge = sc.nextLine();
			System.out.println(edge);
			int nodeOne = edge.charAt(0) - 48;
			int nodeTwo = edge.charAt(2) - 48;
			System.out.println(nodeOne + " " + nodeTwo);
			g.addEdge(nodeOne, nodeTwo);
		}
	}

	private static void printAdjacencyList(Graph g) {
		Iterator it = g.adjacencyList[0].iterator();

	}

	public static void main(String[] args) throws FileNotFoundException {
		Scanner sc = new Scanner(System.in);
		System.out.println("Enter total nodes: ");
		int totalNodes = sc.nextInt();
		System.out.println("Enter filename of edgelist: ");
		String filename = sc.next();
		Graph g = new Graph(totalNodes);
		readFileAndCreateAdjacencyList(filename, g);
		printAdjacencyList(g);
	}	
}