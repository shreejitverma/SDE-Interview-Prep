// Implement a simple stack using an array
// SIngle pointer enough to manipulate the stack.

package stacks;

public class Stack {
	int top = -1;
	int capacity;
	int size = 0;
	int[] input;

	Stack(int cap) {
		capacity = cap;
		input = new int[capacity];
	}

	public void print() {
		System.out.println();
		if (size == 0) {
			System.out.println("No values in the stack to print!");
		}
		for (int idx = 0; idx < size; idx++) {
			System.out.print(input[idx] + " ");
		}
		System.out.println();
	}
	
	public int pop() {
		int val = Integer.MAX_VALUE;
		if (top != -1) {
			val = input[top];
			input[top] = 0;
			top--;
			size -= 1;
		}
		else {
			System.out.println("Stack underflow condition");
		}
		return val;
	}

	public void push(int value) {
		if (top != capacity-1) {
			top++;
			input[top] = value;
			size += 1;
		}
		else {
			System.out.println("Stack overflow condition");
		}
	}

	public int peek() {
		int val = Integer.MAX_VALUE;
		if (top != -1 && top != capacity) {
			val = input[top];
		}
		else {
			System.out.println("Stack overflow/underflow condition");
		}
		return val;
	}

	public int size() {
		return size;
	}
}