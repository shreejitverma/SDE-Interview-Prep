---
type: concept
track: [cloud-ai, ai-eng, sde, distinguished]
level:
status: solid
last_reviewed:
sources:
  - "AWS Global Infrastructure Architecture and Availability Zone Design (AWS 2024)"
  - "Google Cloud Datacenter Design and Campus Infrastructure (Google Cloud 2024)"
  - "AWS Builders' Library: Static Stability Using Availability Zones (AWS 2020)"
  - "The Datacenter as a Computer: An Introduction to the Design of Warehouse-Scale Machines (Barroso, Hoelzle, Ranganathan 2018)"
---

# Physical Failure Domains and Blast Radius

## TL;DR

Cloud Availability Zones (AZs) are engineered physical failure domains designed to be fault-independent: a flood, utility outage, or fire in one zone cannot propagate to another.
An Availability Zone is not necessarily a single building; it is one or more discrete datacenters with separate and redundant power, networking, and connectivity, and different AZs are fed by different power substations (AWS docs, as of 2026-10).
Hyperscalers bind inter-AZ distance to a physics envelope: zones must be far enough apart to survive localized disasters, yet within about 100 km (60 miles) of each other so synchronous replication stays at single-digit millisecond latency (AWS docs, as of 2026-10); light covers about 200 km of fiber per millisecond, so 100 km costs roughly 1 ms of round-trip propagation.
Power topologies enforce strict 2N or N+1 redundancy using independent medium-voltage utility feeds, diesel rotary uninterruptible power supplies (UPS) or lithium-ion battery banks, and dual-corded A/B power distribution to server racks.
Failure domain blast radius is managed through cell-based architectures and shuffle sharding, containing correlated infrastructure failures within localized boundaries.

## Mental Model

Availability Zones construct concentric rings of physical and logical isolation, balancing independent failure blast radiuses against speed-of-light network latency.

```mermaid
flowchart TD
    subgraph Region["Cloud Region Boundary (e.g., us-east-1 / us-central1)"]
        subgraph AZ_A["Availability Zone A (Physical Substation Alpha)"]
            DC1["Data Center Building 1"]
            DC2["Data Center Building 2"]
            DC1 <-->|Dark Fiber Interconnect| DC2
            UPS_A["2N Diesel Generators + Flywheel/Battery UPS"]
            HVAC_A["Dedicated Chilled Water Loop"]
            UPS_A --> DC1
            HVAC_A --> DC1
        end

        subgraph AZ_B["Availability Zone B (Physical Substation Beta)"]
            DC3["Data Center Building 3"]
            DC4["Data Center Building 4"]
            DC3 <-->|Dark Fiber Interconnect| DC4
            UPS_B["2N Diesel Generators + Flywheel/Battery UPS"]
            HVAC_B["Dedicated Chilled Water Loop"]
            UPS_B --> DC3
            HVAC_B --> DC3
        end

        subgraph AZ_C["Availability Zone C (Physical Substation Gamma)"]
            DC5["Data Center Building 5"]
            UPS_C["2N Diesel Generators + Flywheel/Battery UPS"]
            HVAC_C["Dedicated Chilled Water Loop"]
            UPS_C --> DC5
            HVAC_C --> DC5
        end

        RTC["Regional Transit Center (DWDM Mesh Core)"]
        AZ_A <-->|Redundant Terrestrial Conduits (<2ms RTT)| RTC
        AZ_B <-->|Redundant Terrestrial Conduits (<2ms RTT)| RTC
        AZ_C <-->|Redundant Terrestrial Conduits (<2ms RTT)| RTC
    end
```

Every Availability Zone is powered by its own independent utility substation, backed by dedicated diesel generators with days of on-site fuel, and cooled by separate chilled-water loops.
Zones communicate over redundant, geographically disparate underground dark-fiber conduits routed through Regional Transit Centers.
If a backhoe cuts a fiber conduit or an explosion destroys an entire electrical substation, the adjacent Availability Zones continue operating without interruption.

## Internals

### The Physical Anatomy of a Datacenter Facility

Inside a hyperscale cloud datacenter campus, every engineering system is designed with zero single points of failure:

