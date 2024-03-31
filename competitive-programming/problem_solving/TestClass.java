package problem_solving;
import java.util.*;


// Warning: Printing unwanted or ill-formatted data to output will cause the test cases to fail

class TestClass {
    static int count = 0;
    private static void countNumbers(int[] input, int idx, int currLength, int maxLength, String answer) {
        if (answer.length() == maxLength && answer.charAt(0) != '0') {
            count++;
            return;
        }
        else if (currLength > maxLength) {
            return;
        }
        for (int i = idx; i < input.length; i++) {
            countNumbers(input, i+1, currLength+1, maxLength, answer+input[i]);
        }
    }
    
    public static void main(String args[] ) throws Exception {
        //Scanner
        Scanner s = new Scanner(System.in);
        int numbers = s.nextInt();                 // Reading input from STDIN
        int[] input = new int[numbers];
        int idx = 0;
        while (numbers > 0) {
            input[idx++] = s.nextInt();
            numbers--;
        }
        int K = s.nextInt();
        countNumbers(input, 0, 0, K, "");
        System.out.println(count);
    }
}
