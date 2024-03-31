# Python3 program for the above approach

# Recursive function to use implicit stack
# to insert an element at the bottom of stack
def recur(S, N):

    # If stack is empty
    if (len(S) == 0):
        S.append(N)

    else:

        # Stores the top element
        X = S[-1]

        # Pop the top element
        S.pop()

        # Recurse with remaining elements
        S = recur(S, N)

        # Push the previous
        # top element again
        S.append(X)

    return S

# Function to insert an element
# at the bottom of stack


def insertToBottom(S, N):

    # Recursively insert
    # N at the bottom of S
    S = recur(S, N)

    # Print the stack S
    while (len(S) > 0):
        print(S.pop(), end=" ")

# Driver code


# Input
S = []
S.append(5)
S.append(4)
S.append(3)
S.append(2)
S.append(1)

N = 7

insertToBottom(S, N)

# Iterative version``
# Python3 program for the above approach

# Function to insert an element
# at the bottom of a given stack


def insertToBottom(S, N):

    # Temporary stack
    temp = []

    # Iterate until S becomes empty
    while (len(S) != 0):

        # Push the top element of S
        # into the stack temp
        temp.append(S[-1])

        # Pop the top element of S
        S.pop()

    # Push N into the stack S
    S.append(N)

    # Iterate until temp becomes empty
    while (len(temp) != 0):

        # Push the top element of
        # temp into the stack S
        S.append(temp[-1])

        # Pop the top element of temp
        temp.pop()

    # Print the elements of S
    while (len(S) != 0):
        print(S[-1], end=" ")
        S.pop()


# Driver Code
if __name__ == "__main__":

    # Input
    S = []
    S.append(5)
    S.append(4)
    S.append(3)
    S.append(2)
    S.append(1)

    N = 7

    insertToBottom(S, N)