1. Electrical Power Distribution:
   - High-Voltage Utility Ingestion: Utility power enters the datacenter campus at medium-to-high voltages (e.g., 115 kV to 230 kV) from independent utility substations.
   - On-Site Transformation: Dedicated on-site transformers step the voltage down to 13.8 kV or 4.16 kV for campus distribution, and finally to 480 V or 415 V three-phase power at the rack row level.
   - Uninterruptible Power Supply (UPS): When utility power drops, mechanical switches transfer the load to UPS systems within milliseconds.
   Hyperscalers utilize either high-capacity Lithium-Ion battery banks or rotary dynamic UPS systems (kinetic energy stored in spinning steel flywheels) to provide 30 to 120 seconds of bridge power.
   - Diesel Generators: Large on-site diesel generators (2 MW to 3.5 MW per unit) crank, synchronize, and take over the full facility load within 10 to 15 seconds.
   Datacenter campuses maintain on-site diesel fuel reserves to run continuously at full capacity for 48 to 72 hours without refueling, backed by priority fuel-delivery contracts.
   - Dual-Corded Server Power (A/B Rails): Enterprise server power supplies are dual-corded.
   Power Supply 1 connects to Power Distribution Unit A (PDU-A, powered by Generator/UPS System A), while Power Supply 2 connects to PDU-B (powered by Generator/UPS System B).
   Any breaker, transformer, or UPS can fail without interrupting server operation.

2. Thermal Management and Cooling:
   - Thermal Limits: Modern servers follow ASHRAE thermal guidelines, whose recommended inlet-air envelope is 18 degrees Celsius to 27 degrees Celsius (classes A1/A2 allow wider excursions).
   - Chilled Water Systems: Large central chiller plants pump cold water to Computer Room Air Handlers (CRAHs) positioned along the perimeter of the computer room.
   - Hot and Cold Aisle Containment: Server racks are organized into sealed rows.
   Cold air is forced under raised floors or ducted overhead into the cold aisle, passes through server chassis, and exhausts into a sealed hot aisle at temperatures up to 45 degrees Celsius, returning directly to the chillers.
   - Economizer Free Cooling: In temperate climates, facilities utilize evaporative economizers ("free cooling"), drawing in ambient outside air or using water mist evaporation, slashing power consumption.

### The Physics Envelope of an Availability Zone

The geographic separation between Availability Zones is bound by strict physics and operational constraints:
- Speed of Light in Fiber: Light travels through a vacuum at $300,000 \text{ km/s}$.
Inside standard single-mode optical fiber (silica glass with a refractive index of $n \approx 1.468$), light travels at roughly $204,000 \text{ km/s}$ (~204 km per millisecond, or ~5 microseconds per kilometer).
- The Synchronous Quorum Boundary:
Distributed databases (such as Amazon Aurora, Google Cloud Spanner, and etcd) execute synchronous quorum replication across three Availability Zones for every write transaction (Paxos in Spanner, Raft in etcd, a 4-of-6 storage quorum in Aurora).
A common design target for acceptable database commit latencies is:
$$\text{Inter-AZ Round-Trip Time} < 2.0 \text{ milliseconds}$$
At ~5 microseconds per kilometer, 1 millisecond of one-way propagation corresponds to about 200 kilometers of fiber, and a 2 ms round trip to about 200 kilometers of fiber each way.
Because real conduits are 1.3x to 1.5x longer than the straight-line distance, and optical amplifiers, switch buffering, and protocol processing add further delay, the usable straight-line separation inside that budget is well under 200 km.
- Minimum Separation Distance: If two datacenters are 50 meters apart, a single crane collapse, industrial explosion, aircraft disaster, or localized flood can destroy both.
Therefore, AWS places Availability Zones a meaningful distance apart, many kilometers from any other AZ (AWS docs, as of 2026-10); providers do not publish a fixed minimum.
- The Goldilocks Zone: AZs are placed far enough apart to sit on different power substations, distinct flood plains, and separate local hazard zones, yet within about 100 km of each other.
At 100 km straight-line with a 1.5x route stretch, the fiber path is ~150 km, i.e. ~0.75 ms one-way and ~1.5 ms round trip of pure propagation, which leaves headroom inside a 2 ms commit target.

### Dense Wavelength Division Multiplexing (DWDM) and Dark Fiber

