# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma
#
# Topic: Python for Quantitative Finance (NumPy & Pandas)
# Description: Essential operations for data manipulation in Quant roles.

import numpy as np
import pandas as pd

def numpy_demo():
    print("--- NumPy ---")
    # Efficient Arrays
    arr = np.array([1, 2, 3, 4, 5])
    
    # Vectorized Operations (Crucial for performance)
    print(f"Squared: {arr ** 2}")
    
    # Statistics
    print(f"Mean: {np.mean(arr)}, Std Dev: {np.std(arr)}")
    
    # Random Walk Generation (Brownian Motion Sim)
    steps = np.random.normal(0, 1, 1000)
    path = np.cumsum(steps)
    print(f"Random Walk End Point: {path[-1]:.2f}")

def pandas_demo():
    print("\n--- Pandas ---")
    # Time Series Data (Stock Prices)
    dates = pd.date_range("20230101", periods=5)
    df = pd.DataFrame(np.random.randn(5, 2), index=dates, columns=["AAPL", "GOOG"])
    
    # Access
    print("Head:\n", df.head())
    
    # Rolling Window (Simple Moving Average)
    df["AAPL_SMA_3"] = df["AAPL"].rolling(window=3).mean()
    print("\nWith SMA:\n", df)
    
    # Returns Calculation (Log Returns)
    df["Returns"] = np.log(df["AAPL"] / df["AAPL"].shift(1))
    print("\nLog Returns:\n", df["Returns"].head())

if __name__ == "__main__":
    numpy_demo()
    pandas_demo()
