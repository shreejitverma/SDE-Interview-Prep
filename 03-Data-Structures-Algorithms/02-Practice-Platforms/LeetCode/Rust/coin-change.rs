/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(amount * coins.length)
// Space: O(amount)

impl Solution {
    pub fn coin_change(coins: Vec<i32>, amount: i32) -> i32 {
        if amount <= 0 {
            return 0;
        }
        let amt = amount as usize;
        let mut dp = vec![amount + 1; amt + 1];
        dp[0] = 0;

        for i in 1..=amt {
            for &coin in &coins {
                let c = coin as usize;
                if coin > 0 && c <= i {
                    dp[i] = dp[i].min(dp[i - c] + 1);
                }
            }
        }

        if dp[amt] > amount {
            -1
        } else {
            dp[amt]
        }
    }
}
