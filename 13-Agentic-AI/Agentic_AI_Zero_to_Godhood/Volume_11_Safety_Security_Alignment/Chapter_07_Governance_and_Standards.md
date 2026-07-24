# Chapter 07 - Governance and Standards

## What you will master

- The OWASP Top 10 for LLM Applications and its agentic additions, and how to use it as a coverage checklist rather than a compliance ritual.
- The NIST AI Risk Management Framework and its Govern/Map/Measure/Manage structure, and what it actually asks of an engineering team.
- The EU AI Act where it touches agent deployments: risk tiers, obligations, and timelines.
- Model provider usage policies as a binding constraint on what you may build.
- Responsible scaling policies at a high level, and why frontier-lab safety commitments matter to a downstream engineer.
- Internal governance that works: review boards, deployment gates, and incident processes sized to the risk.

Governance is where the engineering of the previous six chapters meets organizational and legal reality.
The failure mode is treating governance as paperwork disconnected from the system; the goal is governance that changes what you build.
Regulatory and standards details are current to early 2026 and are the most perishable content in this volume, so verify current text and timelines before relying on any specific obligation.

## 1. Why an engineer should care about governance

Three reasons, in order of how often they bite.

First, governance frameworks are distilled checklists of failure modes other people already suffered.
The OWASP list in particular is a fast way to find the gap in your threat model, and using it as a coverage check is cheap and high-yield.

Second, some of it is legally binding.
The EU AI Act, sectoral regulation, and your model provider's usage policy constrain what you may deploy, and discovering a constraint after you shipped is expensive.

Third, governance is how safety survives personnel turnover and schedule pressure.
An individual engineer's judgment does not scale or persist; a review gate that requires a threat model before an agent gets production credentials does.
The controls in Chapters 03 through 06 only stay in place if some process keeps them there.

## 2. OWASP Top 10 for LLM Applications

The Open Worldwide Application Security Project published a Top 10 for LLM Applications starting in 2023, with a substantially revised list for 2025 that added agentic concerns.
It is the most practically useful security checklist in this space because it is concrete, community-maintained, and organized around what actually goes wrong.

The 2025 list, with the agent-relevant framing:

- **LLM01 Prompt Injection.** Chapter 02 in its entirety. Consistently ranked first because it is both ubiquitous and unsolved.
- **LLM02 Sensitive Information Disclosure.** The agent reveals data it should not: secrets in context, other users' data, training data, system prompts. Chapters 01 and 03.
- **LLM03 Supply Chain.** Compromised models, datasets, plugins, MCP servers, and libraries. A malicious third-party tool is an adversary in your threat model (Chapter 01).
- **LLM04 Data and Model Poisoning.** Corrupting training or fine-tuning data, or a RAG corpus, to change behavior. For agent builders the practical case is poisoning a knowledge base the agent retrieves from (Volume 05).
- **LLM05 Improper Output Handling.** Downstream systems consuming model output without validation, yielding injection into SQL, shell, or a browser (XSS from rendered output). This is the classic bridge from LLM risk to conventional appsec, and Chapter 04's output guardrails address it.
- **LLM06 Excessive Agency.** Directly the agent problem: too many tools, too broad permissions, too much autonomy. Chapter 03 is the mitigation, and this entry is OWASP's acknowledgment that agents raised the stakes.
- **LLM07 System Prompt Leakage.** Treating the system prompt as a secret and having it extracted. The deeper lesson matches Chapter 01: do not put secrets or security-critical logic in the prompt, because it is not a boundary and it leaks.
- **LLM08 Vector and Embedding Weaknesses.** RAG-specific risks: embedding inversion, cross-tenant leakage in shared vector stores, retrieval poisoning.
- **LLM09 Misinformation.** The model produces confident falsehoods that humans or systems act on. Connects to sycophancy and grounding (Chapters 04 and 05).
- **LLM10 Unbounded Consumption.** Denial of service and denial of wallet: unbounded token spend, runaway loops, resource exhaustion. Chapter 06's resource caps.

How to use it: walk the list against your system and, for each entry, name your control or consciously accept the risk.
The value is coverage - it surfaces the category you forgot - not prescription.
Do not treat it as sufficient; it is a floor, and a system that addresses all ten can still be insecure if the controls are shallow.

OWASP also publishes agent-specific guidance (agentic security initiative material through 2024-2025) covering threats like agent hijacking, tool misuse, memory poisoning, cascading failures in multi-agent systems, and identity and privilege management for non-human actors.
Read it alongside the Top 10 if you build agents specifically.

## 3. NIST AI Risk Management Framework

The US National Institute of Standards and Technology published the AI Risk Management Framework (AI RMF 1.0) in January 2023, with a Generative AI Profile added in July 2024.
It is voluntary, not regulation, but it is widely referenced, often contractually required, and structurally sound.

The framework organizes work into four functions:

