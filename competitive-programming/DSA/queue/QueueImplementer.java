package queue;
import queue.QueueLinkedList;
import java.util.Scanner;

public class QueueImplementer {
	public static void main(String[] args) {
		
		Scanner sc = new Scanner(System.in);
		QueueLinkedList q = new QueueLinkedList();
		while (true) {
			System.out.println("Enter 1/2/3 to enqueue/dequeue/print the Queue: ");
			int val = sc.nextInt();
			switch (val) {
				case 1:
					System.out.println("Enter the value: ");
					val = sc.nextInt();
					q.enQueue(val);
					break;
				case 2:
					val = q.deQueue();
					if (val != Integer.MIN_VALUE) {
						System.out.println("Dequeued!: " + val);
					}
					break;
				case 3:
					q.print();
					break;
				default: break;
			}
		}

	}
}