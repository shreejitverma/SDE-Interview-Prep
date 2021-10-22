/* URL: https://www.geeksforgeeks.org/sudoku-backtracking-7/
 * Class Sudoku : Create a Sudoku Solver (9X9)
 * Input        : An unsolved incomplete sudoku matrix
 * Output       : A complete sudoku matrix
 */

package sudoku;
import java.util.Scanner;
import java.io.File;
import java.io.FileNotFoundException;

public class Sudoku {
	int[][] sudokuMatrix;
	public String fileName;

	public Sudoku(String filename) throws FileNotFoundException {
		fileName = filename;
		sudokuMatrix = fileToMatrix(filename);
	}

	public String getFileName() {
		return fileName;
	}
	
	/* Method to convert from file to a 2D matrix.
	 * filename - Name of the file which store the sudo matrix.
	 */
	public int[][] fileToMatrix(String filename) throws FileNotFoundException {
		int[][] sudokuMatrix = new int[9][9];
		File fileReader = new File(filename);
		Scanner sc = new Scanner(fileReader);
		int i = -1, j = -1;

		while (sc.hasNext()) {
			i++;
			String[] line = sc.nextLine().split(",");
			for (j = 0; j < line.length; j++) {
				sudokuMatrix[i][j] = Integer.parseInt(line[j]);
			}
		}
		return sudokuMatrix;
	}

	/* Method to check if the current value is suitable to 
	 * be put in the sudoku matrix. Need to check for the 
	 * specific row, column and that mini-matrix of 3X3.
	 */
	public boolean isSafe(int i, int j, int valueToPut) {
		// Check that specific row
		for (int k = 0; k < 9; k++) {
			if (sudokuMatrix[i][k] == valueToPut) {
				return false;
			}
		}
		// Check that specific column
		for (int k = 0; k < 9; k++) {
			if (sudokuMatrix[k][j] == valueToPut) {
				return false;
			}
		}
		// Check that specific mini 2X3 matrix
		int countRow = 0;
		int countCol = 0;
		for (int k = i; countRow < 3; k++, k %= 3) {
			countRow += 1;
			for (int l = j; countCol < 3; l++, l %= 3) {
				countCol += 1;
				if (sudokuMatrix[k][l] == valueToPut) {
					return false;
				}
			}
		}
		return true;
	}

	/* Method checks if the current place is empty or not.
	 */
	public boolean isEmpty(int i, int j) {
		return sudokuMatrix[i][j] == -1 ? true : false;
	}

	/* Method to solve the sudoku using backtracking.
	 */
	public void solveIt(int i, int j, int valueToPut) {
		if (i < 0 || j < 0 || i >= 9 || j >= 9) {
			return;
		}
		else {
			for (int k = i; k < 9; k++) {
				for (int l = j; l < 9; l++) {
					if (isEmpty(i, j)) {
						for (int v = 1; v <= 9; v++) {
							if (isSafe(i, j, v)) {
								sudokuMatrix[i][j] = v;
							}
						}
					}
				}
			}
		}
	}
}


