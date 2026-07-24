# Chapter 06 - Tracing and Observability

## What you will master

- Why agent systems need tracing more than traditional services do, and how the span-and-trace model maps onto agent runs.
- The OpenTelemetry GenAI semantic conventions: what they standardize, what remains unstable, and why vendor lock-in at the instrumentation layer is avoidable.
- The tooling landscape as of early 2026: LangSmith, Langfuse, Braintrust, Arize Phoenix, and W&B Weave, with the axes on which they actually differ.
- What to log: full transcripts, tool I/O, token counts, costs, timings, and the metadata that makes traces queryable.
- Privacy in logging: why agent traces are among the most sensitive data a company holds, and the redaction, retention, and access disciplines that follow.
- Debugging from traces: the systematic workflow that turns a failing run into a diagnosis.

## 1. Why agents need tracing

A traditional service handles a request through a known code path; when it fails, the stack trace points at the line.
An agent handles a request through a path the model invents at run time: which tools it calls, in what order, with what arguments, and how it interprets results are all decided per run.
The code is fine in almost every agent failure; what failed is a decision, and decisions are only visible in the record of what the model saw and produced at each step.
That record is the trace, which makes tracing the agent equivalent of the stack trace, the debugger, and the profiler simultaneously.

Three properties of agent systems raise the stakes beyond ordinary distributed tracing:

- Nondeterminism: the failing run cannot be reproduced by re-running; the trace is the only evidence that the failure ever happened in that form.
- Compounding: a run fails at step 19 because of a subtle misreading at step 3; without the full intermediate record, root-causing is guesswork about invisible state.
- Cost opacity: token spend is the dominant marginal cost, it varies per run by orders of magnitude, and it is invisible without per-call accounting; tracing is also the metering layer.

The economic argument from Chapter 2 recurs here: traces are the raw material for evals.
Replayed production contexts become unit-eval inputs, production failures become regression cases, and judge calibration samples come from real traffic.
A team that instruments tracing on day one gets its eval datasets nearly free; a team that defers it must reconstruct reality from memory.

## 2. Spans and traces for agent runs

The model comes from distributed tracing (OpenTelemetry, and Dapper before it): a trace is a tree of spans, each span a named, timed operation with attributes, linked to its parent.
The mapping onto agents is natural and worth making explicit, because a good span hierarchy is what makes traces navigable at scale.

A representative hierarchy for one agent run:

```
trace: handle_user_request            (session_id, user_id*, task metadata)
  span: agent_loop iteration 1
    span: llm_call                    (model, prompt tokens, completion tokens,
                                       cost, latency, stop reason)
    span: tool_call lookup_order      (arguments, result, latency, error)
    span: tool_call search_kb         (arguments, result size, latency)
  span: agent_loop iteration 2
    span: llm_call
    span: tool_call refund
  span: guardrail_check               (verdict, rule triggered)
  span: response_to_user
```

Design rules that experience has converged on:

- One span per model call and one per tool call, always; these are the atomic decision and action units, and everything else aggregates them.
- The trace carries session identity: multi-turn conversations need conversation-level grouping above run-level traces, because many failures (context pollution, memory errors) only appear across turns.
- Sub-agents nest as child spans under the delegating span, so a multi-agent run reads as one tree rather than as disconnected traces; propagate the trace context across process boundaries exactly as in microservice tracing.
- Attributes carry the queryable dimensions: model ID, prompt version, agent version, task category, environment (prod, staging, eval); a trace you cannot filter by prompt version cannot answer "did the new prompt cause this".
- Record errors as span events with the raw provider error, not a sanitized summary; rate-limit errors, truncation stop reasons, and refusals are distinct failure classes that sanitization merges.

The eval harness from Chapter 3 and production share this structure: an eval run is a trace like any other, with task_id and trial attributes, which is what makes offline and online analysis use the same tooling.

## 3. OpenTelemetry GenAI conventions

