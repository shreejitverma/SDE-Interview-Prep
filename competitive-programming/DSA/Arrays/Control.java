/* Understanding whether a language is typically 
 * a call-by-reference or a call-by-value type.
 */

package concepts;

class FirstClass {
	int x;
	String name;
	FirstClass(int val, String n) {
		x = val;
		name = n;
	}
}

public class Control {

	// Swap the objects
	// The formal parameters point to the same object
	// but since they are duplciates they lose their 
	// existence once the scope of the method is gone.
	// Here, it looks like they are passed by call-by-value.
	public static void swap(FirstClass fcOne, FirstClass fcTwo) {
		FirstClass temp = fcOne;
		fcOne = fcTwo;
		fcTwo = temp;
	}

	// Swap the variables inside the objects
	// Here the formal parameters are pointing to the same objects
	// and they have manipulated the original values that is why 
	// the original objects attributes have changed. 
	// Here, the swap functions as call-by-reference.
	private static void swapAttrs(FirstClass fcOne, FirstClass fcTwo) {
		FirstClass temp = new FirstClass(0, "");
		temp.x = fcOne.x;
		temp.name = fcOne.name;
		fcOne.x = fcTwo.x;
		fcOne.name = fcTwo.name;
		fcTwo.x = temp.x;
		fcTwo.name = temp.name;
	}

	private static void print(FirstClass fc) {
		System.out.println("Name : " + fc.name + " x : " + fc.x);
	}

	public static void main(String[] args) {
		FirstClass fcOne = new FirstClass(10, "Sid");
		FirstClass fcTwo = new FirstClass(20, "Srk");
		print(fcOne);
		print(fcTwo);
		// Before swap
		swap(fcOne, fcTwo);
		// After swap
		print(fcOne);
		print(fcTwo);

		swapAttrs(fcOne, fcTwo);
		// After swapAttrs
		print(fcOne);
		print(fcTwo);

	}
}
