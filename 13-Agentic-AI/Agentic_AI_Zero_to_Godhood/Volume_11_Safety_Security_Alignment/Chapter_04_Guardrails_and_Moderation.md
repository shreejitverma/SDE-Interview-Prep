# Chapter 04 - Guardrails and Moderation

## What you will master

- What a guardrail is and is not: a runtime check that surrounds the model, distinct from the model's own trained behavior and distinct from the permission and sandbox boundaries of Chapter 03.
- Input and output guardrail architectures, where each sits in the request path, and why you usually want both.
- Moderation APIs and classifier models, what they cover, and their false-positive and false-negative realities.
- Policy enforcement layers and structured refusal handling that a program can act on rather than a paragraph of prose.
- The latency and cost arithmetic of guardrails, and why a guardrail that doubles your latency budget changes the product.
- Why brittle regex guardrails fail, and how guardrails layer with permissions and sandboxing rather than replacing them.

This chapter is defensive engineering.
Guardrails are one layer in the defense-in-depth stack, valuable but limited, and the most common mistake is treating them as the whole answer.
Details are current to early 2026; the specific moderation products and their coverage change, so treat named services as examples of a category.

## 1. What a guardrail is

A guardrail is a runtime check, external to the model, that inspects an input, an output, or a proposed action and decides to allow, block, modify, or escalate it.
The defining property is that it is code or a separate classifier that runs deterministically in the request path, not a hope encoded in the system prompt.

This distinction matters because of Chapter 01's central claim: the model is not a trust boundary.
"Tell the model not to produce X" is not a guardrail, because the model can be talked out of it.
"Run the model's output through a check that blocks X before it reaches the user" is a guardrail, because the check runs regardless of what the model was persuaded to emit.
The guardrail is a boundary precisely because it does not read the attacker's instructions as instructions; it reads the model's output as data to be inspected.

Guardrails also differ from the permission and sandbox boundaries of Chapter 03, though the line blurs.
Permissions and sandboxes constrain what the agent can do (which tools, which files, which hosts).
Guardrails inspect content - what goes in and what comes out - and make allow/block decisions on that content.
A mature system has both: sandboxing bounds capability, guardrails filter content, and neither substitutes for the other.

## 2. Input and output guardrail architectures

Guardrails sit at two natural points in the request path, and the two do different jobs.

### 2.1 Input guardrails

Input guardrails inspect what enters the model before the model runs.
Placed at the front of the request, they can catch problems cheaply, before you spend a model call, and they can block or sanitize.

What input guardrails check:

- **Policy violations in user input.** Requests for disallowed content, obvious jailbreak phrasings, off-topic or abusive input.
- **Prompt-injection signatures.** The detection classifiers from Chapter 02 section 6.3, flagging instruction-shaped text in content that should be data.
- **PII and sensitive data.** Detecting and optionally redacting personal data before it reaches the model or logs.
- **Topic and scope enforcement.** Keeping a customer-support agent on supported topics, refusing to answer outside its remit.

The architectural choice is whether an input guardrail runs inline (block before the model runs) or in parallel (run alongside and cancel if it trips).
Inline adds its latency to every request; parallel hides the latency but wastes the model call when the guardrail trips.
For cheap guardrails, inline is simplest; for expensive classifier guardrails on latency-sensitive paths, parallel with cancellation is common.

### 2.2 Output guardrails

Output guardrails inspect what the model produces before it reaches the user or a downstream system.
They are the last line before content leaves your control, and they are where several Chapter 02 defenses live.

What output guardrails check:

- **Disallowed content in the response.** Toxicity, unsafe instructions, content the model should not have produced regardless of how it was prompted.
- **Exfiltration shapes.** Encoded blobs in URLs, secret patterns (API keys, tokens), references to data the user is not authorized to see - the output-filtering layer from Chapter 02 section 6.2.
- **Rendering-surface sanitization.** Stripping auto-fetch Markdown (images, non-allowlisted links), the concrete EchoLeak defense.
- **Hallucination and grounding checks.** For RAG systems (Volume 05), verifying the output is supported by retrieved sources, though this is probabilistic and partial.
- **Format and schema conformance.** Ensuring structured output matches the contract before a downstream system consumes it.

Output guardrails have an inherent cost: to check the output, you usually need the full output, so they add latency after generation, and if they run on streamed tokens they complicate streaming (you may have to buffer or retract). This is a real product trade-off (section 5).

### 2.3 Action guardrails

A third position, specific to agents, inspects a proposed tool call before it executes.
This overlaps with the permission system of Chapter 03 but operates on content: is this email body safe to send, does this database query match an allowed pattern, does this shell command fall within policy.
Action guardrails are where content inspection and capability control meet, and for consequential actions they are the checkpoint that pairs with human approval (Chapter 06).

