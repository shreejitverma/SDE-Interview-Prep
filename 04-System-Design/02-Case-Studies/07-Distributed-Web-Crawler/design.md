---
type: case-study
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources:
  - "System Design Interview - An Insider's Guide, Alex Xu"
  - "Mercator: A scalable, extensible web crawler (Heydon & Najork)"
  - "Google Research: Crawling the Web (Brin & Page)"
---

# Design a Distributed Web Crawler (Googlebot)

## 1. TL;DR

A distributed web crawler systematically traverses the World Wide Web to discover, download, and index hyperlinked documents for search engines, web archiving, or large language model (LLM) dataset ingestion.
At commercial scale (e.g., Googlebot or Bingbot), the crawler processes upwards of 1 billion web pages per month, sustaining an ingestion throughput of approximately 400 pages per second and generating 100 terabytes of raw storage monthly.
The primary engineering challenges are enforcing strict **politeness** (never overloading target web servers or violating `robots.txt`), avoiding **crawler traps** (infinite dynamic URL generation loops), eliminating duplicate URLs and near-duplicate content (via **Bloom filters** and **SimHash**), bypassing DNS resolution bottlenecks, and scheduling crawls across priority tiers based on page importance (PageRank) and update frequency.
The industry standard architecture adapts the **Mercator Crawler model**: a two-stage URL Frontier (Priority Queues for freshness and Politeness Queues for domain rate-limiting) coordinating a distributed fleet of asynchronous download workers fronted by an in-memory DNS caching tier.

---

## 2. Mental Model

The crawler operates as a continuous, asynchronous feedback loop orchestrated by the URL Frontier.

```mermaid
flowchart TD
    SeedURLs["Seed URLs (Top Domains)"] --> Frontier["URL Frontier (Priority + Politeness Queues)"]

    subgraph DownloadPipeline["Asynchronous Download Pipeline"]
        Frontier -->|Polite Domain Stream| WorkerPool["Distributed Downloader Fleet"]
        WorkerPool -->|Resolve IP (Local Cache)| DNSCache["In-Memory DNS Resolver Tier"]
        WorkerPool -->|Check Host Rules| RobotsCache["Robots.txt Cache"]
        WorkerPool -->|HTTP GET Fetch| WebServers["External Web Servers (Target Sites)"]
        WebServers -->|Raw HTML Payload| WorkerPool
    end

    subgraph ExtractionDeduplication["Extraction & Deduplication Engine"]
        WorkerPool --> Parser["HTML Parser & Text Extractor"]
        Parser --> SimHashCheck{"Near-Duplicate Content? (SimHash)"}
        SimHashCheck -->|Duplicate| DropContent["Discard Page Content"]
        SimHashCheck -->|Unique| StorageTier[(Raw Document Store: S3 / HDFS / Bigtable)]
        
        Parser --> LinkExtractor["URL Extractor & Normalizer"]
        LinkExtractor --> BloomFilter{"URL Seen? (Bloom Filter)"}
        BloomFilter -->|Already Seen| DropURL["Discard URL"]
        BloomFilter -->|New URL| Frontier
    end
```

---

## 3. Architectural Internals and Deep Dive

### 3.1 The URL Frontier: Politeness vs. Priority (Mercator Model)

The URL Frontier is the central brain of the crawler, responsible for storing unvisited URLs and determining which URL to fetch next.
A naive First-In-First-Out (FIFO) queue causes severe operational disasters:
1. It downloads millions of URLs from a single host consecutively, effectively launching a Distributed Denial of Service (DDoS) attack against that website.
2. It treats high-priority authoritative news homepages identically to low-value spam forums.

The **Mercator URL Frontier** solves this by separating **Priority (Freshness)** from **Politeness (Host Concurrency)**:

