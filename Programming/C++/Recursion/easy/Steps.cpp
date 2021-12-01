// https://leetcode.com/problems/number-of-steps-to-reduce-a-number-to-zero
#include <iostream>
using namespace std;
int helper(int num, int steps);
int numberOfSteps(int num)
{
    return helper(num, 0);
}

int helper(int num, int steps)
{
    if (num == 0)
    {
        return steps;
    }

    if (num % 2 == 0)
    {
        return helper(num / 2, steps + 1);
    }
    return helper(num - 1, steps + 1);
}
int main()
{
    cout << numberOfSteps(5) << "\n";
}
