/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Algorithm: TimSort
 * Time Complexity: O(N log N) worst & average, O(N) best case
 * Space Complexity: O(N) auxiliary
 */

#include <iostream>
#include <vector>
#include <algorithm>

const int RUN = 32;

void insertionSort(std::vector<int>& arr, int left, int right) {
    for (int i = left + 1; i <= right; ++i) {
        int temp = arr[i];
        int j = i - 1;
        while (j >= left && arr[j] > temp) {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = temp;
    }
}

void merge(std::vector<int>& arr, int l, int m, int r) {
    int len1 = m - l + 1, len2 = r - m;
    std::vector<int> left(len1), right(len2);

    for (int i = 0; i < len1; ++i) left[i] = arr[l + i];
    for (int i = 0; i < len2; ++i) right[i] = arr[m + 1 + i];

    int i = 0, j = 0, k = l;
    while (i < len1 && j < len2) {
        if (left[i] <= right[j]) {
            arr[k++] = left[i++];
        } else {
            arr[k++] = right[j++];
        }
    }
    while (i < len1) arr[k++] = left[i++];
    while (j < len2) arr[k++] = right[j++];
}

void timSort(std::vector<int>& arr) {
    int n = static_cast<int>(arr.size());

    for (int i = 0; i < n; i += RUN) {
        insertionSort(arr, i, std::min(i + RUN - 1, n - 1));
    }

    for (int size = RUN; size < n; size *= 2) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = std::min(left + 2 * size - 1, n - 1);
            if (mid < right) {
                merge(arr, left, mid, right);
            }
        }
    }
}
