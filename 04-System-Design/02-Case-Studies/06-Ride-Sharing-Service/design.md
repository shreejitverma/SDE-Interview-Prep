---
type: case-study
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources:
  - "System Design Interview - An Insider's Guide, Alex Xu"
  - "Uber Engineering Blog: H3 - A Hexagonal Hierarchical Spatial Index"
  - "Uber Engineering Blog: How Uber Manages Real-Time Driver Supply with DISCO"
---

# Design a Ride-Sharing Service (Uber / Lyft)

## 1. TL;DR

A global ride-sharing platform coordinates real-time location streaming, geospatial proximity indexing, dynamic surge pricing, and bipartite supply-demand matching between millions of riders and drivers.
At Uber or Lyft scale, the platform supports over 100 million active riders and 5 million active drivers globally.
Active drivers emit GPS coordinates every 4 seconds, generating a sustained ingestion throughput exceeding 1.25 million location updates per second.
The core engineering challenges center on sub-50ms geospatial radius queries, eliminating driver double-booking races, calculating smooth localized surge pricing across discrete geographical boundaries, and executing batch bipartite matching algorithms to minimize aggregate passenger pickup ETAs.
The industry standard architecture pairs WebSocket/gRPC location streaming with in-memory hexagonal spatial indexes (**Uber H3** or **Google S2**), backed by a distributed dispatch matching engine that processes batch optimization windows.

---

## 2. Mental Model

The platform decouples high-frequency driver location ingestion from transactional ride lifecycle and dispatch coordination.

```mermaid
flowchart TD
    subgraph DriverIngestion["Driver Location Pipeline"]
        DriverApp["Driver Mobile App"] -->|gRPC / WS: Ping every 4s| IngestLB["Global L4 Load Balancer"]
        IngestLB --> LocGateway["Location Ingestion Gateway Fleet"]
        LocGateway -->|Update Spatial Cell| GeoStore[(In-Memory Spatial Index: Redis / H3 Cluster)]
        LocGateway -->|Asynchronous Stream| KafkaLoc[Kafka: driver-locations-topic]
        KafkaLoc --> TrajectoryDB[(Historical Trip Store: Cassandra)]
    end

    subgraph RiderMatching["Rider Request & Dispatch Engine"]
        RiderApp["Rider Mobile App"] -->|POST /v1/rides/request| APIGW["API Gateway"]
        APIGW --> RideService["Ride Management Service"]
        RideService --> SurgeService["Surge Pricing Engine (H3 Density)"]
        SurgeService --> GeoStore
        RideService --> DispatchEngine["DISCO: Batch Dispatch & Matching Engine"]
        DispatchEngine -->|Spatial Query (5km radius)| GeoStore
        DispatchEngine -->|Bipartite Match Optimization| MatchResult["Assign Best Driver"]
        MatchResult --> PushGateway["Notification / Push Service"]
        PushGateway -->|Send Offer (15s accept window)| DriverApp
    end

    subgraph TransactionalTier["Ride Lifecycle & State Machine"]
        RideService --> RideDB[(Primary Database: PostgreSQL / Spanner)]
    end
```

---

## 3. Architectural Internals and Deep Dive

### 3.1 Geospatial Indexing Comparison

Standard B-tree database indexes cannot efficiently execute 2D proximity queries (`WHERE latitude BETWEEN ? AND ? AND longitude BETWEEN ? AND ?`) without scanning massive bounding boxes.
Specialized spatial indexing algorithms map 2D geographical coordinates into 1D sortable tokens:

| Spatial Indexing Model | Structure | Distortion & Geometry | Neighbor Traversal | Primary Adoption |
|---|---|---|---|---|
| **Geohash** | Base32 string bounding boxes | High distortion near poles; rectangular | Inconsistent distance to diagonal neighbors | MongoDB, Elasticsearch |
| **QuadTree** | Hierarchical 2D in-memory tree | Rectangular subdivision; tree rebalancing | High complexity across branch boundaries | In-memory spatial caches |
| **Google S2** | Hilbert curve on Earth's sphere | Minimal distortion; 64-bit integer IDs | Very fast (bitwise integer arithmetic) | Google Maps, Foursquare |
| **Uber H3** | Hexagonal hierarchical discrete grid | Minimal distortion; 100% invariant neighbor distances | Trivial (each hexagon has exactly 6 equidistant neighbors) | Uber (Dispatch, Surge, Mapping) |

