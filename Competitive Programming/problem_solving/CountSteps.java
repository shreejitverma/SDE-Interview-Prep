// URL: https://www.hackerrank.com/challenges/ctci-recursive-staircase/problem?h_l=interview&playlist_slugs%5B%5D=interview-preparation-kit&playlist_slugs%5B%5D=recursion-backtracking&h_r=next-challenge&h_v=zen

public class CountSteps {
	static int stepPerms(int n) {
        if (n == 0)
            return 1;
        if (n == 1 || n == 2)
            return n;
        return stepPerms(n-1) + stepPerms(n-2) + stepPerms(n-3);
    }
}