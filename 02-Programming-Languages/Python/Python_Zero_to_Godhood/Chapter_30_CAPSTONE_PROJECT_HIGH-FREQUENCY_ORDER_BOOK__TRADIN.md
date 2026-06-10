# CAPSTONE PROJECT: HIGH-FREQUENCY ORDER BOOK & TRADING ENGINE


### 30.1 High-Frequency Trading Engine Architecture
A High-Frequency Trading (HFT) matching engine requires deterministic execution latencies, minimal memory allocation to avoid garbage collection pauses, and concurrent network I/O. The diagram below illustrates the modular architecture of the trading engine:

```
[UDP/TCP Market Data Feed]
            |
            v (Asynchronous Byte Stream)
+---------------------------------------+
|        asyncio Protocol Parser        |
|  Parses raw binary packets into fixed-|
|  size structs using struct.unpack.    |
+---------------------------------------+
            |
            v (Non-blocking queue)
+---------------------------------------+
|         Lock-Free Ring Buffer         |
|  Decouples network threads from the   |
|  matching engine execution thread.    |
+---------------------------------------+
            |
            v
+---------------------------------------+
|          Order Book Engine            |
|  - Bid/Ask B-Trees or Double arrays.  |
|  - O(1) order hash lookups.           |
|  - In-place crossing matching logic.   |
+---------------------------------------+
            |
            +===> Match Event Execution ---> [ZeroMQ Broadcast Channel]
```

---

### 30.2 Optimized Order Book Implementation
To achieve sub-millisecond execution times, the engine uses:
1.  **`__slots__`**: Avoids class dictionary allocation overhead for incoming order objects.
2.  **`collections.deque`**: Provides $\mathcal{O}(1)$ insertion and pop times for ordering arrays at individual price points.
3.  **Dictionary Cache**: Tracks active order locations by ID to achieve $\mathcal{O}(1)$ cancellations.

Below is a complete implementation of the order book engine:

```python
import asyncio
from collections import deque
import sys

class Order:
    __slots__ = ('order_id', 'side', 'price', 'quantity', 'timestamp')
    
    def __init__(self, order_id: str, side: str, price: float, quantity: int) -> None:
        self.order_id = order_id
        self.side = side          # 'B' for Bid, 'S' for Sell (Ask)
        self.price = price
        self.quantity = quantity
        self.timestamp = asyncio.get_event_loop().time()

    def __repr__(self) -> str:
        return f"Order({self.order_id}, {self.side}, {self.price}, {self.quantity})"

class OrderBook:
    def __init__(self) -> None:
        # Bids (buys): sorted in descending order (highest price first)
        self.bids = {}
        # Asks (sells): sorted in ascending order (lowest price first)
        self.asks = {}
        # Fast lookup map: order_id -> Order object
        self.order_map = {}

    def add_limit_order(self, order: Order) -> list:
        trades = []
        if order.side == 'B':
            # 1. Match against Asks (sells)
            while order.quantity > 0 and self.asks:
                best_ask_price = min(self.asks.keys())
                if order.price < best_ask_price:
                    break  # No cross; order cannot be matched immediately
                
                ask_queue = self.asks[best_ask_price]
                while order.quantity > 0 and ask_queue:
                    matching_order = ask_queue[0]
                    trade_qty = min(order.quantity, matching_order.quantity)
                    
                    # Execute trade
                    order.quantity -= trade_qty
                    matching_order.quantity -= trade_qty
                    trades.append((order.order_id, matching_order.order_id, best_ask_price, trade_qty))
                    
                    if matching_order.quantity == 0:
                        ask_queue.popleft()
                        del self.order_map[matching_order.order_id]
                
                if not ask_queue:
                    del self.asks[best_ask_price]
            
            # 2. Add remaining quantity to Bids book
            if order.quantity > 0:
                if order.price not in self.bids:
                    self.bids[order.price] = deque()
                self.bids[order.price].append(order)
                self.order_map[order.order_id] = order
                
        elif order.side == 'S':
            # 1. Match against Bids (buys)
            while order.quantity > 0 and self.bids:
                best_bid_price = max(self.bids.keys())
                if order.price > best_bid_price:
                    break  # No cross
                
                bid_queue = self.bids[best_bid_price]
                while order.quantity > 0 and bid_queue:
                    matching_order = bid_queue[0]
                    trade_qty = min(order.quantity, matching_order.quantity)
                    
                    # Execute trade
                    order.quantity -= trade_qty
                    matching_order.quantity -= trade_qty
                    trades.append((order.order_id, matching_order.order_id, best_bid_price, trade_qty))
                    
                    if matching_order.quantity == 0:
                        bid_queue.popleft()
                        del self.order_map[matching_order.order_id]
                
                if not bid_queue:
                    del self.bids[best_bid_price]
            
            # 2. Add remaining quantity to Asks book
            if order.quantity > 0:
                if order.price not in self.asks:
                    self.asks[order.price] = deque()
                self.asks[order.price].append(order)
                self.order_map[order.order_id] = order
                
        return trades

    def cancel_order(self, order_id: str) -> bool:
        if order_id not in self.order_map:
            return False
        
        order = self.order_map[order_id]
        price = order.price
        side = order.side
        
        if side == 'B' and price in self.bids:
            self.bids[price].remove(order)
            if not self.bids[price]:
                del self.bids[price]
        elif side == 'S' and price in self.asks:
            self.asks[price].remove(order)
            if not self.asks[price]:
                del self.asks[price]
                
        del self.order_map[order_id]
        return True

    def get_top_of_book(self) -> tuple:
        best_bid = max(self.bids.keys()) if self.bids else None
        best_ask = min(self.asks.keys()) if self.asks else None
        return best_bid, best_ask
```