OpenTelemetry (OTel) is the vendor-neutral standard for traces, metrics, and logs, and since 2024 its semantic-conventions project has been extending into generative AI; as of early 2026 the GenAI conventions cover model-call spans (attributes like `gen_ai.operation.name`, `gen_ai.request.model`, `gen_ai.usage.input_tokens`, `gen_ai.usage.output_tokens`), event payloads for prompts and completions, and draft conventions for agent and tool spans.
The honest status report: the conventions remain marked experimental, the agent-span portions are still moving, and vendors differ in how faithfully they map to them.

Why you should instrument against OTel anyway, stated as a bet with named downsides:

- The payoff: instrumentation is the expensive, invasive part (it touches every call site), while backends are swappable; OTel-shaped telemetry can be exported to nearly every tool in Section 4, so the standard converts a marriage into a rental agreement.
- The cost: experimental conventions rename attributes across versions, so pin the convention version and expect migration toil; and the lowest-common-denominator schema loses some vendor-specific richness, which vendors mitigate with additive custom attributes.

The capture-mechanics decision that matters in practice: prompt and completion content is large and sensitive, so the conventions separate span metadata (always cheap to record) from content events (optionally recorded, optionally redacted).
Decide deliberately which environments record content, because the default of recording everything everywhere is a privacy incident in waiting (Section 6), and the default of recording nothing makes traces undiagnosable.

## 4. The tooling landscape as of early 2026

Five tools dominate the conversation; all five ingest traces, render them as navigable trees, track tokens and costs, support datasets and evals, and offer LLM-judge integration, so the marketing pages read identically.
The real differentiation is on a few axes: open-source and self-hosting, framework coupling, eval-workflow depth, and enterprise ML-platform integration.
Everything here is date-stamped early 2026; this market moves quarterly.

- LangSmith (LangChain): the most polished experience if you live in the LangChain and LangGraph ecosystem, with tight automatic instrumentation of those frameworks; usable without them via SDK and OTel ingestion, but its gravity is the ecosystem; closed source, SaaS-first with enterprise self-hosting tiers.
- Langfuse: the leading open-source option (self-hostable, permissive core), framework-neutral, OTel-friendly, with prompt management, eval scoring, and dataset features; the default choice when data-residency or cost concerns argue for self-hosting, at the cost of operating it yourself and a somewhat leaner analytics layer than the best SaaS offerings.
- Braintrust: eval-first rather than tracing-first; its center of gravity is the experiment loop (datasets, scorers, side-by-side diffs of eval runs, CI integration), with tracing in support; strong choice when evaluation workflow depth is the priority; closed source, with hybrid deployment options for enterprises.
- Arize Phoenix: open-source tracing and eval library from an ML-observability company, built natively on OTel via the OpenInference conventions; strong on drift and embedding-based analysis inherited from Arize's classical ML lineage; pairs a free self-hosted core with a commercial platform.
- W&B Weave (Weights and Biases, part of CoreWeave since 2025): traces, evals, and datasets integrated with the W&B experiment-tracking universe; strongest where the team already runs W&B for training and fine-tuning, so agent evals and model training share one system of record; closed source.

Selection heuristics rather than a ranking, because the right answer is situational:

- Self-hosting or strict data residency required: Langfuse or Phoenix first.
- Deep LangGraph investment: LangSmith first.
- Eval workflow is the bottleneck and budget exists: Braintrust first.
- Existing W&B footprint: Weave first.
- Uncertain: instrument via OTel, pick any backend, and revisit in six months with your own usage data; the OTel bet from Section 3 is what makes this cheap.

The build-your-own option deserves one honest paragraph: a Postgres table of spans plus a simple viewer covers the first weeks, and some teams with unusual constraints stay custom; but replicating diffing, dataset management, and eval UI is a product, not a script, and undifferentiated infrastructure work is usually the wrong place to spend agent-team headcount.

## 5. What to log

The maximalist answer (everything) and the minimalist answer (metrics only) are both wrong; the right answer is layered, with each layer justified by the question it exists to answer.

Layer 1, always, for every call in every environment:

