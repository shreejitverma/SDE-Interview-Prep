---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1109/PROC.1975.9939"]
course: cs6210
lesson: L11
reading: required
venue: "Proceedings of the IEEE 1975"
authors: ["Jerome H. Saltzer", "Michael D. Schroeder"]
tags: [cs6210, cs6210/paper]
aliases: ["The Protection of Information in Computer Systems"]
---

# The Protection of Information in Computer Systems

Proceedings of the IEEE 1975. Reading status: required (typically Section 1). [Link](https://doi.org/10.1109/PROC.1975.9939).

> [!abstract] One-line summary
> This paper defines eight foundational design principles for secure computer systems and explores the mechanics of capability systems versus access control lists.

## Problem

As computers evolved from batch processing systems to multi-user environments, the need arose to protect shared information from unauthorized access, modification, or denial of use.
Existing systems lacked a cohesive framework for understanding and implementing protection mechanisms.
Engineers needed structured guidelines to design systems that could dynamically and securely control sharing.

## Key idea

This seminal tutorial paper establishes the foundational principles of computer security by defining eight core design principles that guide the creation of secure systems.
It systematically explores the mechanics of protecting computer-stored information by analyzing the trade-offs between capability systems and access control lists.
The authors emphasize that security cannot be an afterthought, and their principles provide a framework to minimize flaws, handle dynamic authorizations, and ensure that protection mechanisms are both effective and psychologically acceptable to users.

## Design

The paper outlines eight design principles for security.
Economy of mechanism dictates that designs should be as simple and small as possible to allow for thorough inspection.
Fail-safe defaults ensure that access decisions are based on explicit permission rather than exclusion.
Complete mediation requires every access to every object to be checked against authority.
Open design argues that the security of a mechanism should not depend on the ignorance of attackers.
Separation of privilege states that requiring two keys is more robust than a single key.
Least privilege means every program and user should operate with the minimal privileges necessary to complete their job.
Least common mechanism advises minimizing shared mechanisms to prevent unintended communication paths.
Psychological acceptability emphasizes that the human interface must be easy to use so users routinely apply protection correctly.
The paper then details how Access Control Lists (ACLs) and capability systems implement these goals.
ACLs are list-oriented, placing the authorization check at the point of access.
Capability systems are ticket-oriented, where possession of an unforgeable ticket inherently grants access.

## Evaluation

As a tutorial paper, it does not present a traditional quantitative evaluation with benchmarks.
Instead, it evaluates the logical properties, architectural complexity, and robustness of the proposed mechanisms in the context of systems like Multics and CTSS.
The key takeaways to remember are the eight design principles and the fundamental trade-offs between ACLs and capabilities.

## Limitations and critiques

The authors note that proving a system is entirely secure is a negative requirement and is logically difficult to guarantee.
Capability systems struggle inherently with revocation of access and controlling the propagation of rights.
Access Control Lists can be slower because they require a check on every access, which typically necessitates complex hardware optimization like shadow registers.
Side channels and the confinement problem are identified as difficult issues that are not fully solved by basic protection mechanisms.

## What it led to

This work formed the theoretical foundation for modern operating system security.
The eight design principles are still taught as the standard doctrine for secure software and systems design.
The concepts surrounding ACLs and capabilities directly influenced almost every subsequent secure operating system, including UNIX, Multics, and modern microkernel architectures.

## Exam angles

<details>
<summary>Compare and contrast Access Control Lists (ACLs) and Capability systems in terms of revocation and propagation.</summary>
ACLs make revocation straightforward because removing a user from the list immediately invalidates their access on the next check.
However, ACLs require an authorization check on every access, which can be computationally expensive without hardware support.
Capability systems make access checks extremely fast because possession of the capability is proof of authority.
Conversely, capability systems struggle with revocation and propagation because once a capability is copied and given to another user, it is very difficult to track down and invalidate those copies.
</details>

<details>
<summary>Define the principle of "Complete mediation" and explain how it prevents security flaws.</summary>
Complete mediation requires that every access to every object must be checked for authority.
It forces a system-wide view of access control that covers normal operation, initialization, recovery, and maintenance.
By refusing to rely on remembered results of previous authority checks without systematic updating, it ensures that dynamic changes in permissions are instantly enforced.
</details>

<details>
<summary>Explain the principle of "Fail-safe defaults" and why it is better than exclusion-based access control.</summary>
Fail-safe defaults base access decisions on explicit permission rather than exclusion, meaning the default state is a lack of access.
A conservative design must require arguments for why objects should be accessible, rather than why they should not be.
If a mistake occurs in an explicit permission mechanism, it fails safely by refusing access, whereas a mistake in an exclusion mechanism fails by allowing unauthorized access.
</details>

<details>
<summary>How does the "Open design" principle contrast with "security through obscurity"?</summary>
Open design argues that the security architecture and mechanisms should not be secret, as secrecy relies on the ignorance of attackers.
Security through obscurity is fragile because once the secret mechanism is discovered, the entire system is compromised.
Open design decouples the mechanism from the protection keys, allowing the mechanism to be publicly scrutinized and verified while keeping only the keys secret.
</details>

## Related

- Lessons: [L11a](../Part-5-Internet-Scale-Real-Time-and-Security/L11a-Principles-of-Information-Security.md), [L11b](../Part-5-Internet-Scale-Real-Time-and-Security/L11b-Security-in-the-Andrew-System.md)