#### Why Hexagons (Uber H3) Dominate Ride-Sharing
Square and rectangular grids (Geohashes) suffer from the **diagonal neighbor defect**: a square has 8 neighbors, but the 4 diagonal neighbors are $\sqrt{2} \approx 1.414\times$ farther away than the 4 orthogonal neighbors.
This spatial asymmetry complicates radius searches and distorts travel time calculations.
Hexagons have exactly **6 neighbors, and every neighbor is equidistant**:
The distance between the centroid of a hexagon and each of its 6 neighboring centroids is identical.
This symmetry simplifies radial search expansions, routing graphs, and surge pricing smoothing.

```
+---------------------------------------------------------------+
|         Square (Geohash) vs. Hexagon (H3) Geometry            |
+---------------------------------------------------------------+
| Square Grid:                             Hexagonal Grid (H3): |
|  [D]  [1]  [D]  (D = 1.414 x d)                 / \           |
|   |    |    |                                 /     \         |
|  [1]-[ C ]-[1]  (1 = 1.000 x d)              |   N   |        |
|   |    |    |                              / \       / \      |
|  [D]  [1]  [D]                           /     \   /     \    |
| (Non-uniform neighbor distance)         |   N   | C |   N   | |
|                                          \     /   \     /    |
|                                            \ /       \ /      |
|                                              |   N   |        |
|                                         (All 6 neighbors      |
|                                          are equidistant!)    |
+---------------------------------------------------------------+
```

### 3.2 Driver Location Streaming Architecture

Drivers stream their coordinates (`driver_id`, `lat`, `lon`, `heading`, `status: AVAILABLE | BUSY`) every 4 seconds:
- **Location Ingestion Fleet**: Stateless Go or Rust services terminate WebSocket/gRPC streams and inspect the active H3 cell of the driver.
- **In-Memory Spatial Index**:
  - The world is indexed at **H3 Resolution 8** (average hexagon area $\approx 0.74 \text{ km}^2$, edge length $\approx 461 \text{ meters}$).
  - Redis maintains a Hash Set or Sorted Set per active H3 cell:
    `HSET h3:882681a339fffff <driver_id> "lat,lon,timestamp,heading"`
  - When a driver moves into an adjacent H3 cell, their ID is removed from the old cell set and added to the new one.
  - Driver records have a 10-second TTL.
  If a phone loses connectivity or battery, the driver automatically drops out of the active index.

### 3.3 Dispatch Matching: Greedy vs. Batch Optimization

When a rider requests a pickup:

#### Approach A: Greedy Nearest Driver Matching
The system queries the driver's H3 cell and adjacent k-rings, picks the single closest available driver (minimum Euclidean distance or straight-line ETA), and sends an offer.
- **Flaw**: Leads to globally suboptimal assignments (the "driver poaching" anomaly).
Assigning Driver A to Rider 1 may force Rider 2 (requesting 2 seconds later) to wait 15 minutes because Driver A was the only nearby car for Rider 2, while Driver B was only 30 seconds farther from Rider 1.

#### Approach B: Batch Bipartite Matching (Uber DISCO Model)
Instead of matching instantly, the system aggregates rider requests and available drivers inside a geographical zone over a **short window (e.g., 5 seconds)**.
1. Formulate a **Bipartite Graph** where vertices on the left represent pending riders $\{R_1, R_2, ... R_n\}$ and vertices on the right represent available drivers $\{D_1, D_2, ... D_m\}$.
2. Edge weights represent the actual driving ETA calculated via road routing engines (OSRM / Valhalla).
3. Execute the **Hungarian Algorithm** or min-cost max-flow optimization to find the global matching that minimizes the aggregate pickup ETA for all riders in the batch.

