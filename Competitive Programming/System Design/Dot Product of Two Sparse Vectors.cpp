/*
https://leetcode.com/problems/dot-product-of-two-sparse-vectors/
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
0 <= nums1[i], nums2[i] <= 100
*/

// Time:  ctor: O(n)
//        dot_product: O(min(n, m))
// Space: O(n)

class SparseVector
{
public:
    SparseVector(vector<int> &nums)
    {
        for (int i = 0; i < nums.size(); ++i)
        {
            if (nums[i])
            {
                lookup_[i] = nums[i];
            }
        }
    }

    int dotProduct(SparseVector &vec)
    {
        auto a = this, b = &vec;
        if (a->lookup_.size() > b->lookup_.size())
        {
            swap(a, b);
        }
        int result = 0;
        for (const auto &[i, v] : a->lookup_)
        {
            if (b->lookup_.count(i))
            {
                result += v * b->lookup_[i];
            }
        }
        return result;
    }

private:
    unordered_map<int, int> lookup_;
};

// Your SparseVector object will be instantiated and called as such:
// SparseVector v1(nums1);
// SparseVector v2(nums2);
// int ans = v1.dotProduct(v2);