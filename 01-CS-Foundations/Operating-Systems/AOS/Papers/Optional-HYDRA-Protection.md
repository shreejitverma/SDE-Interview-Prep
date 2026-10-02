---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1145/800213.806532"]
course: cs6210
lesson: optional
reading: optional
venue: "SOSP 1975"
authors: ["Ellis Cohen", "David Jefferson"]
tags: [cs6210, cs6210/paper]
aliases: ["Protection in the HYDRA Operating System"]
---

# Protection in the HYDRA Operating System

SOSP 1975. Reading status: optional. [Link](https://doi.org/10.1145/800213.806532).

> [!abstract] One-line summary
> Hydra introduces a capability-based protection system that separates mechanism from policy, enabling the construction of user-defined protected subsystems and addressing complex protection scenarios.

## Problem

Designing a general-purpose operating system requires flexible protection mechanisms that can support a wide range of security policies, rather than hardcoding specific policies into the kernel.
The system must restrict computations to prevent malicious or accidental disclosure of shared information.
It also needs to handle complex protection scenarios such as mutual suspicion, confinement, and revocation without overly limiting user freedom.

## Key idea

The core philosophy of Hydra is the strict separation of policy from mechanism.
It provides a set of basic capability-based protection mechanisms at the kernel level, which users and higher-level subsystems can combine to construct arbitrary, complex protection policies.
It uniformly treats all system resources as objects distinguished by type, controls access solely through capabilities, and enforces that procedures execute in a dynamically created local name space containing only the necessary rights. 

## Design

Information in Hydra is grouped into objects, each possessing a unique name, a type, and a representation consisting of a data part and a C-list of capabilities.
Access to any object is strictly controlled by these capabilities, which carry access rights represented as bit vectors.
The Local Name Space (LNS) defines the instantaneous protection domain of an executing program.
When a procedure is called, a completely new LNS is instantiated for that execution.
This new LNS inherits internal capabilities from the procedure object and receives parameter capabilities explicitly passed by the caller.
To support protected subsystems, Hydra utilizes a mechanism called rights amplification.
When a capability is passed to a subsystem's procedure, its rights can be amplified within the new LNS, granting the procedure access to the object's internal representation while hiding those details from the caller.
The kernel provides type-independent generic operations for manipulating C-lists and data parts.

## Evaluation

The paper presents an architectural description of the system and does not include a quantitative performance evaluation with setup details and specific numbers.
Instead, it provides qualitative evaluations of how the mechanisms solve specific protection problems.
The Mutual Suspicion problem is solved naturally because both caller and callee execute in their own isolated LNS, sharing only explicitly passed parameters.
Revocation is handled by passing indirect capabilities, called aliases, which can be later destroyed by the creator.

## Limitations and critiques

Hardware limitations of the PDP-11 architecture forced the separation of the object representation into distinct data parts and C-lists, and prevented mapping data parts directly into the user's address space.
Furthermore, domain switching during procedure calls incurs substantial overhead due to these hardware limitations and some design flaws.
This performance cost discourages the use of Hydra procedures for small, frequently called subroutines, violating the ideal that all code should run with minimal privileges.

## What it led to

Hydra's capability model and its emphasis on the principle of policy and mechanism separation heavily influenced later capability-based systems and microkernel architectures.
It formalized concepts like rights amplification and the uniform treatment of all system resources as typed objects.
These concepts became foundational in the design of secure, object-oriented operating systems.

## Exam angles

<details>
<summary>How does Hydra's Local Name Space (LNS) address the mutual suspicion problem between a caller and a callee?</summary>
In Hydra, a procedure call instantiates a new, completely separate LNS.
The caller and callee do not share a protection domain; they only share the specific capabilities passed as parameters.
This ensures that the callee cannot arbitrarily access the caller's environment, and the caller cannot interfere with the callee's internal operations, safely resolving mutual suspicion.
</details>

<details>
<summary>Explain the concept of rights amplification in Hydra and why it is essential for constructing user-defined protected subsystems.</summary>
Rights amplification allows a capability's rights to be expanded when passed as an argument to a specific procedure.
This is essential for protected subsystems because it allows the subsystem's procedures to manipulate the internal representation of an object type they manage, while callers hold capabilities with restricted rights that hide those internal details.
</details>

<details>
<summary>Discuss the distinction Hydra makes between mechanism and policy in the context of system protection.</summary>
Hydra provides basic tools (mechanisms) like capabilities, objects, LNS, and rights amplification at the kernel level.
It does not dictate how these should be used to enforce security rules (policies).
Users and subsystems are free to use these mechanisms to construct policies tailored to their specific needs, such as implementing revocation or confinement.
</details>

## Related

- Lessons: not covered in lectures (optional reading)
