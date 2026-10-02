import asyncio
import sys
import time

class RateLimiter:
    def __init__(self, items_per_sec):
        self.items_per_sec = items_per_sec
        self.tokens = items_per_sec
        self.last_update = time.monotonic()
    
    async def consume(self, amount, timeout):
        start = time.monotonic()
        while True:
            now = time.monotonic()
            elapsed = now - self.last_update
            self.tokens = min(self.items_per_sec, self.tokens + elapsed * self.items_per_sec)
            self.last_update = now
            
            if self.tokens >= amount:
                self.tokens -= amount
                return True
            
            if time.monotonic() - start > timeout:
                return False
                
            await asyncio.sleep(0.01)

async def handle_client(reader, writer, rate_limiter):
    data = await reader.readline()
    if not data:
        return
    try:
        parts = data.decode().strip().split()
        if parts[0] == "GET":
            count = int(parts[1])
            timeout = 0.5
            if await rate_limiter.consume(count, timeout):
                writer.write(f"OK {count}\n".encode())
            else:
                writer.write(b"OVERLOAD\n")
            await writer.drain()
    except Exception as e:
        pass
    writer.close()

async def main(port, capacity):
    limiter = RateLimiter(capacity)
    server = await asyncio.start_server(
        lambda r, w: handle_client(r, w, limiter),
        '127.0.0.1', port)
    
    print(f"Listening on port {port} with capacity {capacity} items/sec")
    async with server:
        await server.serve_forever()

if __name__ == '__main__':
    port = int(sys.argv[1])
    capacity = int(sys.argv[2])
    asyncio.run(main(port, capacity))
