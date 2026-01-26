/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Java Collections Framework
 * Description: Key data structures for Java interviews.
 */

import java.util.*;

public class CollectionsDemo {
    public static void main(String[] args) {
        // 1. ArrayList (Dynamic Array)
        // Access: O(1), Insert/Delete(End): O(1)
        List<String> list = new ArrayList<>();
        list.add("Java");
        list.add("C++");
        
        // 2. HashMap (Hash Table)
        // Avg O(1) Access
        Map<String, Integer> map = new HashMap<>();
        map.put("Alice", 90);
        map.put("Bob", 85);
        
        for (Map.Entry<String, Integer> entry : map.entrySet()) {
            System.out.println(entry.getKey() + ": " + entry.getValue());
        }
        
        // 3. PriorityQueue (Min Heap by default)
        // Insert/Poll: O(log N)
        PriorityQueue<Integer> pq = new PriorityQueue<>();
        pq.add(10);
        pq.add(5);
        System.out.println("Min Element: " + pq.peek()); // 5
        
        // 4. HashSet (Unique elements)
        Set<Integer> set = new HashSet<>();
        set.add(1);
        set.add(1); // Ignored
        System.out.println("Set Size: " + set.size()); // 1
        
        // 5. Deque (Stack/Queue)
        Deque<Integer> deque = new ArrayDeque<>();
        deque.push(1); // Stack Push
        deque.offer(2); // Queue Offer
    }
}
