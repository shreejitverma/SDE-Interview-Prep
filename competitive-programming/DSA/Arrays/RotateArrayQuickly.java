/* Given an array, left rotate the array by the given number of
 * rotations quickly.
 */
package arrays;
import utilities.Utilities;

public class RotateArrayQuickly {
	public static void rotateArray(int[] array, int rotationValue) {
		int idx = rotationValue;
		int count = 0;
		while (count < array.length) {
			Utilities.print(array[idx]);
			idx = ( idx + 1 ) % array.length;
			count += 1;
		}
	}

	public static void main(String[] args) {
		int len = Utilities.takeInputIntegerValue();
		int[] array = new int[len];
		Utilities.takeInputArray(array, len);
		Utilities.print("Enter the number of rotations", ":");
		int rotations = Utilities.takeInputIntegerValue();
		while (rotations > 0) {
			rotations--;
			Utilities.print("\nArray left rotation by", ":");
			int rotationValue = Utilities.takeInputIntegerValue();
			Utilities.print("Rotated Array", ":");
			rotateArray(array, rotationValue);
		}
	}
}