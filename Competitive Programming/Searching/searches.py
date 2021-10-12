
# File:    searches.py
# Author:  EXAMPLE
# Date:    July 23, 2019

def linear_search(v, j):    # v is unordered
    '''return True if j in v, else False'''
    for val in v:
        if val == j:
            return True
    return False

def binary_search(v, j):    # v is sorted (non-descending)
    '''return True if j in v, else False'''
    lo = 0
    hi = len(v) - 1
    while lo < hi:
        mid = (hi + lo) // 2
        if v[mid] < j:
            lo = mid + 1
        else:
            hi = mid
    return True if v[lo] == j else False

def set_search(s, j):    # s is a set
    '''return True if j in s, else False'''
    return j in s

import numpy as np
import time

for v_factor in [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048]:
    v_len = 1000 * v_factor

    # v_len random ints from ~Unif[0, 2_000_000_000)
    #     likely to have several duplicate and/or missing values
    v_raw = np.random.randint(2_000_000_000, size=v_len)
    v_unordered = v_raw.copy()
    v_sorted = v_raw.copy()
    v_sorted.sort()
    v_set = set(v_raw)

    # search sample is always 100_000 ints
    #   from ~Unif[0, 2_000_000_000)
    samples = np.random.randint(2_000_000_000, size=100_000)

    v_linear_begin = time.time()
    for j in samples:
        linear_search(v_unordered, j)
    v_linear_end = time.time()

    v_binary_begin = time.time()
    for j in samples:
        binary_search(v_sorted, j)
    v_binary_end = time.time()

    v_set_begin = time.time()
    for j in samples:
        set_search(v_set, j)
    v_set_end = time.time()

    print('{:5d},{:20.9f},{:12.9f},{:12.9f}'.format(v_factor,
            (v_linear_end - v_linear_begin),
            (v_binary_end - v_binary_begin),
            (v_set_end - v_set_begin)))
   
