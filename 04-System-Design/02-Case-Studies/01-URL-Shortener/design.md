# Design a URL Shortener (TinyURL)

## 1. Requirements
*   **Functional:**
    *   Input: Long URL -> Output: Short URL (e.g., http://tiny.url/xyz123).
    *   Redirect Short URL -> Long URL.
*   **Non-Functional:**
    *   Highly Available (A over C).
    *   Low Latency (Read heavy).
    *   Scale: 100M new URLs/month.

## 2. Capacity Estimation
*   **Traffic:** 100M writes/month ~= 40 writes/sec. 
*   **Reads:** Assume 100:1 ratio -> 4000 reads/sec.
*   **Storage:** 500 bytes/URL. 100M * 500 = 50GB/month -> 3TB/5 years.

## 3. API Design
*   `POST /shorten(long_url) -> short_url`
*   `GET /{short_url} -> 301 Redirect to long_url`

## 4. Database Design
*   **Schema:** `id (PK)`, `short_code (Index)`, `long_url`, `created_at`.
*   **Choice:** NoSQL (Cassandra/DynamoDB) is better for scale, but RDBMS is fine given the simple schema.

## 5. Algorithm (Short Code Generation)
*   **Base62 Encoding:** [a-z, A-Z, 0-9] = 62 chars.
*   **Length:** $62^7 \approx 3.5$ Trillion combinations. 7 chars is enough.
*   **Generation:**
    *   *Approach 1 (Hash):* MD5(long_url) -> take first 7 chars. Collision risk.
    *   *Approach 2 (Counter):* Distributed ID Generator (Snowflake) -> Base62 Encode the ID. **(Preferred)**

## 6. Architecture
1.  **Client** hits **Load Balancer**.
2.  **App Server** checks **Cache** (Redis).
    *   Hit: Return Long URL.
    *   Miss: Query DB.
3.  **Write Path:**
    *   App Server gets Unique ID (KGS/Snowflake).
    *   Encodes to Base62.
    *   Saves to DB & Cache.
