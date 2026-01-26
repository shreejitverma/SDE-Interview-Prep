# Design a Real-Time Chat System (WhatsApp/Slack)

## 1. Requirements
*   **Functional:** 1-on-1 Chat, Group Chat, Online Status, Sent/Delivered/Read Receipts.
*   **Non-Functional:** Low Latency (Real-time), High Availability, Persistent History.

## 2. High Level Design
*   **Protocol:** HTTP is too slow (polling). Use **WebSockets** for bi-directional persistent connections.
*   **Backend:** Stateful Service (Chat Server) vs Stateless (API Server).

### Architecture Components
1.  **Load Balancer:** Directs user to a Chat Server.
2.  **Chat Server (WebSocket Handler):** Maintains active connections.
3.  **Service Discovery (Zookeeper/Redis):** Maps `UserID` -> `ChatServerIP`.
4.  **Pub/Sub (Redis/Kafka):** If User A is on Server 1 and User B is on Server 2, Server 1 publishes message to Server 2 via Pub/Sub.

## 3. Detailed Flows
### A. Sending a Message
1.  User A connects via WebSocket to **Chat Server 1**.
2.  User A sends "Hello".
3.  Chat Server 1 saves msg to **Cassandra** (Write-heavy, huge scale).
4.  Chat Server 1 checks **Redis Cache** to find where User B is connected.
    *   *Case 1:* User B is on Server 1 -> Push directly.
    *   *Case 2:* User B is on Server 2 -> Publish to **Redis Pub/Sub** channel `channel:user:B`. Server 2 subscribes and pushes.
    *   *Case 3:* User B is offline -> Push Notification Service.

### B. Storage Schema (Cassandra)
*   **Table:** `messages`
*   **Partition Key:** `chat_id` (1-on-1 or Group ID)
*   **Clustering Key:** `timestamp` (for ordering)
*   **Query:** `SELECT * FROM messages WHERE chat_id = X AND timestamp > Y ORDER BY timestamp DESC LIMIT 50`

## 4. Key Challenges
*   **Group Chat:** Fan-out issue. If group has 1000 users, 1 message = 1000 pushes. 
    *   *Solution:* Limit group size or use Hybrid approach (pull for large groups).
*   **Ordering:** Use Snowflake IDs (Time-sortable) to guarantee order across distributed servers.
