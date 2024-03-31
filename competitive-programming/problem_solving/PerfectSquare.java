/* URL: https://practice.geeksforgeeks.org/problems/check-if-a-number-is-perfect-square/0/?track=SP-Misc
 * Find if a given number is perfect square or not.
 * Helpful link: https://www.geeksforgeeks.org/rounding-off-errors-java/
 */

package problem_solving;

import java.util.*;
import java.lang.*;
import java.io.*;

class PerfectSquare {
	public static void main (String[] args) {
		Scanner sc = new Scanner(System.in);
		int N = sc.nextInt();
		while (N > 0) {
		    N -= 1;
		    int val = sc.nextInt();
		    double srt = Math.sqrt(val);
		    if (srt == Math.floor(Math.sqrt(val))) {
		        System.out.println(1);
		    }
		    else {
		        System.out.println(0);
		    }
		}
	}
}