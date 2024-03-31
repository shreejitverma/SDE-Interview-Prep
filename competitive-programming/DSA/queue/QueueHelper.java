package queue;
import java.util.Scanner;
import queue.Queue;

public class QueueHelper {
	private static void print(int[] input) {
		System.out.println();
		for (int idx = 0; idx < input.length; idx++) {
			System.out.print(input[idx] + " ");
		}
		System.out.println();
	}

	public static void main(String[] args) {
		Scanner sc = new Scanner(System.in);
		System.out.println("Enter the size of the array: ");
		int size = sc.nextInt();
		Queue q = new Queue(size);
		while (true) {
			System.out.println("Enter 1/2/3/4 to enqueue/dequeue/print/exit: ");
			int val = sc.nextInt();
			switch(val) {
				case 1:
					System.out.println("Enter the value to enqueue: ");
					int value = sc.nextInt();
					q.enQueue(value);
					break;
				case 2:
					int v = q.deQueue();
					if (v != Integer.MAX_VALUE) {
						System.out.println("Dequeued! " + v);
					}
					break;
				case 3:
					print(q.queue);
					break;
				default:
				case 4:
					System.exit(0);
					break;
			}
		}
	}
}