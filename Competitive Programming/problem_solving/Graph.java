//This is a java program to represent graph as a adjacency list
package problem_solving;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Scanner;
 import java.util.*;
public class Graph 
{
    private Map<Integer, List<Integer>> adjacencyList;
    public int farthestNode = -1;
    public int farthestNodeLength = -1;
    public Graph(int v) 
    {
        adjacencyList = new HashMap<Integer, List<Integer>>();
        for (int i = 1; i <= v; i++)
            adjacencyList.put(i, new LinkedList<Integer>());
    }
 
    public void setEdge(int to, int from) 
    {
        if (to > adjacencyList.size() || from > adjacencyList.size())
            System.out.println("The vertices does not exists");
        
            List<Integer> sls = adjacencyList.get(to);
            sls.add(from);
            List<Integer> dls = adjacencyList.get(from);
            dls.add(to);
        
    }
 
    public List<Integer> getEdge(int to) 
    {
        if (to > adjacencyList.size()) 
        {
            System.out.println("The vertices does not exists");
            return null;
        }
        return adjacencyList.get(to);
    }
 
    // Method to store the leaf node which is the farthest from the starting node.
    public void startDFS(int startNode, int dist, boolean[] visited) {
        Iterator<Integer> it = adjacencyList.get(startNode).iterator();
        visited[startNode] = true;
        System.out.print(startNode + "->");
        if (dist > farthestNodeLength) {
            farthestNodeLength = dist;
            farthestNode = startNode;
        }
        while (it.hasNext()) {
            int nextNode = (Integer)it.next();
            if (visited[nextNode] == false) {
                startDFS(nextNode, dist + 1, visited);
            }
        }
    }

    public static void main(String args[]) 
    {
        int v, e, count = 1, to, from;
        Scanner sc = new Scanner(System.in);
        Graph glist;
        try 
        {
            System.out.println("Enter the number of vertices: ");
            v = sc.nextInt();
            System.out.println("Enter the number of edges: ");
            e = sc.nextInt();
 
            glist = new Graph(v);
 
            System.out.println("Enter the edges in the graph : <to> <from>");
            while (count <= e) 
            {
                to = sc.nextInt();
                from = sc.nextInt();
 
                glist.setEdge(to, from);
                count++;
            }
 
            System.out.println("The Adjacency List Representation of the graph is: ");
 
            for (int i = 1; i <= v; i++) 
            {
                System.out.print(i + "->");
                List<Integer> edgeList = glist.getEdge(i);
                for (int j = 1;; j++) 
                {
                    if (j != edgeList.size())
                        System.out.print(edgeList.get(j - 1) + " -> ");
                    else 
                    {
                        System.out.print(edgeList.get(j - 1));
                        break;
                    }
                }
                System.out.println();
            }
            //visited array to store the vertices which are visited once need not be visited again.
            boolean[] visited = new boolean[v+1];
            int startNode = 1;
            glist.startDFS(startNode, 0, visited);

            System.out.println("Farthest node length: " + glist.farthestNodeLength);
            System.out.println("Farthest node from " + startNode + " : " + glist.farthestNode);

            
            for (int i = 1; i <= v; i++) {
                visited[i] = false;
            }
            startNode = glist.farthestNode;
            glist.startDFS(startNode, 0, visited);

            System.out.println("Farthest node length: " + glist.farthestNodeLength);
            System.out.println("Farthest node from " + startNode + " : " + glist.farthestNode);

        } 
        catch (Exception E) 
        {
            System.out.println("Something went wrong" + E);
        }
        sc.close();
    }
}
