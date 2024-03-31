/* URL: https://www.geeksforgeeks.org/queue-using-stacks/
 */

package problem_solving;
import java.util.Stack;
import java.util.LinkedList;
import java.util.Iterator;
import java.util.Scanner;

public class QueueUsingStack {
	private static Stack queue;
	private static int dequeue(Stack queue) {
		Stack temp = new Stack<Integer>();
		Iterator it = queue.iterator();

		while (it.hasNext()) {
			temp.push(it.next());
		}
		int value = (int)temp.pop();
		it = temp.iterator();

		while (!queue.isEmpty()) {
			queue.pop();
		}

		while (it.hasNext()) {
			queue.push(it.next());
		}
		return value;
	}

	private static void enqueue(Stack q, int value) {
		q.push(value);
	}

	private static void print(Stack queue) {
		System.out.println();
		Iterator it = queue.iterator();
		while (it.hasNext()) {
			System.out.print(it.next());
		}
		System.out.println();
	}

	public static void main(String[] args) {
		queue = new Stack<Integer>();
		Scanner sc = new Scanner(System.in);

		while (true) {
			System.out.println("Enter 1/2/3/ for push/pop/print the queue: ");
			int data = sc.nextInt();

			switch (data) {
				case 1:
					System.out.println("Enter the value to push: ");
					int v = sc.nextInt();
					enqueue(queue, v);
					break;
				case 2:
					v = dequeue(queue);
					System.out.println(v);
					break;
				case 3:
					print(queue);
					break;
				default:

					break;
			}

		}
	}	
}