```
+-----------------------------------------------------------------------+
|                    The Mercator URL Frontier                          |
+-----------------------------------------------------------------------+
| 1. Prioritizer: Ingests new URLs, computes PageRank/Freshness score   |
|         |                                                             |
|         v                                                             |
| [ F_1 (High) ]  [ F_2 (Med) ]  ...  [ F_K (Low) ] (Front Queues)      |
|         |                                                             |
|         v (Priority Selector: Biased Random Pick)                     |
|                                                                       |
| 2. Queue Router: Hashes domain name -> assigns to Back Queue          |
|         |                                                             |
|         v                                                             |
| [ B_1: cnn.com ] [ B_2: github.com ] ... [ B_M: wikipedia.org ]       |
|         |                                                             |
|         v                                                             |
| 3. Politeness Heap: Min-Heap ordered by 'ready_time'                  |
|    Enforces minimum delay (e.g., 1000ms) between requests to same host|
+-----------------------------------------------------------------------+
```

1. **Front Queues (Priority Tier)**:
   - $K$ priority queues ($F_1, F_2, ... F_K$).
   - A prioritizer scores URLs based on PageRank, domain authority, and historical update velocity, assigning them to the corresponding priority queue.
   - A selector pulls from queues with a probability proportional to priority (e.g., $F_1$ selected 70% of the time, $F_K$ selected 5%).
2. **Back Queues (Politeness Tier)**:
   - $M$ FIFO back queues ($B_1, B_2, ... B_M$), where each queue holds URLs belonging to a **single distinct hostname**.
   - A queue router maps the URL's domain name to a back queue via consistent hashing.
3. **Politeness Min-Heap**:
   - Maintains a min-heap of active back queues sorted by `ready_time`.
   - `ready_time = last_access_time + delay` (typically 1.0 second delay per host).
   - A worker thread pops the queue with the earliest `ready_time`, waits if `ready_time > now`, downloads the URL, and updates that queue's `ready_time` before re-inserting it into the min-heap.
   - **Guarantee**: At most one worker ever contacts any given host at any single instant.

### 3.2 DNS Resolution Bottleneck

Domain Name Resolution (DNS) is the single biggest bottleneck in high-throughput crawling:
- Standard OS-level DNS lookups (`gethostbyname`) are synchronous blocking calls taking between 20ms and 200ms over the network.
- Crawling 400 pages per second would require thousands of blocked worker threads waiting on DNS responses.
- **Solution**:
  - Deploy a dedicated, asynchronous, in-memory **DNS Resolver Cache** (e.g., custom unbound/bind clusters or local memory hash tables).
  - Pre-resolve hostnames when URLs enter back queues.
  - Cache DNS records locally in RAM with an aggressive TTL policy (e.g., 24 hours), refreshing records asynchronously in the background.

### 3.3 Deduplication: URL Filtering and Content Fingerprinting

The web contains massive URL redundancy and content duplication (mirrored sites, syndication, copied blog posts).

#### 1. URL Normalization and Seen Check (Bloom Filter)
Before enqueueing a URL:
- **Normalization**: Convert scheme and host to lowercase, remove default ports (`:80`), strip anchor fragments (`#section2`), resolve relative paths (`../`), and sort query string parameters alphabetically.
- **Seen Check via Bloom Filter**: An in-memory Bloom filter tracks all visited and enqueued URLs.
For 1 billion URLs with a 0.1% false-positive rate:
$$m = -\frac{n \ln p}{(\ln 2)^2} = -\frac{10^9 \times \ln(0.001)}{(\ln 2)^2} \approx 14.37 \text{ billion bits} \approx 1.79 \text{ GB of RAM}$$
1.79 GB easily fits in a single worker's memory, eliminating 99.9% of database lookups.
If the Bloom filter indicates the URL might exist, a secondary verification is performed against an on-disk key-value store (RocksDB or Cassandra).

