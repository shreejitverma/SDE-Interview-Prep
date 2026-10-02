---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lesson: L11
tags: [cs6210, cs6210/practice]
---

# Practice L11

Original exam-style questions for [L11a](../Part-5-Internet-Scale-Real-Time-and-Security/L11a-Principles-of-Information-Security.md), [L11b](../Part-5-Internet-Scale-Real-Time-and-Security/L11b-Security-in-the-Andrew-System.md).
Each question names the coverage ids it exercises; answers are folded so this page works as a self-test.

## Principles of Information Security

> [!question]- Q1. Compare the Saltzer and Schroeder design principles of "economy of mechanism", "least common mechanism", "open design", and "psychological acceptability". (concepts: L11a-04, L11a-07, L11a-10, L11a-11, P-Protection-Control-of-Information)
> Answer in 4-6 lines.
> Economy of mechanism dictates that security designs should be as simple and small as possible to minimize bugs.
> Least common mechanism dictates that mechanisms shared among users should be minimized to prevent unintended communication channels or interference.
> Open design argues that the security of a system should not rely on the ignorance of attackers (security by obscurity), but rather on the strength of the keys.
> Psychological acceptability means security mechanisms should not interfere unduly with the user's workflow, otherwise users will bypass them.

> [!question]- Q2. A system changes from a 4-digit PIN to an 8-character alphanumeric password. Calculate how the "work factor" changes for a brute-force attack. Explain how "compromise recording" supplements the work factor. (concepts: L11a-12)
> Answer in 4-6 lines.
> The work factor for a 4-digit PIN is 10,000 combinations.
> The work factor for an 8-character alphanumeric password is 62 to the power of 8 combinations.
> The work factor scales exponentially with length and complexity, drastically increasing the time an attacker needs.
> Compromise recording supplements this by reliably detecting and logging unauthorized access attempts, rather than just preventing them, allowing administrators to respond to breaches.

> [!question]- Q3. Explain why revocation is typically an O(1) operation in an Access Control List (ACL) system but O(N) or requires indirection in a capability system. Discuss delegation in both. (concepts: L11a-13)
> Answer in 5-7 lines.
> In an ACL system, permissions are stored centrally on the object itself.
> Revocation simply requires the administrator to remove the user's entry from the list.
> In a capability system, permissions are represented by unforgeable tickets held by the users.
> Revoking a specific user's access requires either tracking down all distributed copies of their ticket, maintaining a global revocation list, or changing the object's cryptographic key.
> Changing the object's key forces all legitimate users to re-acquire capabilities.
> Conversely, delegation is trivial in capability systems because users simply hand a copy of the ticket to another user.
> Delegation is difficult in ACLs because it requires the operating system to modify the central list.

> [!question]- Q4. True or False: "Separation of privilege" means that an application should run with the minimum permissions necessary to complete its task. Justify your answer. (concepts: L11a-08, L11a-09)
> The statement is false.
> Running with the minimum necessary permissions is the definition of "least privilege".
> Separation of privilege means that granting access requires multiple independent conditions or keys to be satisfied, rather than relying on a single check.
> For example, requiring both a physical smartcard and a typed password is an example of separation of privilege.

> [!question]- Q5. An operating system uses a static analyzer to verify that a user-provided script cannot access unprivileged memory before running it in the kernel. Which Saltzer and Schroeder design principles does this primarily enforce? (concepts: L11a-05, L11a-06)
> Answer in 3-5 lines.
> This primarily enforces "fail-safe defaults" and "complete mediation".
> By default, the script has no permission to run or access memory until the static analyzer explicitly proves it is safe, demonstrating fail-safe defaults.
> The analyzer also ensures that every single memory access the script could possibly make is checked and verified before execution begins, demonstrating complete mediation.

> [!question]- Q6. Define the differences between privacy, security, and protection terminology, and identify the three primary security concerns in information systems according to Saltzer and Schroeder. Discuss the levels of protection. (concepts: L11a-01, L11a-02, L11a-03)
> Answer in 5-8 lines.
> Privacy is the sociological right of individuals to determine what information about them is shared.
> Security is the overall framework and mechanisms ensuring that a system protects information.
> Protection refers to the specific mechanisms like memory bounds or ACLs implemented by the operating system.
> The three primary security concerns are unauthorized release of information (reading), unauthorized modification of information (writing), and unauthorized denial of use (denial of service).
> Levels of protection range from unprotected systems, to isolated systems, to systems with fully shared resources requiring fine-grained access controls.

## Security in the Andrew System

> [!question]- Q7. True or False: In the Andrew architecture, the Vice servers completely trust the Virtue client operating systems to enforce file access controls because the network is physically secure. Justify your answer. (concepts: L11b-01, L11b-02, L11b-11, P-Andrew-Security)
> The statement is false.
> The Andrew architecture is explicitly designed for an open, insecure campus network where clients cannot be trusted.
> Security challenges in a distributed file system mean that Vice servers must treat all Virtue clients as mutually suspicious and potentially compromised.
> Vice servers enforce all file access controls centrally; this is a core aspect of defensive design, though the system acknowledges limits regarding resource denial attacks.

> [!question]- Q8. Describe the sequence of events during the RPC session establishment and mutual authentication handshake in Andrew (the BIND process). How does the system establish session keys? (concepts: L11b-05, L11b-06, L11b-08)
> Answer in 6-8 lines.
> The login process starts when the user authenticates with the authentication server to retrieve their tokens.
> To establish an RPC session, the client initiates a BIND handshake by sending a random number encrypted with its handshake key.
> The server decrypts this, proves its identity by returning the incremented random number along with its own random number, encrypted with the handshake key.
> The client verifies the first number, increments the second number, and sends it encrypted to the server.
> Once mutual authentication is complete, the server generates a new session key and an initial sequence number, encrypts them, and sends them to the client for use in subsequent RPCs.

> [!question]- Q9. What is the difference between a Secret Token and a Clear Token in Andrew's login process? How do they leverage private-key encryption for communication? (concepts: L11b-03, L11b-04)
> Answer in 4-6 lines.
> Both tokens are obtained during the login process using private-key encryption for communication.
> The Clear Token contains the handshake key and is held by the Virtue client; it is clear to the client but must be protected from malicious local software.
> The Secret Token contains the same handshake key but is fully encrypted with a master key known only to the authentication and Vice servers.
> The client passes the opaque Secret Token to the Vice server during connection establishment so the server can extract the shared handshake key.

> [!question]- Q10. Why does Andrew use sequence numbers during RPC communication, and what specific attack does this prevent? (concepts: L11b-07)
> Answer in 3-5 lines.
> Andrew uses strictly increasing sequence numbers, initialized during the BIND handshake, to uniquely identify each RPC message.
> This acts as a nonce to prevent replay attacks.
> If an attacker intercepts an encrypted RPC request and attempts to send it again later, the server will reject it because the sequence number is older than the current expected sequence number.

> [!question]- Q11. Explain how Andrew implements its protection domain using users and groups. What happens if a user is granted negative rights in the access control lists? (concepts: L11b-09, L11b-10)
> Answer in 4-6 lines.
> Andrew's protection domain is based on users and groups, where users can belong to multiple groups to simplify permission management.
> Permissions are enforced using Access Control Lists (ACLs) attached to directories rather than individual files.
> Andrew ACLs uniquely support negative rights, which explicitly deny access.
> If a user is granted read access through a group membership, but has a negative right denying read access explicitly assigned to them, the negative right takes precedence and access is denied.
