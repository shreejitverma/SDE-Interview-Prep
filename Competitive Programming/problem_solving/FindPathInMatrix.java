/* URL : https://www.geeksforgeeks.org/find-whether-path-two-cells-matrix/
 */
package problem_solving;
import java.util.Scanner;

public class FindPathInMatrix {
	private static void findPath(int[][] matrix, int[][] pathTraversed, int i, int j) {
		// 0 is a brick wall which cannot be broken.
		if (matrix != null && (i < 0 || j < 0 || i >= matrix[0].length || j >= matrix[0].length || matrix[i][j] == 0))
			return;
		// 2 is the final position to reach.
		if (matrix[i][j] == 2) {
			printMatrix(pathTraversed);
			System.out.println("Found a path to 2 from 1 (The 1s denoting the path to 2)");
			return;
		}
		// Assuming 1 is present only once, as the starting point.
		// Also, keep track of the block visited so that you do not visit it again and end in
		// infinite loop.
		if (pathTraversed[i][j] != 1 && (matrix[i][j] == 1 || matrix[i][j] == 3)) {
			pathTraversed[i][j] = 1;
			findPath(matrix, pathTraversed, i, j+1);
			findPath(matrix, pathTraversed, i-1, j);
			findPath(matrix, pathTraversed, i, j-1);
			findPath(matrix, pathTraversed, i+1, j);
			pathTraversed[i][j] = 0;
		}
	}

	private static void printMatrix(int[][] matrix) {
		System.out.println();
		for (int i = 0; i < matrix[0].length; i++) {
			for (int j = 0; j < matrix[0].length; j++) {
				System.out.print(matrix[i][j] + " ");
			}
			System.out.println();
		}
		System.out.println();
	}

	public static void main(String[] args) {
		Scanner sc = new Scanner(System.in);
		System.out.println("Enter the size of the square matrix: ");
		int length = sc.nextInt();
		
		int[][] matrix = new int[length][length];
		int[][] pathTraversed = new int[length][length];

		for (int i = 0; i < length; i++) {
			for (int j = 0; j < length; j++) {
				matrix[i][j] = sc.nextInt();
				pathTraversed[i][j] = 0;
			}
		}

		printMatrix(matrix);
		findPath(matrix, pathTraversed, 0, 0);

	}
}