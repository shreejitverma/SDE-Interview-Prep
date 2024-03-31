/* URL: https://www.geeksforgeeks.org/a-program-to-check-if-a-binary-tree-is-bst-or-not/
 *
 */
package binary_tree;

class Node {
	int data;
	Node left;
	Node right;

	Node (int d) {
		data = d;
		left = right = null;
	}
}

public class BinaryTree {

	/* In order to check for BST, we can avoid using extra 
	 * auxillary space, and send the right most node of the left 
	 * sub-tree as the value to check with the current root node.
	 * Instead of doing an inOrder and checking if the subsequent
	 * values are sorted or not (which needs an extra O(n) array, 
	 * we can do the same check, without using the array)
	 */
	private static int checkBST(Node root) {
		if (root == null)
			return -1;
		if (root.left == null && root.right == null)
			return root.data;
		int lValue = checkBST(root.left);
		if (lValue > root.data)
			return -1;
		int rValue = checkBST(root.right);
		if (rValue < root.data)
			return -1;
		return rValue;
	}

	public static void main(String[] args) {
		// Hardcoding values here for faster check
		Node root = new Node(3);
		root.left = new Node(2);
		root.right = new Node(5);
		root.left.left = new Node(1);
		root.left.right = new Node(4);
		//root.right.left = new Node(6);
		//root.right.right = new Node(-1);

		boolean isBST = checkBST(root) != -1 ? true : false;

		if (isBST) {
			System.out.println("It is a BST");
		}
		else {
			System.out.println("It is not a BST");
		}
	}
}