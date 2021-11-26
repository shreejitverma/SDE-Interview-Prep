'''
https://www.geeksforgeeks.org/reverse-a-stack-using-recursion/
Write a program to reverse a stack using recursion. You are not allowed to use loop constructs like while, for..etc, and you can only use the following ADT functions on Stack S: 
isEmpty(S) 
push(S) 
pop(S)
The idea of the solution is to hold all values in Function Call Stack until the stack becomes empty. When the stack becomes empty, insert all held items one by one at the bottom of the stack. 
For example, let the input stack be  
    1  <-- top
    2
    3
    4    
First 4 is inserted at the bottom.
    4 <-- top

Then 3 is inserted at the bottom
    4 <-- top    
    3

Then 2 is inserted at the bottom
    4 <-- top    
    3 
    2
     
Then 1 is inserted at the bottom
    4 <-- top    
    3 
    2
    1
So we need a function that inserts at the bottom of a stack using the above given basic stack function.



void insertAtBottom((): First pops all stack items and stores the popped item in function call stack using recursion. And when stack becomes empty, pushes new item and all items stored in call stack.

void reverse(): This function mainly uses insertAtBottom() to pop all items one by one and insert the popped items at the bottom.  '''
# Python program to reverse a
# stack using recursion

# Below is a recursive function
# that inserts an element
# at the bottom of a stack.


def insertAtBottom(stack, item):
    if isEmpty(stack):
        push(stack, item)
    else:
        temp = pop(stack)
        insertAtBottom(stack, item)
        push(stack, temp)

# Below is the function that
# reverses the given stack
# using insertAtBottom()


def reverse(stack):
    if not isEmpty(stack):
        temp = pop(stack)
        reverse(stack)
        insertAtBottom(stack, temp)

# Below is a complete running
# program for testing above
# functions.

# Function to create a stack.
# It initializes size of stack
# as 0


def createStack():
    stack = []
    return stack

# Function to check if
# the stack is empty


def isEmpty(stack):
    return len(stack) == 0

# Function to push an
# item to stack


def push(stack, item):
    stack.append(item)

# Function to pop an
# item from stack


def pop(stack):

    # If stack is empty
    # then error
    if(isEmpty(stack)):
        print("Stack Underflow ")
        exit(1)

    return stack.pop()

# Function to print the stack


def prints(stack):
    for i in range(len(stack)-1, -1, -1):
        print(stack[i], end=' ')
    print()

# Driver Code


stack = createStack()
push(stack, str(4))
push(stack, str(3))
push(stack, str(2))
push(stack, str(1))
print("Original Stack ")
prints(stack)

reverse(stack)

print("Reversed Stack ")
prints(stack)
