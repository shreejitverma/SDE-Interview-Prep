// URL: https://www.hackerrank.com/challenges/bfsshortreach/problem
package problem_solving;

import java.io.*;
import java.util.*;


public class BFS {

    static Map<Integer, List<Integer>> createAdjList(int[][] edges, int n, int m) {
        Map<Integer, List<Integer>> adjList = new HashMap<Integer, List<Integer>>();
        
        for (int i = 1; i <= n; i++) {
            adjList.put(i, new LinkedList<Integer>());
        }
        
        for (int i = 0; i < m; i++) {
            List<Integer> ls = adjList.get(edges[i][0]);
            ls.add(edges[i][1]);
            ls = adjList.get(edges[i][1]);
            ls.add(edges[i][0]);
        }
        return adjList;
    }
    
    // Complete the bfs function below.
    static int[] bfs(int n, int m, int[][] edges, int s) {
        Map<Integer, List<Integer>> adjList = createAdjList(edges, n, m);
        boolean visited[] = new boolean[n+1];
        Queue<Integer> q = new LinkedList<Integer>();
        int[] distance = new int[n];
        
        q.add(s);
        visited[s] = true;
        while (!q.isEmpty()) {
            int currNode = q.remove();
            Iterator<Integer> it = adjList.get(currNode).iterator();
            
            while (it.hasNext()) {
                int neighbor = (Integer)it.next();
                if (visited[neighbor] == false) {
                    visited[neighbor] = true;
                    distance[neighbor-1] = distance[currNode-1] + 6;
                    q.add(neighbor);
                }
            }
        }
        int[] dist = new int[n-1];
        int k =0;
        for (int i = 0; i < n; i++) {
            System.out.print(distance[i] + " ");
        }
        System.out.println();
        for (int i = 0; i < n; i++) {
            // if it is starting node then ignore the value
            if (i == (s-1)) {
                continue;
            }
            if (visited[i+1] == false) {
                dist[k++] = -1;
            }
            else {
                dist[k++] = distance[i];
            }
        }
        return dist;

    }

    private static final Scanner scanner = new Scanner(System.in);

    public static void main(String[] args) throws IOException {
        BufferedWriter bufferedWriter = new BufferedWriter(new FileWriter(System.getenv("OUTPUT_PATH")));

        int q = scanner.nextInt();
        scanner.skip("(\r\n|[\n\r\u2028\u2029\u0085])?");

        for (int qItr = 0; qItr < q; qItr++) {
            String[] nm = scanner.nextLine().split(" ");

            int n = Integer.parseInt(nm[0]);

            int m = Integer.parseInt(nm[1]);

            int[][] edges = new int[m][2];

            for (int i = 0; i < m; i++) {
                String[] edgesRowItems = scanner.nextLine().split(" ");
                scanner.skip("(\r\n|[\n\r\u2028\u2029\u0085])?");

                for (int j = 0; j < 2; j++) {
                    int edgesItem = Integer.parseInt(edgesRowItems[j]);
                    edges[i][j] = edgesItem;
                }
            }

            int s = scanner.nextInt();
            scanner.skip("(\r\n|[\n\r\u2028\u2029\u0085])?");

            int[] result = bfs(n, m, edges, s);

            for (int i = 0; i < result.length; i++) {
                bufferedWriter.write(String.valueOf(result[i]));

                if (i != result.length - 1) {
                    bufferedWriter.write(" ");
                }
            }

            bufferedWriter.newLine();
        }

        bufferedWriter.close();

        scanner.close();
    }
}
