---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lesson: L09
tags: [cs6210, cs6210/practice]
---

# Practice L09

Original exam-style questions for [L09a](../Part-5-Internet-Scale-Real-Time-and-Security/L09a-Giant-Scale-Services.md), [L09b](../Part-5-Internet-Scale-Real-Time-and-Security/L09b-MapReduce.md), [L09c](../Part-5-Internet-Scale-Real-Time-and-Security/L09c-Content-Delivery-Networks.md).
Each question names the coverage ids it exercises; answers are folded so this page works as a self-test.

> [!question]- Q1. Explain the generic service model of giant scale services and why clusters are preferred as workhorses over traditional large symmetric multiprocessors (SMPs). (concepts: L09a-01, L09a-02, P-Giant-Scale-Services)
> The generic service model involves a load manager distributing incoming internet requests across a massive pool of commodity nodes.
> Clusters of commodity PCs are preferred because they offer a better price-to-performance ratio and are inherently scalable.
> Unlike SMPs, where adding CPUs hits memory bandwidth bottlenecks and scaling cost is non-linear, a cluster can scale out linearly by adding more independent commodity machines.

> [!question]- Q2. Compare and contrast load management using round-robin DNS, layer 4 switches, and layer 7 switches. (concepts: L09a-03, L09a-04)
> Round-robin DNS distributes load at the name resolution level by rotating IP addresses, but it cannot account for node health or handle DNS caching issues effectively.
> Layer 4 routing makes routing decisions based on IP addresses and TCP port numbers, providing fast, hardware-level load balancing without inspecting the payload.
> Layer 7 routing inspects the application payload (such as HTTP URLs) to make fine-grained routing decisions, allowing requests to be sent to specialized servers (like image or dynamic content servers) but incurring a higher processing overhead.

> [!question]- Q3. Define the DQ principle and explain how it relates to graceful degradation in the context of uptime, yield, and harvest. (concepts: L09a-05, L09a-06, L09a-08, P-Web-Search-for-a-Planet)
> The DQ principle states that the product of queries per second (Data/Query rate) and data size (Q) is a constant capacity constraint for a given hardware base.
> Uptime is the fraction of time a service is responding, yield is the fraction of answered queries, and harvest is the fraction of the database reflected in a response.
> When a system approaches overload or experiences partial failure, it can gracefully degrade by dropping harvest (returning results from the available subset of the database) or dropping yield (refusing some queries entirely) to maintain uptime.

> [!question]- Q4. How does replication differ from partitioning in scaling giant scale services, and what techniques enable their online evolution? (concepts: L09a-07, L09a-09)
> Replication involves copying the entire dataset across multiple nodes to improve read throughput and fault tolerance, while partitioning divides the dataset across nodes so each node serves a disjoint subset, effectively scaling storage and write capacity.
> To evolve such systems without taking them offline, engineers use techniques like fast reboots, rolling upgrades (updating nodes sequentially), and the big flip (switching traffic between two complete, distinct cluster versions).

> [!question]- Q5. How does the Google cluster architecture leverage commodity hardware for web search, and how does this contrast with the broader vision of standardized Web Services? (concepts: L09a-10, P-Web-Services-SOAP-WSDL-UDDI, P-Next-Step-in-Web-Services)
> The Google cluster architecture scales web search by distributing the inverted index across thousands of commodity Linux machines, relying on redundant software layers to tolerate frequent hardware failures instead of using expensive reliable hardware.
> This highly specialized, proprietary internal architecture focuses exclusively on performance and fault tolerance for search.
> In contrast, the vision for standardized Web Services relies on interoperable protocols like SOAP, WSDL, and UDDI to allow loosely coupled, diverse applications to communicate over the web, prioritizing standardized external interfaces over specialized internal optimizations.

