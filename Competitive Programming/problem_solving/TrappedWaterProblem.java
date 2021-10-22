/* URL: https://codepumpkin.com/trapping-rain-water-algorithm-problem/
 */

package problem_solving;
import java.util.Scanner;

public class TrappedWaterProblem {

	/* Store the highest buildings on the left and right of the respective buildings.
	 * Find the minimum of both.
	 * And subtract that value with the current building.
	 */
	private static int findWaterTrapped(int[] buildingHeight) {
		int waterTrapped = 0;

		int[] leftHighestBuild = new int[buildingHeight.length];
		int[] rightHighestBuild = new int[buildingHeight.length];
		leftHighestBuild[0] = buildingHeight[0];
		rightHighestBuild[buildingHeight.length-1] = buildingHeight[buildingHeight.length-1];

		for (int idx = 1; idx < buildingHeight.length; idx++) {
			leftHighestBuild[idx] = Math.max(leftHighestBuild[idx-1], buildingHeight[idx]);
		}
		for (int idx = buildingHeight.length - 2; idx >= 0; idx--) {
			rightHighestBuild[idx] = Math.max(rightHighestBuild[idx+1], buildingHeight[idx]);
		}

		for (int idx = 0; idx < buildingHeight.length; idx++) {
			int min = Math.min(leftHighestBuild[idx], rightHighestBuild[idx]);
			
				waterTrapped += (min - buildingHeight[idx]);
			
		}
		return waterTrapped;
	}
	

	public static void main(String[] args) {
		Scanner sc = new Scanner(System.in);
		System.out.println("Enter the size of array: ");
		int size = sc.nextInt();
		int[] buildingHeight = new int[size];
		int idx = 0;

		while (size > 0) {
			size--;
			buildingHeight[idx++] = sc.nextInt();
		}

		System.out.println("Water trapped between the walls: " + findWaterTrapped(buildingHeight));
	}
}