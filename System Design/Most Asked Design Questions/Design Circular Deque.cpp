/*
641. Design Circular Deque
Medium
54352Add to ListShare
Design your implementation of the circular double-ended queue (deque).
Implement the MyCircularDeque class:
	• MyCircularDeque(int k) Initializes the deque with a maximum size of k.
	• boolean insertFront() Adds an item at the front of Deque. Returns true if the operation is successful, 
    or false otherwise.
	• boolean insertLast() Adds an item at the rear of Deque. Returns true if the operation is successful, 
    or false otherwise.
	• boolean deleteFront() Deletes an item from the front of Deque. Returns true if the operation is successful, 
    or false otherwise.
	• boolean deleteLast() Deletes an item from the rear of Deque. Returns true if the operation is successful, 
    or false otherwise.
	• int getFront() Returns the front item from the Deque. Returns -1 if the deque is empty.
	• int getRear() Returns the last item from Deque. Returns -1 if the deque is empty.
	• boolean isEmpty() Returns true if the deque is empty, or false otherwise.
	• boolean isFull() Returns true if the deque is full, or false otherwise.
 
Example 1:
Input
["MyCircularDeque", "insertLast", "insertLast", "insertFront", "insertFront", "getRear", "isFull", "deleteLast", "insertFront", "getFront"]
[[3], [1], [2], [3], [4], [], [], [], [4], []]
Output
[null, true, true, true, false, 2, true, true, true, 4]
Explanation
MyCircularDeque myCircularDeque = new MyCircularDeque(3);
myCircularDeque.insertLast(1);  // return True
myCircularDeque.insertLast(2);  // return True
myCircularDeque.insertFront(3); // return True
myCircularDeque.insertFront(4); // return False, the queue is full.
myCircularDeque.getRear();      // return 2
myCircularDeque.isFull();       // return True
myCircularDeque.deleteLast();   // return True
myCircularDeque.insertFront(4); // return True
myCircularDeque.getFront();     // return 4
 
Constraints:
	• 1 <= k <= 1000
	• 0 <= value <= 1000
	• At most 2000 calls will be made to insertFront, insertLast, deleteFront, deleteLast, getFront, getRear, isEmpty, isFull.
*/
class MyCircularDeque
{
public:
    vector<int> q;
    int head, curSize;
    bool isStart;

    /** Initialize your data structure here. Set the size of the deque to be k. */
    MyCircularDeque(int k)
    {
        q = vector<int>(k, -1);
        head = 0;
        curSize = 0;
        isStart = true;
    }

    /** Adds an item at the front of Deque. Return true if the operation is successful. */
    bool insertFront(int value)
    {
        if (isFull())
            return false;
        if (isEmpty())
        {
            head = 0;
        }
        else
        {
            head = (head - 1 + q.size()) % q.size();
        }
        q[head] = value;
        curSize++;
        // for(int i = 0; i < q.size(); i++){
        //     cout << q[i] << " ";
        // }
        // cout << endl;
        return true;
    }

    /** Adds an item at the rear of Deque. Return true if the operation is successful. */
    bool insertLast(int value)
    {
        if (isFull())
            return false;
        q[(head + curSize) % q.size()] = value;
        curSize++;
        // for(int i = 0; i < q.size(); i++){
        //     cout << q[i] << " ";
        // }
        // cout << endl;
        return true;
    }

    /** Deletes an item from the front of Deque. Return true if the operation is successful. */
    bool deleteFront()
    {
        if (isEmpty())
            return false;
        q[head] = -1;
        head = (head + 1) % q.size();
        curSize--;
        // for(int i = 0; i < q.size(); i++){
        //     cout << q[i] << " ";
        // }
        // cout << endl;
        return true;
    }

    /** Deletes an item from the rear of Deque. Return true if the operation is successful. */
    bool deleteLast()
    {
        if (isEmpty())
            return false;
        q[(head + curSize - 1) % q.size()] = -1;
        curSize--;
        // for(int i = 0; i < q.size(); i++){
        //     cout << q[i] << " ";
        // }
        // cout << endl;
        return true;
    }

    /** Get the front item from the deque. */
    int getFront()
    {
        return q[head];
    }

    /** Get the last item from the deque. */
    int getRear()
    {
        return q[(head + curSize - 1) % q.size()];
    }

    /** Checks whether the circular deque is empty or not. */
    bool isEmpty()
    {
        return curSize == 0;
    }

    /** Checks whether the circular deque is full or not. */
    bool isFull()
    {
        return curSize == q.size();
    }
};

/**
 * Your MyCircularDeque object will be instantiated and called as such:
 * MyCircularDeque* obj = new MyCircularDeque(k);
 * bool param_1 = obj->insertFront(value);
 * bool param_2 = obj->insertLast(value);
 * bool param_3 = obj->deleteFront();
 * bool param_4 = obj->deleteLast();
 * int param_5 = obj->getFront();
 * int param_6 = obj->getRear();
 * bool param_7 = obj->isEmpty();
 * bool param_8 = obj->isFull();
 */
