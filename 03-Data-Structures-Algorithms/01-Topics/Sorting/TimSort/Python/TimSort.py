"""
Author: Shreejit Verma
GitHub: https://github.com/shreejitverma

Algorithm: TimSort
Time Complexity: O(N log N) worst & average, O(N) best case
Space Complexity: O(N) auxiliary
"""

RUN = 32

def insertion_sort(arr: list[int], left: int, right: int) -> None:
    for i in range(left + 1, right + 1):
        temp = arr[i]
        j = i - 1
        while j >= left and arr[j] > temp:
            arr[j + 1] = arr[j]
            j -= 1
        arr[j + 1] = temp

def merge(arr: list[int], l: int, m: int, r: int) -> None:
    left = arr[l : m + 1]
    right = arr[m + 1 : r + 1]
    i, j, k = 0, 0, l

    while i < len(left) and j < len(right):
        if left[i] <= right[j]:
            arr[k] = left[i]
            i += 1
        else:
            arr[k] = right[j]
            j += 1
        k += 1

    while i < len(left):
        arr[k] = left[i]
        i += 1
        k += 1

    while j < len(right):
        arr[k] = right[j]
        j += 1
        k += 1

def tim_sort(arr: list[int]) -> None:
    n = len(arr)

    for i in range(0, n, RUN):
        insertion_sort(arr, i, min(i + RUN - 1, n - 1))

    size = RUN
    while size < n:
        for left in range(0, n, 2 * size):
            mid = left + size - 1
            right = min(left + 2 * size - 1, n - 1)
            if mid < right:
                merge(arr, left, mid, right)
        size *= 2
