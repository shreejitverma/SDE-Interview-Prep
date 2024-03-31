//https://www.geeksforgeeks.org/?p=18754
// Implement 2 stacks using an array.

package stacks;
/* Start the two stacks from the two extreme ends of the 
 * array. And keep incrementing by one as and when the 
 * values are added. Check for underflow/overflow condition.
 * Return a value/print the conditions.
 */
class TwoStack {
	static int topFirst = -1;
	static int topSecond = -1;
	static int[] input;

	TwoStack(int size) {
		input = new int[size];
		topSecond = size;
	}

	public static void printStack() {
		System.out.println();
		for (int idx = 0; idx < input.length; idx++) {
			System.out.print(input[idx] + " ");
		}
		System.out.println();
	}
	public static void pushFirstStack(int value) {
		if ((topFirst+1) != topSecond) {
			topFirst++;
			input[topFirst] = value;
		}
		else {
			System.out.println("First Stack overflow condition");
		}
	}

	public static void pushSecondStack(int value) {
		if ((topSecond-1) != topFirst) {
			topSecond--;
			input[topSecond] = value;
		}
		else {
			System.out.println("Second Stack overflow condition");
		}
	}

	public static int popFirstStack() {
		int value = Integer.MAX_VALUE;
		if (topFirst != -1) {
			value = input[topFirst];
			input[topFirst] = 0;
			topFirst--;
		}
		else {
			System.out.println("First stack underflow condition");
		}
		return value;
	}

	public static int popSecondStack() {
		int value = Integer.MAX_VALUE;
		if (topSecond != input.length) {
			value = input[topSecond];
			input[topSecond] = 0;
			topSecond++;
		}
		else {
			System.out.println("Second Stack underflow condition");
		}
		return value;
	}
}
