package queue;
// URL: https://www.geeksforgeeks.org/implement-stack-using-queue/
// Implementing a queue using an integer array of size given as input.
// Code written under the assumptions:
// 1. Integer array
// 2. Front is one step behind the value at the front.
// 3. Rear is at the value in the array.

public class Queue {
	static int front = -1;
	static int rear = -1;
	static int size = 0;
	static int capacity;
	static int[] queue;


	Queue(int cap) {
		capacity = cap;
		queue = new int[capacity];
	}

	public static boolean isFull() {
		return size == capacity ? true:false;
	}
	/* Rear pointer stays at the last value entered in the queue.
	 * So, first step is to check if there is an overflow condition.
	 * 
	 */
	public static void enQueue(int value) {
		if (isFull()) {
			System.out.println("Queue overflow condition!");
		}
		else {
			rear = (rear + 1) % capacity;
			queue[rear] = value;
			size += 1;
		}
	}

	public static boolean isEmpty() {
		return size == 0 ? true:false;
	}

	/* Front pointer is one step back than the actual value at the 
	 * beginning of the queue. So, first need to check the 
	 * underflow condition,
	 * Second is to dequeue the value from the queue.
	 * If the queue is empty, then reset the values back to starting
	 * position.
	 */
	public static int deQueue() {
		int val = Integer.MAX_VALUE;
		if (isEmpty()) {
			front = -1;
			rear = -1;
			System.out.println("Queue underflow condition");
		}
		else {
			front = (front + 1) % capacity;
			val = queue[front];
			queue[front] = 0; 
			size -= 1;
		}
		return val;
	}
}