import sudoku.Sudoku;
import java.util.Scanner;
import java.io.FileNotFoundException;

class SudokuTester {
	
	// Print the sudo
	public static void print(int[][] matrix) {
		System.out.println("---------------------");
		System.out.println("---------------------");

		for (int i = 0; i < matrix.length; i++) {
			for (int j = 0; j < matrix.length; j++) {
				System.out.print(matrix[i][j] + " ");
			}
			System.out.println();
		}

		System.out.println("---------------------");
		System.out.println("---------------------");
	}

	public static void main(String[] args) throws FileNotFoundException {
		Sudoku sudo = new Sudoku("sudoku_file");
		// check fileToMatrix method
		int[][] sudoMatrix = sudo.fileToMatrix(sudo.fileName);
		print(sudoMatrix);

		// check isSafe() method
		System.out.println("isSafe() method testcase:      " + ((sudo.isSafe(0, 0, 1) == true) ? "Passed" : "Failed"));
		System.out.println("isSafe() method testcase:      " + ((sudo.isSafe(0, 0, 2) == false) ? "Passed" : "Failed"));
		System.out.println("isSafe() method testcase:      " + ((sudo.isSafe(4, 4, 8) == false) ? "Passed" : "Failed"));

		// check isEmpty() method
		System.out.println("isEmpty() mthod testcase:      " + ((sudo.isEmpty(0, 0) == false) ? "Passed" : "Failed"));
		System.out.println("isSafe() method testcase:      " + ((sudo.isEmpty(0, 1) == true) ? "Passed" : "Failed"));
	
		// Check the file name
		System.out.println("getFileName() method testcase: " + ((sudo.getFileName() == "sudoku_file") ? "Passed" : "Failed"));
	}
}