#### 2. Near-Duplicate Content Detection via SimHash
Identical articles frequently exist across different URLs with minor variations (different banner ads, copyright dates, navigation menus).
Cryptographic hashes (MD5, SHA-256) are useless because changing a single character changes 50% of the hash bits (avalanche effect).
**SimHash (Locality-Sensitive Hashing)** produces a 64-bit fingerprint where the Hamming distance between two fingerprints is directly proportional to document text similarity:
1. Tokenize document text into weighted words.
2. Hash each word into an $N$-bit sequence.
3. Sum the weights across each bit position (+1 if bit is 1, -1 if bit is 0).
4. Set the final fingerprint bit to 1 if the sum is positive, 0 if negative.
5. If the **Hamming distance** (count of differing bits) between two 64-bit document fingerprints is $\le 3$, the documents are classified as near-duplicates, and the duplicate is dropped.

### 3.4 Handling Crawler Traps (Spider Traps)

A crawler trap is a set of web pages that causes a crawler to loop infinitely:
- Dynamic calendar pages linking to the next day forever (`/calendar/2026/10/07`, `/calendar/2026/10/08`...).
- Deeply nested recursive directory symlinks (`/about/about/about/about...`).
- E-commerce faceted navigation creating infinite permutation URLs (`/items?color=red&size=m&sort=asc&price=low...`).

#### Defenses Against Crawler Traps:
1. **URL Length Ceiling**: Reject any URL exceeding 255 characters.
2. **Path Segment Limit**: Reject URLs with more than 6 directory slashes or repeating directory tokens.
3. **Per-Host Crawl Quota**: Cap the total number of pages crawled from any single domain per crawl cycle (e.g., maximum 50,000 pages per domain).
4. **Heuristic Trap Detection**: Flag domains where the ratio of newly discovered URLs to unique extracted text content exceeds a predefined threshold.

### 3.5 Robots.txt Protocol Parsing

Websites publish access guidelines via `/robots.txt`:
```
User-agent: Googlebot
Disallow: /admin/
Disallow: /private/
Crawl-delay: 2
```
- The crawler fetches and parses `/robots.txt` before issuing requests to any host.
- Rules are parsed using an RFC-compliant parser and cached in an in-memory Redis cluster with a 24-hour TTL.
- If `/robots.txt` returns `HTTP 404 Not Found`, the crawler proceeds with full site access.
- If `/robots.txt` returns `HTTP 5xx Server Error`, the crawler aborts crawling the site until the error resolves.

---

## 4. Trade-offs and Comparisons

| Dimension | Breadth-First Search (BFS) | Depth-First Search (DFS) | PageRank-Biased Priority Crawl |
|---|---|---|---|
| Traversal Strategy | Explores all neighbor links first | Dives deep into single site branch | Pulls from highest-authority links first |
| Page Freshness | Poor for important sites | Very poor | Optimal (authoritative sites refreshed quickly) |
| Memory Overhead | Extremely high (wide queue) | Low (stack-based) | Moderate (priority queue management) |
| Crawler Trap Vulnerability | Moderate | Severe (instantly trapped) | Low (traps possess low PageRank scores) |
| Industry Adoption | Academic baseline | Anti-pattern for web | Universal standard in production search engines |

---

## 5. Failure Modes and Mitigations

### 5.1 Target Web Server Throttling and IP Bans
- **Failure Mode**: Downloader threads hammer a fragile web server, triggering Cloudflare or Akamai rate limits that ban the crawler's IP blocks.
- **Mitigation**: Strictly respect the Politeness Min-Heap delay ($\ge 1\text{s}$ per host).
Honor the `Crawl-delay` directive in `robots.txt`.
If a target returns `HTTP 429 Too Many Requests` or `HTTP 503`, dynamically increase that host's back queue delay by $4\times$.

### 5.2 Malicious Content / Decompression Bomb (Zip Bomb)
- **Failure Mode**: A web server serves a tiny 10 KB gzip file that decompresses into 10 gigabytes of null bytes, crashing downloader workers with out-of-memory errors.
- **Mitigation**: Enforce streaming decompression with hard size ceilings.
If uncompressed HTML payload exceeds 10 MB, immediately terminate the connection and discard the stream.

### 5.3 DNS Resolver Failover and Cache Stampede
- **Failure Mode**: The central DNS cache crashes, causing millions of worker threads to query public DNS root servers directly, causing massive request timeouts.
- **Mitigation**: Deploy a multi-tier local caching layer: workers maintain an L1 in-process DNS cache (5-minute TTL) backed by an L2 distributed caching tier (e.g., CoreDNS/Unbound cluster).

