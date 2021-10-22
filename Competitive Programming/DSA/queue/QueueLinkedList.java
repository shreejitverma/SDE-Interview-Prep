/* URL: https://www.geeksforgeeks.org/queue-set-2-linked-list-implementation/
 * Queue underflow condition easier to check using size of the queue.
 */

package queue;

class QueueNode {
	int value;
	QueueNode next;
	QueueNode(int val) {
		value = val;
		next = null;
	}
}

public class QueueLinkedList {
	QueueNode rear;
	QueueNode front;
	int size = 0;

	QueueLinkedList() {
		rear = null;
		front = null;
	}

	public void print() {
		System.out.println();
		QueueNode q = front;
		if (q != null) {
			while (q != null) {
				System.out.print(q.value + " ");
				q = q.next;
			}
		}
		else {
			System.out.println("Queue is empty!");
		}
		System.out.println();
	}

	public QueueNode createNode(int val) {
		return new QueueNode(val);
	}

	/* Create a new queue node and add the value.
	 * Point the rear to this node.
	 * No nede to check overflow since this is linked list.
	 */
	public void enQueue(int val) {

		QueueNode q = createNode(val);
		if (size == 0) {
			front = q;
			rear = q;
		}
		else {
			rear.next = q;
			rear = q;
		}
		size++;
	}

	/* Dequeue from the front.
	 * Jump to the next node.
	 * Here need to check the condition of underflow.
	 */
	public int deQueue() {
		int val = Integer.MIN_VALUE;
		if (size != 0) {

			val = front.value;
			QueueNode q = front;
			front = front.next;
			q = null;
			size--;
		}
		else {
			System.out.println("Queue underflow condition");
		}
		return val;
	}

}