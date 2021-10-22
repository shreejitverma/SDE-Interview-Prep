/*URL https://www.hackerrank.com/challenges/ctci-linked-list-cycle/problem?h_l=interview&playlist_slugs%5B%5D=interview-preparation-kit&playlist_slugs%5B%5D=linked-lists
Detect a cycle in a linked list. Note that the head pointer may be 'null' if the list is empty.
A Node is defined as: 
*/
package problem_solving;

class Node {
    int data;
    Node next;
    Node (int data) {
        this.data = data;
        next = null;
    }

    boolean hasCycle(Node head) {
        if (head == null || head.next == null)
            return false;
        if (head.next == head)
            return true;
        Node slow = head;
        Node fast = head.next;
        while (fast != null && slow != fast && fast.next != null) {
            slow = slow.next;
            fast = fast.next.next;
        }
        if (slow == fast && fast != null)
            return true;
        return false;
        
    }
}

class DetectCycle {
    public static void main(String[] args) {
        Node head = new Node(1);
        head.next = new Node(2);
        head.next.next = new Node(3);
        head.next.next.next = new Node(4);
        head.next.next.next.next = new Node(5);
    }
}