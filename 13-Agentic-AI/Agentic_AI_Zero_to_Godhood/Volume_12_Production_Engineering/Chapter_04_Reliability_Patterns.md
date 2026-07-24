# Chapter 04 - Reliability Patterns

## What you will master

- Retries with exponential backoff and jitter, adapted to the specifics of LLM providers and agent loops.
- Timeout budgets that compose across a multi-step session instead of fighting each other.
- Idempotency for side-effecting tools, and why it is the load-bearing wall of agent safety.
- Fallback models, graceful degradation, and circuit breakers as a coherent degradation ladder.
- Surviving provider outages and rate-limit storms, queue-based decoupling, and the honest treatment of exactly-once for side-effecting tools.

## 1. The reliability stack for agents

Agent systems inherit every failure mode of distributed systems and add two of their own: the model can fail semantically while succeeding operationally, and the loop can amplify small failures into storms because one user request fans out into many provider calls and tool invocations.
The patterns in this chapter are the classical distributed-systems toolkit, each re-derived for the agent context, because the naive transplant of each pattern breaks in a specific way.
The organizing principle: handle failures at the lowest layer that can handle them correctly, and make every layer's failure behavior explicit rather than emergent.

The layers, bottom to top: a single provider or tool call (retries, timeouts), a single step (validation, local recovery), a session (checkpoints, budgets, fallback), and the fleet (circuit breakers, queues, load shedding).
A failure that escapes one layer should arrive at the next as a typed, classified event, not as a raw exception, because every pattern below branches on failure class.

Classify failures before designing recovery.

- Transient infrastructure failures: network resets, 500/502/503 from the provider, 429 rate limits; retryable by definition.
- Permanent request failures: 400 invalid request, context-length exceeded, content-policy refusals; retrying the identical request is pure waste and often makes things worse.
- Semantic failures: the call succeeded but the output is wrong, malformed, or off-task; the retry unit is a corrected request, not the same one.
- Tool-side failures: the external system errored or timed out; retryability depends on the tool's idempotency, which is section 4's subject.

## 2. Retries with exponential backoff and jitter

The base pattern is standard: on a retryable failure, wait an interval that doubles per attempt, capped, with randomization so that a population of clients does not retry in lockstep.

```python
import random, time

def backoff_delay(attempt: int, base: float = 1.0, cap: float = 60.0) -> float:
    # Full jitter: sample uniformly from [0, min(cap, base * 2^attempt)].
    return random.uniform(0, min(cap, base * (2 ** attempt)))
```

Full jitter (sampling uniformly from zero to the exponential ceiling) is the variant to default to, because the alternative of adding small jitter to a deterministic schedule still produces synchronized waves after a mass failure, and synchronized waves are what turn a provider blip into a self-inflicted outage.
The mechanism matters: after an outage, every in-flight request fails at once, and without jitter every client returns at the same instant, re-creating the overload; jitter spreads the retry load across the window.

Agent-specific adaptations, each with its reason.

Respect explicit backpressure over your own schedule.
Providers return retry-after hints on 429 and overload responses (Anthropic and OpenAI both do as of early 2026); when present, the hint overrides your computed delay, because the provider knows its recovery time and you do not.

Classify before retrying.
Retrying a 400 or a context-length error burns quota and time with a guaranteed identical failure; these must route to a different handler (request repair, compaction, or session failure), never to the backoff loop.
Content-policy refusals are especially insidious in retry loops because they are deterministic: the same input yields the same refusal forever, and a naive loop spins until its attempt cap while emitting nothing useful.

Budget retries at the session level, not only per call.
A per-call policy of three attempts looks tame, but a 30-step session where every step retries three times against a degraded provider generates a 4x request storm exactly when the provider is least able to serve it, and multiplies your own latency and cost.
The fix is a session-scoped retry budget (for example, at most five retried calls per session) that, when exhausted, escalates to the degradation ladder of section 5 instead of continuing to hammer.

Do not blindly retry streamed calls whose output the user has seen.
A transparent retry of a half-streamed response either duplicates visible text or retracts it; the retry must happen below the streaming surface (before tokens were shown) or become a visible regeneration.