---

## 6. Hands-On Verification

The following standalone Python implementation demonstrates a production-grade URL Frontier with host politeness delays, robots.txt parsing, Bloom filter URL deduplication, and SimHash text fingerprinting.

```python
#!/usr/bin/env python3
"""
Production-grade demonstration of Distributed Web Crawler components:
- Mercator URL Frontier (Politeness Min-Heap)
- In-memory Bloom Filter for URL seen check
- SimHash content near-duplicate detection
- Normalized URL link extractor
"""

import math
import time
import heapq
import hashlib
from typing import Dict, List, Set, Tuple, Optional
from urllib.parse import urlparse, urljoin


class SimpleBloomFilter:
    """In-memory Bloom filter for URL seen checks."""
    def __init__(self, capacity: int = 100000, error_rate: float = 0.001):
        self.capacity = capacity
        self.error_rate = error_rate
        self.bit_size = int(-(capacity * math.log(error_rate)) / (math.log(2) ** 2))
        self.hash_count = int((self.bit_size / capacity) * math.log(2))
        self.bit_array = [False] * self.bit_size

    def _get_hashes(self, item: str) -> List[int]:
        hashes = []
        for seed in range(self.hash_count):
            h = int(hashlib.md5(f"{seed}:{item}".encode()).hexdigest(), 16)
            hashes.append(h % self.bit_size)
        return hashes

    def add(self, item: str):
        for h in self._get_hashes(item):
            self.bit_array[h] = True

    def contains(self, item: str) -> bool:
        return all(self.bit_array[h] for h in self._get_hashes(item))


def compute_simhash(text: str, hash_bits: int = 64) -> int:
    """Computes a 64-bit SimHash fingerprint for document near-duplicate detection."""
    tokens = text.lower().split()
    if not tokens:
        return 0
    v = [0] * hash_bits
    for token in tokens:
        token_hash = int(hashlib.md5(token.encode()).hexdigest(), 16)
        for i in range(hash_bits):
            bit = (token_hash >> i) & 1
            v[i] += 1 if bit == 1 else -1

    fingerprint = 0
    for i in range(hash_bits):
        if v[i] > 0:
            fingerprint |= (1 << i)
    return fingerprint


def hamming_distance(h1: int, h2: int) -> int:
    """Computes Hamming distance between two 64-bit fingerprints."""
    x = h1 ^ h2
    dist = 0
    while x > 0:
        dist += x & 1
        x >>= 1
    return dist


class PoliteURLFrontier:
    """Mercator-style Politeness Frontier enforcing per-host delays."""
    def __init__(self, politeness_delay_sec: float = 0.5):
        self.delay = politeness_delay_sec
        self.seen_filter = SimpleBloomFilter()
        # host -> list of URLs (Back Queues)
        self.host_queues: Dict[str, List[str]] = {}
        # Min-Heap of (ready_time, host)
        self.politeness_heap: List[Tuple[float, str]] = []
        self.active_hosts_in_heap: Set[str] = set()

    def enqueue_url(self, url: str) -> bool:
        parsed = urlparse(url)
        host = parsed.netloc.lower()
        if not host:
            return False

        if self.seen_filter.contains(url):
            return False  # Already seen
        self.seen_filter.add(url)

        self.host_queues.setdefault(host, []).append(url)
        if host not in self.active_hosts_in_heap:
            heapq.heappush(self.politeness_heap, (time.time(), host))
            self.active_hosts_in_heap.add(host)
        return True

    def get_next_url(self) -> Optional[str]:
        if not self.politeness_heap:
            return None

        ready_time, host = heapq.heappop(self.politeness_heap)
        now = time.time()
        if ready_time > now:
            time.sleep(ready_time - now)

        queue = self.host_queues.get(host, [])
        if not queue:
            self.active_hosts_in_heap.discard(host)
            return self.get_next_url()

        url = queue.pop(0)

        # Reschedule host queue with politeness delay
        if queue:
            next_ready = time.time() + self.delay
            heapq.heappush(self.politeness_heap, (next_ready, host))
        else:
            self.active_hosts_in_heap.discard(host)
        return url


if __name__ == "__main__":
    frontier = PoliteURLFrontier(politeness_delay_sec=0.2)

    print("--- 1. Enqueueing URLs Across Multiple Hosts ---")
    frontier.enqueue_url("https://example.com/page1")
    frontier.enqueue_url("https://example.com/page2")
    frontier.enqueue_url("https://github.com/explore")
    frontier.enqueue_url("https://example.com/page1")  # Duplicate: should drop

    print("--- 2. Fetching URLs with Enforced Host Politeness ---")
    start = time.time()
    crawled = []
    while True:
        url = frontier.get_next_url()
        if not url:
            break
        crawled.append(url)
        print(f"[{time.time()-start:.3f}s] Dequeued: {url}")

    assert len(crawled) == 3, f"Expected 3 unique URLs, got {len(crawled)}"

    print("\n--- 3. Testing SimHash Near-Duplicate Detection ---")
    doc_original = "The quick brown fox jumps over the lazy dog in downtown Seattle."
    doc_mirror = "The quick brown fox jumps over the lazy dog in downtown Seattle! Copyright 2026."
    doc_unrelated = "Distributed systems require consensus algorithms like Raft and Paxos."

    h_orig = compute_simhash(doc_original)
    h_mirror = compute_simhash(doc_mirror)
    h_unrelated = compute_simhash(doc_unrelated)

    dist_near = hamming_distance(h_orig, h_mirror)
    dist_diff = hamming_distance(h_orig, h_unrelated)
    print(f"Hamming Distance (Original vs. Mirror):    {dist_near} bits (Near-Duplicate <= 3)")
    print(f"Hamming Distance (Original vs. Unrelated): {dist_diff} bits")

    assert dist_near <= 3, "Near-duplicate documents should have Hamming distance <= 3!"
    assert dist_diff > 15, "Unrelated documents should have high Hamming distance!"
    print("\nVerification Passed: URL Frontier and SimHash operate correctly!")
```

