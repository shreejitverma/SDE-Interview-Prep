# Chapter 05 - Interoperability Protocols

## What you will master

- The layering that makes protocol discussions coherent: agent-to-tool, agent-to-agent, and agent-to-user are different problems with different protocols.
- MCP's role as the agent-to-tool layer and why it won adoption while agent-to-agent protocols have not (yet).
- A2A in technical detail: agent cards, tasks, messages, artifacts, and the discovery-and-delegation model.
- ACP and the other 2025 protocol efforts, and the consolidation that followed.
- The adoption picture as of early 2026, and a defensible answer to "do agent protocols matter yet or is this premature standardization".

Everything in this chapter is date-stamped: protocol specs and adoption facts below reflect the state as of early 2026 and will rot faster than any other chapter in this volume.

## 1. Three layers, not one debate

"Agent interoperability" conflates three distinct interface problems, and most confused takes come from mixing them.

Agent-to-tool: how an agent invokes capabilities (APIs, databases, files) that are not themselves agents.
The interface is request-response, the caller holds all the intent, and the callee is stateless from the caller's perspective.
This layer is MCP's territory, covered in full in Volume 09; this chapter covers only its position in the interoperability landscape.

Agent-to-agent: how one agent delegates work to another opaque agent, possibly operated by a different organization, without seeing its internals.
The interface must handle long-running tasks, streaming progress, multi-turn clarification, and negotiation of capabilities, none of which request-response tool calls model well.
This is the territory of A2A and its 2025-era competitors.

Agent-to-user: how an agent surfaces its state to human interfaces (approvals, progress, rich UI); efforts like AG-UI (CopilotKit, 2025) target this layer.
It matters for products but is orthogonal to the multi-agent concerns of this volume, so it gets only this mention.

The layering explains a fact that otherwise looks paradoxical: a subagent invoked through a tool interface (Chapter 04) is agent-to-agent organizationally but agent-to-tool architecturally, and it needs no protocol at all because both ends live in one process under one owner.
Protocols enter only when the two agents cross a trust or ownership boundary.

## 2. MCP: the layer that actually shipped

The Model Context Protocol, released by Anthropic in November 2024, standardizes how a model-hosting client connects to servers exposing tools, resources, and prompts, over JSON-RPC 2.0 with stdio and HTTP-based transports.
Its adoption curve is the reference case for what protocol success looks like in this space: OpenAI announced MCP support in March 2025, Google DeepMind followed in spring 2025, Microsoft integrated it across its developer stack, and the server ecosystem grew to thousands of public servers within a year of launch.
In late 2025 Anthropic announced that MCP would move to neutral open governance under the Linux Foundation umbrella, the standard endgame for a protocol that has become shared infrastructure (announced December 2025; verify current governance before citing it, as this will move).

Why MCP won is instructive for judging the agent-to-agent efforts.
It standardized the layer with the most acute M-times-N pain: every client integrating every tool source separately was already an obvious, quantifiable waste.
It had a killer app on day one (IDEs and desktop assistants wiring into local files and services) rather than a hypothetical future ecosystem.
And it demanded nothing organizational: one developer could write and run a server locally with no counterparty, no registry, and no trust negotiation.
Keep these three properties in mind; the agent-to-agent protocols lack all three, which is most of the adoption story.

## 3. A2A: the agent-to-agent bid

### 3.1 Origin and governance

Google announced the Agent2Agent protocol in April 2025 with an unusually large launch coalition, upwards of fifty named partners across enterprise software (Salesforce, SAP, ServiceNow, Atlassian and others) plus consultancies.
Microsoft announced support in Azure AI Foundry and Copilot Studio in May 2025.
In June 2025 Google donated A2A to the Linux Foundation, moving it to neutral governance early, a lesson learned from ecosystem suspicion of single-vendor protocols.

### 3.2 The technical model

A2A is JSON-RPC 2.0 over HTTPS, with Server-Sent Events for streaming and webhook-style push notifications for long-running work; a later spec revision (v0.3 era, second half of 2025) added a gRPC binding.
Its core abstractions:

- Agent Card: a JSON self-description an agent publishes at a well-known URL (originally `/.well-known/agent.json`, renamed to an agent-card-specific path in a later revision), declaring identity, endpoint, supported capabilities and skills, input and output modalities, and authentication requirements.
  The card is the discovery mechanism: a client agent fetches cards to decide which remote agent can handle a task, which makes the card a machine-readable capability advertisement and, inevitably, a new prompt-injection and fraud surface (an agent card is unverified marketing until you have out-of-band trust; Volume 11 territory).
- Task: the unit of delegation, with an explicit lifecycle (submitted, working, input-required, completed, failed, canceled).
  The input-required state is the design's most honest feature: it admits that delegated work is multi-turn, letting the remote agent pause and ask the client for clarification instead of guessing.
- Message and Part: turns exchanged within a task, with typed parts (text, files, structured data) so payloads are not forced through prose.
- Artifact: the durable outputs of a task, distinct from conversational messages, matching the message-vs-artifact split argued in Chapter 03.

Deliberate design choice worth noting: A2A treats the remote agent as opaque.
No shared memory, no shared context, no visibility into the remote agent's reasoning; you send a task and receive messages and artifacts.
This is the correct trust posture for cross-organization delegation, and it also means every warning in this volume about lossy boundaries and implicit-decision divergence applies at full strength across an A2A link, with no option to fall back to shared traces.

### 3.3 MCP and A2A are complementary, mostly

The official framing from both camps: MCP connects agents to tools, A2A connects agents to agents, use both.
The framing is broadly right, with one honest blur: a remote agent can trivially be wrapped as an MCP tool (task in, result out), and many teams do exactly that instead of adopting A2A.
What the MCP-wrapping approach loses is the long-running task lifecycle, streaming progress, and the input-required clarification loop; what it gains is riding an ecosystem that already exists.
As of early 2026, for delegations that are short and one-shot, MCP-wrapping is the pragmatic winner; A2A's differentiated value shows only when tasks are long, interactive, or cross organizational boundaries.

## 4. ACP and the rest of the 2025 field

The Agent Communication Protocol (ACP) came out of IBM's BeeAI work in early 2025: REST-first, plain HTTP with ordinary JSON, deliberately avoiding JSON-RPC on the argument that ordinary web tooling should suffice to call an agent.
It moved to the Linux Foundation, and in the second half of 2025 the ACP effort was folded into A2A rather than continuing as a competitor, the first significant consolidation in the space; as of early 2026 ACP is best understood as a historical input to A2A rather than a live choice.

Other efforts you should be able to place.
AGNTCY, initiated by Cisco with LangChain and others in 2025 as an "Internet of Agents" collective covering discovery, identity, and messaging, also moved under the Linux Foundation umbrella in late 2025; it is broader and earlier-stage than A2A.
ANP (Agent Network Protocol) is a decentralized-identity-flavored effort with primarily academic and Chinese-ecosystem traction.
Academic proposals such as Agora explore protocol negotiation, where agents converse in natural language and then agree on a compact structured protocol for repeated interactions; intellectually important, not deployed at scale.
The pattern across all of them: 2025 produced far more protocol design than protocol traffic, and the consolidation under neutral foundations (MCP, A2A, AGNTCY all at or near the Linux Foundation by early 2026) is the ecosystem hedging against fragmentation before real usage arrives.

## 5. Adoption reality as of early 2026

Separate three levels of adoption evidence, in descending order of strength: production traffic, shipped integrations, and announced support.

MCP has all three: it is the default way agent products consume external capabilities, and building a tool integration without MCP support now requires justification.
A2A has the second and third but little verified public evidence of the first: enterprise platforms ship A2A endpoints and the framework ecosystem (LangChain-family, Microsoft's agent frameworks, Google's ADK) supports it, but publicly documented cases of meaningful cross-organization agent-to-agent production traffic remain scarce.
Intra-organization multi-agent systems, where the volume you are reading lives, still overwhelmingly use in-process delegation, subagent tools, and shared workspaces rather than any wire protocol, because both ends are owned by one team and a protocol adds ceremony without adding trust.

