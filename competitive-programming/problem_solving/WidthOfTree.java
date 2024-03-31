/* URL: https://www.geeksforgeeks.org/maximum-width-of-a-binary-tree/
 * Solution: Can use a queue and do level-order traversal to count the 
 * number of children of a tree.
 * Complexity: O(n) the number of values in the binary tree.
 */

package problem_solving;
import java.util.Scanner;
import java.util.Queue;
import java.util.LinkedList;

class Node {
	int value;
	Node left;
	Node right;

	Node(int v) {
		value = v;
		left = null;
		right = null;
	}
}

public class WidthOfTree {
	private static int doLevelOrderTraversal(Node root) {
		Queue<Node> queue = new LinkedList<Node>();

		Node temp = root;
		int maxWidth = 0;
		queue.add(temp);
		while (!queue.isEmpty()) {
			int count = queue.size();
			maxWidth = Math.max(count, maxWidth);

			while (count-- > 0) {
				Node t = queue.remove();
				if (t.left != null) {
					queue.add(t.left);
				}
				if (t.right != null) {
					queue.add(t.right);
				}
			}
		}
		return maxWidth;
	}
	
	public static void main(String[] args) {
		//Scanner sc = new Scanner(System.in);
		//int input = sc.nextInt();
		Node root = new Node(1);
		root.left = new Node(1);
		root.right = new Node(2);
		root.left.left = new Node(3);
		root.left.right = new Node(4);
		root.right.left = new Node(5);
		root.right.right = new Node(6);
		root.left.left.left = new Node(7);
		root.left.left.right = new Node(8);
		root.right.left.left = new Node(9);
		root.right.left.right = new Node(10);

		System.out.println("Maximum widht of the tree: " + doLevelOrderTraversal(root));
	}
}