### CLI Verification

Execute crawler assertions across environments:

```bash
# Linux / macOS: Verify robots.txt compliance via curl
curl -s -L https://www.google.com/robots.txt | head -n 20

# Linux / macOS: Benchmark DNS lookup latency using dig
dig +noall +stats https://www.wikipedia.org

# Windows PowerShell: Inspect Web Headers and Content Encoding
$res = Invoke-WebRequest -Uri "https://www.example.com" -Method Head
$res.Headers
```

---

## 7. Performance Characteristics and Capacity Planning

### 7.1 Throughput and Bandwidth Calculations
- **Target Ingestion**: 1 billion pages per month.
- **Average Throughput**:
  $$\text{QPS} = \frac{1,000,000,000}{30 \times 86,400} \approx 385.8 \text{ pages/sec}$$
- **Peak Throughput ($2.5\times$ peak)**: $\approx 965 \text{ pages/sec}$.
- **Network Bandwidth**:
  - Assume average downloaded web page size = 100 KB (HTML + text).
  - Sustained Bandwidth = $386 \text{ pages/sec} \times 100 \text{ KB} = 38.6 \text{ MB/sec} = 308.8 \text{ Mbps}$.
  - Peak Bandwidth = $965 \text{ pages/sec} \times 100 \text{ KB} \approx 96.5 \text{ MB/sec} = 772 \text{ Mbps}$.

### 7.2 Storage Calculations
- Monthly raw storage:
  $$1,000,000,000 \times 100 \text{ KB} = 100 \text{ TB/month}$$
- With gzip/zstd text compression ($3\times$ compression factor):
  $$\frac{100 \text{ TB}}{3} \approx 33.3 \text{ TB/month}$$
- 5-Year Storage Capacity:
  $$33.3 \text{ TB} \times 60 \text{ months} \approx 2.0 \text{ PB}$$
