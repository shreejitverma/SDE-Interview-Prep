---
type: playbook
track: [ai-eng, sde]
level:
status: draft
last_reviewed:
sources: [https://arxiv.org/abs/2406.12045, https://arxiv.org/abs/2107.03374, https://arxiv.org/abs/2306.05685, https://www.swebench.com, https://opentelemetry.io/docs/specs/semconv/gen-ai/, https://www.anthropic.com/engineering/writing-tools-for-agents]
---

# Evals, reliability, observability

What separates senior candidates.
The core message: agents are non-deterministic distributed systems; you evaluate them statistically and operate them like production services.

---

## Q28. How do you evaluate an agent?

Three layers:

1. **Final outcome:** was the task achieved?
   Check environment state where possible (tests pass, the database row is correct, the file exists), not just text similarity.
2. **Trajectory:** were the right tools called with sensible arguments?
   How many steps and tokens, any unnecessary or unsafe actions, any policy violations?
3. **Component:** tool-selection accuracy, retrieval quality, individual prompts.

**Grader types:**

| Grader | Use for | Strength | Weakness |
| --- | --- | --- | --- |
| Code (assertions, state checks, tests) | Anything verifiable | Deterministic, cheap | Only checks what you thought of |
| LLM-as-judge with a rubric | Open-ended quality | Scales to prose | Biased, needs calibration |
| Human review | Calibration, samples, high stakes | Ground truth | Slow, expensive |

**Pass rates over repeated trials.**
Run each task n times and count c successes.

- **pass@k** is the probability that at least one of k tries succeeds; unbiased estimate `1 - C(n-c, k) / C(n, k)` (from the Codex paper).
- **pass^k** (from tau-bench) is the probability that all k tries succeed; estimate `C(c, k) / C(n, k)`.
- pass^k is the one that matters for reliability: a customer does not get k tries.

**Worked example:** n = 10 trials, c = 7 successes, k = 3.
pass@3 = 1 - C(3,3)/C(10,3) = 1 - 1/120 = 0.99.
pass^3 = C(7,3)/C(10,3) = 35/120 = 0.29.
The same agent looks nearly perfect or unreliable depending on which number you report.

**Benchmarks to name:** SWE-bench Verified (coding), tau-bench and tau2-bench (tool use with a simulated user, policy compliance), GAIA (general assistant), WebArena (web), OSWorld (computer use), Terminal-Bench (terminal tasks), BFCL (function calling).
Public benchmarks pick a model shortlist; your own eval set picks the model.

Go deeper: [Eval Types and Graders](../Agentic_AI_Zero_to_Godhood/Volume_10_Evaluation_and_Observability/Chapter_02_Eval_Types_and_Graders.md), [Building Agent Evals](../Agentic_AI_Zero_to_Godhood/Volume_10_Evaluation_and_Observability/Chapter_03_Building_Agent_Evals.md), [The Benchmark Landscape](../Agentic_AI_Zero_to_Godhood/Volume_10_Evaluation_and_Observability/Chapter_04_The_Benchmark_Landscape.md).

---

## Q29. What are the pitfalls of LLM-as-judge?

Biases: position (prefers the first or second answer), verbosity (prefers longer), self-preference (prefers its own family's output), and inconsistency across runs.

Mitigations:

- Specific rubrics with binary or low-cardinality scores ("does the answer cite a source for every number: yes or no").
- One criterion per judge call rather than one holistic score.
- Swap the order in pairwise comparisons and count only consistent verdicts.
- Reasoning before the verdict.
- A different model family as the judge.
- Measure agreement with human labels (Cohen's kappa or simple agreement) on a calibration set before trusting the judge, and re-check when the judge model changes.

Go deeper: [LLM As Judge](../Agentic_AI_Zero_to_Godhood/Volume_10_Evaluation_and_Observability/Chapter_05_LLM_As_Judge.md).

---

## Q30. Why do agents fail in production?

- **Error compounding:** at 95% per-step accuracy, a 20-step task succeeds 0.95^20 = 36% of the time; at 99%, 82%.
  Every step you remove or verify helps more than a slightly better model.
- Tool misuse or hallucinated arguments.
- Infinite loops and repetition.
- Context overflow and lost instructions (context rot).
- Premature "done" (declares success without checking).
- Ambiguous specifications.
- Brittle external APIs and rate limits.
- Prompt injection.
- Gaming the verifier: editing the test, weakening a lint rule, or special-casing the check instead of fixing the bug.

**Fixes:**

- Fewer, better tools.
- Verification steps the agent must run (tests, schema checks) and verification outside the agent (a gate it cannot skip).
- Step, token, and spend budgets.
- Loop detection (the same call repeated N times).
- Checkpoint and resume.
- Human-in-the-loop for risky actions.
- Protect the verifier: my harness hook denies edits to lint and gate configs in unattended runs (see [Question Bank Q27](../Agentic-Harness/interview/Question-Bank.md)).
- Evaluation-driven iteration: every production failure becomes an eval case.

---

## Q31. How do you make agent execution durable and reliable?

- Checkpoint state after each step (LangGraph checkpointers, Temporal-style durable execution where each LLM and tool call is a recorded activity).
- Idempotency keys on side-effecting tools, so a retried call does not refund twice.
- Retries with exponential backoff and jitter for transient errors only (see [Q59](09-Coding-Round.md)).
- Timeouts per tool call and a deadline per task.
- Circuit breakers on failing dependencies.
- Fallback models and providers behind a gateway.
- Resume from the last checkpoint instead of restarting.

My trading background is directly relevant: exactly-once order semantics, recovery after a crash mid-order, and kill switches are the same problems.

Go deeper: [Reliability Patterns](../Agentic_AI_Zero_to_Godhood/Volume_12_Production_Engineering/Chapter_04_Reliability_Patterns.md), [State and Persistence](../Agentic_AI_Zero_to_Godhood/Volume_06_Memory_and_Context_Engineering/Chapter_06_State_and_Persistence.md).

---

## Q32. How do you observe and debug agents?

- Trace every step: prompts, tool calls, results, tokens, latency, cost.
  One span per LLM call and per tool call, nested under one span per agent run.
- **Tools:** OpenTelemetry with the GenAI semantic conventions, LangSmith, Langfuse, Arize Phoenix, Braintrust.
- Replay failed traces against a new prompt or model.
- Cluster failures to find patterns (by tool, by error, by user intent).
- Dashboards for cost per task, success rate, p95 latency, steps per task, and escalation rate.

**A span, sketched with GenAI semantic-convention attributes** (names follow the spec at the time of writing; check the current version):

```text
span: execute_tool lookup_order
  gen_ai.operation.name = execute_tool
  gen_ai.tool.name      = lookup_order
  duration_ms           = 84
  status                = OK
parent span: chat claude-...
  gen_ai.operation.name       = chat
  gen_ai.request.model        = <model id>
  gen_ai.usage.input_tokens   = 12400
  gen_ai.usage.output_tokens  = 310
```

Log content (prompts and outputs) separately from metadata, with redaction and retention rules, because it may contain PII.

Go deeper: [Tracing and Observability](../Agentic_AI_Zero_to_Godhood/Volume_10_Evaluation_and_Observability/Chapter_06_Tracing_and_Observability.md).

---

## Q33. How do you handle non-determinism in testing?

- Temperature 0 does not give full determinism: batched inference changes floating-point reduction order, and providers change models behind aliases.
- Mock LLM responses for unit tests of the orchestration logic (the tests in [Q55](09-Coding-Round.md) script the model).
- Record and replay tool responses.
- Statistical evaluation suites run in CI with thresholds, on pinned model versions.
- Regression sets built from production failures.
- Assert on properties (valid schema, cites a source, called the refund tool at most once), not exact strings.

---

## Expansion questions (E1-E10)

**E1. How do you build an eval set from zero?**
Start with 20-50 real tasks from logs or users, each with a success check you can automate.
Include easy cases, hard cases, and cases where the right answer is to refuse, escalate, or ask.
Grow it from production failures; a small eval that exists beats a large one that is planned.

**E2. Your change moved the success rate from 78% to 81% on 100 tasks. Ship it?**
Not on that evidence.
The 95% confidence interval half-width at p = 0.8 and n = 100 is `1.96 * sqrt(0.8 * 0.2 / 100)` = 7.8 points; at n = 400 it is 3.9 points.
Compare paired results on the same tasks (McNemar's test or a bootstrap over tasks) and run several trials per task.

**E3. How do you evaluate trajectory quality without a reference trajectory?**
Check invariants instead of exact paths: required tools called, forbidden tools not called, no duplicate side effects, step count under budget, policy respected.
Many valid paths exist; grade the outcome and the constraints.

**E4. What is the difference between offline and online evaluation?**
Offline runs a fixed suite before release; online measures live traffic (success signals, user feedback, escalations, sampled judge scores).
You need both: offline catches regressions, online catches distribution shift.

**E5. What SLOs would you set for an agent?**
Task success rate (pass^1 on a canary set), p95 end-to-end latency, cost per successful task, escalation rate, and a safety SLO (zero unapproved irreversible actions).
Alert on burn rate, like any service.

**E6. What is reward hacking in agents, and how do you catch it?**
The agent optimizes the check rather than the goal: deleting a failing test, hard-coding an expected output, catching and ignoring an exception.
Catch it with checks the agent cannot edit, diff review focused on test and config changes, and holdout tests the agent never sees.

**E7. How do you test the orchestration code itself?**
Deterministic unit tests with a scripted fake model: tool-call parsing, error feedback, loop detection, budgets, and resume from a checkpoint.
The model is a dependency; mock it like a database.

**E8. How do you avoid benchmark contamination?**
Prefer private eval sets, date-cut public ones, and tasks created after the model's training cutoff; watch for suspiciously perfect scores on public data.

**E9. How do you roll out a new prompt or model safely?**
Offline eval gate, then shadow mode (run alongside, do not act), then a canary percentage with automatic rollback on SLO burn, then full rollout.
Version prompts and models together so a rollback restores both.

**E10. What do you log for audit in a regulated setting?**
Inputs, retrieved sources, tool calls with arguments and results, model and prompt versions, approvals, and final actions, tied to a user and a request ID, retained per policy.
The goal is to reproduce what the agent saw and why it acted.

Go deeper: [Production Evaluation](../Agentic_AI_Zero_to_Godhood/Volume_10_Evaluation_and_Observability/Chapter_07_Production_Evaluation.md), [Evals Are The Bottleneck](../Agentic_AI_Zero_to_Godhood/Volume_10_Evaluation_and_Observability/Chapter_01_Evals_Are_The_Bottleneck.md).
