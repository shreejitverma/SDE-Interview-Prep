/* URL: Reversing a linked list.
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

class LinkedList {
	// Create a linked list with atleast one node
	Node head;
	LinkedList(int data) {
		head = new Node(data);
	}

	public void addNode(int data) {
		Node temp = new Node(data);
		temp.next = head;
		head = temp;
	}

	public void reverseLinkedList() {
		// No need for changes or reversal.
		if (head == null || head.next == null) {
			return;
		}
		else {
			Node temp = head;
			Node curr = head.next;
			temp.next = null;
			while (curr != null && curr.next != null) {
				Node newNode = curr.next;
				curr.next = temp;
				temp = curr;
				curr = newNode;
			}

			curr.next = temp;
			head = curr;

		}
	}

	public void printLinkedList() {
		Node temp = head;
		System.out.println();
		while (temp != null) {
			System.out.print(temp.data + " ");
			temp = temp.next;
		}
		System.out.println();
	}
}

public class LinkedListWork {
	public static void main(String[] args) {
		LinkedList ll = new LinkedList(1);
		//ll.addNode(2);
		//ll.addNode(3);
		//ll.addNode(4);
		//ll.addNode(5);
		ll.printLinkedList();

		ll.reverseLinkedList();
		ll.printLinkedList();
	}
}