- Raw HTML is archived into object storage (Amazon S3 Standard-IA / Glacier) or Apache HDFS, while parsed text and posting lists are stored in distributed Bigtable / HBase instances.

---

## 8. In Production: Real-World Architecture (Googlebot)

Google's crawler architecture evolved from early academic prototypes into a massive global pipeline:
1. **Mercator Lineage**: The dual-queue priority/politeness structure remains the foundational standard for commercial search engines.
2. **Headless Chrome Rendering Tier**: Modern web pages rely heavily on client-side JavaScript rendering (React, Vue, Angular).
Googlebot maintains a secondary rendering queue that executes HTML inside headless Chromium instances (WRS - Web Rendering Service) to discover dynamically injected links and content.
3. **Crawl Budget Optimization**: Google dynamically computes a "crawl budget" for every registered domain, calculated as a function of the host's server response speed (crawl health) and the site's overall search demand (popularity).

---

## 9. Interview Questions and Deep Dives

> [!question] Question 1: How does the URL Frontier enforce politeness without starving high-priority pages?
> [!success]- Answer
> By using the two-tier Mercator architecture:
> Front Queues manage **Priority**, ranking URLs by importance (PageRank/freshness) and selecting them probabilistically.
> Back Queues manage **Politeness**, grouping URLs strictly by domain name (one queue per host).
> A Politeness Min-Heap schedules back queues by `ready_time = last_access + delay`.
> This guarantees that high-priority pages are chosen first from the front queues, but once assigned to a domain back queue, they must wait for that host's scheduled politeness slot, ensuring no single server is ever hit faster than once per delay interval.

> [!question] Question 2: Why are standard cryptographic hashes (SHA-256) useless for detecting near-duplicate content?
> [!success]- Answer
> Cryptographic hashes exhibit the **avalanche effect**: changing a single whitespace character or date string completely randomizes the output hash, making the Hamming distance between the two hashes ~50% of the bits.
> Near-duplicate detection requires **Locality-Sensitive Hashing (LSH)**, such as **SimHash**.
> In SimHash, similar text documents generate fingerprints that differ by only a few bits.
> Comparing the Hamming distance ($\le 3$ bits) reliably flags near-duplicate articles even if headers, dates, or navigation bars differ.

> [!question] Question 3: How does a crawler identify and avoid falling into crawler traps (spider traps)?
> [!success]- Answer
> Crawler traps generate infinite dynamic URLs (e.g., calendar loops `/2026/10/08`, recursive directory structures `/a/b/a/b`).
> Mitigations:
> 1. Impose a hard URL character length cap (e.g., 255 characters).
> 2. Limit maximum path segment depth (e.g., maximum 6 `/` segments).
> 3. Detect repeating directory substrings using regular expressions.
> 4. Cap the maximum number of pages crawled per domain per crawl cycle.
> 5. Monitor content similarity: if a site yields hundreds of URLs that produce near-identical SimHash fingerprints, abort crawling that subtree.

> [!question] Question 4: How does a distributed crawler handle DNS resolution without bottlenecking downloaders?
> [!success]- Answer
> Synchronous OS DNS lookups block worker threads for up to 200ms.
> The system decouples DNS resolution via:
> 1. An in-memory asynchronous DNS resolver service caching records locally in RAM.
> 2. Pre-resolving domain names in batches when URLs are assigned to back queues, before the worker thread initiates the HTTP connection.
> 3. Enforcing an aggressive DNS cache TTL (e.g., 24 hours), resolving records in the background.

> [!question] Question 5: Why is a Bloom filter preferred over a hash table for tracking visited URLs?
> [!success]- Answer
> Storing 1 billion raw URLs (average 100 bytes each) in a hash table requires over **100 GB of RAM**, exceeding single-machine memory budgets and requiring distributed cache coordination.
> A Bloom filter tracks 1 billion URLs with a 0.1% false-positive rate using only **1.79 GB of RAM**, fitting entirely in local memory.
> While a Bloom filter may occasionally produce a false positive (falsely skipping 1 in 1,000 unvisited URLs), it **never produces false negatives**, ensuring the crawler never revisits already-crawled URLs.

