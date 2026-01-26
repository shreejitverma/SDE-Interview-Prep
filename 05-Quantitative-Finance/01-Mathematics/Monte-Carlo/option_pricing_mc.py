# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma
#
# Topic: Monte Carlo Option Pricing
# Description: Prices a European Call Option using Monte Carlo Simulation.
#              dS = r*S*dt + sigma*S*dW (Geometric Brownian Motion)

import numpy as np

def monte_carlo_call_price(S, K, T, r, sigma, num_simulations=100000):
    """
    S: Spot Price
    K: Strike Price
    T: Time to Maturity (Years)
    r: Risk-free Rate
    sigma: Volatility
    """
    np.random.seed(42) # For reproducibility

    # 1. Simulate End Prices (S_T)
    # S_T = S * exp( (r - 0.5*sigma^2)*T + sigma*sqrt(T)*Z )
    # where Z ~ N(0, 1)
    
    Z = np.random.standard_normal(num_simulations)
    S_T = S * np.exp((r - 0.5 * sigma**2) * T + sigma * np.sqrt(T) * Z)

    # 2. Calculate Payoffs: max(S_T - K, 0)
    payoffs = np.maximum(S_T - K, 0)

    # 3. Discount back to present value
    price = np.exp(-r * T) * np.mean(payoffs)
    
    return price

if __name__ == "__main__":
    S, K, T, r, sigma = 100, 100, 1, 0.05, 0.2
    
    mc_price = monte_carlo_call_price(S, K, T, r, sigma)
    print(f"Monte Carlo Call Price: ${mc_price:.4f}")
    
    # Verification: Black Scholes price should be approx $10.45
