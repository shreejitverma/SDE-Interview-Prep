import asyncio
import sys
import time
import random

async def fetch(port, count):
    try:
        reader, writer = await asyncio.open_connection('127.0.0.1', port)
        writer.write(f"GET {count}\n".encode())
        await writer.drain()
        data = await reader.readline()
        writer.close()
        await writer.wait_closed()
        
        response = data.decode().strip()
        if response.startswith("OK"):
            return int(response.split()[1])
        else:
            return 0
    except Exception:
        return 0

async def query_replicated(ports, total_items):
    port = random.choice(ports)
    harvest = await fetch(port, total_items)
    return harvest

async def query_partitioned(ports, items_per_partition):
    tasks = [fetch(port, items_per_partition) for port in ports]
    results = await asyncio.gather(*tasks)
    return sum(results)

async def load_generator(mode, ports, qps, duration, total_items):
    queries = int(qps * duration)
    delay = 1.0 / qps
    
    harvests = []
    
    async def worker():
        if mode == "replicated":
            h = await query_replicated(ports, total_items)
        else:
            h = await query_partitioned(ports, total_items // len(ports))
        harvests.append(h)

    start_time = time.monotonic()
    tasks = []
    for i in range(queries):
        tasks.append(asyncio.create_task(worker()))
        target_time = start_time + (i + 1) * delay
        now = time.monotonic()
        if target_time > now:
            await asyncio.sleep(target_time - now)
            
    await asyncio.gather(*tasks)
    
    successful_queries = sum(1 for h in harvests if h > 0)
    yield_pct = (successful_queries / queries) * 100.0 if queries > 0 else 0
    
    total_requested = queries * total_items
    total_harvested = sum(harvests)
    harvest_pct = (total_harvested / total_requested) * 100.0 if total_requested > 0 else 0
    
    print(f"Mode: {mode}")
    print(f"Yield: {yield_pct:.1f}% ({successful_queries}/{queries} queries)")
    print(f"Harvest: {harvest_pct:.1f}% ({total_harvested}/{total_requested} items)")

if __name__ == '__main__':
    mode = sys.argv[1]
    qps = float(sys.argv[2])
    duration = float(sys.argv[3])
    ports_str = sys.argv[4]
    ports = [int(p) for p in ports_str.split(',')]
    
    total_items = 1000
    asyncio.run(load_generator(mode, ports, qps, duration, total_items))