## 3. Moderation APIs and classifiers

The workhorse of content guardrails is a classifier that scores text against harm categories.

### 3.1 Provider moderation services

Major providers ship moderation endpoints and content classifiers, current examples as of early 2026 including OpenAI's Moderation API, Azure AI Content Safety, Google's text-moderation and safety attributes, and Anthropic's safety classifiers and prompt-shield features on their platforms.
These return category scores (violence, sexual content, self-harm, hate, and often a jailbreak or injection category) that you threshold to allow or block.

Value: they are cheap or free relative to a generation call, low-latency, maintained by the provider, and cover the common harm taxonomy.
They are a sensible default first layer for content moderation.

Limits, stated honestly:

- **Coverage gaps.** They target a fixed harm taxonomy and will miss domain-specific policy violations (your company's specific rules) unless you add your own classifiers.
- **False negatives.** Adapted, obfuscated, or novel content slips through, exactly as in Chapter 02.
- **False positives.** Legitimate content trips the filter: a medical query flagged as self-harm, a security discussion flagged as malicious, a benign message in a language the classifier handles poorly. False positives are a real product cost, not a rounding error, because they block real users.
- **Language and context weakness.** Classifiers are stronger in English and on short, decontextualized snippets, and weaker on long, contextual, or multilingual content.

### 3.2 Custom classifiers

For domain-specific policy, you train or configure your own classifier, either a small fine-tuned model or an LLM-as-judge prompt (Volume 10) that evaluates content against your written policy.
LLM-based guardrails are flexible and easy to update (change the policy prompt), but they are themselves models, so they are themselves subject to injection and to the same probabilistic limits, and they cost a model call.
A guardrail that is itself an LLM reading untrusted content is a guardrail that can be injected, which is a subtle but real trap: your safety check can be turned off by the content it inspects if you are not careful about how you frame its task and isolate it.

## 4. Policy enforcement and structured refusals

Guardrails are only useful if the system does something coherent when they trip, and the handling should be structured, not a wall of prose.

### 4.1 Policy as a layer

Separate policy from the model.
Encode your rules (what is disallowed, what requires approval, what must be redacted) in a policy layer the guardrails consult, so policy can be reviewed, versioned, and updated without retraining or reprompting the model.
This mirrors the Chapter 03 principle that trust decisions belong in code: the policy is code (or data that code enforces), not a paragraph you hope the model internalized.

### 4.2 Structured refusal handling

When a guardrail blocks, the system needs a machine-actionable result, not just a refusal string.
Return a structured signal - a category, a reason code, an action (block, redact, escalate, ask for confirmation) - so the surrounding program can respond appropriately: show the user a specific message, log the event for review, route to a human, or offer a safe alternative.

The anti-pattern is a refusal that is only natural-language text, because downstream code cannot reliably branch on prose, and because a plain refusal often gives the user nothing actionable.
A well-designed refusal tells the user what was blocked and why in human terms, while emitting a structured event the system can act on and monitor.

### 4.3 Fail-open versus fail-closed

Decide, per guardrail, what happens when the guardrail itself fails (the moderation service times out, the classifier errors).
Fail-closed (block on guardrail failure) is safer but hurts availability and can block legitimate traffic during an outage.
Fail-open (allow on guardrail failure) preserves availability but means an outage silently disables your safety layer.
The right choice depends on the stakes: fail-closed for high-harm categories and consequential actions, fail-open only for low-stakes checks where availability dominates, and always alert loudly on guardrail failures so a silent fail-open does not become a permanent blind spot.

## 5. Latency and cost arithmetic

Guardrails are not free, and the arithmetic changes the product, so do it explicitly.

Each guardrail adds latency and cost:

- An input classifier adds its inference time before the model runs.
- An output classifier adds its time after generation, and if it needs the full output, it defeats streaming or forces buffering.
- An LLM-as-judge guardrail adds a full model call, potentially doubling latency and cost for that request.
- Multiple guardrails compound: three sequential checks add three latencies.

The design levers:

- **Run in parallel where possible.** Independent guardrails run concurrently, so the cost is the max latency, not the sum.
- **Tier by stakes.** Cheap heuristics on every request, expensive classifiers only when the cheap layer or the context flags risk.
- **Cache and short-circuit.** Cache guardrail results for identical inputs; short-circuit on a definitive early signal.
- **Choose the classifier size deliberately.** A small, fast moderation model on the hot path, reserving large-model judgment for escalation.

The honest trade-off: every guardrail you add makes the system safer and slower and more expensive, and past some point the latency degrades the product enough that users route around it or you lose the interaction.
A guardrail stack that adds two seconds to a conversational agent has changed what the product is.
Budget guardrail latency as a first-class constraint, measure it, and cut guardrails that do not earn their latency in actual blocked harm.

## 6. Why brittle regex guardrails fail

The tempting first guardrail is a regex or keyword list: block messages containing certain words, block outputs matching a pattern.
These have a place for narrow, well-defined patterns (a specific secret format, a specific disallowed exact string), but as a content-safety guardrail they fail, and understanding why generalizes to all brittle guardrails.

- **Language is not a regular language.** Harmful intent is not captured by keywords; the same words are benign in one context and harmful in another, and harmful content uses no flagged keyword at all. A regex cannot read context.
- **Trivial evasion.** Attackers add spaces, use synonyms, misspell, use homoglyphs, encode, or switch languages, and the regex misses all of it. Chapter 02's obfuscation techniques defeat keyword filters by construction.
- **False positives on legitimate content.** A keyword blocklist blocks the medical, legal, security, and educational uses of the same words, frustrating real users - the Scunthorpe problem, decades old and still unsolved by keyword matching.
- **Maintenance treadmill.** Every evasion spawns a new rule, the ruleset grows unbounded, and it never converges because the input space is open.

The general lesson: a brittle guardrail is one that matches surface patterns rather than meaning, and surface patterns are both over-inclusive (false positives) and under-inclusive (false negatives) against an adaptive adversary and against the natural diversity of language.
Use pattern matching only where the pattern is genuinely well-defined and stable (a credential format, an exact known string), and use classifiers or richer checks where the property is semantic.
And never rely on any single guardrail, brittle or not, which is the point of the next section.

## 7. Layering guardrails with permissions and sandboxing

Guardrails are one layer, and their limits are exactly why Chapters 03 and 06 exist.
The correct posture places guardrails in a stack where each layer catches what the others miss and no layer is load-bearing alone.

- **Permissions and sandboxing (Chapter 03)** bound what the agent can do, so that content that slips past a guardrail still cannot reach an unauthorized action or destination. If output filtering misses an exfiltration string, egress control still blocks the connection.
- **Guardrails (this chapter)** inspect content in and out, catching harmful content and closing specific channels like rendering-surface auto-fetch.
- **Human oversight (Chapter 06)** gates the consequential and irreversible actions that no automated guardrail should be trusted to approve alone.
- **Alignment (Chapter 05)** is the model's trained tendency to behave, which reduces how often the guardrails have to fire but is never itself a boundary.

The failure that this layering guards against is single-point reliance.
A team that ships a moderation classifier and calls the agent safe has one probabilistic layer between an adaptive adversary and real harm.
A team that layers a scoped credential, an egress allowlist, an output filter, a moderation classifier, and a human approval gate on irreversible actions has five independent layers, and the adversary must beat all of them.
Guardrails earn their place in that stack, and they lose their value the moment they are asked to be the whole stack.

## 8. Claims that will rot

The guardrail concept, the input/output/action positions, the regex-brittleness argument, and the layering doctrine are stable.
The specific moderation services, their category coverage, their accuracy, and their pricing are current to early 2026 and change frequently; treat named products as category examples and re-verify current capabilities and costs before building on them.

## Exercises

1. For an agent you run, place every guardrail on the input/output/action map and identify which position is missing. Add the missing one and state what it catches that the others cannot.
2. Take a moderation classifier and find one plausible false positive (legitimate content it would block) and one plausible false negative (harmful content it would miss). Explain what each reveals about the classifier's limits.
3. Compute the added latency and cost of your guardrail stack on a representative request. Identify the single most expensive guardrail and decide whether its blocked-harm rate justifies its latency.
4. Design the structured refusal for a blocked request: the user-facing message, the structured event, and the downstream branch for each of block, redact, and escalate.
5. Argue to a colleague who wants to ship a regex-only content filter why it will both over-block and under-block, using a concrete word that is harmful in one context and benign in another.

## Godhood check

You have mastered this chapter when you can:

- Define a guardrail as a runtime check external to the model and explain why "tell the model not to" is not one.
- Place input, output, and action guardrails in the request path and state the distinct job and cost of each.
- Explain what provider moderation classifiers cover and their false-positive and false-negative realities, including why an LLM-based guardrail can itself be injected.
- Do the latency-and-cost arithmetic of a guardrail stack and decide which guardrails earn their place.
- Explain why brittle regex guardrails both over-block and under-block, and articulate why guardrails must layer with permissions, sandboxing, and human oversight rather than stand alone.
