/* URL: https://www.geeksforgeeks.org/connect-nodes-at-same-level/
 * We need to connect the nodes through a pointer at the same level
 * in a binary tree.
 * Queue is an interface in java. It needs a concrete class like
 * LinkedList to be used. But, it is not a thread-safe implementation
 * in java.
 */

package problem_solving;

import java.util.Queue;
import java.util.LinkedList;
import java.util.Scanner;

class Node {
	int data;
	Node left;
	Node right;
	Node sameLevelRight;

	Node (int d) {
		data = d;
		left = right = sameLevelRight = null;
	}
}

/* Logic of the implementation: Use the queue to do a level order traversal.
 * Now, store the levels of the nodes.
 * For nodes at the same level, connect the pointers to their values present
 * right next to them.
 * Complete!
 */
public class ConnectNodeAtSameLevel {

	private static void doLevelOrderTraversal(Node root) {
		Queue<Node> q = new LinkedList<Node>();
		q.add(root);
		// Adding null to distinguish between levels.
		q.add(null);
		
		while (!q.isEmpty()) {
			Node currentNode = q.remove();
			if (currentNode != null) {
				if (q.peek() != null) {
					currentNode.sameLevelRight = q.peek();
				}
				if (currentNode.left != null) {
					q.add(currentNode.left);
				}
				if (currentNode.right != null) {
					q.add(currentNode.right);
				}
			}
			// One level is over
			else if (!q.isEmpty()) {
				q.add(null);
			}
		}
	}

	private static void connectNodes(Node root) {
		
		doLevelOrderTraversal(root);
	}
	
	private static void getRightValue(Node root, int value) {
		if (root == null)
			return;
		else if (root.sameLevelRight != null && root.data == value) {
			System.out.println(root.sameLevelRight.data);
			return;
		}
		else {
			getRightValue(root.left, value);
			getRightValue(root.right, value);
		}
	}


	public static void main(String[] args) {
		Node root = new Node(1);
		root.sameLevelRight = root;
		root.left = new Node(2);
		root.right = new Node(3);
		root.left.left = new Node(4);
		root.left.right = new Node(5);
		root.right.left = new Node(6);
		root.right.right = new Node(7);

		connectNodes(root);
		System.out.println("Enter the value whose right value you want: ");
		Scanner sc = new Scanner(System.in);
		int value = sc.nextInt();
		getRightValue(root, value);
	}
}