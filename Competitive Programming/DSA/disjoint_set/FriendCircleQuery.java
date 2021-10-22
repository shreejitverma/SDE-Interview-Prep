// URL: https://www.hackerrank.com/challenges/friend-circle-queries/problem?h_l=interview&playlist_slugs%5B%5D=interview-preparation-kit&playlist_slugs%5B%5D=miscellaneous

package disjoint_set;
import java.util.Vector;
import java.io.File;
import java.util.Scanner;

import java.io.IOException;
import java.util.Comparator;
import java.util.Collections;
import java.util.Iterator;

class UnionFind {
	Vector<Edge> edges;
	// To store the parent nodes of all the nodes
	int[] parent;
	// To store the rank of all the parent nodes; Rank is not equal to height of tree
	// rank(x) <= height(x) where x is the parent node of the tree that is getting formed.
	int[] rank;

	UnionFind(int totalNodes) {
		edges = new Vector<Edge>();
		parent = new int[totalNodes];
		rank = new int[totalNodes];

		// Initialise parent and rank arrays
		for (int i = 0; i < totalNodes; i++) {
			parent[i] = i;
			rank[i] = -1;
		}
	}

	// Here, need to implement path compression to improve traversal of the parent tree.
	int findParent(int x) {
		if (parent[x] != x) {
			parent[x] = findParent(parent[x]);
		}
		return x;
	}

	// Union of the nodes by rank
	// Based on the rank, combine the nodes so that the rank of x after the union is one
	// more than the old one.

	void unionByRank(int x, int y) {
		int parentX = findParent(x);
		int parentY = findParent(y);

		// If  both are part of the same set/tree then adding this will make a cycle.
		if (parentX == parentY) {
			return;
		}

		else if (rank[x] > rank[y]) {
			parent[y] = parentX;
		}
		else if (rank[x] < rank[y]) {
			parent[x] = parentY;
		}
		else {
			parent[x] = parentY;
			rank[y]++;
		}
	}

	void addEdge(Vector<Edge> edgelist) {
		edges = edgelist;
	}

	// Method to sort the edges by weight.
	// Used in Kruskal's Algorithm for Minimum Spanning Tree.
	// For Union-Find algorithm.
	void sortEdgeByWeight() {
		// Implementing the compare() method so that the edges can be 
		// sorted by weight.
		Collections.sort(edges, new Comparator<Edge>() {
			@Override
			public int compare(Edge a, Edge b) {
				if (a.weight < b.weight)
					return -1;
				return 1;
			}
		});
	}

	// Print the edges using Iterator class.
	void printEdge() {
		Iterator<Edge> it = edges.iterator();

		while (it.hasNext()) {
			Edge e = it.next();
			System.out.println(e.x + " " + e.y + " " + e.weight);
		}
	}

	// Print the parent of all the nodes.
	void printParent() {
		System.out.println("----------Parent Array----------");
		for (int i = 0; i < parent.length; i++) {
			System.out.print(i + " " + parent[i]);
		}
		System.out.println();
	}

	//Rank only matters for the root nodes.
	// rank will not change for the other nodes which are not root.
	void printRank() {
		System.out.println("------------Rank Array----------");
		for (int i = 0; i < rank.length; i++) {
			System.out.print(i + " " + rank[i]);
		}
		System.out.println();
	}
}


public class FriendCircleQuery {
	// Get the total number of nodes present in the graph/edgelist.
	// This works provided the edgelist has consecutive node numbers assigned and they are positive.
	public static int totalNodes(Vector<Edge> edgelist) {
		Iterator<Edge> it = edgelist.iterator();

		int totalNodes = -1;
		while (it.hasNext()) {
			Edge edge = it.next();
			if (totalNodes < edge.x) {
				totalNodes = edge.x;
			} 
			else if (totalNodes < edge.y) {
				totalNodes = edge.y;
			}
		}
		return totalNodes;
	}

	public static void main(String[] args) {
		try {
			File f = new File("edgelist");
			Vector<Edge> edgelist = new Vector<Edge>();
			
			Scanner sc = new Scanner(f);

			while (sc.hasNext()) {
				String split[] = sc.nextLine().split(" ");
				int a = Integer.parseInt(split[0]);
				int b = Integer.parseInt(split[1]);
				int wt = Integer.parseInt(split[2]);
				edgelist.add(new Edge(a, b, wt));
			}

			int totalNodes = totalNodes(edgelist);
			UnionFind ufind = new UnionFind(totalNodes);
			ufind.addEdge(edgelist);
			ufind.sortEdgeByWeight();
			System.out.println("-------Sorted edge list  by weight--------");
			ufind.printEdge();
			System.out.println("------------------------------------------");


		}
		catch(IOException e) {
			System.out.println(e);
		}



	}
}