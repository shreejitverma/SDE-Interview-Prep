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
  - "The Datacenter as a Computer: An Introduction to the Design of Warehouse-Scale Machines (Barroso, Holzle, Ranganathan 2018)"
---

# Physical Failure Domains and Blast Radius

## TL;DR

Cloud Availability Zones (AZs) are engineered physical failure domains designed to be fault-independent: a flood, utility outage, or fire in one zone cannot propagate to another.
An Availability Zone is not a single building; a single AZ typically comprises one to six physically distinct datacenter facilities separated by several kilometers on separate flood plains with independent utility power substations.
Hyperscalers bind inter-AZ distance to a strict physics envelope: datacenters must be far enough apart to survive localized natural disasters, yet within a 100-kilometer fiber perimeter to guarantee sub-2-millisecond speed-of-light round-trip latency for synchronous quorums.
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
   - Thermal Limits: Modern servers follow ASHRAE A1/A2 thermal guidelines, operating reliably in ambient air temperatures between 18 degrees Celsius and 27 degrees Celsius.
   - Chilled Water Systems: Large central chiller plants pump cold water to Computer Room Air Handlers (CRAHs) positioned along the perimeter of the computer room.
   - Hot and Cold Aisle Containment: Server racks are organized into sealed rows.
   Cold air is forced under raised floors or ducted overhead into the cold aisle, passes through server chassis, and exhausts into a sealed hot aisle at temperatures up to 45 degrees Celsius, returning directly to the chillers.
   - Economizer Free Cooling: In temperate climates, facilities utilize evaporative economizers ("free cooling"), drawing in ambient outside air or using water mist evaporation, slashing power consumption.

### The Physics Envelope of an Availability Zone

The geographic separation between Availability Zones is bound by strict physics and operational constraints:
- Speed of Light in Fiber: Light travels through a vacuum at $300,000 \text{ km/s}$.
Inside standard single-mode optical fiber (silica glass with a refractive index of $n \approx 1.468$), light travels at roughly $204,000 \text{ km/s}$ (~204 km per millisecond, or ~5 microseconds per kilometer).
- The Synchronous Quorum Boundary:
Distributed databases (such as Amazon Aurora, Google Cloud Spanner, and etcd) execute synchronous quorum consensus (Raft or Paxos) across three Availability Zones for every write transaction.
To maintain acceptable database commit latencies:
$$\text{Inter-AZ Round-Trip Time} < 2.0 \text{ milliseconds}$$
Accounting for fiber routing inefficiencies, optical amplifiers, and switch buffering, 1 millisecond of one-way latency corresponds to approximately 100 kilometers of physical cable.
- Minimum Separation Distance: If two datacenters are 50 meters apart, a single crane collapse, industrial explosion, aircraft disaster, or localized flood can destroy both.
Therefore, cloud providers enforce a minimum physical distance (typically 10 to 20 kilometers) between distinct Availability Zones.
- The Goldilocks Zone: AZs are placed far enough apart (10 km to 100 km) to sit on independent electrical utility grids, distinct flood plains, and separate seismic fault lines, yet close enough (<100 km) to sustain synchronous database commits under 2 milliseconds.

### Dense Wavelength Division Multiplexing (DWDM) and Dark Fiber

Inter-AZ network backbones require massive, low-latency bandwidth:
- Dark Fiber Infrastructure: Cloud providers lease or lay hundreds of dedicated strands of dark fiber (unlit glass) in underground conduits along diverse physical rights-of-way (railroad tracks, highway corridors).
Conduits are encased in reinforced concrete at depths of 1 to 2 meters to minimize accidental backhoe fiber cuts.
- DWDM Technology: Dense Wavelength Division Multiplexing multiplexes dozens of distinct light wavelengths (colors) onto a single hair-thin glass fiber strand.
Using modern coherent optical transponders, a single fiber pair carries up to 96 distinct channels, each operating at 400 Gbps or 800 Gbps, yielding over 76 Terabits per second per fiber pair.
- Regional Transit Centers (RTC): Dedicated routing hubs that cross-connect all AZs in a mesh.
Every AZ connects to at least two distinct RTCs via separate conduits.
If an excavator severs the primary fiber conduit between AZ-A and AZ-B, optical switches detect loss of light within 50 milliseconds and reroute traffic around the ring through an alternate RTC.

### Logical Availability Zone Mapping and Randomization

In AWS and Google Cloud, the logical names assigned to zones (such as `us-east-1a` or `us-central1-b`) do not correspond to the same physical datacenters across different customer accounts:
- Account-Level Randomization: To prevent all customers from blindly provisioning resources in `us-east-1a` and causing severe capacity exhaustion, cloud providers randomize the mapping.
What Account 1 sees as `us-east-1a` may physically map to Datacenter Cluster 3, while in Account 2, `us-east-1a` maps to Datacenter Cluster 1.
- AZ ID Anchor: AWS provides an invariant physical identifier called the AZ ID (e.g., `use1-az1`, `use1-az2`, `use1-az4`).
Engineers building latency-sensitive or cross-account shared architectures must align instances using `az_id` rather than logical zone names to guarantee physical co-location or physical isolation.

## Numbers to Know

