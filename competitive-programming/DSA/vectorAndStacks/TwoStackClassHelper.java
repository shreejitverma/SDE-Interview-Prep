//https://www.geeksforgeeks.org/?p=18754
// Implement 2 stacks using an array.

package stacks;
import stacks.TwoStack;
import java.util.Scanner;

/* Helper class to implement the two stack using a single array
 * question.
 */
public class TwoStackClassHelper {
	public static void main(String[] args) {
		Scanner sc = new Scanner(System.in);
		System.out.println("Enter the size of the array which will implement the stacks: ");
		int size = sc.nextInt();
		TwoStack ts = new TwoStack(size);
		int stack;
		while (true) {
			System.out.println("Enter 1/2/3 for Push/Pop/Print");
			int val = sc.nextInt();
			switch(val) {
				case 1:
					System.out.println("Enter the value to push: ");
					int inp = sc.nextInt();
					System.out.println("Enter the stack(1/2) to push: ");
					stack = sc.nextInt();
					switch(stack) {
						case 1:
							ts.pushFirstStack(inp);
							break;

						case 2:
							ts.pushSecondStack(inp);
							break;
						default:
							break;
					}
					break;
				case 2:
					System.out.println("Enter the stack(1/2) to pop from: ");
					stack = sc.nextInt();
					switch(stack) {
						case 1:
							val = ts.popFirstStack();
							if (val != Integer.MAX_VALUE) {
								System.out.println("Popped from first stack! " + val);
							}
							break;

						case 2:
							val = ts.popSecondStack();
							if (val != Integer.MAX_VALUE) {
								System.out.println("Popped from second stack! " + val);
							}
							break;
						default:
							break;
					}
					break;
				case 3:
				ts.printStack();
				default:
					break;
			}
		}
	}
}