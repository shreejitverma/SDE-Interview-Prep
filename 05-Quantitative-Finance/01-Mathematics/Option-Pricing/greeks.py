# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma
#
# Topic: The Greeks (Option Risk Sensitivities)
# Description: Calculates Delta, Gamma, Theta, Vega, and Rho.
#              These measure how option price changes with respect to market variables.
#
#   Delta (Δ): Sensitivity to Underlying Price.
#   Gamma (Γ): Sensitivity to Delta (Convexity).
#   Theta (Θ): Sensitivity to Time Decay.
#   Vega  (ν): Sensitivity to Volatility.
#   Rho   (ρ): Sensitivity to Interest Rate.

import math
from scipy.stats import norm

class OptionGreeks:
    def __init__(self, S, K, T, r, sigma):
        self.S = S          # Spot Price
        self.K = K          # Strike Price
        self.T = T          # Time to Maturity (Years)
        self.r = r          # Risk-free Rate
        self.sigma = sigma  # Volatility

    def _d1(self):
        return (math.log(self.S / self.K) + (self.r + 0.5 * self.sigma ** 2) * self.T) / (self.sigma * math.sqrt(self.T))

    def _d2(self):
        return self._d1() - self.sigma * math.sqrt(self.T)

    def calculate_greeks(self, option_type="call"):
        d1 = self._d1()
        d2 = self._d2()
        
        # Common terms
        pdf_d1 = norm.pdf(d1)
        cdf_d1 = norm.cdf(d1)
        cdf_d2 = norm.cdf(d2)
        cdf_minus_d1 = norm.cdf(-d1)
        cdf_minus_d2 = norm.cdf(-d2)

        greeks = {}

        # 1. Delta
        if option_type == "call":
            greeks['delta'] = cdf_d1
        else:
            greeks['delta'] = cdf_d1 - 1

        # 2. Gamma (Same for Call and Put)
        greeks['gamma'] = pdf_d1 / (self.S * self.sigma * math.sqrt(self.T))

        # 3. Vega (Same for Call and Put)
        greeks['vega'] = self.S * pdf_d1 * math.sqrt(self.T) / 100 # Usually expressed per 1% vol change

        # 4. Theta
        term1 = -(self.S * pdf_d1 * self.sigma) / (2 * math.sqrt(self.T))
        if option_type == "call":
            term2 = self.r * self.K * math.exp(-self.r * self.T) * cdf_d2
            greeks['theta'] = (term1 - term2) / 365 # Daily Theta
        else:
            term2 = self.r * self.K * math.exp(-self.r * self.T) * cdf_minus_d2
            greeks['theta'] = (term1 + term2) / 365

        # 5. Rho
        if option_type == "call":
            greeks['rho'] = self.K * self.T * math.exp(-self.r * self.T) * cdf_d2 / 100
        else:
            greeks['rho'] = -self.K * self.T * math.exp(-self.r * self.T) * cdf_minus_d2 / 100

        return greeks

if __name__ == "__main__":
    # Example:
    op = OptionGreeks(S=100, K=100, T=1, r=0.05, sigma=0.2)
    
    print("--- Call Option Greeks ---")
    call_greeks = op.calculate_greeks("call")
    for k, v in call_greeks.items():
        print(f"{k.capitalize()}: {v:.4f}")

    print("\n--- Put Option Greeks ---")
    put_greeks = op.calculate_greeks("put")
    for k, v in put_greeks.items():
        print(f"{k.capitalize()}: {v:.4f}")