Inter-AZ network backbones require massive, low-latency bandwidth:
- Dark Fiber Infrastructure: Cloud providers lease or lay hundreds of dedicated strands of dark fiber (unlit glass) in underground conduits along diverse physical rights-of-way (railroad tracks, highway corridors).
Conduits are encased in reinforced concrete at depths of 1 to 2 meters to minimize accidental backhoe fiber cuts.
- DWDM Technology: Dense Wavelength Division Multiplexing multiplexes dozens of distinct light wavelengths (colors) onto a single hair-thin glass fiber strand.
Using modern coherent optical transponders at 400 Gbps to 800 Gbps per wavelength, a single fiber pair carries tens of terabits per second; higher per-channel rates need wider channel spacing, so the channel count falls as the per-channel rate rises (96 channels fit only at the 50 GHz spacing of 100G-era systems).
- Regional Transit Centers (RTC): Dedicated routing hubs that cross-connect all AZs in a mesh.
Every AZ connects to at least two distinct RTCs via separate conduits.
If an excavator severs the primary fiber conduit between AZ-A and AZ-B, optical switches detect loss of light within 50 milliseconds and reroute traffic around the ring through an alternate RTC.

### Logical Availability Zone Mapping and Randomization

In AWS, the logical names assigned to zones (such as `us-east-1a`) do not correspond to the same physical Availability Zone across different customer accounts:
- Account-Level Randomization: To prevent all customers from blindly provisioning resources in `us-east-1a` and concentrating load in one zone, AWS maps physical AZs to AZ names randomly for each account (AWS RAM docs, as of 2026-10).
What Account 1 sees as `us-east-1a` may physically map to Datacenter Cluster 3, while in Account 2, `us-east-1a` maps to Datacenter Cluster 1.
- AZ ID Anchor: AWS provides an invariant physical identifier called the AZ ID (e.g., `use1-az1`, `use1-az2`, `use1-az4`).
Engineers building latency-sensitive or cross-account shared architectures must align instances using `az_id` rather than logical zone names to guarantee physical co-location or physical isolation.
- Google Cloud: Google documents no per-project remapping of zone names and exposes no separate zone-ID API; the zone name (e.g., `us-central1-a`) is the placement identifier used across projects (Google Cloud docs, as of 2026-10).
Google does note that zones are logical abstractions: in Stockholm, Osaka, and Montreal, three zones are housed in one or two physical datacenters (Google Cloud docs, as of 2026-10).

## Numbers to Know

| Parameter | Quantitative Metric | Operational Context / Physical Constraint | Sourced As-Of |
|---|---|---|---|
| Speed of Light in Fiber Glass | ~204 kilometers per millisecond | $c / n$ where silica refractive index $n \approx 1.468$ | Optical Physics Standard |
| Inter-AZ Round-Trip Latency | Single-digit milliseconds (no SLA; measure per AZ pair) | Close enough for synchronous replication | AWS fault isolation whitepaper (as of 2026-10) |
| Intra-AZ Host-to-Host RTT | Less than 100 microseconds | Traverses local spine-and-leaf Clos fabric | AWS/GCP Benchmarks (as of 2026-10) |
| AZ Physical Distance | Many kilometers apart, all within ~100 km (60 miles) | Balances disaster isolation against quorum latency | AWS fault isolation whitepaper (as of 2026-10) |
| On-Site Diesel Fuel Autonomy | 48 to 72 hours continuous operation | Maintained without external utility grid or fuel delivery | Cloud SRE Whitepapers (as of 2026-10) |
| UPS Bridge Battery/Flywheel Time | 30 to 120 seconds | Bridges the 10-15s gap until diesel generators run | Uptime Institute Specs |
| Optical Protection Switching Speed | Less than 50 milliseconds | Automatic fiber loop healing upon dark fiber cuts | ITU-T G.841 Standard |
| Physical Facilities per AZ | One or more discrete datacenters | An AZ can be an aggregate of buildings, not a lone building | AWS fault isolation whitepaper (as of 2026-10) |

## AWS vs GCP Mechanism Parity

