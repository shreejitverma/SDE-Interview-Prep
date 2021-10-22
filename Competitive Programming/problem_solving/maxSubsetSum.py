#URL : https://www.hackerrank.com/challenges/max-array-sum
#!/bin/python

import math
import os
import random
import re
import sys

# Complete the maxSubsetSum function below.
def maxSubsetSum(arr):
    if len(arr) == 1:
        return arr[0]
    if len(arr) == 2:
        return max(arr[0], arr[1])
    maxSum = [0] * len(arr)
    maxSum[0] = arr[0]
    maxSum[1] = arr[1]
    
    for i in range(len(arr)):
        if arr[i] > 0 and maxSum[i-2] < 0:
            maxSum = max(maxSum[i-1], arr[i])
        else:
            maxSum[i] = max(maxSum[i-2] + arr[i], maxSum[i-1])
            
    return maxSum[len(arr)-1]
    
if __name__ == '__main__':
    
    n = int(raw_input())

    arr = map(int, raw_input().rstrip().split())

    res = maxSubsetSum(arr)

    print res