> [!question]- Q6. Describe the MapReduce programming model for big data applications and provide a simple example using the Map and Reduce functions. (concepts: L09b-01, L09b-02, L09b-03, P-MapReduce)
> The MapReduce programming model abstracts parallel computation by dividing the workload into a Map phase that processes input data into intermediate key-value pairs, and a Reduce phase that merges all values associated with the same intermediate key.
> In a big data application like a word counter, the Map function takes a document and emits a key-value pair of the word and the number one for each word encountered.
> The Reduce function then takes a word and an iterator over its counts, summing them to emit the total count for that word.

> [!question]- Q7. How does the MapReduce runtime handle the execution overview, fault tolerance, and what master data structures are used to track progress? (concepts: L09b-04, L09b-05, L09b-06)
> The runtime heavy lifting handles input partitioning, scheduling tasks across workers, and managing inter-machine communication.
> The master maintains data structures recording the state (idle, in-progress, or completed) and location of each map and reduce task.
> If a worker fails, the master detects it via missed heartbeats and handles fault tolerance by re-executing any in-progress tasks or completed map tasks (since their local output is lost) on other available workers.

> [!question]- Q8. Explain how MapReduce optimizes performance through locality, task granularity, backup tasks for stragglers, combiners, and ordering guarantees. (concepts: L09b-07, L09b-08, L09b-09)
> The master scheduler optimizes locality by assigning map tasks to machines that already store the corresponding input data chunks, minimizing network congestion.
> Fine task granularity ensures better load balancing and faster recovery upon failures.
> To mitigate stragglers (slow-performing machines that delay overall completion), the master schedules backup executions of the remaining in-progress tasks near the end of the job and accepts the first result to finish.
> A combiner function can run locally on a mapper to perform partial reduction of data before it is sent over the network, reducing bandwidth requirements.
> Partitioning handles output distribution across reducers, and ordering guarantees ensure that within a given partition, intermediate key-value pairs are processed in strictly increasing key order.

> [!question]- Q9. How do distributed hash tables function as an overlay network for CDNs, and what are the mechanics of the key and node ID space in traditional greedy key-based routing? (concepts: L09c-01, L09c-02, L09c-03, L09c-04, P-Coral)
> A distributed hash table operates as a peer-to-peer overlay network providing a decentralized lookup service that maps keys to nodes for a content delivery network.
> Both data keys and node identifiers are hashed into the same logical identifier space.
> In traditional greedy key-based routing, a node forwards a lookup request to a neighbor whose ID is numerically closer to the target key, converging on the node responsible for the key efficiently.

> [!question]- Q10. What is tree saturation in a CDN, and how does Coral use a sloppy DHT and distance halving to mitigate the metadata server overload problem during put and get operations? (concepts: L09c-05, L09c-06, L09c-07, L09c-08)
> Tree saturation occurs when a highly popular object causes the node holding its metadata to be overwhelmed by concurrent requests, creating a hotspot.
> Coral resolves this with a sloppy DHT, which trades strict consistency for performance by allowing data to be cached at any node near the requester.
> Coral employs distance halving in its key-based routing, bouncing requests through a hierarchical set of clusters based on network latency.
> Consequently, put and get operations frequently terminate early at a nearby node that already holds a pointer to the data, shielding the root node from overload.

> [!question]- Q11. Explain how Dynamo ensures high availability and incremental scalability using consistent hashing with virtual nodes, quorums, vector clocks, and hinted handoff. (concepts: L09c-09, L09c-10, P-Dynamo)
> Dynamo uses consistent hashing with virtual nodes to evenly distribute data across a ring, allowing physical nodes to join or leave with minimal data movement.
> It employs a quorum-like system where reads and writes only need acknowledgement from a subset of replicas to ensure durability without sacrificing availability.
> Vector clocks are used to capture causality between different versions of data, enabling the system to resolve or expose concurrent updates.
> Hinted handoff provides resilience during temporary node failures by writing data to an alternate healthy node with a hint about its true destination, ensuring the write succeeds and is delivered once the original node recovers.
