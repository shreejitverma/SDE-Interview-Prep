---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1145/1294261.1294287"]
course: cs6210
lesson: optional
reading: optional
venue: "SOSP 2007"
authors: [Ripal Nathuji, Karsten Schwan]
tags: [cs6210, cs6210/paper]
aliases: ["VirtualPower: Coordinated Power Management in Virtualized Enterprise Systems"]
---

# VirtualPower: Coordinated Power Management in Virtualized Enterprise Systems

SOSP 2007. Reading status: optional. [Link](https://doi.org/10.1145/1294261.1294287).

> [!abstract] One-line summary
> VirtualPower bridges the semantic gap between guest OS applications and hypervisor hardware control by exporting virtual power states and translating them into coordinated soft and hard scaling to optimize enterprise power efficiency.

## Problem

Power management in virtualized enterprise systems suffers from a semantic gap because hypervisors control hardware power states but lack insight into application performance requirements, while guest VMs understand their application needs but cannot access actual hardware power controls.
This separation leads to inefficient power management where either energy is wasted during periods of low utilization or application quality of service is unacceptably degraded.

## Key idea

VirtualPower bridges the semantic gap by exposing Virtual Power Management (VPM) states to guest VMs, allowing them to run local power policies that request specific virtual performance levels based on their application needs.
The hypervisor intercepts these requests via VPM channels and translates them into physical power management actions using a combination of hardware voltage scaling and software-based CPU scheduling adjustments.
By coordinating these VM-level requests across the entire physical platform, the hypervisor can optimize system-wide power consumption and enforce global power capping policies without violating the performance constraints of individual guest applications.

## Design

The system exports a set of virtual frequencies to guest VMs, such as 3.2GHz down to 800MHz.
Since the physical hardware may only support a few frequency steps, VirtualPower implements soft scaling by adjusting the CPU time slices allocated to a VM in the hypervisor scheduler.
Guest VMs run local policies (PM-L) that monitor application slack and request appropriate VPM states.
The hypervisor runs global policies (PM-G) that aggregate these requests and use hardware scaling, soft scaling, consolidation, and migration to satisfy the virtual state requests while minimizing physical power draw.

## Evaluation

The system was evaluated on a Pentium 4 dual-core platform using SPEC CPU2000 benchmarks and the RUBiS web service application.
For transactional VMs, the basic mapping policy reduced power consumption by up to 31 percent when the application tolerated reduced performance.
For the multi-tier RUBiS application, a smoothing policy kept performance degradation within 5 percent while effectively reducing overall power.
Soft scaling extended the system's power throttling capability from 20 percent using hard scaling alone up to 40 percent.

## Limitations and critiques

The approach assumes that guest VMs are cooperative and accurately report their VPM states, which might not hold true in a multi-tenant cloud environment with malicious or selfish users.
While soft scaling reduces active CPU power consumption, overall energy savings are still strictly bounded by the static idle power of the underlying physical platform.

## What it led to

This work was foundational for power-aware virtualization and influenced subsequent cloud resource managers that coordinate application-level metrics with infrastructure-level power controls.

## Exam angles

<details>
<summary>How does VirtualPower use soft scaling to provide more power states than the underlying hardware supports?</summary>
It uses the hypervisor's CPU scheduler to limit the execution time slices allocated to a guest VM, simulating a lower processor frequency even when the physical hardware is running at a higher speed.
</details>

<details>
<summary>What is the semantic gap in virtualized power management, and how do VPM channels address it?</summary>
The hypervisor has physical power control but lacks application insight, while the guest VM has application insight but lacks physical control.
VPM channels bridge this gap by allowing the guest to request virtual power states, which the hypervisor then translates into physical actions.
</details>

<details>
<summary>Why might a hypervisor choose a higher physical frequency but lower soft-scaled frequency for a VM instead of simply lowering the physical frequency?</summary>
A higher physical frequency allows multiple soft-scaled VMs to be consolidated onto a single physical core, potentially freeing up other cores to enter deep sleep states and reducing overall system power.
</details>

## Related

- Lessons: not covered in lectures (optional reading)