| Dimension | Amazon Web Services | Google Cloud Platform | Architectural Mechanism |
|---|---|---|---|
| Zone Identifier Invariant | AZ ID (e.g., `use1-az1`, `use1-az2`) | Zone name (e.g., `us-central1-a`); no separate zone-ID API | AWS maps AZ names to physical AZs randomly per account and surfaces AZ IDs as the cross-account invariant; Google documents no per-project remapping, so the zone name itself is the identifier. |
| Datacenter Power Topology | Custom in-rack micro-UPS battery packs and power supplies (re:Invent 2020) | Custom high-efficiency battery packs integrated into racks | Both moved backup batteries into the rack: Google with 48V rack battery packs, AWS with per-rack micro-UPS units replacing large central UPS rooms, so a single battery failure affects one rack rather than a data hall. |
| Synchronous Multi-AZ Primitive | Multi-AZ Aurora (4-of-6 quorum across 3 AZs) | Spanner / Regional Persistent Disk (Synchronous 2-AZ write) | AWS builds multi-AZ quorums into specific service engines (Aurora, RDS Multi-AZ) while EBS stays single-AZ; GCP also exposes synchronous multi-AZ storage at the raw block layer. |
| Instance Network Transport | Scalable Reliable Datagram (SRD) over Nitro, opt-in: EFA traffic stays within one AZ; ENA Express works within an AZ and, since May 2026, across AZs in most Regions | Andromeda SDN; Jupiter fabric with optical circuit switching inside datacenters | Default VPC traffic between AZs is standard IP over the regional metro fiber; SRD multipath spraying is opt-in per interface, and its tail-latency benefit is documented for same-AZ traffic (AWS docs, as of 2026-10). |

## Worked Example: AZ ID Discovery and Multi-Account Alignment in OpenTofu

When coordinating multi-account infrastructure (such as VPC peering, transit gateways, or distributed consensus clusters), relying on logical zone names (`us-east-1a`) causes subtle cross-account cross-zone traffic, adding 1ms of latency and $0.02/GB in egress fees.
We use OpenTofu / Terraform to dynamically look up physical AZ IDs and deploy subnets aligned to identical physical failure domains.

### Runnable OpenTofu / Terraform Configuration: Physical AZ ID Mapping

```hcl
# az_alignment.tf - Dynamic Physical AZ ID Alignment
terraform {
  required_version = ">= 1.6.0"
  required_providers {
    aws = {
      source  = "hashicorp/aws"
      version = "~> 5.0"
    }
  }
}

provider "aws" {
  region = "us-east-1"
}

# Query AWS for the physical AZ IDs available in this account
data "aws_availability_zones" "available" {
  state = "available"
  filter {
    name   = "opt-in-status"
    values = ["opt-in-not-required"]
  }
}

resource "aws_vpc" "aligned_vpc" {
  cidr_block           = "10.100.0.0/16"
  enable_dns_hostnames = true
  enable_dns_support   = true

  tags = {
    Name = "cloud-ai-aligned-vpc"
  }
}

# Subnet 1: Explicitly pinned to physical AZ ID "use1-az1"
resource "aws_subnet" "subnet_az1" {
  vpc_id     = aws_vpc.aligned_vpc.id
  cidr_block = "10.100.1.0/24"

  # Pinning directly to invariant physical AZ ID instead of logical name
  availability_zone_id = "use1-az1"

  tags = {
    Name        = "subnet-physical-az1"
    PhysicalAZ = "use1-az1"
  }
}

# Subnet 2: Explicitly pinned to physical AZ ID "use1-az2"
resource "aws_subnet" "subnet_az2" {
  vpc_id     = aws_vpc.aligned_vpc.id
  cidr_block = "10.100.2.0/24"

  # Pinning directly to invariant physical AZ ID instead of logical name
  availability_zone_id = "use1-az2"

  tags = {
    Name        = "subnet-physical-az2"
    PhysicalAZ = "use1-az2"
  }
}

# Output the mapping for cross-account alignment audits
output "physical_zone_mapping" {
  value = {
    for idx, name in data.aws_availability_zones.available.names :
    name => data.aws_availability_zones.available.zone_ids[idx]
  }
  description = "Mapping of logical zone names to invariant physical AZ IDs"
}

# Teardown Command:
# tofu destroy -auto-approve
```

## Failure Modes and Pitfalls

### Pitfall 1: Logical AZ Name Desynchronization in Multi-Account VPC Peering
An enterprise provisions an Elasticsearch cluster in Account A in `us-east-1a` and an ingestion worker fleet in Account B in `us-east-1a`, expecting low intra-AZ latency and zero network egress fees.
Because logical zone names are randomized per account, Account A's `us-east-1a` maps to physical `use1-az1`, while Account B's `us-east-1a` maps to physical `use1-az4`.
All network traffic silently traverses the inter-AZ regional backbone.
Symptoms: 1.5ms added to every packet, intermittent latency spikes, and unexpected tens of thousands of dollars in inter-AZ data transfer fees.
Mitigation: Audit all multi-account VPC peering connections against `availability_zone_id`.
Never configure production subnets using logical names in enterprise landing zones.