Semantic retries are a different animal and deserve their own policy.
When output fails validation (malformed JSON, schema violation, failed test), the productive retry modifies the request: append the error as feedback and re-ask, switch to a stricter output mode (structured outputs or tool-enforced schemas where the provider supports them), or escalate a model tier.
Retrying the identical prompt at nonzero temperature is a legitimate but weak lever (it resamples), and at temperature zero it is nearly pointless; measure how often each semantic-retry strategy converts a failure before trusting it, because unmeasured retry-with-feedback loops can oscillate between two wrong answers indefinitely.
Cap semantic retries low (two or three) and make the cap an explicit trajectory event that the session-level logic sees.

## 3. Timeout budgets

Timeouts exist to convert unbounded waiting into a typed failure you can handle.
The agent-specific difficulty is composition: a session contains nested operations (session contains steps, steps contain provider calls and tool calls, tools contain downstream calls), and independently chosen timeouts at each layer either starve inner operations or let outer ones hang.

The pattern that composes is deadline propagation.
The session owns a total wall-clock budget set by product context (an interactive turn might get 30 seconds, a background job 20 minutes).
Each step receives the remaining budget, reserves a margin for its own bookkeeping, and passes the remainder down to the calls it makes; any call whose minimum plausible duration exceeds the remaining budget fails fast instead of starting work it cannot finish.

```python
import time

class Deadline:
    def __init__(self, seconds: float):
        self.expires = time.monotonic() + seconds

    def remaining(self) -> float:
        return max(0.0, self.expires - time.monotonic())

    def sub_budget(self, fraction: float, floor: float, cap: float) -> float:
        # Give a child operation a bounded share of what is left.
        return min(cap, max(floor, self.remaining() * fraction))
```

Sizing rules that survive contact with reality.

Size per-call timeouts from measured distributions, not intuition: a timeout at roughly p99.5 of the healthy latency distribution catches hangs without amputating legitimate slow calls, and for LLM calls the healthy distribution depends on output length, so long-generation steps need proportionally longer timeouts or, better, streaming-based liveness (next paragraph).
For streamed calls, prefer an inter-token idle timeout over a total-duration timeout: a call that is actively emitting tokens is alive no matter how long it runs, while a call that has emitted nothing for 60 seconds is almost certainly stuck; a total ceiling then serves only as the backstop.
Tool timeouts must reflect the tool: a database read at two seconds, a browser automation step at 30, a sandboxed test run at minutes; a uniform tool timeout is always wrong in both directions.
And every timeout needs a defined next action (retry, degrade, checkpoint and park, or fail with partial results), because a timeout without a handler is just a slower crash.

The trade-off inherent in tight budgets: aggressive timeouts convert slow successes into failures, and for expensive long steps (a nearly-complete 5,000-token generation killed at the deadline) the waste is real.
Checkpointing partial work where possible, and setting interactive budgets by user tolerance rather than provider capability, are the mitigations; accepting a fatter tail is sometimes the right call for background work.

## 4. Idempotency for tool actions

Retries and at-least-once delivery mean every tool call in your system may execute more than once.
For reads this is a latency cost; for writes it is the difference between "sent the email once" and "sent it three times," which makes idempotency the single most important property of a side-effecting tool contract.