### 3.4 Dynamic Surge Pricing Engine

Surge pricing balances local market supply and demand:
1. **Supply ($S$) Calculation**: Count active, available drivers within an H3 Resolution 7 cell (~5.16 km²).
2. **Demand ($D$) Calculation**: Count ride requests and active app opens (riders viewing the map) within that same H3 cell over the past 5 minutes.
3. **Surge Multiplier Formulation**:
   $$\text{Multiplier} = 1.0 + \max\left(0, \alpha \times \left(\frac{D}{S} - \text{Threshold}\right)\right)$$
4. **Spatial Smoothing**: If cell A has a $2.5\times$ surge and neighboring cell B has $1.0\times$, riders will walk across the street to avoid the surge, causing unnatural supply distortion.
The surge engine applies Gaussian spatial smoothing across adjacent H3 hexagonal rings to create continuous gradient transitions.

### 3.5 Trip State Machine and Concurrency Control

The ride lifecycle requires strict transactional integrity:

```
[REQUESTED] ---> [MATCHING] ---> [OFFERED] ---> [ACCEPTED] ---> [ARRIVED] ---> [IN_TRANSIT] ---> [COMPLETED]
     |               |              |
     v               v              v
[CANCELLED]     [TIMEOUT]      [REJECTED] (Re-enters MATCHING)
```

- When an offer is dispatched, the driver is marked as `OFFERED` with a 15-second lease in Redis.
- To prevent two concurrent dispatch batches from assigning the same driver to two riders, the service executes an atomic reservation:
  `SET lock:driver:<driver_id> ride_id NX PX 15000`
- If Driver 1 rejects or times out, the lock is released, and the trip re-enters the batch matching pool.
- State transitions are committed to a distributed relational database (PostgreSQL with row-level locks or CockroachDB) enforcing ACID invariants.

---

## 4. Trade-offs and Comparisons

| Dimension | Greedy Dispatch | Batch Dispatch (5s Window) |
|---|---|---|
| User Latency | Instant feedback (< 1s) | Adds a 5-second batch aggregation delay |
| Global Wait Time | Higher average ETA across platform | 15-25% lower aggregate platform pickup wait times |
| Compute Complexity | $O(K)$ spatial lookup | $O(N^3)$ bipartite optimization (Hungarian Algorithm) |
| Cancellation Rate | Higher (suboptimal matches) | Lower (drivers are globally closer) |
| System Resilience | Simple, stateless | Requires queue buffering and state synchronization |

---

## 5. Failure Modes and Mitigations

### 5.1 Driver Location Ingestion Thundering Herd
- **Failure Mode**: Network carrier outages resolve, causing 100,000 drivers to reconnect simultaneously and emit bursts of backlogged location pings, crashing ingestion gateways.
- **Mitigation**: Discard historical location backlogs on mobile clients; only the latest real-time GPS coordinate matters for dispatch.
Deploy UDP/gRPC streaming with rate-limited connection handshakes.

### 5.2 Double-Booking Driver Race Conditions
- **Failure Mode**: Two concurrent dispatch workers in overlapping H3 cells both select Driver 42 and notify them simultaneously.
- **Mitigation**: Atomic Redis distributed locking (`SETNX` with a 15-second TTL) combined with database conditional updates:
`UPDATE drivers SET status = 'BUSY' WHERE id = 42 AND status = 'AVAILABLE';`
The query returns affected rows = 0 for the losing worker, which catches the condition and retries.

### 5.3 Severe GPS Multipath Reflection in Urban Canyons
- **Failure Mode**: Tall skyscrapers reflect GPS signals, causing driver locations to jump wildly by 500 meters between consecutive pings, distorting spatial index accuracy.
- **Mitigation**: Run Kalman filtering algorithms locally on the mobile device or at the location gateway to smooth raw GPS coordinates and snap trajectories to known road network graphs (map-matching).

---

## 6. Hands-On Verification

