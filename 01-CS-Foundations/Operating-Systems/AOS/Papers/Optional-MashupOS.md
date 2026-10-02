---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1145/1294261.1294263"]
course: cs6210
lesson: optional
reading: optional
venue: "SOSP 2007"
authors: [Helen J. Wang, Xiaofeng Fan, Jon Howell, Collin Jackson]
tags: [cs6210, cs6210/paper]
aliases: ["Protection and Communication Abstractions for Web Browsers in MashupOS"]
---

# Protection and Communication Abstractions for Web Browsers in MashupOS

SOSP 2007. Reading status: optional. [Link](https://doi.org/10.1145/1294261.1294263).

> [!abstract] One-line summary
> MashupOS introduces new browser abstractions like ServiceInstance and Friv to provide secure isolation, flexible display layout, and safe cross-domain communication for client-side web mashups.

## Problem

Modern client-side web mashups combine content and code from mutually untrusted domains, but traditional web browsers only offer the rigid Same-Origin Policy for isolation.
This forces developers to either use cross-domain iframes that prevent seamless layout integration, or include third-party scripts directly, which grants them full access to the hosting application's resources and creates severe security vulnerabilities like Cross-Site Scripting.

## Key idea

MashupOS introduces a set of new browser abstractions that provide a middle ground between complete isolation and complete trust for web applications.
It isolates components using ServiceInstance and sandboxes while allowing them to negotiate layout through the Friv abstraction and exchange data through the CommRequest asynchronous messaging protocol.
This architecture establishes a View-Origin Policy that lets browser-side components communicate safely across domains, treating them as secure extensions of their respective server-side applications rather than forcing them to share a single execution context.

## Design

The ServiceInstance abstraction isolates memory and persistent state for a given domain, acting like an operating system process for web content.
The Friv abstraction provides flexible, div-like display layout across isolation boundaries, allowing a child document to negotiate its size with a parent container.
The Sandbox and OpenSandbox elements host unauthorized content, ensuring untrusted scripts cannot access the parent page's DOM or resources.
CommRequest provides a data-only, asynchronous communication channel that uses local port names for safe messaging between different ServiceInstances.

## Evaluation

The authors implemented MashupOS as a Script Engine Proxy prototype within the Internet Explorer browser architecture.
Microbenchmarks showed a 33 percent overhead for DOM object interposition, but negligible overhead for pure JavaScript object manipulation.
Macrobenchmarks measuring page load times for the top 500 MSN search results showed negligible overall impact on loading performance.

## Limitations and critiques

The prototype did not fully implement bounds on commodity resources like CPU or network bandwidth, focusing primarily on memory and state isolation.
The Friv layout negotiation protocol introduces a potential side-channel where a malicious parent might infer sensitive information about a child document based on how its layout size changes.

## What it led to

MashupOS heavily influenced the evolution of modern web standards, paving the way for features like the HTML5 postMessage API for cross-origin communication and the iframe sandbox attribute for containing untrusted code.

## Exam angles

<details>
<summary>What is the fundamental problem with the Same-Origin Policy when building client-side mashups?</summary>
It forces an all-or-nothing approach where components are either fully isolated in rigid frames without integration, or fully trusted as included scripts with complete access to the integrator's resources.
</details>

<details>
<summary>How does the Friv abstraction in MashupOS differ from traditional div and iframe elements?</summary>
Unlike an iframe, a Friv allows the child document's layout requirements to flow to the parent container for flexible sizing, and unlike a div, it maintains a strict isolation boundary between the content of different domains.
</details>

<details>
<summary>What is the difference between a Sandbox and an OpenSandbox regarding access rules?</summary>
A private Sandbox can only be accessed by the enclosing page if they share the same domain, while an OpenSandbox allows the enclosing page full access to its content regardless of the hosting domain.
</details>

## Related

- Lessons: not covered in lectures (optional reading)
