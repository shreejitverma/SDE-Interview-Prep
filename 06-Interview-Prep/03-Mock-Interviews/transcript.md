# Mock Interview: System Design (Rate Limiter)

**Interviewer (I):** "Hello! Today I'd like you to design a Rate Limiter for our API Gateway."

**Candidate (C):** "Sure. Before we dive in, can I clarify some requirements?"

**I:** "Go ahead."

**C:** "What kind of scale are we looking at? Is this for a single server or distributed system?"

**I:** "Distributed. We have about 100 million active users."

**C:** "Got it. And what's the strictness? Can we allow a few extra requests, or must it be hard-capped?"

**I:** "It should be fairly strict to prevent abuse, but we prioritize low latency."

**C:** "Okay. I propose using a **Token Bucket** algorithm backed by **Redis** for shared state. 
1.  **Why Token Bucket?** It allows burst traffic (unlike Leaky Bucket) which is good for user experience.
2.  **Why Redis?** We need a fast, in-memory store accessible by all API servers.
3.  **Latency:** Using Lua scripts in Redis ensures atomicity and reduces round trips."

**(Self-Correction / Good Signal)**
**C:** "Wait, checking Redis for *every* request might add 2-3ms latency and load the Redis cluster heavily. 
We could optimize this by implementing a **multi-tier cache**. A small local counter in the application memory (e.g., allow 5 req/sec locally) and sync with Redis asynchronously. However, this trades off consistency for performance. Given the requirement for 'strictness', I will stick to the pure Redis Lua approach for now, but we can discuss scaling Redis via sharding later."

**I:** "That sounds reasonable. How would you handle the Redis keys?"

**C:** "I'd use `user_id` as the key. But to prevent the 'Thundering Herd' problem where all keys expire at the same time, we can add a small random jitter to the expiration times."

---

## 🔑 Key Takeaways

1.  **Clarify Requirements First:** Don't just start coding. Ask about scale, strictness, and latency.
2.  **Propose Options:** Discuss Token Bucket vs Leaky Bucket vs Sliding Window.
3.  **Address Bottlenecks:** The candidate identified that Redis could be a bottleneck and proposed a solution (Local Cache / Sharding).
4.  **Trade-offs:** Explicitly mentioning "Consistency vs Performance" is a Senior Engineer trait.
