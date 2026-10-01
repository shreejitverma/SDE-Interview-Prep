---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1145/35037.35060"]
course: cs6210
lesson: L08
reading: required
venue: "TOCS 1988"
authors: [Roger Haskin, Yoni Malachi, Wayne Sawdon, Gregory Chan]
tags: [cs6210, cs6210/paper]
aliases: ["Recovery Management in QuickSilver"]
---

# Recovery Management in QuickSilver

TOCS 1988. Reading status: required. [Link](https://doi.org/10.1145/35037.35060).

> [!abstract] One-line summary
> An extensible distributed operating system that provides atomic transactions as a unified, system-wide failure recovery mechanism for client-server interactions.

## Problem

In extensible, distributed systems relying on a client-server architecture, servers maintain complex state on behalf of clients. Ad-hoc recovery mechanisms like timeouts, virtual circuits, or purely stateless servers fail to adequately handle failures, maintain consistency, or support atomic operations across multiple independent servers.

## Key idea

Quicksilver institutionalizes atomic transactions as the core recovery paradigm for the entire operating system. Instead of hiding transactions within a language or a rigid recoverable object manager, it exposes basic commit protocol and log recovery primitives directly to clients and servers, allowing them to tailor recovery semantics efficiently based on their specific needs.

## Design

The recovery architecture consists of a Transaction Manager (TM) and a Log Manager (LM). Every interprocess communication (IPC) request is tagged with a globally unique Transaction Identifier (Tid). IPC tracks server participation to build a directed graph topology for distributed transactions. The TM supports multiple commit protocols: a one-phase protocol for servers maintaining volatile state, and a two-phase presumed-abort protocol for recoverable state. It introduces a `vote-commit-volatile` response to optimize logging for replicated volatile state. Log recovery is driven entirely by the individual servers rather than the LM.

## Evaluation

Quicksilver demonstrates that the overhead of a generalized transaction management system is low enough to be practical for everyday OS services. For local transactions, the commit cost is minimal, while distributed commit costs are primarily bound by network latency. The architecture significantly simplified the development of system services by concentrating recovery coordination in the TM.

## Limitations and critiques

Exposing raw transaction primitives shifts the burden of ensuring correct concurrency control and log recovery onto the developers of individual servers. If a server is poorly written, it can compromise the integrity of its own recoverable state. Additionally, building a reliable transaction graph requires strict rules regarding when clients can commit uncompleted asynchronous requests.

## What it led to

Quicksilver served as a pioneering example of utilizing transactions for general-purpose OS state management rather than restricting them to databases. It influenced subsequent transactional operating systems and research into integrating recovery semantics natively within IPC and kernel designs.

## Exam angles

<details>
<summary>Why does Quicksilver offer different variants of the commit protocol?</summary>
To balance recoverability with efficiency. Simple servers maintaining only volatile state can use a lightweight one-phase commit, avoiding the overhead of logging and the two-phase handshake required for servers maintaining persistent, recoverable state.
</details>

<details>
<summary>How does Quicksilver optimize the two-phase commit protocol for servers with replicated volatile state?</summary>
It introduces a `vote-commit-volatile` response. This allows a server to vote to commit and request outcome notification without forcing the TM to write costly commit protocol log records, as the server's state is not persistently recoverable.
</details>

<details>
<summary>How does Quicksilver resolve cycles in the transaction graph during distributed commit?</summary>
Each subordinate TM identifies the first superior TM that issued a request to it. It responds to vote requests from any subsequent superior TMs with a `vote-commit-read-only`, and only completes the commit processing and responds to its first superior once it has collected votes from its own subordinates.
</details>

## Related

- Lessons: [L08a](../Part-4-Distributed-Subsystems-and-Recovery/L08a-Lightweight-Recoverable-Virtual-Memory.md), [L08b](../Part-4-Distributed-Subsystems-and-Recovery/L08b-RioVista.md), [L08c](../Part-4-Distributed-Subsystems-and-Recovery/L08c-Quicksilver.md)
