'''https://www.geeksforgeeks.org/queue-set-1introduction-and-array-implementation/

Queue is a linear structure which follows a particular order in which the operations are performed. The order is First In First Out (FIFO).  A good example of queue is any queue of consumers for a resource where the consumer that came first is served first. 
The difference between stacks and queues is in removing. In a stack we remove the item the most recently added; in a queue, we remove the item the least recently added.
Operations on Queue: 
Mainly the following four basic operations are performed on queue:
Enqueue: Adds an item to the queue. If the queue is full, then it is said to be an Overflow condition. 
Dequeue: Removes an item from the queue. The items are popped in the same order in which they are pushed. If the queue is empty, then it is said to be an Underflow condition. 
Front: Get the front item from queue. 
Rear: Get the last item from queue. 

Applications of Queue: 
Queue is used when things don’t have to be processed immediately, but have to be processed in First InFirst Out order like Breadth First Search. This property of Queue makes it also useful in following kind of scenarios.
1) When a resource is shared among multiple consumers. Examples include CPU scheduling, Disk Scheduling. 
2) When data is transferred asynchronously (data not necessarily received at same rate as sent) between two processes. Examples include IO Buffers, pipes, file IO, etc.
See this for more detailed applications of Queue and Stack.
Array implementation Of Queue 
For implementing queue, we need to keep track of two indices, front and rear. We enqueue an item at the rear and dequeue an item from the front. If we simply increment front and rear indices, then there may be problems, the front may reach the end of the array. The solution to this problem is to increase front and rear in circular manner.

Output: 

10 enqueued to queue
20 enqueued to queue
30 enqueued to queue
40 enqueued to queue
10 dequeued from queue
Front item is 20
Rear item is 40
Complexity Analysis:  

Time Complexity:
Operations              Complexity
Enque(insertion)           O(1)
Deque(deletion)            O(1)
Front(Get front)           O(1)
Rear(Get Rear)             O(1)              
Auxiliary Space: O(N). 
N is the size of array for storing elements.
Pros of Array Implementation:  

Easy to implement.
Cons of Array Implementation:  

Static Data Structure, fixed size.
If the queue has a large number of enqueue and dequeue operations, at some point (in case of linear increment of front and rear indexes) we may not be able to insert elements in the queue even if the queue is empty (this problem is avoided by using circular queue).
'''

# Python3 program for array implementation of queue

# Class Queue to represent a queue


class Queue:

    # __init__ function
    def __init__(self, capacity):
        self.front = self.size = 0
        self.rear = capacity - 1
        self.Q = [None]*capacity
        self.capacity = capacity

    # Queue is full when size becomes
    # equal to the capacity
    def isFull(self):
        return self.size == self.capacity

    # Queue is empty when size is 0
    def isEmpty(self):
        return self.size == 0

    # Function to add an item to the queue.
    # It changes rear and size
    def EnQueue(self, item):
        if self.isFull():
            print("Full")
            return
        self.rear = (self.rear + 1) % (self.capacity)
        self.Q[self.rear] = item
        self.size = self.size + 1
        print("% s enqueued to queue" % str(item))

    # Function to remove an item from queue.
    # It changes front and size
    def DeQueue(self):
        if self.isEmpty():
            print("Empty")
            return

        print("% s dequeued from queue" % str(self.Q[self.front]))
        self.front = (self.front + 1) % (self.capacity)
        self.size = self.size - 1

    # Function to get front of queue
    def que_front(self):
        if self.isEmpty():
            print("Queue is empty")

        print("Front item is", self.Q[self.front])

    # Function to get rear of queue
    def que_rear(self):
        if self.isEmpty():
            print("Queue is empty")
        print("Rear item is",  self.Q[self.rear])


# Driver Code
if __name__ == '__main__':

    queue = Queue(30)
    queue.EnQueue(10)
    queue.EnQueue(20)
    queue.EnQueue(30)
    queue.EnQueue(40)
    queue.DeQueue()
    queue.que_front()
    queue.que_rear()