- Identifiers: trace, span, session, user (pseudonymous), agent version, prompt version, model ID, tool name.
- Token counts, computed cost, latency, stop reason, error payloads, retry counts.
- These are small, non-sensitive-ish, and power dashboards, alerts, cost accounting, and regression detection; there is no privacy or cost argument for dropping them.

Layer 2, full content, environment-dependent:

- Complete prompts including system prompts and tool schemas, complete completions, complete tool arguments and results, and retrieved documents.
- This layer is what makes debugging and eval-case extraction possible, and it is where all the sensitive data lives; record it fully in dev, staging, and eval environments, and in production record it under the privacy regime of Section 6 (redaction, sampling, retention limits, access control) rather than by default.
- Log the exact rendered prompt, not the template plus variables reconstruction; template-rendering bugs are a recurring failure class that reconstruction hides by construction.

Layer 3, context for interpretation:

- Feedback signals attached to the trace: thumbs ratings, edits, retries, escalations (the online signals of Chapter 7).
- Guardrail verdicts, judge scores from online evaluation, and experiment assignment (which A/B arm), so traces join cleanly to outcome analysis.

Two logging disciplines that pay for themselves:

- Log at the boundary, symmetrically: capture exactly what was sent to the provider and exactly what came back, byte-accurate, because "what the model saw" is the ground truth of every debugging session, and any transformation between capture and transmission is a place for bugs to hide.
- Make cost a first-class recorded field computed at write time from the pricing table then in force; reconstructing historical costs after a price change is miserable, and per-trace cost is the metric that catches runaway-loop incidents fastest.

## 6. Privacy in logging

An agent trace is a recording of a user's interaction plus everything the agent retrieved on their behalf: messages, documents, account data, sometimes credentials pasted by users who did not know better.
Trace stores therefore concentrate exactly the data your security team spends its life protecting, in a new store that default tooling configurations happily retain forever; treat the trace store as a tier-one sensitive system from day one, because retrofitting is far harder.

The disciplines, in decreasing order of leverage:

- Minimize at capture: redact known-pattern secrets and PII (API keys, card numbers, government IDs, emails) before the trace leaves the process, with detector-based scrubbing in the exporter pipeline; capture-side redaction is the only kind that guarantees the data never lands anywhere.
- Sample content: full-content logging of a fraction of production traffic, plus targeted full capture for flagged sessions (errors, low feedback scores), often answers every debugging need at a fraction of the exposure surface; metadata (Layer 1) stays at 100 percent.
- Retention tiers: content expires on a short clock (days to weeks), metadata on a long one (months to years); eval-case extraction happens inside the content window, and the extracted case is then deliberately curated, consented where required, and re-redacted before it enters the long-lived eval dataset.
- Access control and audit: trace content access is role-gated and logged, because "engineer browses production conversations out of curiosity" is both a real incident pattern and, under regimes like GDPR, a compliance violation; debugging access should be purpose-bound and auditable.
- Regulatory alignment: traces are personal data under GDPR-class regimes, which brings deletion rights (user deletion must cascade into the trace store, which is why traces need user pseudonyms as keys), purpose limitation, and data-residency constraints that feed directly back into the self-hosting axis of Section 4.
- Vendor flow-through: sending traces to a SaaS observability vendor is a data-processor relationship with the same diligence obligations as any subprocessor; the redaction-before-export discipline reduces what that relationship must cover.

The tension to manage honestly: every redaction reduces debuggability, and over-aggressive scrubbing produces traces from which nothing can be learned, pushing engineers toward workarounds worse than governed access.
The stable equilibrium most mature teams reach: aggressive automatic redaction of high-confidence secret patterns, sampled full content under access control for everything else, and short content retention, revisited whenever the product's data sensitivity changes.

## 7. Debugging from traces

Trace debugging is a learnable systematic skill, not an art; the workflow below turns "the agent did something weird" into a named defect.