The missing prerequisites for real A2A-style adoption are not protocol features.
They are commercial and trust primitives: agent identity that means something across organizations, authorization models for delegated actions with financial consequences, liability when a remote agent errs, payment and metering for agent services, and reputation signals that make an unknown agent card worth acting on.
Protocols can carry these once they exist; protocols cannot create them.
Emerging work on agentic payments and delegated authority (multiple industry efforts in late 2025) targets exactly this gap, and its maturation, not any spec revision, is what would change the adoption picture.

## 6. Premature standardization, or necessary plumbing?

The case that it is premature.
Standardization before usage locks in abstractions chosen by committee rather than discovered by practice; the graveyard of pre-adoption standards (much of WS-*, much of SOAP-era choreography) shows how that ends.
The dominant real multi-agent patterns today are in-process and need no wire protocol, so the standards are solving a problem most builders do not yet have.
And capability churn is extreme: model improvements keep changing what a sensible delegation boundary even is (Chapter 01's moving coupled-task frontier), so freezing interaction semantics now risks standardizing 2025's limitations.

The case that it is necessary plumbing.
MCP demonstrates that a well-timed standard in this space compounds fast, and being spec-ready before the market turn is cheap insurance for platform vendors.
Enterprise buyers demand interoperability commitments before adopting agent platforms, so the standards function as procurement collateral even ahead of traffic, which is a real economic function.
And the consolidation into neutral foundations has kept the field from fragmenting into a dozen incompatible vendor dialects, which is a genuine achievement even if the pipes are still mostly dry.

The defensible synthesis for a working engineer as of early 2026.
Treat MCP as settled infrastructure: learn it deeply (Volume 09), use it by default at the agent-to-tool layer.
Treat A2A as an option to keep cheap: design your agents with clean task-in, artifacts-out boundaries and explicit capability descriptions, which costs nothing (it is just Chapter 03 discipline) and makes future A2A exposure a wrapper rather than a rewrite.
Do not architect internal multi-agent systems around wire protocols today; the ceremony is real, the benefit inside one trust domain is not.
Revisit this judgment when you see the trust and payment primitives of section 5 shipping, because that, not spec version numbers, is the leading indicator.

## Exercises

1. Write agent cards (A2A card shape, hand-authored JSON) for two agents from Chapter 04's examples: a read-only dependency auditor and a research worker; then critique your own cards as an adversary deciding whether to trust them.
2. Wrap a simple agent as an MCP tool and sketch the same agent behind an A2A task lifecycle; list concretely which behaviors (clarification, streaming progress, cancellation) the MCP wrapping loses.
3. Take one real cross-organization delegation from your work (any vendor API acting on your behalf) and enumerate which trust primitives from section 5 it relies on and how each was established; this calibrates what agent-to-agent adoption actually requires.
4. Design the task lifecycle state machine for a delegation that can pause for human approval on the client side and clarification on the server side, and compare your states against A2A's; explain any state you needed that it lacks.
5. Write a one-page position memo, dated today, on "should our platform expose an A2A endpoint this quarter", committing to a recommendation with explicit reversal conditions.

## Godhood check

- Name the three interface layers and place MCP, A2A, and AG-UI on them without hesitation.
- Explain the three properties that drove MCP adoption and show which ones A2A lacks.
- Describe the A2A abstractions (agent card, task lifecycle, message parts, artifacts) and the trust posture behind treating remote agents as opaque.
- State what wrapping a remote agent as an MCP tool loses relative to A2A, and when that loss is acceptable.
- Summarize the fate of ACP and the consolidation pattern across 2025 protocol efforts.
- List the missing non-technical primitives gating cross-organization agent traffic, and explain why protocols cannot supply them.
- Give the synthesis position on premature standardization and the observable signal that should trigger revisiting it.
