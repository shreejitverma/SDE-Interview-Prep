# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma
#
# Topic: Mean Reversion Strategy (Bollinger Bands)
# Description: A simple algorithmic trading strategy.
#            - Buy when price < Lower Band (Oversold)
#            - Sell when price > Upper Band (Overbought)

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

def generate_signals(prices, window=20, num_std=2):
    """
    Generates Buy/Sell signals based on Bollinger Bands.
    """
    df = pd.DataFrame(prices, columns=['Price'])
    
    # Calculate Moving Average and Standard Deviation
    df['MA'] = df['Price'].rolling(window=window).mean()
    df['Std'] = df['Price'].rolling(window=window).std()
    
    # Calculate Bands
    df['Upper'] = df['MA'] + (df['Std'] * num_std)
    df['Lower'] = df['MA'] - (df['Std'] * num_std)
    
    # Signals
    df['Signal'] = 0
    # Buy condition
    df.loc[df['Price'] < df['Lower'], 'Signal'] = 1 
    # Sell condition
    df.loc[df['Price'] > df['Upper'], 'Signal'] = -1
    
    return df

if __name__ == "__main__":
    # Simulate Price Data (Random Walk with Mean Reversion)
    np.random.seed(42)
    returns = np.random.normal(0, 1, 100)
    price_series = 100 + np.cumsum(returns)
    
    signals = generate_signals(price_series, window=5)
    
    print(signals.tail(10))
    
    # In a real project, you would backtest this using a library like 'backtrader'.