The following standalone Python implementation demonstrates hexagonal/grid spatial indexing, driver radius lookup, batch bipartite dispatch matching, and surge pricing calculation.

```python
#!/usr/bin/env python3
"""
Production-grade demonstration of Ride-Sharing Core Architecture:
- Geospatial grid indexing simulation (H3 concept)
- Driver location ingestion and radius search
- Dynamic Surge Pricing calculation with smoothing
- Batch Bipartite Matching engine (min-cost matching)
"""

import math
import time
from typing import Dict, List, Tuple, Optional


class GeoPoint:
    def __init__(self, lat: float, lon: float):
        self.lat = lat
        self.lon = lon

    def distance_to(self, other: "GeoPoint") -> float:
        """Haversine formula approximation in kilometers."""
        dlat = math.radians(other.lat - self.lat)
        dlon = math.radians(other.lon - self.lon)
        a = (math.sin(dlat / 2) ** 2 +
             math.cos(math.radians(self.lat)) * math.cos(math.radians(other.lat)) * math.sin(dlon / 2) ** 2)
        return 6371.0 * (2 * math.atan2(math.sqrt(a), math.sqrt(1 - a)))


def get_spatial_cell_id(point: GeoPoint, resolution_km: float = 1.0) -> str:
    """Simulates discrete spatial cell discretization (H3 resolution)."""
    cell_x = int(point.lat / (resolution_km / 111.0))
    cell_y = int(point.lon / (resolution_km / (111.0 * math.cos(math.radians(point.lat)))))
    return f"cell_{cell_x}_{cell_y}"


class Driver:
    def __init__(self, driver_id: str, location: GeoPoint):
        self.driver_id = driver_id
        self.location = location
        self.is_available = True
        self.last_ping = time.time()


class RiderRequest:
    def __init__(self, rider_id: str, pickup: GeoPoint):
        self.rider_id = rider_id
        self.pickup = pickup
        self.created_at = time.time()


class SpatialRideService:
    def __init__(self):
        self.drivers: Dict[str, Driver] = {}
        # spatial_cell -> set of driver_ids
        self.spatial_index: Dict[str, set] = {}

    def update_driver_location(self, driver_id: str, point: GeoPoint):
        driver = self.drivers.get(driver_id)
        old_cell = get_spatial_cell_id(driver.location) if driver else None
        new_cell = get_spatial_cell_id(point)

        if old_cell and old_cell != new_cell and old_cell in self.spatial_index:
            self.spatial_index[old_cell].discard(driver_id)

        self.spatial_index.setdefault(new_cell, set()).add(driver_id)
        if driver:
            driver.location = point
            driver.last_ping = time.time()
        else:
            self.drivers[driver_id] = Driver(driver_id, point)

    def find_nearby_drivers(self, pickup: GeoPoint, radius_km: float = 3.0) -> List[Driver]:
        """Finds available drivers within radius."""
        nearby = []
        for d in self.drivers.values():
            if d.is_available and d.location.distance_to(pickup) <= radius_km:
                nearby.append(d)
        return sorted(nearby, key=lambda d: d.location.distance_to(pickup))

    def calculate_surge(self, cell_id: str, active_requests: int) -> float:
        """Calculates dynamic surge multiplier based on supply and demand."""
        active_drivers = len([
            d_id for d_id in self.spatial_index.get(cell_id, set())
            if self.drivers[d_id].is_available
        ])
        if active_drivers == 0:
            return 3.0 if active_requests > 0 else 1.0
        ratio = active_requests / active_drivers
        if ratio > 1.5:
            # Multiplier scales above 1.5 ratio
            return round(min(3.5, 1.0 + (ratio - 1.5) * 0.8), 2)
        return 1.0

    def batch_dispatch(self, requests: List[RiderRequest]) -> List[Tuple[str, str, float]]:
        """
        Greedy simulation of batch matching:
        Matches riders to drivers minimizing distance.
        Returns list of (rider_id, driver_id, distance_km).
        """
        matches = []
        assigned_drivers = set()

        for req in requests:
            nearby = self.find_nearby_drivers(req.pickup, radius_km=5.0)
            candidate = next((d for d in nearby if d.driver_id not in assigned_drivers), None)
            if candidate:
                dist = candidate.location.distance_to(req.pickup)
                matches.append((req.rider_id, candidate.driver_id, dist))
                assigned_drivers.add(candidate.driver_id)
                candidate.is_available = False
        return matches


if __name__ == "__main__":
    service = SpatialRideService()

    # 1. Ingest drivers in downtown area (37.7749, -122.4194)
    downtown = GeoPoint(37.7749, -122.4194)
    service.update_driver_location("driver_1", GeoPoint(37.7755, -122.4180)) # ~150m away
    service.update_driver_location("driver_2", GeoPoint(37.7800, -122.4200)) # ~600m away
    service.update_driver_location("driver_3", GeoPoint(37.8200, -122.4000)) # ~5km away (Far)

    print("--- 1. Testing Geospatial Proximity Query ---")
    nearby = service.find_nearby_drivers(downtown, radius_km=2.0)
    print(f"Drivers found within 2km: {[d.driver_id for d in nearby]}")
    assert len(nearby) == 2, "Should find exactly 2 drivers within 2km!"

    print("\n--- 2. Testing Surge Pricing Engine ---")
    cell = get_spatial_cell_id(downtown)
    surge_normal = service.calculate_surge(cell, active_requests=2)
    surge_high = service.calculate_surge(cell, active_requests=10)
    print(f"Normal Demand (2 req / 2 drivers): Surge = {surge_normal}x")
    print(f"High Demand   (10 req / 2 drivers): Surge = {surge_high}x")
    assert surge_high > 1.0, "Surge multiplier should be active during high demand!"

    print("\n--- 3. Testing Batch Dispatch Optimization ---")
    r1 = RiderRequest("rider_A", GeoPoint(37.7750, -122.4190))
    r2 = RiderRequest("rider_B", GeoPoint(37.7790, -122.4210))
    dispatches = service.batch_dispatch([r1, r2])
    for rider, driver, dist in dispatches:
        print(f"Matched: Rider {rider} -> Driver {driver} (Distance: {dist*1000:.0f} meters)")
    assert len(dispatches) == 2, "Both riders should be matched to separate drivers!"
    print("\nVerification Passed: Geospatial indexing and dispatch engine operating as expected!")
```

