/* URL: https://www.geeksforgeeks.org/write-an-efficient-c-function-to-convert-a-tree-into-its-mirror-tree/
 *
 */

package binarytree;
import  java.util.Scanner;

class Node {
	Node left;
	Node right;
	int value;

	Node (int v) {
		value = v;
		right = null;
		left = null;
	}
}


public class MirrorImageOfTree {
	private static void inOrder(Node root) {
		if (root == null)
			return;
		inOrder(root.left);
		System.out.print(root.value + " ");
		inOrder(root.right);
	}

	private static void findMirror(Node root) {
		if (root == null)
			return;
		if (root.left == null && root.right == null)
			return;
		// Interchange left and right pointers
		Node temp = root.left;
		root.left = root.right;
		root.right = temp;
		// Recurse over the left and right sub-trees doing the same.
		if (root.left != null)
			findMirror(root.left);
		if (root.right != null)
			findMirror(root.right);
	}

	public static void main(String[] args) {
		//Scanner sc = new Scanner(System.in);
		//System.out.println("Enter the bianry tree values: ");
		Node root = new Node(1);
		root.left = new Node(3);
		root.right = new Node(2);
		root.right.left = new Node(5);
		root.right.right = new Node(4);
		root.left.left = new Node(7);
		root.left.right = new Node(8);

		inOrder(root);
		System.out.println();
		findMirror(root);
		System.out.println();
		inOrder(root);
		System.out.println();


	}
}