The mechanism is the idempotency key.
The orchestrator generates a unique key per logical action (not per attempt), passes it with every attempt of that action, and the tool's backend deduplicates: the first execution records the key with its result, and subsequent attempts with the same key return the recorded result without re-executing.
Payment APIs standardized this pattern (Stripe's Idempotency-Key header is the canonical example), and your internal tools should implement the same contract: key storage with a TTL comfortably longer than any plausible retry horizon, atomic check-and-record so concurrent duplicates cannot both execute, and stored results so replays return the original outcome rather than an error.

Agent-specific rules layered on top.

Derive the key from the trajectory position, not from a fresh random draw at call time.
A key like hash(session_id, step_index, tool_name, canonical_args) means that a crashed-and-replayed step (Chapter 05's durable execution replays make this routine) reuses the same key and deduplicates, whereas a random key minted on each attempt defeats the entire mechanism.
Include the arguments in the key so that a genuinely different action (the model corrected the recipient and re-called the tool) executes rather than being swallowed by deduplication.

Classify every tool in its definition metadata as safe (read-only), idempotent-by-nature (set-to-value operations like "set status to closed"), idempotent-by-key (create/send/post operations wrapped with keys), or non-idempotent-and-unwrapped.
The orchestrator's retry policy branches on this field: free retries for the first two classes, keyed retries for the third, and no automatic retries at all for the fourth, which instead surfaces to the model or a human with "the call may or may not have happened," the honest state.
Prefer converting tools into the second class where you control the API: "append item" is dangerous under retry while "put item with client-generated id" is safe, and this one design change eliminates a whole failure category.

The unavoidable gap: third-party APIs without idempotency support.
For these, the wrapper pattern is check-then-act with verification: before retrying an ambiguous write, query whether the effect already exists (was the message sent, does the record exist), and only re-execute on confirmed absence.
This has a race window and costs an extra read, and where the third party offers neither idempotency keys nor a way to check, the correct engineering answer is to not auto-retry that tool at all and to say so in its definition so the model treats failures as ambiguous.

## 5. Fallbacks and graceful degradation

Design degradation as an explicit ladder, ordered from full service to refusal, with defined triggers for each descent.
The alternative, improvised behavior under duress, reliably produces the worst outcome: silent quality collapse.

A representative ladder for an agent product:

1. Primary model, full capability.
2. Same provider, adjacent model tier: on primary-model overload or elevated error rate, route to the sibling model (for example, a Sonnet-class model standing in for an Opus-class one, in Anthropic's lineup as of early 2026); prompt compatibility is highest within a provider family, so this rung degrades quality least.
3. Secondary provider: a different vendor's model behind a translation layer; this rung survives whole-provider outages but costs real engineering (different APIs, different tool-calling conventions, separately maintained prompt variants, forfeited prompt caches) and should be exercised regularly or it will not work when needed (Chapter 06 weighs the multi-provider decision fully).
4. Reduced autonomy: disable long trajectories and expensive capabilities, serve short bounded interactions only, queue heavy jobs for later; this trades functionality for stability while staying honest with users.
5. Deferred service: accept and durably queue requests with an explicit "delayed" acknowledgment, processing when capacity returns; correct for background workloads and far better than erroring.
6. Refusal with a clear message; the floor, and still a designed state with its own copy and telemetry rather than a raw 500.

Rules that make the ladder work.
Descend automatically but ascend cautiously: triggers down are fast (error-rate thresholds, circuit-breaker state), recovery up is gradual and probe-based, because flapping between rungs is worse than sitting one rung low for ten minutes.
Mark degraded sessions in telemetry and, where quality visibly differs, in the UX; silent tier-downgrades corrupt your quality metrics (a bad week of judge scores that was actually a fallback week) and quietly burn user trust.
Test every rung in anger: scheduled game days that force rung 3 and rung 5 in production-like conditions are the only way to know the translation layer and the deferred queue actually function; an unexercised fallback is a hypothesis, not a capability.
And accept the standing cost: multi-rung readiness means maintaining prompt variants, running periodic cross-model evals, and paying for capacity you rarely use, which is exactly why the ladder should be as short as your availability requirements allow, and no shorter.

## 6. Circuit breakers

A circuit breaker stops calling a dependency that is failing, converting slow cascading failure into fast local failure and giving the dependency room to recover.
The classic three states apply unchanged: closed (calls flow, failures counted), open (calls fail immediately or route to fallback, no load reaches the dependency), half-open (a trickle of probe calls tests recovery, success closes the breaker, failure re-opens it).

Agent-specific design points.

Scope breakers per dependency and per model, not globally: the Opus-class endpoint being overloaded says nothing about the Haiku-class endpoint or about your vector database, and a global breaker turns a partial outage into a total one.
Trip on more than error rate: for LLM endpoints, sustained latency inflation and elevated truncation or refusal rates are degradation signals that precede hard errors, and a breaker that watches only 5xx opens late.
Wire the open state into the degradation ladder rather than into raw failure: an open breaker on the primary model is precisely the trigger for rung 2 of section 5, which is how the patterns compose into one system.
Give tool dependencies breakers too: an external API in a failure loop otherwise gets hammered by every session whose model keeps retrying the tool, and the breaker's fast failure is also better information for the model ("this tool is currently unavailable" as a tool result) than a timeout, because the model can plan around a stated outage but interprets repeated timeouts as something to keep retrying.

The known downsides: thresholds are tuning-sensitive (too twitchy and you flap, too sluggish and the breaker adds nothing), half-open probes sacrifice a few real requests as canaries, and breakers add a piece of distributed state that must itself be shared correctly across your fleet (a per-process breaker in a 200-worker fleet opens 200 times slower than a shared one).

## 7. Provider outages and rate-limit storms

Providers are the deepest dependency of an agent product, and as of early 2026 every major provider has had visible degradation incidents; the design assumption must be that provider errors and throttling are routine weather, not exceptional events.

The storm dynamics to internalize: when a provider degrades, three amplifiers activate simultaneously.
Your retries multiply request volume (bounded only by the budgets of section 2), your queued sessions pile up and arrive together when service resumes (the thundering herd, bounded only by jittered, rate-limited drain), and your users retry at the human layer by resubmitting stalled requests (bounded only by UX that acknowledges the incident and dedupes resubmissions).
A system with none of these bounds turns a 10-minute provider blip into a multi-hour self-inflicted incident, which is the single most common reliability post-mortem in the field.

The composed defense, using this chapter's parts: classified retries with session budgets and honored retry-after (section 2), per-model breakers tripping the degradation ladder (sections 5 and 6), admission control at the front door so new work is deferred rather than admitted into a degraded system, and a drain policy that resumes queued work slowly with jitter and priority ordering rather than all at once.
Rate-limit handling specifically benefits from client-side pacing: a token-bucket limiter in your orchestrator set just under your provider quota converts a chaotic mix of 429s and retries into a smooth queue you control, and it is the difference between consuming your quota and fighting it (Chapter 06 covers quota shapes and capacity planning).

## 8. Queue-based decoupling

Synchronous request-response couples your availability to every downstream dependency's worst moment.
Putting a durable queue between request acceptance and agent execution decouples them: acceptance is a cheap, highly-available write, execution is an asynchronous worker pull, and the queue absorbs bursts, outages, and retries as depth rather than as user-facing errors.

What the queue buys, concretely: burst absorption (spiky agent traffic meets fixed worker capacity, Chapter 06), retry orchestration (failed sessions re-enqueue with backoff instead of blocking a caller), outage bridging (during rung-5 degradation, work accumulates instead of failing), priority scheduling (interactive-adjacent work ahead of batch), and crash recovery (a worker death returns the message to the queue via visibility timeout).
What it costs: latency (queue hop plus scheduling delay, unacceptable for chat-style turns, fine for anything a user expects to take minutes), infrastructure and operational surface (the queue, its dead-letter handling, its monitoring), and the exactly-once problem, which the queue does not solve but relocates (section 9).

The agent-specific queue details that bite.
Visibility timeouts must exceed the longest legitimate session or be extended by worker heartbeats, because a 20-minute session against a 5-minute visibility timeout gets redelivered mid-flight and executed twice; heartbeat-based extension is the robust choice since session durations are heavy-tailed.
Dead-letter queues need an owner and a runbook: an agent session that fails repeatedly carries a user's pending work, and DLQ depth is a user-facing metric wearing an infrastructure costume.
Poison-message detection matters more than usual because some agent failures are deterministic (a prompt that always trips a policy refusal will fail identically on every redelivery), so the retry counter, not the error type, must be the escalation trigger.
The natural architecture pairs the queue with durable execution for the session internals, which is Chapter 05's territory: the queue schedules sessions, the durable engine makes each session's steps recoverable.

## 9. Exactly-once concerns for side-effecting tools

The distributed-systems folklore is correct: exactly-once delivery does not exist, but exactly-once processing is achievable as at-least-once delivery plus idempotent effects.
For agent systems the statement needs sharpening, because "effect" spans both your infrastructure and the model's behavior.

At the infrastructure layer, the recipe is the composition of earlier sections.
Every side-effecting step gets a deterministic idempotency key derived from trajectory position (section 4).
Execution follows the transactional-outbox shape: record the intent durably, execute against the external system with the key, record the outcome durably, and on crash-recovery replay, consult the record and the key store before re-executing.
The residual gap is the ambiguous window: the crash that happens after the external call was sent but before the outcome was recorded, where the effect's status is genuinely unknown; keyed APIs close this window by replay-with-same-key, and unkeyed third-party APIs leave it open no matter what you build, which is the honest limit stated in section 4.

The model layer adds a failure mode infrastructure cannot see: the model, not the infrastructure, duplicates the action.
A model that calls send_invoice, receives a slow or error-shaped tool result, and decides on its own to call send_invoice again has produced a second logical action with a second idempotency key, and every layer below will faithfully execute both.
Defenses: return unambiguous tool results ("the invoice was sent, id inv_9312" rather than a bare timeout, using the check-then-act verification of section 4 to resolve ambiguity before the model sees it), instruct explicitly that ambiguous side-effect failures must be verified rather than re-fired, and for the highest-stakes tools add a server-side semantic guard (refuse a second identical high-stakes action within a window without an explicit override flag the model must consciously set).
None of these is airtight alone; layered, they push duplicate-action rates low enough that the maturity-ladder math of Chapter 01 can justify autonomy for the tool in question.

Finally, reconciliation is the backstop for everything above.
A periodic job that compares intended effects (your outcome records) against actual external state (the provider's records, the CRM's records) catches the residue that slips through every layer, and for money-adjacent tools it is not optional; the reconciliation report's discrepancy count is the true measured rate of your exactly-once machinery's failure, and it belongs on the reliability dashboard next to the SLOs of Chapter 01.

## Exercises

1. Implement the retry layer as a small Python module: failure classification (transient, permanent, semantic, tool-ambiguous), full-jitter backoff with a cap, retry-after override, per-call attempt limits, and a session-scoped retry budget object threaded through calls. Write the unit test that proves a deterministic 400 is never retried and a 429 with retry-after is delayed accordingly.
2. Design the timeout budget tree for a research agent with a 10-minute session budget: step reservations, per-call ceilings for a frontier model with streaming (inter-token idle plus total backstop), and tool ceilings for search, fetch, and a sandboxed code run. Show the arithmetic for the worst-case path and identify which step the budget starves first.
3. Specify the idempotency contract for a create_calendar_event tool end to end: key derivation, server-side storage schema with TTL and atomicity requirements, replay semantics, and the wrapper behavior when the upstream calendar API offers no idempotency support. Include the race window analysis for the check-then-act path.
4. Write the degradation ladder and circuit-breaker configuration for a two-provider deployment: per-model breaker thresholds (error rate, latency inflation, refusal rate), rung triggers, recovery probe policy, and the telemetry fields that mark degraded sessions. State the standing monthly engineering cost you are accepting and what availability improvement justifies it.
5. Trace a crash at the worst possible moment: a worker dies after an unkeyed third-party send_sms call returns nothing and before the outcome record is written, with the session then redelivered by the queue. Walk the recovery step by step under (a) your section-9 machinery present and (b) absent, and write the reconciliation query that would catch the failure in each case.

## Godhood check

You have mastered this chapter when you can classify any failure in an agent trace into transient, permanent, semantic, or tool-ambiguous, and state the correct handling layer and pattern for each without hesitation.
You can explain why full jitter beats naive exponential backoff after a mass failure, why session-level retry budgets exist, and why deadline propagation is the only timeout scheme that composes across a nested loop.
You can write the idempotency contract for a new side-effecting tool from memory, including key derivation from trajectory position and the honest limits against unkeyed third-party APIs.
You can draw the composed picture: breakers feeding the degradation ladder, queues absorbing the storm, durable records plus keys delivering effective exactly-once, model-layer duplication defenses on top, and reconciliation underneath, and you can say which piece fails first in a specific architecture you are shown.