1. Locate the divergence: walk the span tree to the first step where the run left the good path; everything after it is usually consequence, not cause, so resist starting from the visibly broken final answer.
2. Read what the model saw: open the exact rendered prompt for the divergent call, because the majority of agent bugs are context bugs visible right there: truncated documents, a stale system-prompt version, tool results in an unparseable format, contradictory instructions accumulated across turns, or the right information present but buried.
3. Classify the failure: a small taxonomy applied consistently beats ad hoc description; a workable starter set: context defect (model saw wrong or missing information), decision defect (model saw the right things and chose badly), tool defect (tool returned wrong or malformed data), environment defect (external state differed from assumption), harness defect (retry, truncation, or parsing logic misbehaved).
- The classification routes the fix: context defects are engineering (prompt assembly, retrieval), decision defects are prompt or model work, tool defects are integration work, and only by classifying across many traces do you learn which class dominates your product, which is what should drive roadmap.
4. Test the hypothesis by replay: re-run the divergent call with the captured context, minimally edited to test the hypothesis (un-truncate the document, fix the format), and see whether the decision flips; replay of captured contexts is the agent debugging superpower, and it requires the byte-accurate boundary logging of Section 5.
5. Close the loop: the captured context plus corrected expectation becomes a unit eval case, and the task becomes an end-to-end regression case; a debugging session that does not deposit an eval case fixed one run instead of one defect.

At fleet scale, the same discipline aggregates: cluster failing traces by failure class and first-divergence span, and rank clusters by frequency times severity; the biggest cluster is the roadmap item, and this trace-cluster-to-eval-case-to-fix pipeline is the operational core of the data flywheel that Chapter 7 completes.

## Exercises

1. Instrument a small agent (the Chapter 3 harness agent suffices) with OTel-shaped spans: one span per model call and tool call, with the Layer 1 attributes from Section 5; render the resulting trace tree and verify you can answer "what did the run cost" and "where did the time go" from attributes alone.
2. Take the OTel GenAI semantic conventions document current as of early 2026 and map each attribute you logged in exercise 1 to its standard name; list the attributes you needed that the conventions do not yet cover, and add them as custom attributes with a consistent namespace.
3. Design the span hierarchy for a multi-agent system where an orchestrator delegates to two sub-agents that each call tools; show how trace context propagates and what the tree looks like when a sub-agent fails, and state which span carries the user-visible error.
4. Write a capture-side redaction function that scrubs API keys, email addresses, and card-number patterns from tool results before export; then construct a tool result where redaction destroys the information needed to debug a real failure, and propose the governed-access alternative.
5. Take five failing runs from any agent you have access to and run the Section 7 workflow on each: first divergence, what the model saw, failure class, replay test, deposited eval case; report the class distribution and what it implies about where the next week of engineering should go.
6. Draft the one-page trace-privacy policy for a support-agent product under GDPR: what is captured, what is redacted at source, content and metadata retention clocks, who can read content and under what audit, and how user deletion cascades.

## Godhood check

You have internalized this chapter when you can do the following without reference.

- Explain why traces are the agent equivalent of stack traces, and the three properties (nondeterminism, compounding, cost opacity) that make agent tracing higher-stakes than service tracing.
- Draw the span hierarchy for an agent run including sub-agents, and state the design rules: atomic spans per model and tool call, session grouping, version attributes, raw error payloads.
- Summarize what the OTel GenAI conventions standardize, their experimental status as of early 2026, and the instrument-to-the-standard bet with its named downsides.
- Compare LangSmith, Langfuse, Braintrust, Phoenix, and Weave on the axes that actually differ (open source, framework coupling, eval depth, platform integration) and give the selection heuristic for each.
- Recite the three logging layers and justify each, including boundary-symmetric byte-accurate capture and first-class cost fields.
- Argue why trace stores are tier-one sensitive systems, and list the privacy disciplines in leverage order: capture-side redaction, content sampling, retention tiers, audited access, deletion cascade.
- Run the five-step trace-debugging workflow from memory and explain why every debugging session must deposit an eval case.