- **Govern.** Establish the policies, roles, accountability, and culture. Who decides what ships, who owns risk, what the escalation path is. This is the function most organizations skip and most need.
- **Map.** Understand context and identify risks. What is the system for, who does it affect, what could go wrong. This is the threat modeling of Chapter 01, formalized and documented.
- **Measure.** Analyze and track risks with metrics and evaluation. Volume 10's evaluation work, plus the monitoring of Chapter 06.
- **Manage.** Act on risks: prioritize, mitigate, monitor, respond to incidents.

What it asks of an engineering team, concretely: document the system's purpose and affected parties, enumerate risks, define and measure metrics that track those risks, implement mitigations, and maintain an incident process, all with named accountable owners.

Its honest limitation is that it is a process framework, not a technical standard.
It tells you to measure risk; it does not tell you that your agent needs an egress allowlist.
Use it for organizational structure and pair it with the concrete technical content of the previous chapters, or you get well-documented insecure systems.

## 4. The EU AI Act

The EU AI Act entered into force in August 2024 as the first comprehensive AI regulation, with obligations phasing in over the following years.
It applies extraterritorially: if your system's output is used in the EU, it can reach you regardless of where you are.

Its structure is risk-tiered:

- **Prohibited practices.** A small set of banned uses (certain manipulative techniques, social scoring, some biometric categorization and untargeted facial-image scraping). Prohibitions applied from February 2025.
- **High-risk systems.** AI used in enumerated sensitive domains (employment, education, credit, essential services, law enforcement, critical infrastructure, and safety components of regulated products). These carry the heavy obligations: risk management systems, data governance, technical documentation, logging, human oversight, accuracy and robustness and cybersecurity requirements, and conformity assessment.
- **Limited risk / transparency obligations.** Systems that interact with humans or generate synthetic content must disclose that fact. A user-facing agent generally must make clear it is an AI, and synthetic media must be marked.
- **Minimal risk.** Everything else, with no specific obligations.

Separately, obligations on general-purpose AI (GPAI) model providers - transparency, documentation, copyright policy, and additional obligations for models with systemic risk - began applying in August 2025.

Where it touches agents specifically:

- **Human oversight is a legal requirement for high-risk systems**, not just good practice. Chapter 06's approval gates and kill switches become compliance artifacts.
- **Logging and traceability** are required for high-risk systems, matching Chapter 06's audit trails.
- **Transparency**: an agent interacting with people must generally disclose it is an AI.
- **Accuracy, robustness, and cybersecurity** requirements for high-risk systems map onto the technical controls of Chapters 03 and 04.
- Deploying an agent in a high-risk domain moves you from "we should have oversight" to "we must document our oversight," and the documentation burden is substantial.

Timelines and interpretive detail were still settling through 2025 and into 2026, including discussion of adjustments to phase-in dates, so treat every date here as needing verification and consult counsel for an actual compliance determination.
This chapter orients you; it is not legal advice.

## 5. Model provider usage policies

Before any regulation applies, your model provider's terms bind you contractually, and they are the constraint engineers most often overlook.

Every major provider (Anthropic, OpenAI, Google, and others) publishes a usage policy prohibiting categories of use, and these typically cover generating certain harmful content, weapons development, critical-infrastructure manipulation, surveillance and profiling in prohibited ways, impersonation and fraud, and various abusive applications.
Providers commonly add requirements for high-stakes domains, such as requiring human review for legal, medical, or financial advice, and disclosure that users are interacting with AI.

For agent builders the operative points:

- **You are responsible for what your agent does with the model.** Autonomy does not transfer responsibility to the provider. If your agent, injected or not, produces prohibited output or takes a prohibited action, that is your policy violation.
- **Some capabilities carry extra requirements.** Computer use, code execution, and autonomous action often come with provider guidance on sandboxing and human oversight, which becomes a contractual reason to implement Chapters 03 and 06 even where you were otherwise inclined to skip it.
- **Enforcement is real.** Violations can cost you API access, which for a production system is an availability incident.

Read the current policy of the provider you use, and re-read when you add a capability such as autonomous action or a new domain.
Policies changed repeatedly through 2024-2025 as agentic capabilities shipped.

## 6. Responsible scaling policies

Frontier labs publish frameworks describing how they gate the development and release of increasingly capable models against safety evaluations.
Anthropic's Responsible Scaling Policy (first published 2023, revised subsequently) defines AI Safety Levels (ASL) with capability thresholds and corresponding required safeguards, committing to not deploy or continue scaling past a threshold without the matching protections.
OpenAI's Preparedness Framework and Google DeepMind's Frontier Safety Framework serve analogous roles with different structures.

Why a downstream engineer should care, given you do not train frontier models:

- **They tell you what the labs think the dangerous capabilities are.** The tracked categories (bio, cyber, autonomy) are a map of what capable models might enable, which informs your own threat modeling.
- **They constrain what will be available and under what conditions.** Deployment safeguards, access controls, and capability restrictions on frontier models affect what you can build on.
- **They model the practice you should imitate at your scale.** The core idea - define capability thresholds, evaluate against them, and gate deployment on safeguards being in place - is exactly the internal governance pattern of section 7, scaled down.

Treat them as an input to your risk picture and as a template, not as a guarantee that the models you use are safe.

## 7. Internal governance for agent deployments

