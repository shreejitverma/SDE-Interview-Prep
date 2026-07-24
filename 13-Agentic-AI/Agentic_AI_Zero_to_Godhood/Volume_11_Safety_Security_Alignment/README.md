# Volume 11 - Safety, Security, and Alignment

Defensive security education for engineers who build agent systems.
The premise of this volume is that you study attacks in order to build systems that survive them.

The spine of the argument runs across all seven chapters.
Agents act on the world rather than only emitting text, which turns model output into a control signal (Chapter 01).
Prompt injection is unsolved and will remain a live risk (Chapter 02).
The model's trained good behavior is a tendency, not a boundary (Chapter 05).
Therefore safety comes from code-enforced boundaries around the model - least privilege and sandboxing (Chapter 03), content guardrails (Chapter 04), human oversight and reversibility (Chapter 06) - held in place by governance proportional to blast radius (Chapter 07).

Content is current to early 2026.
Attack techniques, defensive tooling, and especially regulatory obligations move fast, so date-stamped claims should be re-verified before you rely on them.

## Chapters

- **[Chapter 01 - The Agent Threat Model](Chapter_01_The_Agent_Threat_Model.md)** - Why agents change the security game, the lethal trifecta of private data plus untrusted content plus external communication, trust boundaries, asset and adversary enumeration, and thinking like an attacker.
- **[Chapter 02 - Prompt Injection](Chapter_02_Prompt_Injection.md)** - Direct versus indirect injection, why it is fundamentally unsolved, attack shapes, real incidents from 2023-2025 including EchoLeak-class Markdown exfiltration and tool-result injection, and defense in depth from demarcation to dual-LLM and CaMeL-style capability approaches.
- **[Chapter 03 - Sandboxing and Least Privilege](Chapter_03_Sandboxing_and_Least_Privilege.md)** - Permission systems, autonomy levels, the containment ladder from process sandboxes through containers to gVisor and Firecracker microVMs, egress control, and credential handling that assumes compromise.
- **[Chapter 04 - Guardrails and Moderation](Chapter_04_Guardrails_and_Moderation.md)** - Input, output, and action guardrail architectures, moderation APIs and classifiers, structured refusal handling, guardrail latency and cost arithmetic, why brittle regex guardrails fail, and how guardrails layer with permissions and sandboxing.
- **[Chapter 05 - Alignment For Engineers](Chapter_05_Alignment_For_Engineers.md)** - What RLHF does and does not guarantee, reward hacking and specification gaming, sycophancy, evaluation awareness, Anthropic's 2025 agentic-misalignment research, Constitutional AI, and why "the model refused" is not a security boundary.
- **[Chapter 06 - Human Oversight and Reversibility](Chapter_06_Human_Oversight_and_Reversibility.md)** - Approval-gate design and approval fatigue, audit trails, reversibility as a design axis, blast-radius limiting, kill switches, and monitoring agent actions in production.
- **[Chapter 07 - Governance and Standards](Chapter_07_Governance_and_Standards.md)** - The OWASP Top 10 for LLM Applications and its agentic additions, the NIST AI RMF, the EU AI Act where it touches agents, provider usage policies, responsible scaling policies, and internal governance that changes what ships.

## How to read this volume

Read Chapters 01 and 02 first and in order; they establish the threat model and the unsolved core problem that every later chapter responds to.
Chapters 03, 04, and 06 are the constructive defenses and can be read in any order, though 03 is the most load-bearing.
Chapter 05 can be read at any point and is the corrective for anyone tempted to rely on model behavior as a control.
Chapter 07 is best read last, once you know what the controls are that governance must keep in place.

## Related volumes

- Volume 03 covers the agent loop that makes model output a control signal.
- Volume 05 covers RAG, whose retrieval corpora are a poisoning surface.
- Volume 07 covers multi-agent systems, where injections propagate through shared state.
- Volume 09 covers the Model Context Protocol, whose third-party servers are an untrusted-content and supply-chain surface.
- Volume 10 covers evaluation and observability, which supply the measurement and monitoring this volume depends on.
- Volume 13 covers coding agents and computer use, the deployments where sandboxing matters most.