### Pitfall 2: Static Stability Violation: Dynamic Capacity Spikes during Zonal Loss
A system runs 30 nodes in Zone A, 30 in Zone B, and 30 in Zone C (90 nodes total) at 85% average CPU utilization.
Zone A experiences a total electrical power trip.
Traffic immediately redirects to the 60 remaining nodes in Zone B and Zone C (30 nodes each).
Average utilization spikes from 85% to `85% * (90 / 60) = 127.5%`.
Zones B and C experience CPU throttling, health check timeouts, and cascading failure, knocking out the entire regional service.
Mitigation: Design for static stability.
Ensure the surviving zones have pre-allocated buffer headroom to absorb a complete zonal loss without needing to provision new compute instances during the outage:
$$\text{Max Steady-State Utilization Target} \le \frac{N - 1}{N} \times 100\%$$
For a 3-AZ deployment, steady-state utilization must not exceed 66.7%.

### Pitfall 3: The 2-AZ Quorum Anti-Pattern
An engineering team deploys a distributed database (such as ZooKeeper, etcd, or Consul) across two Availability Zones, placing 2 nodes in Zone A and 1 node in Zone B (3 nodes total).
The team assumes the system is fault-tolerant.
If Zone A fails, 2 out of 3 nodes disappear simultaneously.
The surviving node in Zone B cannot achieve a majority quorum ($N/2 + 1 = 2$).
The cluster freezes and refuses all read and write operations.
Mitigation: Consensus quorums require three independent failure domains.
Deploy across three Availability Zones (1 node in Zone A, 1 in Zone B, 1 in Zone C).
If operating in a 2-AZ region, deploy a lightweight external witness node in a third region or separate cloud provider.

## Security and Cost Angles

- Physical Security Perimeters: Hyperscale datacenters enforce multi-layered defense-in-depth physical security: perimeter fencing, biometric access traps, thermal imaging cameras, vehicle crash barriers, and strict security clearance escorts.
Physical media destruction protocols sanitize every failed drive before scrap leaves the campus: hard drives are degaussed and/or shredded, while SSDs, which degaussing cannot erase because flash stores charge rather than magnetic domains, are cryptographically erased or purged per NIST SP 800-88 and physically shredded.
- Inter-AZ Data Transfer Cost Economics: Data transfer across Availability Zones within the same region costs $0.01/GB in each direction ($0.02/GB round-trip) on AWS and $0.01/GB on GCP.
An AI training job or distributed database transferring 500 TB of uncompressed data between AWS zones per month incurs `500,000 GB * $0.02 = $10,000` in unnecessary transfer billing.
Co-locating high-bandwidth training workloads inside a single AZ within a `cluster` placement group drops network transfer costs to $0.00.
- Cross-Zone Egress Siphoning via NAT Gateways: In an AWS VPC, placing a single shared AWS NAT Gateway in `us-east-1a` forces private instances in `us-east-1b` and `us-east-1c` to route all internet-bound traffic across zones to reach the NAT Gateway.
This incurs inter-AZ data transfer fees on top of the NAT Gateway processing surcharge ($0.045/GB).
Deploy one independent NAT Gateway per Availability Zone to eliminate cross-zone egress hops.

## Interview Questions and Model Answers

### Question 1 (L5 - Senior)
What is the difference between a cloud Region and an Availability Zone, and why does an Availability Zone typically comprise multiple physical buildings?

#### Model Answer
A cloud Region is a separate geographic area (such as Northern Virginia, Frankfurt, or Tokyo) containing multiple, isolated physical locations known as Availability Zones.
An Availability Zone (AZ) is an independent physical failure domain engineered to be insulated from failures in other AZs.
Crucially, an Availability Zone is not a single server room or single building.
AWS defines an AZ as one or more discrete datacenters with separate and redundant power, networking, and connectivity, and large AZs are aggregates of several buildings.
These buildings sit close together relative to the inter-AZ spacing and are connected by redundant low-latency fiber.
Building an AZ across multiple physical structures provides intra-zone redundancy: a local fire, roof collapse, or localized electrical breaker failure in Building 1 does not exhaust the capacity of the entire Availability Zone, while maintaining uniform sub-millisecond network latency across the zone's cluster fabric.

