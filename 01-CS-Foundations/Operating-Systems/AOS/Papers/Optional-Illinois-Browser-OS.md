---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://www.usenix.org/conference/osdi10/trust-and-protection-illinois-browser-operating-system"]
course: cs6210
lesson: optional
reading: optional
venue: "OSDI 2010"
authors: [Shuo Tang, Haohui Mai, Samuel T. King]
tags: [cs6210, cs6210/paper]
aliases: ["Trust and Protection in the Illinois Browser Operating System"]
---

# Trust and Protection in the Illinois Browser Operating System

OSDI 2010. Reading status: optional. [Link](https://www.usenix.org/conference/osdi10/trust-and-protection-illinois-browser-operating-system).

> [!abstract] One-line summary
> The Illinois Browser Operating System treats the web browser as an operating system built on a microkernel, aggressively minimizing the trusted computing base by isolating complex components into untrusted processes.

## Problem

Modern web browsers have grown into massive, complex platforms with millions of lines of code running inside the trusted computing base.
A single bug in the rendering engine, network stack, or underlying operating system libraries can compromise the entire browser, allowing attackers to execute remote code or steal sensitive user data.

## Key idea

The Illinois Browser Operating System (IBOS) redesigns the browser as a specialized operating system by running a minimal browser kernel directly on top of a microkernel.
It drastically shrinks the trusted computing base by pushing massive components like the network stack, rendering engine, and device drivers into isolated, untrusted user-space processes.
By enforcing strict, policy-driven isolation at the lowest hardware level, IBOS ensures that even if complex browser components are compromised by attacks, the damage is strictly contained and the core system remains secure.

## Design

IBOS is built on the L4Ka microkernel and replaces traditional monolithic OS services with specialized browser subsystems.
It isolates each web page instance in its own protection domain, along with dedicated instances of the network stack and storage modules.
Display isolation is enforced by the IBOS kernel by mapping the screen's frame buffer memory exclusively to the currently active web page instance.
A custom message-passing interface allows the minimal IBOS kernel to inspect semantics and enforce security invariants between these isolated components.

## Evaluation

The system was evaluated by counting the lines of code and analyzing historical vulnerabilities from Linux and Chrome.
The IBOS trusted computing base was measured at roughly 42,000 lines of code, compared to over 5 million lines for Firefox on Linux.
Vulnerability analysis showed that IBOS could prevent 27 of 28 sampled OS and library vulnerabilities, and successfully contained or eliminated 77 percent of 175 historical Chrome vulnerabilities.
Performance benchmarks demonstrated that the IBOS architecture did not slow down page load latency significantly compared to existing browsers.

## Limitations and critiques

The architecture does not address denial-of-service attacks or perform fine-grained resource management, meaning a compromised component could still consume all available CPU time.
Its display isolation relies on coarse-grained primitives without hardware acceleration, using software rastering instead, which could impact rendering performance for complex graphics.

## What it led to

This project demonstrated the feasibility of treating the browser as the primary operating system, conceptually aligning with and influencing the development of highly compartmentalized browser architectures and systems like Chrome OS.

## Exam angles

<details>
<summary>How does IBOS achieve display isolation without including a window manager in the trusted computing base?</summary>
The IBOS kernel reserves the top portion of the screen for trusted UI elements and enforces isolation by mapping the remaining frame buffer memory only to the currently active web page instance.
</details>

<details>
<summary>Why does IBOS run a separate network stack instance for different web pages, and what attacks does this mitigate?</summary>
Running separate network stacks prevents a compromised network process from accessing or exfiltrating data belonging to other web pages, strongly enforcing the Same-Origin Policy at the network layer.
</details>

<details>
<summary>Despite its reduced trusted computing base, why is IBOS still unable to prevent XSS attacks, and how does it limit their impact?</summary>
XSS attacks exploit logic within the web application and the rendering engine rather than memory safety bugs, but IBOS limits their impact by containing the compromised rendering engine to a single web page instance without exposing the rest of the system.
</details>

## Related

- Lessons: not covered in lectures (optional reading)
