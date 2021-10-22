/* URL: https://practice.geeksforgeeks.org/problems/detect-loop-in-linked-list/1/?track=SP-Misc
 * Find a loop in the linked list if present.
 */

package problem_solving;
import java.util.Scanner;
import java.util.HashMap;

/* Node user-defined class */
class Node {
	int value;
	Node next;

	Node(int v) {
		value = v;
		next = null;
	}
}

public class DetectLoop {
	// Method which uses an extra store to detect a loop
	// O(length of list) complexity
	private static int detectLoop(Node head) {
		// Store the visited nodes in the HashMap
		HashMap<Node, Integer> hm = new HashMap<Node, Integer>();

		while (head != null) {
			if (hm.containsKey(head) == true) {
				return head.value;
			}
			else {
				hm.put(head, 1);
			}
			head = head.next;
		}
		return -1;
	}
	
	// Avoid storing extra store in HashMap
	private static int detectLoop(Node head, int avoid) {
		if (head == null)
			return -1;
		Node tortoise = head;
		Node hare     = head.next;

		while (tortoise != null && hare != null && hare.next != null && tortoise != hare) {
			tortoise = tortoise.next;
			hare     = hare.next.next;
		}

		if (tortoise == hare) {
			return hare.value;
		}
		return -1;
	}
	public static void main(String[] args) {
		Scanner sc = new Scanner(System.in);
		Node head = new Node(1);
		head.next = new Node(2);
		head.next.next = new Node(3);
		head.next.next.next = new Node(4);
		head.next.next.next.next = new Node(5);
		head.next.next.next.next = head.next.next;

		//int result = detectLoop(head);
		//Avoid hashmap storage
		int result = detectLoop(head, 1);

		if (result == -1) {
			System.out.println("No loop in the list");
		}
		else {
			System.out.println("Loop present in the node of value: " + result);	
		}
	}
}