# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma
#
# Topic: Black-Scholes Option Pricing
# Description: Standard implementation of the Black-Scholes formula for European Options.
#              This is a fundamental concept for Quantitative Analyst interviews.

import math
from scipy.stats import norm

class BlackScholes:
    def __init__(self, S, K, T, r, sigma):
        """
        S: Spot Price of the underlying asset
        K: Strike Price
        T: Time to maturity (in years)
        r: Risk-free interest rate (decimal, e.g., 0.05 for 5%)
        sigma: Volatility of the underlying asset (decimal)
        """
        self.S = S
        self.K = K
        self.T = T
        self.r = r
        self.sigma = sigma

    def d1(self):
        return (math.log(self.S / self.K) + (self.r + 0.5 * self.sigma ** 2) * self.T) / (self.sigma * math.sqrt(self.T))

    def d2(self):
        return self.d1() - self.sigma * math.sqrt(self.T)

    def call_price(self):
        """Calculates the price of a Call Option"""
        return self.S * norm.cdf(self.d1()) - self.K * math.exp(-self.r * self.T) * norm.cdf(self.d2())

    def put_price(self):
        """Calculates the price of a Put Option"""
        return self.K * math.exp(-self.r * self.T) * norm.cdf(-self.d2()) - self.S * norm.cdf(-self.d1())

if __name__ == "__main__":
    # Example: Apple Stock
    S = 150  # Spot Price
    K = 155  # Strike Price
    T = 0.5  # 6 Months
    r = 0.03 # 3% Risk-free rate
    sigma = 0.2 # 20% Volatility

    bs = BlackScholes(S, K, T, r, sigma)

    print(f"Underlying Price: ${S}")
    print(f"Strike Price:     ${K}")
    print(f"Time to Expiry:   {T} years")
    print("-" * 30)
    print(f"Call Option Price: ${bs.call_price():.2f}")
    print(f"Put Option Price:  ${bs.put_price():.2f}")