External frameworks do not secure your system; your internal process does.
Build governance sized to your risk, because governance that is heavier than the risk gets routed around and becomes theater.

### 7.1 A deployment gate

The single highest-value internal control is a gate between "an agent works" and "an agent has production credentials and real authority."
Require, proportional to the agent's blast radius:

- A written threat model with the assets, adversaries, and (adversary, asset, capability) triples of Chapter 01, and an explicit trifecta analysis.
- The permission and credential design: which tools, which scopes, which autonomy level, which sandbox rung (Chapter 03).
- The guardrail and oversight design: which gates, which audit trail, which monitoring, which kill switch (Chapters 04 and 06).
- Evaluation results, including adversarial testing (Volume 10), not just happy-path accuracy.
- A named owner accountable for the agent in production.

Scale the depth to the stakes: a read-only internal agent gets a lightweight checklist, an agent that moves money or touches customer data gets a full review.

### 7.2 Review boards

For higher-risk deployments, a cross-functional review body (security, legal, product, and the engineering owner) evaluates before launch.
The value is diverse failure imagination: security sees the attack, legal sees the obligation, product sees the misuse, and each catches what the others miss.
The cost is latency, so reserve it for deployments where the stakes justify it and give lower-risk work a fast path, or teams will avoid the process by mislabeling risk.

### 7.3 Incident process

Assume incidents, because Chapters 02 and 05 guarantee residual risk.
Define in advance:

- **Detection and reporting.** How an incident is noticed (Chapter 06 monitoring) and how anyone reports a suspected one without friction.
- **Triage and severity.** Criteria for how bad it is and who is woken up.
- **Containment.** The kill switch and credential revocation, exercised.
- **Investigation.** Using audit trails to reconstruct what happened and diagnose injection versus gaming versus bug.
- **Remediation and feedback.** Fix the instance and tighten the boundary, so the class cannot recur.
- **Disclosure.** Who must be told - users, customers, regulators - and by when, which for regulated deployments is a legal question with deadlines.

Also define a path for external researchers to report vulnerabilities in your agent, because someone will find one, and you want them telling you rather than the internet.

### 7.4 Inventory and ownership

You cannot govern what you cannot enumerate.
Maintain an inventory of deployed agents with their tools, data access, credentials, autonomy level, and owner.
Shadow agents - built by a team, wired to production data, governed by nobody - are the AI-era shadow IT problem, and an inventory plus a credential-issuance chokepoint is how you prevent them.

## 8. Making governance change the artifact

The test of governance is whether it changes what ships.
Governance that produces documents while the system remains unchanged is worse than none, because it manufactures false assurance.

Signals your governance is real:

- Deployments have been blocked or materially changed by the review.
- Threat models reference specific tools, credentials, and trifecta legs of the actual system, not generic prose.
- Incidents produce boundary changes (a narrowed scope, a new gate), traceable in the code.
- The inventory matches reality, verified rather than self-reported.
- Engineers can state their agent's blast radius and worst case without preparing.

If none of these hold, you have a compliance artifact rather than a safety program, and the technical controls of the previous chapters are the only thing actually protecting you.

## 9. Claims that will rot

This is the most perishable chapter in the volume.
The OWASP list contents and version, NIST framework profiles, EU AI Act obligations and especially its timelines, provider usage policies, and lab scaling frameworks are all current to early 2026 and all change, some annually.
Verify the current text of anything you rely on, and obtain legal advice for actual compliance determinations rather than relying on this summary.
The durable content is the shape: use a community checklist for coverage, a process framework for organizational structure, a deployment gate proportional to blast radius, and an incident process that feeds back into boundaries.

## Exercises

1. Walk the OWASP Top 10 for LLM Applications against an agent you run. For each entry, name your control or explicitly accept the risk with a reason. Find at least one gap you did not know about.
2. Map the four NIST AI RMF functions onto your team's actual practice. Identify which function is weakest (it is usually Govern) and propose one concrete change.
3. Determine whether an agent you are building would be high-risk under the EU AI Act. Justify your classification against the enumerated domains, and list the obligations that would follow if you are wrong.
4. Read your model provider's current usage policy and find one requirement your system does not currently satisfy, or confirm satisfaction with evidence for each relevant clause.
5. Design the deployment gate for your organization: the checklist, the risk tiers that determine depth, and the fast path for low-risk agents. Then argue why a team would not route around it.

## Godhood check

You have mastered this chapter when you can:

- Use the OWASP Top 10 for LLM Applications as a coverage check on an unfamiliar system and identify the entries with no control, including the agentic ones such as Excessive Agency.
- Explain the NIST AI RMF's four functions and what each asks of an engineering team, plus its limitation as a process rather than technical standard.
- Determine whether a given agent deployment is likely high-risk under the EU AI Act and name the obligations that follow, while flagging that timelines and interpretation need current verification.
- State why provider usage policies bind you regardless of agent autonomy, and what changes when you add autonomous action.
- Design internal governance sized to risk - a deployment gate, a proportionate review, an incident process, an inventory - and name the signals that distinguish real governance from compliance theater.
