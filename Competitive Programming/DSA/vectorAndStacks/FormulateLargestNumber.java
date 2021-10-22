/* Re-arrange the array elements such that
 * we have the largest number on combining
 * all the elements of the array together
 */
// Arrange the numbers to form the largest number
// This will create a new clas file with *$1.class
// name in the end. It's called an Anonymous Inner Class
// since this does not have a name.
package vectors;
import java.util.Collections;
import java.util.Iterator;
import java.util.Vector;
import java.util.Comparator;
import java.util.Scanner;

// Application of vectors here

public class FormulateLargestNumber {
	public static void formulateLargestNumberFromArray(Vector<String> array) {
		// Need to override compare method of Comparator Class(Anonymous class)
		// to modify the way string will be compared.
		Collections.sort(array, new Comparator<String>() {
			@Override
			public int compare(String X, String Y) {
				String XY = X + Y;
				String YX = Y + X;
				return XY.compareTo(YX) > 0 ? -1: 1;
			}
		});
		// Iterator usage
		Iterator it = array.iterator();
		while (it.hasNext()) {
			System.out.print(it.next());
		}
	}

	public static void main(String[] args) {
		System.out.println("Enter the length of the array");
		Scanner sc = new Scanner(System.in);
		int len = sc.nextInt();
		int idx = 0;
		Vector<String> array = new Vector<String>(len);
		
		while(len > 0) {
			len--;
			array.add(sc.next());
		}
		formulateLargestNumberFromArray(array);
		
	}
}