#### Follow-up Interviewer Question
Why does AWS randomize the mapping of logical zone names (e.g., `us-east-1a`) to physical Availability Zones per account, how can an engineer align resources across accounts despite it, and does Google Cloud behave the same way?

### Question 2 (L6 - Staff)
Explain the "Static Stability" principle in multi-AZ cloud architecture, and calculate the exact capacity sizing required to survive a complete zonal outage without autoscaling.

#### Model Answer
Static stability means that a system continues to operate correctly and deliver its service level agreements without requiring control-plane intervention, dynamic autoscaling, or state reconfiguration during a failure.
In a disaster scenario where an entire Availability Zone crashes, the cloud provider's control plane (the APIs responsible for launching new EC2 instances, provisioning EBS volumes, or expanding Kubernetes nodes) is simultaneously bombarded by thousands of automated autoscalers from affected customers.
This creates severe control-plane throttling, resource exhaustion, and API rate limiting.
A statically stable architecture does not attempt to launch replacement compute instances during an outage.
Instead, it pre-provisions sufficient capacity and buffer headroom in the surviving zones beforehand.
Capacity Calculation:
Let $N$ be the number of independent Availability Zones across which the service is distributed.
Let $L$ be the total aggregate peak load (in requests per second or compute units).
To survive the total failure of any single zone without performance degradation:
1. The surviving $N - 1$ zones must be capable of absorbing 100% of the peak load $L$.
2. Capacity allocated per zone $C_{\text{zone}}$ must satisfy:
$$C_{\text{zone}} = \frac{L}{N - 1}$$
3. For a 3-AZ deployment ($N = 3$), each zone must be provisioned for:
$$C_{\text{zone}} = \frac{L}{3 - 1} = \frac{L}{2} = 50\% \text{ of peak load}$$
Total provisioned capacity across all three zones equals $3 \times 50\% = 150\%$ of peak load.
Under normal operations, each zone operates at exactly $33.3\%$ of peak load, leaving $16.7\%$ idle headroom per zone.
When Zone A fails, the load shifts equally to Zones B and C, so each now carries $L/2$, exactly its provisioned capacity (100% utilization), maintaining full performance with zero dynamic instance launches.

#### Follow-up Interviewer Question
How does cell-based architecture enhance static stability when isolating corrupt data or "poison-pill" requests?

### Question 3 (L7+ - Principal)
You are the Chief Infrastructure Architect designing a multi-region active-active distributed transactional database. Explain how the speed-of-light propagation latency in silica optical fiber sets hard theoretical constraints on distributed consensus, and defend the placement of data nodes, witness nodes, and network paths across an inter-continental topology.

#### Model Answer
Physics and Theoretical Latency Constraints:
1. Index of Refraction: Light in standard single-mode silica fiber ($n \approx 1.468$) propagates at $v = c / n \approx 204,000 \text{ km/s}$.
This yields a theoretical minimum propagation delay of $\sim 4.9 \text{ microseconds per kilometer}$ of one-way travel, or $\sim 9.8 \text{ microseconds per kilometer}$ of round-trip time (RTT).
2. Terrestrial Path Inefficiencies: Real-world terrestrial conduits follow highways, railroads, and terrain detours, introducing a typical "distance stretch factor" of 1.3x to 1.5x over great-circle geographic distance.
3. Optical Amplification and Switching Overhead: Erbium-Doped Fiber Amplifiers (EDFAs) and optical add-drop multiplexers (ROADMs) add additional dispersion compensation and processing latency.
Consequence for Distributed Consensus:
In Paxos or Raft, a leader cannot commit a transaction until a majority quorum of replicas acknowledge the write log.
For an $M$-node cluster, a quorum requires $Q = \lfloor M/2 \rfloor + 1$ confirmations:
- If nodes are distributed globally (e.g., Northern Virginia, Frankfurt, Singapore):
  - Northern Virginia to Frankfurt: Geographic distance $\approx 6,300 \text{ km}$; fiber RTT $\approx 75 \text{ ms}$.
  - Northern Virginia to Singapore: Geographic distance $\approx 15,500 \text{ km}$; fiber RTT $\approx 180 \text{ ms}$.
