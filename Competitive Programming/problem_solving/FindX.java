// URL: https://www.hackerearth.com/challenge/hiring/cars24-java-hiring-challenge/problems/194e5a57fe0e44fc85ab28c24e455a6d/
package problem_solving;

import java.util.*;

class FindX {
    public static void main(String args[] ) throws Exception {

        Scanner s = new Scanner(System.in);
        int T = s.nextInt();                 // Reading input from STDIN
        while (T > 0) {
            T -= 1;
            int n = s.nextInt();
            int size = n;
            int idx = 0;
            long sum = 0;
            while (n > 0) {
                n -= 1;
                sum += s.nextInt();
            }
            if (sum % size == 0) {
                System.out.println(sum / size);
            }
            else {
                System.out.println(-1);
            }
            
        }
    }
}
