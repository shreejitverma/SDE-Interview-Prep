/* URL: https://www.geeksforgeeks.org/write-a-c-function-to-print-the-middle-of-the-linked-list/
 *
 */

package problem_solving;

class Node {
	int data;
	Node next;

	Node(int d) {
		data = d;
		next = null;
	}
}

public class FindMiddleLinkedList {
	/* Can try the tortoise and hare solution.
	 *
	 */
	private static Node findMiddleOfLinkedList(Node head) {
		if (head == null) {
			return head;
		}
		Node tortoise = head;
		Node hare = head.next;
		Node middle = tortoise;

		while (tortoise != null && hare != null && hare.next != null) {
			tortoise = tortoise.next;
			hare = hare.next.next;
		}
		middle = tortoise;
		return middle;
	}

	public static void main(String[] args) {
		Node head = new Node(1);
		head.next = new Node(2);
		head.next.next = new Node(3);
		head.next.next.next = new Node(4);
		head.next.next.next.next = new Node(5);
		head.next.next.next.next.next = new Node(6);
		head.next.next.next.next.next.next = new Node(7);
		head.next.next.next.next.next.next.next = new Node(8);
		Node middle = findMiddleOfLinkedList(head);
		System.out.println("Middle value: " + middle.data);
	}
}