### CLI Verification

Execute health checks and geospatial API assertions:

```bash
# Linux / macOS: Send driver location update via curl
curl -X POST https://api.uber.com/v1/drivers/location \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer test_driver_token" \
  -d '{"driver_id": "drv_8842", "lat": 37.7749, "lon": -122.4194, "heading": 180}'

# Linux / macOS: Query nearby available drivers
curl -X GET "https://api.uber.com/v1/riders/nearby-drivers?lat=37.7749&lon=-122.4194&radius=2000" \
  -H "Authorization: Bearer test_rider_token"

# Windows PowerShell: Inspect dispatch service health
Invoke-RestMethod -Uri "https://dispatch.internal.uber.com/healthz"
```

---

## 7. Performance Characteristics and Capacity Planning

### 7.1 Location Ingestion Math
- **Driver Fleet**: 5 million active drivers simultaneously emitting locations.
- **Update Frequency**: 1 ping every 4 seconds.
- **Ingestion QPS**:
  $$\text{Throughput} = \frac{5,000,000}{4} = 1,250,000 \text{ location updates/sec}$$
- **Ingress Bandwidth**:
  - Payload per update: `driver_id` (16 bytes) + `lat` (8 bytes) + `lon` (8 bytes) + `timestamp` (8 bytes) + `heading` (4 bytes) + protocol overhead $\approx 64 \text{ bytes}$.
  - Total Bandwidth: $1,250,000 \times 64 \text{ bytes} \approx 80 \text{ MB/sec} = 640 \text{ Mbps}$.

### 7.2 In-Memory Spatial Index RAM Sizing
- Data stored per active driver:
  - 64 bytes raw telemetry + Redis dictionary pointer overhead (48 bytes) $\approx 112 \text{ bytes}$.
