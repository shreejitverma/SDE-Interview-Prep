'''https://practice.geeksforgeeks.org/problems/merge-two-binary-max-heap0144/1
Merge two binary Max heaps
Easy Accuracy: 58.28% Submissions: 5087 Points: 2
Given two binary max heaps as arrays, merge the given heaps to form a new max heap.



Example 1:

Input  :
n = 4 m = 3
a[] = {10, 5, 6, 2},
b[] = {12, 7, 9}
Output :
{12, 10, 9, 2, 5, 7, 6}
Explanation :

Submit
Submit
Submit




Your Task:
You don't need to read input or print anything. Your task is to complete the function mergeHeaps() which takes the array a[], b[], its size n and m, as inputs and return the merged max heap. Since there can be multiple solutions, therefore, to check for the correctness of your solution, your answer will be checked by the driver code and will return 1 if it is correct, else it returns 0.



Expected Time Complexity: O(n.Logn)
Expected Auxiliary Space: O(1)



Constraints:
1 <= n, m <= 105
1 <= a[i], b[i] <= 2*105'''

   def heapify(self, arr, n, i):
        largest = i
        l = 2*i+1
        r = 2*i+2
        if l < n and arr[l] > arr[i]:
            largest = l
        if r < n and arr[r] > arr[i]:
            largest = r
        if largest != i:
            arr[i], arr[largest] = arr[largest], arr[i]
            self.heapify(arr, n, largest)

    def mergeHeaps(self, a, b, n, m):
        arr = a
        self.heapify(arr, n, 0)
        for i in b:
            arr.append(i)
            n += 1
            t = n
            lastleaf = (n//2)-1
            while(lastleaf > -1 and arr[t-1] > arr[lastleaf]):
                self.heapify(arr, t, lastleaf)
                t = lastleaf+1
                lastleaf = ((lastleaf+1)//2)-1
        return arr
        # your code here

# {
#  Driver Code Starts
# Initial Template for Python 3


def isMerged(arr1, arr2, merged):
    if (len(arr1) + len(arr2) != len(merged)):
        return False
    arr1 += arr2
    arr1.sort()
    mergedCopy = sorted(merged)
    if (arr1 != mergedCopy):
        return False
    for i in range(1, len(merged)):
        if merged[i] > merged[(i-1)//2]:
            return False

    return True


if __name__ == "__main__":
    for _ in range(int(input())):
        n, m = map(int, input().split())
        a = [int(i) for i in input().split()]
        b = [int(i) for i in input().split()]
        copyA = a[:]
        copyB = b[:]
        obj = Solution()
        merged = obj.mergeHeaps(a, b, n, m)
        flag = isMerged(copyA, copyB, merged)
        print(0 if flag == False else 1)

# } Driver Code Ends