---

### 30.3 High-Throughput Asyncio Server
To receive external market orders, we deploy an optimized asyncio TCP socket server. The server reads packet strings asynchronously, parsing and applying them directly to the order book instance.

```python
class TradingEngineServer:
    def __init__(self, host: str, port: int) -> None:
        self.host = host
        self.port = port
        self.order_book = OrderBook()
        self.order_counter = 0

    async def handle_client(self, reader: asyncio.StreamReader, writer: asyncio.StreamWriter) -> None:
        print("[Engine] Client connection established.")
        try:
            while True:
                data = await reader.readline()
                if not data:
                    break
                
                # Parse instruction string: "ADD B 100.50 500" or "CANCEL id"
                line = data.decode().strip()
                if not line:
                    continue
                
                parts = line.split()
                cmd = parts[0]
                
                if cmd == "ADD":
                    side, price_str, qty_str = parts[1], parts[2], parts[3]
                    self.order_counter += 1
                    order_id = f"ORD_{self.order_counter:06d}"
                    
                    order = Order(order_id, side, float(price_str), int(qty_str))
                    trades = self.order_book.add_limit_order(order)
                    
                    # Send response back to broker client
                    writer.write(f"ACK {order_id}\n".encode())
                    for trade in trades:
                        writer.write(f"TRADE {trade[0]} <-> {trade[1]} Price: {trade[2]} Qty: {trade[3]}\n".encode())
                    await writer.drain()
                    
                elif cmd == "CANCEL":
                    order_id = parts[1]
                    success = self.order_book.cancel_order(order_id)
                    status = "SUCCESS" if success else "NOT_FOUND"
                    writer.write(f"CANCELED {order_id} Status: {status}\n".encode())
                    await writer.drain()
                    
                # Periodic output of best bid/ask spreads
                bid, ask = self.order_book.get_top_of_book()
                print(f"[OrderBook] Best Bid: {bid} | Best Ask: {ask}")
                
        except Exception as e:
            print(f"[Engine] Exception encountered: {e}", file=sys.stderr)
        finally:
            writer.close()
            await writer.wait_closed()
            print("[Engine] Client connection terminated.")

    async def run(self) -> None:
        server = await asyncio.start_server(self.handle_client, self.host, self.port)
        addr = server.sockets[0].getsockname()
        print(f"[Engine] Listening on socket: {addr}")
        async with server:
            await server.serve_forever()

if __name__ == '__main__':
    # Runs the server loop locally on port 9999
    # Run: telnet localhost 9999
    # Input: ADD B 100.50 1000
    # Input: ADD S 100.45 500
    engine = TradingEngineServer('127.0.0.1', 9999)
    try:
        asyncio.run(engine.run())
    except KeyboardInterrupt:
        print("\n[Engine] Server stopped.")
```

---

