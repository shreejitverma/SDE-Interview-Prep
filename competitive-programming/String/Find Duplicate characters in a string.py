# https://www.geeksforgeeks.org/print-all-the-duplicates-in-the-input-string/

# Python 3 program to count all duplicates
# from string using maps
from collections import defaultdict


def printDups(st):

    count = defaultdict(int)
    for i in range(len(st)):
        count[st[i]] += 1

    for it in count:
        if (count[it] > 1):
            print(it, ", count = ", count[it])


# Driver code
if __name__ == "__main__":

    st = "test string"
    printDups(st)
