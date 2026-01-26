# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma
#
# Topic: Event-Driven Backtesting Framework
# Description: A skeleton for simulating trading strategies tick-by-tick.
#              Simulates the interaction between DataHandler, Strategy, Portfolio, and Execution.

import queue
import time

class Event:
    pass

class MarketEvent(Event):
    def __init__(self):
        self.type = 'MARKET'

class SignalEvent(Event):
    def __init__(self, symbol, datetime, signal_type):
        self.type = 'SIGNAL'
        self.symbol = symbol
        self.datetime = datetime
        self.signal_type = signal_type # 'LONG' or 'SHORT'

class OrderEvent(Event):
    def __init__(self, symbol, order_type, quantity, direction):
        self.type = 'ORDER'
        self.symbol = symbol
        self.order_type = order_type # 'MKT' or 'LMT'
        self.quantity = quantity
        self.direction = direction # 'BUY' or 'SELL'

# --- Components ---

class DataHandler:
    def __init__(self, event_queue):
        self.event_queue = event_queue
    
    def update_bars(self):
        # In real life, fetch next tick from CSV/API
        print("Data: New Market Data received.")
        self.event_queue.put(MarketEvent())

class Strategy:
    def __init__(self, event_queue):
        self.event_queue = event_queue

    def calculate_signals(self, event):
        if event.type == 'MARKET':
            # Simple Logic: Always Buy
            print("Strategy: Generating Buy Signal.")
            signal = SignalEvent("AAPL", time.time(), "LONG")
            self.event_queue.put(signal)

class Portfolio:
    def __init__(self, event_queue):
        self.event_queue = event_queue

    def update_signal(self, event):
        if event.type == 'SIGNAL':
            print(f"Portfolio: Received {event.signal_type} signal for {event.symbol}.")
            # Risk Management checks here...
            order = OrderEvent(event.symbol, 'MKT', 100, 'BUY')
            self.event_queue.put(order)

class ExecutionHandler:
    def execute_order(self, event):
        if event.type == 'ORDER':
            print(f"Execution: Sent {event.direction} order for {event.quantity} {event.symbol} to Broker.")

# --- Main Loop ---

def run_backtest():
    events = queue.Queue()
    
    data = DataHandler(events)
    strategy = Strategy(events)
    portfolio = Portfolio(events)
    execution = ExecutionHandler()

    # Simulation Loop
    for i in range(3): # Simulate 3 ticks
        print(f"\n--- Tick {i+1} ---")
        data.update_bars()

        while True:
            try:
                event = events.get(False)
            except queue.Empty:
                break
            
            if event.type == 'MARKET':
                strategy.calculate_signals(event)
            elif event.type == 'SIGNAL':
                portfolio.update_signal(event)
            elif event.type == 'ORDER':
                execution.execute_order(event)

if __name__ == "__main__":
    run_backtest()