> [!question] Question 6: What should a crawler do when encountering an `HTTP 429 Too Many Requests` or `HTTP 503 Service Unavailable` response?
> [!success]- Answer
> Treat the response as an urgent signal of host strain:
> 1. Immediately back off that specific host queue by multiplying its politeness delay exponentially (e.g., increasing delay from 1s to 4s, then 16s).
> 2. Check the response for a `Retry-After` header and set the queue's `ready_time` accordingly.
> 3. Re-enqueue the failed URL into the back queue with a retry counter (aborting after 3 failed attempts).

> [!question] Question 7: How do modern crawlers handle Single-Page Applications (SPAs) built with React or Angular?
> [!success]- Answer
> Traditional HTTP crawlers download raw HTML and see only empty `<div id="root"></div>` shells without content or links.
> Modern search engines maintain a two-stage crawling pipeline:
> 1. Fast Path: Raw HTML is downloaded and indexed immediately for standard static websites.
> 2. Render Queue: JavaScript-heavy pages are enqueued into a headless browser rendering farm (running headless Chromium/Playwright).
> The headless browser executes JavaScript, renders the Document Object Model (DOM), and feeds the post-rendered HTML back into the link extraction and indexing engine.

> [!question] Question 8: How do you partition the URL Frontier across multiple physical crawler machines?
> [!success]- Answer
> Shard the Frontier using **consistent hashing on the URL's hostname**:
> `worker_node = hash(hostname) % NUM_WORKERS`
> This guarantees that all URLs belonging to a specific domain (e.g., `wikipedia.org`) are always assigned to the same crawler node.
> This simplifies politeness tracking, because a single node manages that host's back queue and rate limits without requiring cross-network distributed locks.

> [!question] Question 9: What is the recrawl and freshness scheduling policy for existing web pages?
> [!success]- Answer
> Web pages update at drastically different intervals: news homepages change every minute, while academic papers never change.
> The crawler models page update frequency using a Poisson process:
> High-velocity pages (tracked via previous HTTP `Last-Modified` and `ETag` headers) are scheduled for frequent recrawling (e.g., every 30 minutes).
> Static pages are scheduled for infrequent recrawling (e.g., every 30 days).

> [!question] Question 10: How do you store and index 100 terabytes of crawled HTML per month?
> [!success]- Answer
> Raw HTML documents are compressed with zstd and appended sequentially into large archive containers (such as WARC - Web ARPath format or Hadoop SequenceFiles) stored in Amazon S3 or HDFS.
> Storing millions of individual tiny 100 KB files exhausts filesystem inode tables; bundling documents into 1 GB archive chunks optimizes I/O throughput.
> Metadata and extracted text tokens are indexed in distributed column-family datastores (Google Bigtable or Apache HBase).

---

## 10. Related Concepts and Wikilinks

- [[Consistent-Hashing]]: Sharding host queues across distributed crawler nodes.
- [[MapReduce-Architecture]]: Batch link inversion, PageRank computation, and search index creation.
- [[Hadoop-and-HDFS]]: Distributed file systems for petabyte-scale raw HTML archiving.
- [[Amazon-S3-and-Object-Storage]]: Long-term storage tier for compressed WARC document archives.
- [[Load-Balancing]]: Egress proxy pooling and IP rotation.

---

## 11. Further Reading

- Heydon, Allan, and Marc A. Najork. *Mercator: A scalable, extensible web crawler*. World Wide Web 2.4 (1999): 219-229.
- Brin, Sergey, and Lawrence Page. *The Anatomy of a Large-Scale Hypertextual Web Search Engine*. Computer Networks (1998).
- Charikar, Moses S. *Similarity Estimation Techniques from Rounding Algorithms (SimHash)*. STOC 2002.
- The Internet Archive: *WARC (Web ARChive) File Format Specification (ISO 28500)*.
