#https://www.hackerrank.com/challenges/minimum-swaps-2/problem?h_l=interview&playlist_slugs%5B%5D=interview-preparation-kit&playlist_slugs%5B%5D=arrays
#You are given an unordered array consisting of consecutive integers  [1, 2, 3, ..., n] without any duplicates. You are allowed to swap any two elements. You need to find the minimum number of swaps required to sort the array in ascending order.

def minimumSwaps(arr):
    idx = 0
    swap = 0
    while idx < len(arr)-1:
        #print arr
        if arr[idx] == (idx+1):
            idx += 1
        else:
            t = arr[idx]
            arr[idx] = arr[arr[idx] - 1]
            arr[t-1] = t
            swap += 1
    return swap