| Parameter | Quantitative Metric | Operational Context / Physical Constraint | Sourced As-Of |
|---|---|---|---|
| Speed of Light in Fiber Glass | ~204 kilometers per millisecond | $c / n$ where silica refractive index $n \approx 1.468$ | Optical Physics Standard |
| Inter-AZ Round-Trip Latency | 1.0 to 1.8 milliseconds (p50) | Sub-2.0ms round-trip budget for synchronous quorums | AWS/GCP SLA specs (as of 2026-10) |
| Intra-AZ Host-to-Host RTT | Less than 100 microseconds | Traverses local spine-and-leaf Clos fabric | AWS/GCP Benchmarks (as of 2026-10) |
| Typical AZ Physical Distance | 15 to 100 kilometers | Balances disaster isolation against quorum latency | AWS re:Invent (as of 2026-10) |
| On-Site Diesel Fuel Autonomy | 48 to 72 hours continuous operation | Maintained without external utility grid or fuel delivery | Cloud SRE Whitepapers (as of 2026-10) |
| UPS Bridge Battery/Flywheel Time | 30 to 120 seconds | Bridges the 10-15s gap until diesel generators run | Uptime Institute Specs |
| Optical Protection Switching Speed | Less than 50 milliseconds | Automatic fiber loop healing upon dark fiber cuts | ITU-T G.841 Standard |
| Physical Facilities per AZ | 1 to 6 distinct datacenter buildings | An AZ is an aggregate campus, not a lone building | AWS Global Infra (as of 2026-10) |

## AWS vs GCP Mechanism Parity

| Dimension | Amazon Web Services | Google Cloud Platform | Architectural Mechanism |
|---|---|---|---|
| Zone Identifier Invariant | AZ ID (e.g., `use1-az1`, `use1-az2`) | Cloud Zone Mapping API / Project Zone Mapping | AWS explicitly surfaces invariant AZ IDs across accounts; GCP manages zone resource balancing primarily via internal scheduler pools. |
| Datacenter Power Topology | 2N / N+1 with custom flywheel and battery systems | Custom high-efficiency battery packs integrated into racks | Google integrates 48V backup battery packs directly into custom rack architectures; AWS leverages localized central UPS and containerized generators. |
| Synchronous Multi-AZ Primitive | Multi-AZ Aurora (4-of-6 quorum across 3 AZs) | Spanner / Regional Persistent Disk (Synchronous 2-AZ write) | AWS builds storage quorums into specific service engines (Aurora, EBS Physalia); GCP exposes synchronous multi-AZ storage at the raw block layer. |
| Inter-Zone Network Protocol | Scalable Reliable Datagram (SRD) over Nitro | Andromeda SDN over Jupiter optical fabric | AWS optimizes inter-zone latency via hardware-sprayed SRD packets; GCP optimizes through optical circuit switching and unified SDN flow routing. |

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
Traffic immediately redirects to Zone B and Zone C (45 nodes each).
Average utilization spikes from 85% to `85% * (90 / 60) = 127.5%`.
Zones B and C experience CPU throttling, health check timeouts, and cascading failure, knocking out the entire regional service.
Mitigation: Design for static stability.
Ensure the surviving zones have pre-allocated buffer headroom to absorb a complete zonal loss without needing to provision new compute instances during the outage:
$$\text{Max Steady-State Utilization Target} \le \frac{N - 1}{N} \times 100\%$$
For a 3-AZ deployment, steady-state utilization must not exceed 66.6%.

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
Physical media destruction protocols mandate degaussing and mechanical shredding of defective hard drives and NVMe SSDs before scrap material leaves the campus.
- Inter-AZ Data Transfer Cost Economics: Data transfer across Availability Zones within the same region costs $0.01/GB in each direction ($0.02/GB round-trip) on AWS and $0.01/GB on GCP.
An AI training job or distributed database transferring 500 TB of uncompressed data between zones per month incurs $10,000 in unnecessary egress billing.
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
Hyperscalers frequently engineer an AZ as an aggregate campus containing two to six distinct physical datacenter buildings.
These buildings are separated by short distances (hundreds of meters to several kilometers) on the same campus, connected by redundant low-latency dark fiber loops.
Building an AZ across multiple physical structures provides intra-zone redundancy: a local fire, roof collapse, or localized electrical breaker failure in Building 1 does not exhaust the capacity of the entire Availability Zone, while maintaining uniform sub-millisecond network latency across the zone's cluster fabric.

#### Follow-up Interviewer Question
Why do AWS and GCP randomize the mapping of logical zone names (e.g., `us-east-1a`) to physical datacenters across customer accounts, and how can an engineer bypass this randomization?

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
When Zone A fails, the load shifts equally to Zones B and C, bringing their utilization to exactly $50\% + 50\% = 100\%$, maintaining full performance with zero dynamic instance launches.

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

## Further Reading

- [The Datacenter as a Computer: Warehouse-Scale Machines](https://www.morganclaypool.com/doi/abs/10.2200/S00874ED3V01Y201809CAC046) - Luiz André Barroso and Urs Hölzle's classic book on datacenter architecture, power topologies, and physical failure domains.
- [AWS Builders' Library: Static Stability Using Availability Zones](https://aws.amazon.com/builders-library/static-stability-using-availability-zones/) - Becky Weiss's masterclass on designing systems that survive zonal loss without dynamic control-plane recovery.
- [Google Cloud Architecture Framework: Reliability and Failure Domains](https://cloud.google.com/architecture/framework/reliability) - Google's engineering principles for blast radius containment and regional disaster recovery.
- [Subsea Cables and the Global Cloud Backbone (Telegeography / AWS Infrastructure)](https://infrastructure.aws/) - Global interactive mapping and technical documentation of terrestrial conduits, subsea cables, and regional transit centers.
