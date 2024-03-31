/* Given a matrix find the sum of sub-matrices denoted by 
 * index positions given as input. You need to do it quickly
 * without calculating it at run time.
 */

package arrays;
import utilities.Utilities;

public class SubMatrixSumQuery {

	// no need to return the matrix array since it manipulates the matrix in-place 
	// and by reference.
	public static void findCumulativeSumOfMatrix(int[][] buffer, int[][] matrix, int rowLen, int colLen) {
		// initialize first row and first column in buffer
		for (int idx = 0; idx < rowLen; idx++) {
			buffer[0][idx] = matrix[0][idx];
		}
		for (int idx = 0; idx < colLen; idx++) {
			buffer[idx][0] = matrix[idx][0];
		}
		// Sum row-wise
		for (int idx = 0; idx < rowLen; idx++) {
			for (int _idx = 1; _idx < colLen; _idx++) {
				buffer[idx][_idx] = buffer[idx][_idx-1] + matrix[idx][_idx];
			}
		}
		// Sum column-wise
		for (int idx = 0; idx < rowLen; idx++) {
			for (int _idx = 1; _idx < colLen; _idx++) {
				buffer[_idx][idx] = buffer[_idx][idx] + buffer[_idx-1][idx];
			}
		}
	}

	public static int findSumOfSubMatrix(int[][] cumulativeMatrix, int rowL, int colL, int rowR, int colR) {
		if (rowL == colL && rowL == 0)
			return cumulativeMatrix[rowR][colR];
		return (cumulativeMatrix[rowR][colR] - 
			(cumulativeMatrix[rowR][colL-1] + cumulativeMatrix[rowL-1][colR]) + 
			cumulativeMatrix[rowL-1][colL-1]);
	}

	public static void main(String[] args) {
		// Assuming to be a square matrix
		Utilities.print("Enter the matrix size", ":");
		int len = Utilities.takeInputIntegerValue();
		int[][] matrix = new int[len][len];
		int[][] cumulativeMatrix = new int[len][len];

		Utilities.print("Enter the matrix values", "");
		for (int idx = 0; idx < len; idx++) {
			for (int _idx = 0; _idx < len; _idx++) {
				matrix[idx][_idx] = Utilities.takeInputIntegerValue();
			}
		}
		Utilities.printMatrix(matrix, len, len);
		Utilities.print("Enter the position of sub-matrices whose sum is needed", "");
		Utilities.print("Example", ":");
		Utilities.print("LeftRow", ":", 1);
		Utilities.print("LeftCol", ":", 1);
		Utilities.print("RightRow", ":", 2);
		Utilities.print("RightCol", ":", 2);
		findCumulativeSumOfMatrix(cumulativeMatrix, matrix, len, len);
		Utilities.printMatrix(cumulativeMatrix, len, len);
		Utilities.print("Sum of the sub-matrix from (1, 1) to (2, 2) is", ":", findSumOfSubMatrix(cumulativeMatrix, 1, 1, 2, 2));
		// INput required:
		// left row position
		// left column position
		// righ row position
		// right column position
		Utilities.print("Enter the position of sub-matrices whose sum is needed", "");
		int rowL = Utilities.takeInputIntegerValue();
		int colL = Utilities.takeInputIntegerValue();
		int rowR = Utilities.takeInputIntegerValue();
		int colR = Utilities.takeInputIntegerValue();
		Utilities.print("Sum of the sub-matrix is", ":", findSumOfSubMatrix(cumulativeMatrix, rowL, colL, rowR, colR));


	}
}