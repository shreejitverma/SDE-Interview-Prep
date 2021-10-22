package stacks;
import stacks.Stack;
import java.util.Scanner;

public class StackHelper {
	public static void main(String[] args) {
		Scanner sc = new Scanner(System.in);
		System.out.println("Enter the capacity of the stack: ");
		int cap = sc.nextInt();
		Stack st = new Stack(cap);
		while (true) {
			System.out.println("Enter 1/2/3/4 to push/pop/print/exit the stack");
			int val = sc.nextInt();
			switch(val) {
				case 1:
					System.out.print("Enter the value to push: ");
					val = sc.nextInt();
					st.push(val);
					break;
				case 2:
					val = st.pop();
					if (val != Integer.MAX_VALUE) {
						System.out.println("Popped! " + val);
					}
					break;
				case 3:
					st.print();
					break;
				case 4:
					System.exit(0);
				default:
					break;
			}
		}
	}
}