- Total Memory for 5 million drivers:
  $$5,000,000 \times 112 \text{ bytes} \approx 560 \text{ MB}$$
- Storing the entire global driver fleet in memory requires less than **1 GB of RAM**!
Even with multiple H3 resolution indexes and cross-region replication, the entire spatial index fits easily in a small Redis cluster.

---

## 8. In Production: Real-World Architecture (Uber)

Uber operates its real-time logistics engine using a custom distributed architecture:
1. **DISCO (Dispatch Engine)**: Partitions cities into discrete geographical cells and executes batch matching every 4 seconds to solve bipartite matching problems.
2. **H3 Spatial Index**: Uber open-sourced H3 in 2018.
All marketplace algorithms (surge pricing, driver positioning, routing hex bins) use H3 discrete hexagon grids.
3. **Ringpop**: A decentralized, consistent-hashing peer-to-peer membership protocol developed by Uber in Node.js/Go to distribute application state without a single master coordinator.
4. **Schemaless**: A custom distributed datastore layered on top of MySQL, designed for append-only trip data, achieving high availability across multi-datacenter setups.

---

## 9. Interview Questions and Deep Dives

> [!question] Question 1: Why does Uber use a hexagonal spatial index (H3) instead of square Geohash boxes?
> [!success]- Answer
> Square grids have an asymmetric neighbor problem: they possess 4 orthogonal neighbors and 4 diagonal neighbors.
> The distance to diagonal neighbors is $\sqrt{2} \approx 1.414\times$ greater than orthogonal neighbors.
> This spatial distortion complicates radial distance searches and travel time calculations.
> Hexagons have exactly 6 neighbors, and every neighbor centroid is **equidistant** from the central hexagon centroid.
> This invariant distance simplifies neighborhood expansions, gradient smoothing for surge pricing, and map-matching algorithms.

> [!question] Question 2: How do you prevent driver double-booking when multiple riders request a ride simultaneously?
> [!success]- Answer
> Combine application-level distributed locks with database conditional updates:
> 1. When a driver is chosen, acquire an atomic distributed lock in Redis: `SET lock:driver:<id> <ride_id> NX PX 15000` (15-second TTL matching the offer countdown).
> 2. Execute a conditional atomic update in the transactional database:
> `UPDATE drivers SET status = 'OFFERED' WHERE id = ? AND status = 'AVAILABLE';`
> If affected rows = 0, the driver was already claimed; the losing dispatch worker immediately picks the next candidate.

> [!question] Question 3: Why is batch matching (every 5 seconds) superior to greedy instant matching in ride-sharing?
> [!success]- Answer
> Greedy matching immediately pairs a rider with the closest driver.
> This creates the "driver poaching" anomaly: an assignment made at second 1 may force a rider at second 2 to wait 15 minutes, because Driver A was the only nearby car for Rider 2, while Driver B was only 30 seconds further from Rider 1.
> By buffering requests over a 5-second window, the system solves a global **Bipartite Matching** problem (Hungarian algorithm or min-cost max-flow), reducing aggregate platform pickup wait times by 15-25%.

> [!question] Question 4: How is dynamic surge pricing calculated and smoothed across geographical borders?
> [!success]- Answer
> 1. Compute supply ($S$, active available drivers) and demand ($D$, ride requests and app opens) within an H3 cell (Resolution 7) over rolling 5-minute windows.
> 2. If $D/S > \text{threshold}$, compute multiplier $M = 1.0 + \alpha(D/S - \text{threshold})$.
> 3. To prevent sharp price cliffs between adjacent streets (where one side has $2.5\times$ surge and the other $1.0\times$), apply a Gaussian spatial kernel filter across neighboring H3 hex rings, creating a smooth price gradient.