- A quorum involving Singapore guarantees a minimum write commit latency floor of at least 180ms, making high-throughput interactive OLTP physically impossible.
Architectural Blueprint for Inter-Continental Topology:
1. Asymmetric Quorum Clustering (Regional Quorums with Global Read Replicas):
   - Rather than forming a single global Paxos quorum across three continents, we establish a localized regional consensus cluster across adjacent regions with low fiber latency.
   - Example AWS deployment: Leader and primary replicas placed across `us-east-1` (Virginia) and `us-east-2` (Ohio), separated by ~500 km (fiber RTT $\approx 11 \text{ ms}$).
   - A lightweight witness node (participating in voting but storing zero user payload) is placed in `us-west-2` (Oregon, RTT $\approx 60 \text{ ms}$).
   - The majority quorum ($Q = 2 \text{ of } 3$) is routinely satisfied between Virginia and Ohio in under 12 milliseconds.
   The Oregon witness is only queried if either Virginia or Ohio suffers a total partition.
2. Cross-Continental Replication:
   - Replicas in Europe (Frankfurt) and Asia (Singapore) receive asynchronous log streams from the Virginia leader.
   - Local clients in Europe and Asia execute read-only queries against local replicas using bounded staleness or snapshot isolation (e.g., reading data at a historical timestamp $T - 500\text{ms}$), delivering sub-5ms local read latency.
   - Any write originating in Europe or Asia is forwarded over dedicated cloud backbones to the Virginia leader, paying the speed-of-light tax once, but preserving strict serializability and preventing distributed split-brain deadlock.

#### Follow-up Interviewer Question
If an earthquake severs the terrestrial dark fiber cables connecting Virginia and Ohio simultaneously, how does the consensus engine detect whether Ohio is dead or partitioned, and how does the Oregon witness node prevent split-brain execution?

### Question 4 (L5 - Senior)
A stateless API tier and a cache tier each run evenly across three AWS Availability Zones, and every request is routed to a random cache node. The tiers exchange 200 TB per month, and all internet-bound traffic leaves through a single NAT Gateway in one AZ. Estimate the avoidable cross-AZ cost and explain how you would reduce it.

#### Model Answer
With uniform random routing across three equal zones, a request lands on a cache node in its own zone only one time in three, so two thirds of the traffic crosses an AZ boundary.
Cross-AZ volume is `200 TB * 2/3 = ~133.3 TB`, i.e. about 133,333 GB.
AWS bills inter-AZ transfer at $0.01/GB on each side, so the effective rate is $0.02/GB and the cost is `133,333 GB * $0.02 = ~$2,667 per month` (AWS pricing, as of 2026-10).
The single NAT Gateway adds a second leak: instances in the other two AZs pay inter-AZ transfer to reach it on top of the $0.045/GB NAT processing charge, and they lose internet egress entirely if that one AZ fails.
The fixes are mechanism changes, not discounts.
First, make routing zone-aware: each API node prefers cache nodes in its own AZ, which requires the cache to be replicated or sharded per zone so that every AZ holds a complete copy of the hot set.
Second, deploy one NAT Gateway per AZ and point each private subnet's route table at its local gateway, which removes the cross-AZ hop and the shared failure point at the price of extra hourly NAT charges.
The trade-offs must be named explicitly.
Zone affinity concentrates load: if one AZ has fewer healthy nodes, its local cache nodes run hotter, so the router needs a spill-over rule that sends traffic cross-zone when local capacity is short.
Per-zone cache replication multiplies cache memory and write fan-out, so it pays off only when read traffic dominates.
Finally, affinity must never become a hard dependency: during an AZ failure, the surviving zones must still serve all traffic, which ties this design back to static stability.

#### Follow-up Interviewer Question
How would you measure cross-AZ traffic per service pair before and after the change, given that billing reports only aggregate inter-AZ bytes per account?

### Question 5 (L6 - Staff)
Explain how cell-based architecture and shuffle sharding limit the blast radius of a poison-pill customer, and quantify the isolation shuffle sharding provides.

