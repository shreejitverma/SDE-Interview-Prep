# Design a Rate Limiter

## 1. Requirements
*   **Functional:**
    *   Limit requests to `N` requests per `T` seconds (e.g., 10 req / 1 sec).
    *   Should work in a distributed environment (multiple app servers).
*   **Non-Functional:**
    *   Low Latency (Don't slow down legitimate requests).
    *   High Availability.
    *   Accurate enough (Strict counting).

## 2. Algorithms
### A. Token Bucket (Preferred)
*   **Concept:**
    *   A bucket holds `N` tokens.
    *   Refill tokens at rate `R` per second.
    *   Request consumes 1 token.
    *   If bucket empty -> Reject.
*   **Pros:** Allows bursts of traffic. Memory efficient.

### B. Leaky Bucket
*   **Concept:** Requests enter a queue. Processed at constant rate.
*   **Pros:** Smooths out traffic (no bursts).

### C. Fixed Window Counter
*   **Concept:** Counter resets every minute.
*   **Cons:** Spike at edges of window (e.g., 59th second and 1st second allows 2x load).

## 3. High Level Design
1.  **Client** sends request.
2.  **Load Balancer** forwards to API Server.
3.  **API Server** checks **Rate Limiter Middleware**.
4.  **Middleware** talks to **Redis** (Shared State).

## 4. Implementation Details (Redis)
We need atomic operations to prevent race conditions.

### Approach 1: Redis `INCR` (Fixed Window)
*   Key: `user_id:timestamp` (e.g., `user_123:16100001`)
*   Value: Count.
*   TTL: 1 second.
*   *Problem:* Race conditions without Lua.

### Approach 2: Sliding Window Log (Sorted Sets)
*   Key: `user_id`
*   Value: Sorted Set (ZSET) of timestamps.
*   **Logic:**
    1.  `ZREMRANGEBYSCORE key -inf (now - window_size)` (Remove old timestamps).
    2.  `ZCARD key` (Count current requests).
    3.  If count < limit: `ZADD key now now` (Add current).
    4.  Else: Reject.
*   *Pros:* Very accurate.
*   *Cons:* High memory (stores all timestamps).

### Approach 3: Token Bucket in Redis (Lua Script)
*   Store `tokens_left` and `last_refill_time` in a Hash.
*   **Lua Script (Atomic):**
    ```lua
    local key = KEYS[1]
    local rate = tonumber(ARGV[1])
    local capacity = tonumber(ARGV[2])
    local now = tonumber(ARGV[3])
    
    local info = redis.call("HMGET", key, "tokens", "last_refilled")
    local tokens = tonumber(info[1])
    local last_refilled = tonumber(info[2])
    
    if tokens == nil then
        tokens = capacity
        last_refilled = now
    end
    
    local delta = math.max(0, now - last_refilled)
    local filled_tokens = math.min(capacity, tokens + (delta * rate))
    
    if filled_tokens >= 1 then
        redis.call("HMSET", key, "tokens", filled_tokens - 1, "last_refilled", now)
        return 1 -- Allowed
    else
        return 0 -- Rejected
    end
    ```

## 5. Optimization
*   **Local Caching:** Keep a small counter in App Server memory to reduce Redis calls (Trade-off: consistency).
*   **Tiered Limits:** 10 req/sec, but 1000 req/hour.
