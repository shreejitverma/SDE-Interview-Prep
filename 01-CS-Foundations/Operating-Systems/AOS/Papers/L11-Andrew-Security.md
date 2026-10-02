---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1145/65000.65002"]
course: cs6210
lesson: L11
reading: required
venue: "TOCS 1989"
authors: ["M. Satyanarayanan"]
tags: [cs6210, cs6210/paper]
aliases: ["Integrating Security in a Large Distributed System"]
---

# Integrating Security in a Large Distributed System

TOCS 1989. Reading status: required. [Link](https://doi.org/10.1145/65000.65002).

> [!abstract] One-line summary
> The Andrew system achieves security at scale by treating all client workstations and networks as untrusted, relying on end-to-end encrypted RPC and token-based authentication.

## Problem

The Andrew computing environment was designed to span a large campus with over 5,000 workstations.
In such a massive distributed environment, traditional timesharing security assumptions fail entirely.
Workstations can be physically compromised, operating systems can be modified, and network segments can be tapped.
A laissez-faire attitude towards security is not viable because the relative anonymity of users requires strict enforcement of protection policies.

## Key idea

In a large-scale distributed environment like Andrew, security cannot rely on the physical integrity of client workstations or the network.
The key idea is to assume that all clients and networks are entirely untrusted and to place complete trust only in the central Vice file servers.
To enforce this model, Andrew implements a robust two-step authentication process based on the Needham-Schroeder protocol, utilizes end-to-end encryption at the remote procedure call layer, and combines comprehensive access control lists with standard UNIX mode bits to deliver secure, scalable, and transparent distributed file sharing.

## Design

The system separates components into Virtue (untrusted client workstations running Venus) and Vice (trusted central servers).
A secure Remote Procedure Call (RPC) mechanism is the foundation, offering four levels of security ranging from unencrypted to fully encrypted.
Authentication uses a two-step scheme where a user provides a password once at login to an Authentication Server.
The server issues a pair of authentication tokens (a secret token and a clear token) that Venus uses to transparently authenticate with file servers.
The protection domain consists of users and groups, with permissions granted through Access Control Lists (ACLs) managed by Vice.
Virtue maps these ACLs to UNIX file semantics, using the standard UNIX owner mode bits to indicate readability, writability, and executability for files, while Vice enforces the actual ACLs.
End-to-end encryption is used to prevent network eavesdropping and message modification.

## Evaluation

At the time of evaluation, the system was deployed to over 400 workstations serving 1,200 active users.
The file system stored 15 gigabytes of data spread over 15 servers.
End-to-end encryption using software DES was too slow (under 10 KB/s), so the project developed prototype hardware encryption devices.
With the prototype hardware, the asymptotic encryption rate reached about 200 KB/s, maintaining an RPC latency of 20 to 25 milliseconds and a file transfer rate of 50 to 70 KB/s.

## Limitations and critiques

The system does not fully solve resource denial attacks, such as flooding the network or exhausting server CPU cycles.
If a workstation is physically compromised, a malicious user can install a Trojan horse to capture passwords before they are encrypted.
Software encryption proved to be an intolerable performance bottleneck, making expensive hardware encryption a strict requirement for the system to be viable.
Tokens expire after 24 hours, which can disrupt users running long computational jobs if they forget to re-authenticate.
The system does not gracefully handle remote CPU usage, leading to conflicts when users attempt to utilize idle cycles on other workstations.

## What it led to

The Andrew File System (AFS) became a highly influential standard for distributed file systems.
Its core security assumptions regarding untrusted networks and trusted servers shaped the evolution of modern distributed security.
The token-based authentication scheme directly influenced Kerberos in Project Athena and the Distributed Computing Environment (DCE).

## Exam angles

<details>
<summary>Why does Andrew not trust the Virtue workstations, and how does this affect its security design compared to timesharing systems?</summary>
In a large, public campus environment, any workstation can be physically tampered with, booted with a modified operating system, or tapped at the network level.
Unlike timesharing systems where the central mainframe is physically secure and completely controls the software, Andrew must assume the client software is potentially hostile.
This forces Andrew to push all access control enforcement to the trusted Vice servers and rely entirely on end-to-end encryption rather than trusting the host operating system.
</details>

<details>
<summary>Describe the two-step authentication process in Andrew and why tokens are used instead of sending passwords for every connection.</summary>
First, the user supplies their password to an Authentication Server via secure RPC to obtain a pair of tokens (clear and secret).
Second, the Venus client uses these tokens to establish authenticated RPC connections to individual Vice file servers as needed.
This process avoids storing the user's password in the clear on the workstation and prevents the user from having to repeatedly type their password for every new server connection.
</details>

<details>
<summary>How does Andrew reconcile UNIX file protection semantics with its Vice access control lists?</summary>
Vice enforces access using its own Access Control Lists (ACLs) evaluated at the directory level, but Virtue must emulate UNIX semantics so standard applications continue to work.
Virtue caches protection information and uses the UNIX owner mode bits on files to represent the permissions (read, write, execute) granted by the Vice ACL.
The mode bits tell the user what can be done to the file, while Vice ACLs dictate who can do it, effectively merging the two security models.
</details>

<details>
<summary>What is the role of encryption in Andrew, and why was hardware encryption deemed necessary?</summary>
End-to-end encryption prevents eavesdroppers from reading network traffic and stops attackers from injecting or modifying packets.
Software encryption using DES was found to be far too slow, running at less than 10 KB/s and causing an intolerable performance bottleneck.
Hardware encryption was necessary to reach speeds of 200 KB/s, which kept RPC latency low and maintained acceptable file transfer rates.
</details>

## Related

- Lessons: [L11a](../Part-5-Internet-Scale-Real-Time-and-Security/L11a-Principles-of-Information-Security.md), [L11b](../Part-5-Internet-Scale-Real-Time-and-Security/L11b-Security-in-the-Andrew-System.md)