#### Model Answer
Cell-based architecture partitions a service into independent, complete copies of the stack (cells), each serving a fixed subset of customers, with only a thin routing layer shared between them.
A bad deployment, a corrupt record, or an overload inside one cell can only affect that cell's customers, so the blast radius is roughly `1 / number_of_cells` of the fleet.
Cells are usually aligned with physical failure domains, for example one cell per AZ or several cells per AZ, so that infrastructure failures and software failures share the same containment boundary.
Shuffle sharding refines this inside a shared fleet: instead of assigning each customer to one fixed partition, each customer is assigned a random combination of k workers out of n.
The number of distinct shards is the binomial coefficient C(n, k).
With n = 8 workers and k = 2, there are `C(8, 2) = 28` shards, so if one customer's requests crash every worker they touch, a random other customer shares both of its workers with probability `1 / 28`, about 3.6%, compared with 100% in an unsharded fleet and 25% with four fixed pairs.
With n = 100 and k = 5, there are `C(100, 5) = 75,287,520` shards, so a full overlap is vanishingly rare.
The guarantee depends on clients being fault tolerant: a customer whose shard partially overlaps the poison pill's shard must retry on its remaining healthy workers, otherwise partial overlap still causes errors.
The trade-offs are concrete.
Each customer's capacity is bounded by its k workers, so large customers need larger shards or dedicated cells.
Cells duplicate fixed infrastructure and complicate cross-cell queries, migrations, and capacity balancing, and the shared routing layer becomes the one component whose failure is not contained, so it must be kept minimal and statically stable.

#### Follow-up Interviewer Question
How would you migrate a large customer from one cell to another without downtime, and which component in the cell router must stay consistent during the move?

### Question 6 (L7+ - Principal)
Availability Zones are designed to fail independently, yet many real outages spread across zones. As the principal engineer for a regional service, identify the paths by which failures correlate across zones and design the controls that preserve Availability Zone independence.

#### Model Answer
Physical isolation is necessary but not sufficient: AWS keeps generators, cooling, and power substations separate per AZ and staggers deployments to AZs in the same Region in time to prevent correlated failure (AWS docs, as of 2026-10), but a service can reintroduce correlation above the physical layer.
The main correlation paths are the following.
First, software and configuration: a bad build or config pushed to all zones at once fails all zones at once, regardless of physical separation.
Second, cross-zone dependencies in the data plane: if a request in AZ-A calls a database primary, a lock service, or a cache in AZ-B, an AZ-B impairment fails AZ-A's requests too.
Third, control-plane dependencies during recovery: designs that rely on launching instances, attaching volumes, or changing DNS during the outage compete with every other customer doing the same.
Fourth, gray failures: a zone that is partially degraded (packet loss, elevated latency) often keeps passing shallow health checks, so traffic keeps flowing into it.
Fifth, logical zones that share physical facilities: Google notes that in Stockholm, Osaka, and Montreal three zones are housed in one or two physical datacenters, so zone-level redundancy there does not imply building-level independence (Google Cloud docs, as of 2026-10).
The controls map one-to-one onto those paths.
Deploy zone by zone with bake time and automatic rollback, so a bad change is caught in one zone before it reaches the next.
Keep request paths AZ-local wherever possible, with each zone holding its own replicas, caches, and queues, and treat any synchronous cross-AZ call as a reviewed exception.
Pre-provision capacity for the loss of a zone (static stability) so recovery needs no control-plane actions.
Detect gray failures with per-AZ outlier metrics rather than global averages, and evacuate the zone proactively, for example with Amazon Application Recovery Controller (ARC) zonal shift, which moves supported load balancer traffic away from one AZ.
Verify the physical topology of each region instead of assuming three zones means three facilities, and place quorum members accordingly.
The cost is real: AZ-local replicas multiply storage and memory, staggered deployments slow every release, and evacuation drills consume engineering time, but these are the price of turning three zones into three genuinely independent failure domains.

#### Follow-up Interviewer Question
How would you run a game day that proves your service survives the loss of one zone, including a gray failure that degrades rather than kills the zone, without harming real customers?

## Further Reading

- [The Datacenter as a Computer: Warehouse-Scale Machines](https://www.morganclaypool.com/doi/abs/10.2200/S00874ED3V01Y201809CAC046) - Luiz Andre Barroso, Urs Hoelzle, and Parthasarathy Ranganathan's classic book on datacenter architecture, power topologies, and physical failure domains.
- [AWS Builders' Library: Static Stability Using Availability Zones](https://aws.amazon.com/builders-library/static-stability-using-availability-zones/) - Becky Weiss's masterclass on designing systems that survive zonal loss without dynamic control-plane recovery.
- [Google Cloud Architecture Framework: Reliability and Failure Domains](https://cloud.google.com/architecture/framework/reliability) - Google's engineering principles for blast radius containment and regional disaster recovery.
- [Subsea Cables and the Global Cloud Backbone (Telegeography / AWS Infrastructure)](https://infrastructure.aws/) - Global interactive mapping and technical documentation of terrestrial conduits, subsea cables, and regional transit centers.
