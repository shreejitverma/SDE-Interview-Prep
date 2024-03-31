/* URL: https://www.geeksforgeeks.org/boggle-set-2-using-trie/
 *
 */

package trie;
import trie.Trie;
import java.util.Scanner;
import java.util.Vector;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.Iterator;

public class Boggle {

	private static boolean isSafe(int i, int j, int[][] flagArr, Character[][] boggle) {
		// If the call has reached the end of the array
		if (i >= 0  && j >= 0 && i < boggle[0].length && j < boggle[0].length && flagArr[i][j] == 0) {
			return true;
		}
		return false;
	}

	public static void traverseBoggle(Vector<String> boggleWords, int[][]flagArr, Character[][] boggle, int row, int col, String tempWord, TrieNode root) {
	
		if (root.isEndOfWord == true) {
			//System.out.println("Here");
			System.out.println(tempWord);
			boggleWords.add(tempWord);
		}
		// Recurse over the next character and check
		// int alphabet = boggle[row][col] - 'a';
		if (flagArr[row][col] != 1) {

			flagArr[row][col] = 1;
			for (int k = 0; k < 26; k++) {
				if (root.alphabets[k] != null) {
					int alphabet = (char)(k + 'a');

					if(isSafe(row, col+1, flagArr, boggle) && boggle[row][col+1] == alphabet) {
						traverseBoggle(boggleWords, flagArr, boggle, row, col+1, tempWord + (char)alphabet, root.alphabets[k]);
					}
					if(isSafe(row+1, col, flagArr, boggle) && boggle[row+1][col] == alphabet) {
						traverseBoggle(boggleWords, flagArr, boggle, row+1, col, tempWord + (char)alphabet, root.alphabets[k]);
					}
					if(isSafe(row, col-1, flagArr, boggle) && boggle[row][col-1] == alphabet) {
						traverseBoggle(boggleWords, flagArr, boggle, row, col-1, tempWord + (char)alphabet, root.alphabets[k]);
					}
					if(isSafe(row-1, col, flagArr, boggle) && boggle[row-1][col] == alphabet) {
						traverseBoggle(boggleWords, flagArr, boggle, row-1, col, tempWord + (char)alphabet, root.alphabets[k]);
					}
				}
			}
			flagArr[row][col] = 0;
		}
	}

	private static int[][] initialize(int size) {
		int[][] flagArr = new int[size][size];
		for (int idx = 0; idx < size; idx++) {
			for (int idx_y = 0; idx_y < size; idx_y++) {
				flagArr[idx][idx_y] = 0;
			}
		}
		return flagArr;
	}

	public static void playBoggle(Character[][] boggle, Trie dictionary) {
		// Store the boggle words here
		Vector<String> boggleWords = new Vector<String>();
		int[][] flagArr = initialize(boggle[0].length);
		String tempWord = "";

		// Better solution is to iterate over the array and check if the 
		// characters are present in the dictionary or not.
		for (int idx = 0; idx < boggle.length; idx++) {
			for (int idx_y = 0; idx_y < boggle.length; idx_y++) {
				int alphabet = boggle[idx][idx_y] - 'a';
				if (dictionary.root.alphabets[alphabet] != null) {
					tempWord = tempWord + boggle[idx][idx_y];
					traverseBoggle(boggleWords, flagArr, boggle, idx, idx_y, tempWord, dictionary.root);
					tempWord = "";
				}
			}
		}
		//traverseBoggle(boggleWords, flagArr, boggle, 0, 0, "", dictionary);
		printBoggleWords(boggleWords);
	}

	public static void printBoggleWords(Vector<String> boggleWords) {
		Iterator<String> it = boggleWords.iterator();
		System.out.println();
		while (it.hasNext()) {
			System.out.print(it.next() + ", ");
		}
		System.out.println();
	}

	public static void main(String[] args) throws FileNotFoundException {
		Scanner sc = new Scanner(System.in);
		System.out.println("Enter the size of boggle crossword(must be a square): ");
		int size = sc.nextInt();
		int row = size;
		System.out.println("Enter the boggle crossword: ");
		Character boggle[][] = new Character[size][size];
		int idx = 0;
		while (size > 0) {
			size--;
			String input = sc.next();
			for (int idx_col = 0; idx_col < row; idx_col++) {
				boggle[idx][idx_col] = input.charAt(idx_col);
			}
			idx++;
		}

		// Read the words to put in the dictionary so that the code
		// can play boggle using those words
		File fPtr = new File("most_common_english_words.txt");
		sc = new Scanner(fPtr);
		Trie dictionary = new Trie();

		// Iterate over the file and insert in the dictionary line-by-line.
		while (sc.hasNextLine()) {
			String word = sc.nextLine();
			dictionary.insert(word);
		}

		playBoggle(boggle, dictionary);
	}	
}