'''https://leetcode.com/problems/dot-product-of-two-sparse-vectors/
1570. Dot Product of Two Sparse Vectors
Medium

485

61

Add to List

Share
Given two sparse vectors, compute their dot product.

Implement class SparseVector:

SparseVector(nums) Initializes the object with the vector nums
dotProduct(vec) Compute the dot product between the instance of SparseVector and vec
A sparse vector is a vector that has mostly zero values, you should store the sparse vector efficiently and compute the dot product between two SparseVector.

Follow up: What if only one of the vectors is sparse?

 

Example 1:

Input: nums1 = [1,0,0,2,3], nums2 = [0,3,0,4,0]
Output: 8
Explanation: v1 = SparseVector(nums1) , v2 = SparseVector(nums2)
v1.dotProduct(v2) = 1*0 + 0*3 + 0*0 + 2*4 + 3*0 = 8
Example 2:

Input: nums1 = [0,1,0,0,0], nums2 = [0,0,0,0,2]
Output: 0
Explanation: v1 = SparseVector(nums1) , v2 = SparseVector(nums2)
v1.dotProduct(v2) = 0*0 + 1*0 + 0*0 + 0*0 + 0*2 = 0
Example 3:

Input: nums1 = [0,1,0,0,2,0,0], nums2 = [1,0,0,0,3,0,4]
Output: 6
 

Constraints:

n == nums1.length == nums2.length
1 <= n <= 10^5
0 <= nums1[i], nums2[i] <= 100'''

# Runtime: 1772 ms, faster than 80.16% of Python3 online submissions for Dot Product of Two Sparse Vectors.
# Memory Usage: 18.5 MB, less than 61.88% of Python3 online submissions for Dot Product of Two Sparse Vectors.

nums1 = [1, 0, 0, 2, 3]
nums2 = [0, 3, 0, 4, 0]


class SparseVector:
    def __init__(self, nums):

        self.nums = nums

    def dotProduct(self, vec):

        running = 0
        for i, v in enumerate(self.nums):
            product = (v * vec.nums[i])
            running += product
        return running


# Your SparseVector object will be instantiated and called as such:
v1 = SparseVector(nums1)
v2 = SparseVector(nums2)
print(v1.nums)
print(v2.nums)
ans = v1.dotProduct(v2)
print(ans)
# Your SparseVector object will be instantiated and called as such:
# v1 = SparseVector(nums1)
# v2 = SparseVector(nums2)
# ans = v1.dotProduct(v2)
