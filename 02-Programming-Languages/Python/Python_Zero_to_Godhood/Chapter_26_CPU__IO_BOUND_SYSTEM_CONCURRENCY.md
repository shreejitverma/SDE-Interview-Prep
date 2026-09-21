---
type: concept
track: [sde, quant-dev, low-latency]
level:
status: draft
last_reviewed:
sources: []
---

# CPU & I/O BOUND SYSTEM CONCURRENCY

*   **Process Pools**: Using `concurrent.futures.ProcessPoolExecutor` to distribute computations across multiple cores.
*   **uvloop**: Dropping in Libuv event loops to speed up asyncio execution.
