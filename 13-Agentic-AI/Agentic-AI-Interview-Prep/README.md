---
type: moc
track: [ai-eng, sde]
level:
status: draft
last_reviewed:
sources: [https://www.anthropic.com/engineering/building-effective-agents, https://modelcontextprotocol.io/specification, https://genai.owasp.org/llm-top-10/]
---

# Agentic AI Interview Prep

A complete question bank for Agentic AI, AI Engineer, and applied-LLM interviews, ordered from most frequently asked to most specialized.
It covers 232 questions with model answers, worked numbers, runnable and tested code, and system-design walkthroughs.
Answers are tied to my own projects wherever possible.

## How to use this pack

- Every question has a short spoken answer first, then depth for follow-ups.
- **[fill in]** marks a detail only I know (exact architecture, a measured number).
  Replace it before the interview or do not raise the topic; an interviewer who catches an unbacked detail discounts everything else.
- "Go deeper" links point into the [Agentic AI Zero to Godhood](../Agentic_AI_Zero_to_Godhood/README.md) curriculum.
- For the agentic harness specifically, the evidenced pack in [Agentic Harness](../Agentic-Harness/README.md) is the source of truth; this pack links to it rather than repeating it.
- Fast-moving facts (framework names, SDK APIs, spec versions, benchmark leaders) were checked on 2026-09-23; re-check the ones the target company uses.

## The notes

| # | Note | Questions | Why it matters |
| --- | --- | --- | --- |
| 1 | [Pitch and Resume Deep Dives](01-Pitch-and-Resume-Deep-Dives.md) | Q1-Q6, R1-R12 | Asked in every loop; where most candidates lose points |
| 2 | [Core Agent Concepts](02-Core-Agent-Concepts.md) | Q7-Q20, C1-C12 | The most-asked technical questions |
| 3 | [RAG and Knowledge Systems](03-RAG-and-Knowledge-Systems.md) | Q21-Q27, G1-G10 | Almost always asked for agent roles |
| 4 | [Evals, Reliability, Observability](04-Evals-Reliability-Observability.md) | Q28-Q33, E1-E10 | Separates senior candidates |
| 5 | [Safety and Security](05-Safety-and-Security.md) | Q34-Q38, S1-S10 | Increasingly a pass/fail topic |
| 6 | [LLM Fundamentals and Inference](06-LLM-Fundamentals-and-Inference.md) | Q39-Q46, F1-F10 | Expect a few, with numbers |
| 7 | [Cost and Latency](07-Cost-and-Latency.md) | Q47-Q48, L1-L8 | Practical, often asked |
| 8 | [System Design](08-System-Design.md) | Q49-Q54, D1-D6 | One 30-45 minute round |
| 9 | [Coding Round](09-Coding-Round.md) | Q55-Q63, K1-K11 | Tested solutions to write from memory |
| 10 | [Behavioral](10-Behavioral.md) | Q64-Q69, B1-B10 | STAR stories mapped to my projects |
| 11 | [Rapid Fire and Closing](11-Rapid-Fire-and-Closing.md) | 75 definitions, questions to ask | The last hour before the interview |
| 12 | [More Frequently Asked](12-More-Frequently-Asked.md) | A1-A64 | Openers, prompting, frameworks, product, AI coding tools, opinions |

Numbering: Q1-Q69 keep the numbering of the original prep list; letter-prefixed questions (R, C, G, E, S, F, L, D, K, B, A) are the expansion.

## Priority plan when time is short

1. Rehearse **Q1-Q5 out loud**, with a timer.
   Resume stories are where candidates lose the most.
2. Nail **Q7, Q8, Q10, Q11, Q12, Q14, Q17, Q28, Q30, Q34**.
   These are the most-asked technical questions.
3. Write the **Q55** agent loop once from memory, then compare with the tested version.
4. Do one system design (**Q49** or **Q51**) out loud with a 35-minute timer, using the 9-step frame in [System Design](08-System-Design.md).
5. Research the company.
   If they use a specific framework, read its current docs; if they are in finance, lean on **Q52** and the regulated-environment answers (**Q5**, **S8**).
6. Skim the starred list at the top of [More Frequently Asked](12-More-Frequently-Asked.md), and read its framework section (A31-A40) if the company uses one of those frameworks.
7. Read [Rapid Fire and Closing](11-Rapid-Fire-and-Closing.md) last.

## Three-day plan when there is more time

| Day | Morning | Afternoon | Evening |
| --- | --- | --- | --- |
| 1 | Notes 1-2, rehearse pitches | Note 3 (RAG), code Q56-Q58 | Note 9 Q55 from memory |
| 2 | Notes 4-5 | Notes 6-7 with the worked numbers | One timed system design |
| 3 | Note 8, second timed design | Note 10, record yourself on three STAR stories | Notes 11-12 |

## Five ideas that carry most answers

1. **Simplest thing that works.**
   Single call, then workflow, then agent; agents trade cost, latency, and predictability for flexibility.
2. **Verification beats generation.**
   The bottleneck in agent systems is checking output, not producing it: tests, schemas, graders, gates.
3. **Policy lives in code, not in the prompt.**
   Anything that must hold (auth, spend limits, refund caps) is enforced outside the model.
4. **Context is a budget.**
   Every token the model sees costs money, latency, and accuracy; curate it.
5. **Measure with statistics.**
   Agents are non-deterministic; report pass rates over repeated trials with confidence intervals, not single runs.
