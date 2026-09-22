---
role: Low-Latency
aliases: [Low Latency Systems Engineer, HFT Developer, Infrastructure Engineer]
tags: [role-hub, low-latency]
type: playbook
track: [sde, quant-dev, quant-research, low-latency, ai-eng, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Low Latency Systems Engineer - Preparation Hub

> **Target Companies:** Optiver, Jump Trading, Citadel Securities, HRT, IMC, Virtu Financial, Tower Research
> **Target Levels:** Mid to Senior Systems Engineer / Infrastructure

---

## What Low Latency Interviews Test

| Round | Weight | What They Want |
|-------|--------|----------------|
| **C++ Systems Coding** | 35% | Lock-free, cache-aware, zero-allocation hot-path code |
| **Systems Architecture** | 25% | Kernel bypass, DPDK, FPGA, network stack, OS tuning |
| **Hardware Sympathy** | 20% | Cache hierarchy, branch prediction, memory models, NUMA |
| **Networking** | 15% | TCP/UDP internals, multicast, kernel bypass, NIC offloading |
| **Behavioral** | 5% | Working under pressure, precision, attention to detail |

---

## Study Plan

→ [[Study-Plan|Detailed Week-by-Week Study Plan]]

### Quick Priority Matrix

| Topic | Priority | Your Level | Target Level | Vault Resource |
|-------|----------|-----------|-------------|----------------|
| Lock-Free Programming | Critical | | 5/5 | [[14-Low-Latency-Systems/08 - Low-Latency Programming]] |
| Memory Models & Atomics | Critical | | 5/5 | [[14-Low-Latency-Systems/08 - Low-Latency Programming]] |
| Cache Architecture | Critical | | 5/5 | [[14-Low-Latency-Systems/04 - Hardware Mechanical Sympathy]] |
| Kernel Bypass / DPDK | Critical | | 4/5 | [[14-Low-Latency-Systems/06 - Networking]] |
| OS & Kernel Tuning | Critical | | 4/5 | [[14-Low-Latency-Systems/05 - OS & Kernel Tuning]] |
| Networking (TCP/UDP) | High | | 4/5 | [[14-Low-Latency-Systems/06 - Networking]] |
| FPGA Concepts | High | | 3/5 | [[14-Low-Latency-Systems/12 - FPGAs & Hardware Acceleration]] |
| Time & Measurement | High | | 4/5 | [[14-Low-Latency-Systems/07 - Time & Measurement]] |
| IPC & Messaging | Medium | | 3/5 | [[14-Low-Latency-Systems/09 - Messaging & IPC]] |
| Protocols (FIX/SBE) | Medium | | 3/5 | [[14-Low-Latency-Systems/10 - Protocols & Codecs]] |

---

## Target Companies

```dataview
TABLE WITHOUT ID
  file.link AS "Company", industry AS "Industry", status AS "Status"
FROM "16-Interview-Command-Center/02-Companies"
WHERE contains(target_roles, "Low-Latency")
SORT file.name ASC
```

## Active Interviews

```dataview
TABLE WITHOUT ID
  company AS "Company", level AS "Level", stage AS "Stage",
  confidence + "/5" AS "Conf", next_action_date AS "Deadline"
FROM "16-Interview-Command-Center/03-Pipeline"
WHERE contains(track, "low-latency") AND stage AND type != "round" AND stage != "rejected" AND stage != "withdrawn"
SORT next_action_date ASC
```

---

## Role Resources
- → [[Skill-Matrix]] | → [[Question-Bank]] | → [[Common-Patterns]] | → [[Resources]]

## Key Vault Links - Your 14-Module LL Knowledge Base
| # | Module | Link |
|---|--------|------|
| 01 | Market & Microstructure | [[14-Low-Latency-Systems/01 - Market & Microstructure Fundamentals]] |
| 02 | Exchange Architecture | [[14-Low-Latency-Systems/02 - Exchange Architecture]] |
| 03 | Matching Engine Internals | [[14-Low-Latency-Systems/03 - Matching Engine Internals]] |
| 04 | Hardware Mechanical Sympathy | [[14-Low-Latency-Systems/04 - Hardware Mechanical Sympathy]] |
| 05 | OS & Kernel Tuning | [[14-Low-Latency-Systems/05 - OS & Kernel Tuning]] |
| 06 | Networking | [[14-Low-Latency-Systems/06 - Networking]] |
| 07 | Time & Measurement | [[14-Low-Latency-Systems/07 - Time & Measurement]] |
| 08 | Low-Latency Programming | [[14-Low-Latency-Systems/08 - Low-Latency Programming]] |
| 09 | Messaging & IPC | [[14-Low-Latency-Systems/09 - Messaging & IPC]] |
| 10 | Protocols & Codecs | [[14-Low-Latency-Systems/10 - Protocols & Codecs]] |
| 11 | Participant-Side Systems | [[14-Low-Latency-Systems/11 - Participant-Side Systems]] |
| 12 | FPGAs & HW Acceleration | [[14-Low-Latency-Systems/12 - FPGAs & Hardware Acceleration]] |
| 13 | Reliability, Ops & Testing | [[14-Low-Latency-Systems/13 - Reliability, Ops & Testing]] |
| 14 | Industry Map & Canon | [[14-Low-Latency-Systems/14 - Industry Map & Canon]] |
| - | Interview Prep | [[14-Low-Latency-Systems/Interview/interview]] |
| - | 12-Week Roadmap | [[14-Low-Latency-Systems/Roadmap - 12-Week Production Calibration]] |
