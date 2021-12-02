// Time:  O(n)
// Space: O(1)

class Solution
{
public:
    int majorityElement(int nums[], int size)
    {

        // your code here
        int ans = nums[0], cnt = 1;
        for (int i = 0; i < size; i++)
        {
            if (nums[i] == ans)
            {
                ++cnt;
            }
            else
            {
                --cnt;
                if (cnt == 0)
                {
                    ans = nums[i];
                    cnt = 1;
                }
            }
        }
        int ct = 0;

        for (int i = 0; i < size; i++)
        {
            if (nums[i] == ans)
                ct++;
        }

        return (ct > size / 2) ? ans : -1;
    }
};
