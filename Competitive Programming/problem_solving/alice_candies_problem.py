# Complete the candies function below.
def candies(n, arr):
    candiesList = [1] * n
    for i in range(1, len(arr)):
        if arr[i] > arr[i-1]:
            candiesList[i] = candiesList[i-1]+1

    for i in range(len(arr)-2, -1, -1):
        if arr[i] > arr[i+1] and candiesList[i] <= candiesList[i+1]:
            candiesList[i] = max(candiesList[i], candiesList[i+1]+1)

    return sum(candiesList)