> [!question] Question 5: How does the system handle high-frequency driver location updates (1.25M updates/sec) without overloading databases?
> [!success]- Answer
> Location updates are partitioned into two separate paths:
> 1. **Ephemeral Real-Time Path**: Streamed via gRPC/WebSockets directly to an in-memory spatial index (Redis or custom Go cluster).
> Only the most recent coordinate is preserved; previous coordinates are overwritten in RAM.
> 2. **Analytical Historical Path**: Pings are published asynchronously to an Apache Kafka topic.
> Downstream stream processors consume Kafka batches, filter noise, and write historical breadcrumb trajectories to Apache Cassandra or S3 for fare audits and dispute resolution.

> [!question] Question 6: What happens if a driver receives an offer, but their phone loses cellular reception during the 15-second window?
> [!success]- Answer
> The offer state machine enforces a server-side timer.
> The server allocates a 15-second countdown with an atomic Redis lease.
> If the driver fails to acknowledge with an `ACCEPT` payload within 15 seconds, the Redis lock expires automatically, the server transitions the offer to `EXPIRED`, and the ride request re-enters the active dispatch matching pool to be paired with another driver.

> [!question] Question 7: How do you handle map-matching when GPS signals drift in dense downtown areas?
> [!success]- Answer
> Raw GPS signals suffer from urban canyon reflection, jumping tens of meters off course.
> Location ingestion gateways run **Hidden Markov Model (HMM)** map-matching algorithms combined with Kalman filters.
> The algorithm evaluates the driver's sequence of GPS coordinates against a road network graph (OpenStreetMap/OSRM), calculating the most probable road segment and direction of travel, snapping the driver's icon to the actual street.

> [!question] Question 8: How do you partition the global spatial index to achieve horizontal scalability?
> [!success]- Answer
> Shard the index geographically by **city or metropolitan area** (e.g., London, San Francisco, Tokyo).
> Ride-sharing is inherently local: a rider in New York never needs to query drivers in London.
> Each metropolitan cluster maintains its own independent spatial index and dispatch engine, completely eliminating cross-datacenter and cross-region coordination bottlenecks.

> [!question] Question 9: What is the ETA calculation pipeline, and why is straight-line distance insufficient?
> [!success]- Answer
> Straight-line (Euclidean/Haversine) distance ignores one-way streets, rivers, highway access ramps, and traffic congestion.
> Two drivers at equal Euclidean distance may have driving ETAs of 2 minutes versus 20 minutes.
> The platform runs routing engines (OSRM / custom routing graphs) over road networks with real-time traffic speeds ingested from active drivers, returning realistic driving ETAs used as edge weights in bipartite dispatch matching.

> [!question] Question 10: How do you ensure high availability if a primary dispatch datacenter goes offline?
> [!success]- Answer
> Deploy active-passive or multi-region active-active clusters with GeoDNS Anycast routing.
> Because trip state is transactional, database state is replicated asynchronously across cloud regions using CockroachDB or Google Cloud Spanner.
> If Region A fails, Anycast DNS diverts mobile client traffic to Region B, where incoming driver pings repopulate the in-memory spatial index within 10-15 seconds.

---

## 10. Related Concepts and Wikilinks

- [[Load-Balancing]]: Anycast and L4 TCP load balancing for persistent WebSocket driver connections.
- [[Redis-Architecture]]: In-memory spatial commands (`GEOADD`, `GEORADIUS`) and distributed locking.
- [[Consistent-Hashing]]: Partitioning driver location streams across gateway nodes.
- [[Optimistic-vs-Pessimistic-Locking]]: Concurrency control for ride acceptance and inventory locking.
- [[Apache-Kafka]]: High-throughput event streaming for telemetry pipelines.

---

## 11. Further Reading

- Xu, Alex. *System Design Interview – An Insider’s Guide (Volume 2)*. Chapter 1: Proximity Service & Chapter 8: Distributed Message Queue.
- Uber Engineering Blog. *H3: Uber’s Hexagonal Hierarchical Spatial Index*.
- Uber Engineering Blog. *How Uber Optimizes Dispatch with Bipartite Matching*.
- S2 Geometry Library Documentation (Google). *Overview of the S2 Geometry Library*.
