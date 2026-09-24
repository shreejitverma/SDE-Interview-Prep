---
type: playbook
track: [ai-eng, sde]
level:
status: draft
last_reviewed:
sources: [https://genai.owasp.org/llm-top-10/, https://simonwillison.net/2025/Jun/16/the-lethal-trifecta/, https://simonwillison.net/2023/Apr/25/dual-llm-pattern/, https://arxiv.org/abs/2503.18813, https://modelcontextprotocol.io/specification, https://www.federalreserve.gov/supervisionreg/srletters/sr1107.htm]
---

# Safety and security

Increasingly a pass/fail topic.
The senior answer is always defense in depth plus policy enforced in code, never "the prompt tells it not to."

---

## Q34. What is prompt injection, and how do you defend against it?

- **Direct:** the user tries to override instructions ("ignore previous instructions").
- **Indirect:** malicious instructions sit inside retrieved documents, web pages, emails, issue comments, or tool outputs.
  This is the big risk for agents, because the attacker never talks to the agent.

There is **no complete fix**, so use defense in depth:

- Least-privilege tools, scoped per task and per user.
- Treat all tool output as untrusted data, never as instructions.
- Separate privileged and quarantined LLMs: the **Dual-LLM** pattern, and **CaMeL**-style control-flow and data-flow separation, where a privileged model writes a plan from the trusted user request only, and untrusted data can fill values but never change which tools run.
- Human approval for sensitive actions.
- Output filtering, for example blocking markdown images or links to unknown domains (a classic exfiltration channel).
- Allowlisted network egress.
- Injection classifiers as one signal, not the defense.

**The "lethal trifecta"** (Simon Willison): an agent that has **access to private data**, **exposure to untrusted content**, **and a way to communicate externally** can be tricked into exfiltrating data.
Remove at least one of the three for any given task.

Go deeper: [Prompt Injection](../Agentic_AI_Zero_to_Godhood/Volume_11_Safety_Security_Alignment/Chapter_02_Prompt_Injection.md), [The Agent Threat Model](../Agentic_AI_Zero_to_Godhood/Volume_11_Safety_Security_Alignment/Chapter_01_The_Agent_Threat_Model.md).

---

## Q35. How do you sandbox an agent that executes code?

- Containers for trusted code, microVMs (Firecracker) or user-space kernels (gVisor) for untrusted code.
- No network, or allowlisted egress through a proxy.
- Resource limits: CPU, memory, disk, process count, wall-clock time.
- Read-only mounts except a scratch directory.
- Ephemeral environments, destroyed after the task.
- No secrets in the environment; a proxy injects credentials into outbound requests so the agent never sees them.
- Run as a non-root user with dropped capabilities.

My git-worktree isolation is a filesystem-level version of this: it stops agents colliding, not agents attacking.
For untrusted input I would add a container or microVM per task.

Go deeper: [Sandboxing and Least Privilege](../Agentic_AI_Zero_to_Godhood/Volume_11_Safety_Security_Alignment/Chapter_03_Sandboxing_and_Least_Privilege.md), [Code Execution As A Tool](../Agentic_AI_Zero_to_Godhood/Volume_03_Tool_Use_and_the_Agent_Loop/Chapter_07_Code_Execution_As_A_Tool.md).

---

## Q36. How do you put guardrails around agents?

| Layer | Examples | Enforced where |
| --- | --- | --- |
| Input | PII detection, jailbreak and injection classifiers, topic filters | Before the model |
| Output | Schema validation, policy checks, groundedness checks, PII redaction | After the model, before the user |
| Action | Permission tiers (read, write, destructive), confirmation for irreversible actions, spend and rate limits | In the tool layer, in code |

- **Tools:** NeMo Guardrails, Guardrails AI, Llama Guard, provider moderation endpoints, and custom validators.
- Action guardrails matter most for agents, because the damage comes from what the agent does, not what it says.
- Run cheap guardrails in parallel with the main call to hide their latency, and block the output until they pass.

**Concrete example from my harness:** a PreToolUse hook classifies every shell command and edit before it runs: it always denies hook bypasses and force-pushes to shared branches, asks a human before other destructive commands, and in unattended runs denies edits to lint and gate configs.
It has 51 unit tests, and the gate's review found fail-open bypasses in it that I fixed (see [STAR Story 1](../Agentic-Harness/interview/STAR-Stories.md)).

Go deeper: [Guardrails and Moderation](../Agentic_AI_Zero_to_Godhood/Volume_11_Safety_Security_Alignment/Chapter_04_Guardrails_and_Moderation.md).

---

## Q37. When do you put a human in the loop?

- Irreversible or high-cost actions: payments, deletions, external emails, production deploys, trades.
- Low model confidence or failed verification.
- Policy edge cases and first-time actions.

Implement it as an interrupt: persist the state, notify the approver with the proposed action and its evidence, resume on approval (LangGraph `interrupt`, or a job in a pending-approval state).
Make the approval specific ("refund $420 to order ORD-123 because the item arrived damaged, photo attached"), or reviewers rubber-stamp.

Trading analogy: pre-trade risk checks, four-eyes approval, and kill switches.

Go deeper: [Human Oversight and Reversibility](../Agentic_AI_Zero_to_Godhood/Volume_11_Safety_Security_Alignment/Chapter_06_Human_Oversight_and_Reversibility.md).

---

## Q38. OWASP Top 10 for LLM applications? Name some.

The 2025 list:

| ID | Risk | Agent example |
| --- | --- | --- |
| LLM01 | Prompt injection | A web page tells the browsing agent to email the user's files |
| LLM02 | Sensitive information disclosure | The agent returns another customer's order |
| LLM03 | Supply chain | A compromised MCP server or model file |
| LLM04 | Data and model poisoning | Poisoned documents in the RAG corpus |
| LLM05 | Improper output handling | Model output passed to a shell or SQL without escaping |
| LLM06 | Excessive agency | A support bot with a delete-account tool |
| LLM07 | System prompt leakage | Secrets or internal rules in the system prompt |
| LLM08 | Vector and embedding weaknesses | No ACL filter at retrieval; embedding inversion |
| LLM09 | Misinformation | Confident, unsupported answers |
| LLM10 | Unbounded consumption | A loop that burns tokens or calls a paid API forever |

"Excessive agency" is the one most directly about agents; its fix is fewer tools, narrower permissions, and approval for impact.
OWASP has also published agent-specific threat guidance under its GenAI Security Project.

---

## Expansion questions (S1-S10)

**S1. How do you handle secrets in an agent system?**
Never put secrets in prompts, tool descriptions, or the agent's environment.
Tools call out through a service that holds credentials and scopes them to the current user; log redaction catches accidents.

**S2. The agent acts for a user. How do you stop it acting on the wrong user's data?**
Authenticate the user outside the model, bind the user ID to the session server-side, and have every tool read the ID from the session, never from model-supplied arguments.
This closes the confused-deputy hole.

**S3. What MCP-specific attacks worry you, and what are the controls?**
Tool poisoning and rug pulls: pin and review tool definitions, alert when they change, prefer vetted servers.
Injection through results: treat results as data.
Token passthrough: a server must use its own audience-bound token for upstream calls, never forward the client's.
Over-permissioned servers: run them with least privilege and per-server scopes.

**S4. How do you red-team an agent?**
Build an attack suite: direct and indirect injections in every untrusted input channel, exfiltration attempts, privilege escalation through tool chaining, and resource-exhaustion prompts.
Run it in CI like any eval, and track attack success rate over time.

**S5. How do you prevent the agent from being used to exfiltrate data through rendered output?**
Strip or proxy images and links to non-allowlisted domains, disable automatic URL fetching of model-generated links, and apply a content-security policy in the UI.

**S6. What is memory poisoning?**
An attacker gets an instruction saved to long-term memory, where it is recalled in a later, unrelated session.
Controls: write memories only from trusted channels, show users their memories, store provenance, and treat recalled memory as untrusted.

**S7. How do you make destructive actions reversible?**
Soft deletes, dry-run modes, staged changes that need a commit step, snapshots before writes, and compensating actions recorded with each side effect.

**S8. What changes in a regulated bank?**
Data residency (on-prem or in-region models), model risk management documentation (SR 11-7: development, validation, ongoing monitoring), full audit trails, segregation of duties (the agent proposes, a human approves), change control for prompts and models, and vendor risk review for every provider and MCP server.
My BNP on-prem work and the harness's attested gate map to these directly; see harness [Question Bank Q35](../Agentic-Harness/interview/Question-Bank.md).

**S9. How do you rate-limit and budget an agent?**
Per-user and per-tenant token and dollar budgets enforced at the gateway, per-task step limits, and circuit breakers on tool errors; alert before the budget, stop at it.

**S10. Is alignment your problem as an application engineer?**
Partly: you inherit the model's behavior, but you control the environment.
Specify goals precisely, constrain actions, verify outcomes, and keep humans on irreversible decisions; do not rely on the model to refuse.

Go deeper: [Governance and Standards](../Agentic_AI_Zero_to_Godhood/Volume_11_Safety_Security_Alignment/Chapter_07_Governance_and_Standards.md), [MCP Security and Ecosystem Patterns](../Agentic_AI_Zero_to_Godhood/Volume_09_Model_Context_Protocol/Chapter_07_Security_and_Ecosystem_Patterns.md).
