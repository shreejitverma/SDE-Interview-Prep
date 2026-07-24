# Chapter 03 - Building Agent Evals

## What you will master

- Outcome evaluation versus trajectory evaluation, and when each is the right lens.
- Environment design for agent evals: mock tools, seeded state, sandboxes, and the realism-versus-control trade-off.
- Task specification: what a well-formed agent eval task contains and the ambiguity failure modes of underspecified tasks.
- pass@k versus pass^k, the arithmetic of each, and why production reliability is a pass^k claim, not a pass@k claim.
- User simulation in the tau-bench style: why multi-turn evals need a simulated user, and how simulator quality caps eval validity.
- Sample sizes, variance, and statistical significance for stochastic agents, with concrete formulas.
- A worked, runnable eval harness in Python that ties all of the above together.

## 1. Outcome evaluation versus trajectory evaluation

An agent run produces two evaluable artifacts: the final state of the world (the outcome) and the sequence of steps that produced it (the trajectory).

Outcome evaluation asks: is the world in the desired state after the run.
It is the primary lens because it corresponds to what the user paid for, and because it is agnostic to strategy: an agent that solves the task an unexpected way still passes, which is correct, since penalizing unanticipated valid strategies punishes exactly the flexibility that makes agents useful.

Trajectory evaluation asks: were the steps themselves acceptable.
It matters in three situations that outcome grading cannot see.

- Cost and efficiency: two agents reach the same outcome, one in 6 tool calls and one in 60; outcome grading scores them identically, but the second is ten times more expensive and slower.
- Forbidden intermediate states: an agent that briefly emailed the wrong customer and then recalled it reaches a clean final state through an unacceptable path; side effects that touched the world matter even when later reversed.
- Diagnosis: when outcomes fail, trajectory analysis tells you where; step-level failure attribution (wrong tool chosen, right tool with wrong arguments, correct call misread) is how you convert a failing score into an engineering task.

The recommended composition: gate on outcome, constrain on trajectory, diagnose with trajectory.
Concretely, a task passes only if outcome checks pass and no trajectory invariant is violated (no forbidden tool, no unauthorized side effect, step count under a ceiling), and every failure stores the full trajectory for analysis.
The downside of trajectory constraints is that each one encodes an assumption about how the task should be solved, so keep them to genuine invariants (safety, cost, side effects) and resist encoding your favorite strategy as a constraint.

## 2. Environment design

The environment is the half of the eval you build: the tools the agent calls and the state those tools read and write.
Environment design sits on a realism-versus-control spectrum, and every point on it is a legitimate choice with a known cost.

### 2.1 Mock tools

A mock tool implements the tool interface against an in-memory or on-disk fixture instead of a real service.
The mock payment API holds a dictionary of orders; `refund(order_id, amount)` mutates it; the grader inspects the dictionary afterward.

Why mocks are the default for eval environments:

- Determinism: the same task starts from the same state every run, so score changes reflect agent changes, not environment drift.
- Safety: no real refunds, emails, or deletions happen.
- Inspectability: the grader reads final state directly instead of scraping a real service.
- Fault injection: a mock can return errors, timeouts, and malformed payloads on schedule, which is the only practical way to eval error handling.

The cost is fidelity drift: the mock encodes your beliefs about the real API, and where the beliefs are wrong, the eval validates behavior that fails in production.
Real APIs have latency, pagination quirks, undocumented error shapes, and rate limits that mocks omit unless deliberately modeled.
Two mitigations: generate mock behavior from recorded real traffic where possible (record-replay), and keep a small periodic eval against real staging services to detect fidelity drift, accepting its flakiness as the price of ground truth.

### 2.2 Seeded state

Every task begins from a defined environment state: the fixture.
Seeding discipline is what separates reproducible evals from flaky ones.

- Each task declares its full initial state: database rows, files, calendar entries, conversation history.
- State is rebuilt from the seed before every run; runs never share mutable state, because cross-task contamination produces failures that depend on execution order, which are miserable to debug.
- Seeds contain realistic clutter: a customer database with one customer is a toy; real tasks require finding the right record among near-misses (two customers with similar names, an old canceled order next to the current one), and clutter is where retrieval and disambiguation failures live.
- Time is part of state: if tasks reference "yesterday" or expiry windows, the environment must supply a frozen clock, or tasks rot as wall time advances.

### 2.3 Sandboxes

When the agent executes code or shell commands, the environment must be a sandbox: a container or VM rebuilt from an image per run, with pinned dependencies, no network by default, and resource limits.
The eval-specific requirements beyond ordinary sandboxing (covered operationally in Volume 12): snapshot-restore speed determines eval throughput, since environment reset is often the wall-clock bottleneck of a large suite; and the sandbox must expose post-run state to the grader (the diff, the test results, the filesystem) through a stable interface.
Network egress deserves an explicit policy: many real tasks need the network, but network access makes runs non-reproducible and lets contaminated resources leak in, so the common compromise is an allowlisted local mirror of the resources tasks legitimately need.

## 3. Task specification

A well-formed agent eval task contains five parts.

1. Instruction: the user-visible request, phrased the way a real user would phrase it, including the ambiguity real users produce.
2. Initial state: the seed, as above.
3. Available tools: the exact toolset, since tool availability is part of task difficulty.
4. Success criteria: outcome checks plus trajectory invariants, executable, with negative assertions.
5. Metadata: category, difficulty, provenance (synthetic, production-derived, adversarial), and creation date, which powers slicing and staleness audits later.

The recurring specification failure is criteria that resolve ambiguity the instruction did not.
If the instruction says "book me a flight to Berlin next Friday" and the criteria demand the 9am Lufthansa flight specifically, an agent that reasonably books the 11am flight fails the eval while satisfying the user.
Either tighten the instruction until the criteria follow from it, or loosen the criteria to accept every reasonable reading, and make deliberate ambiguity its own task category whose success criterion is that the agent asked a clarifying question.
The reverse failure also occurs: criteria looser than the instruction, letting degenerate outcomes pass; every task should be reviewed with both questions, "what correct behavior fails this" and "what wrong behavior passes this".

## 4. pass@k versus pass^k

These two metrics look like typographic cousins and answer opposite questions; confusing them inflates reliability claims by large factors.

Let p be the per-attempt success probability of the agent on a task, with attempts independent.

pass@k is the probability that at least one of k attempts succeeds:

```
pass@k = 1 - (1 - p)^k
```

pass^k is the probability that all k attempts succeed:

```
pass^k = p^k
```

pass@k measures capability under selection: it is the right metric when a verifier can pick the good attempt, as in best-of-k code generation against a test suite, or research settings asking whether the model can ever solve the task.
pass^k measures reliability under repetition: it is the right metric when every attempt reaches a user, which is what production deployment means; k users issuing the task each experience one attempt, and pass^k is the probability all k are served correctly.

The arithmetic makes the gap vivid.
At p = 0.9: pass@8 is about 0.99999997, while pass^8 is about 0.43.
The same agent supports the claim "essentially always solvable with retries and a verifier" and the claim "fails a majority of batches of eight consecutive users" simultaneously.
The tau-bench authors introduced pass^k precisely because agents with respectable pass@1 numbers showed steep pass^k decay, exposing consistency as a distinct axis from capability, as of the 2024 paper.

Three consequences for eval design:

- Report pass^k (or equivalently per-task success rates across repeated trials) for any agent intended for production, not just pass@1 and never pass@k alone.
- pass^k requires multiple independent runs per task, which multiplies eval cost by k; this cost is not optional, because a single run per task cannot distinguish a 60 percent task from a 100 percent task.
- The estimator matters: estimate p per task from n trials, then compute p^k per task and average across tasks; averaging p across tasks first and then raising to k is wrong, because the function is convex and task heterogeneity is the whole point.
- pass@k retains a legitimate production role where you can afford a verifier-plus-retry architecture; in that case the deployed unit is the retry loop, and you should eval the loop end to end rather than quoting pass@k of the inner agent.

## 5. User simulation

Single-turn tasks hand the agent a complete instruction; real usage is conversational, with the user revealing requirements gradually, changing their mind, and answering the agent's questions.
Evaluating multi-turn behavior requires someone to play the user, and at eval scale that someone is an LLM: the user simulator.

The tau-bench construction, from the 2024 paper and its tau2-bench successor, is the reference design.
A simulated user is an LLM prompted with a persona and a goal state ("you want to return the smaller of your two recent orders; you do not remember the order number; you will provide your email if asked"), interacting with the agent over an API-mediated conversation, while the agent works against a mocked business database governed by written policy; grading compares final database state and checks policy compliance.

What user simulation buys:

- Multi-turn coverage: clarification, correction, and negotiation behavior become evaluable at all.
- Information-gathering pressure: the agent must ask for what the instruction withholds, which single-turn tasks cannot test.
- Adversarial personas: impatient users, contradictory users, users who ask the agent to break policy; the last category is how tau-bench tests whether agents uphold rules under social pressure.

What it costs, stated bluntly: the simulator is now part of the measurement instrument, and its failures contaminate scores in both directions.
A simulator that volunteers information too readily makes the agent look better than it is; one that answers incoherently makes it look worse; one that forgets its own goal produces ungradeable episodes.
The tau2-bench work explicitly reduced simulator burden after analysis showed simulator errors were a nontrivial fraction of apparent agent failures in the original benchmark, as of 2025.
Practical discipline follows: keep simulator prompts simple and goal-driven, give the simulator private ground truth it reveals only when asked, audit a sample of transcripts every eval cycle specifically for simulator faults, and score simulator-caused failures as invalid episodes rather than agent failures.
Pin the simulator model and prompt version; upgrading the simulator is a change to your measuring stick and requires re-baselining, exactly like changing a grader.

## 6. Sample sizes and statistical significance

Agent evals are noisy: the policy is stochastic, judges are stochastic, environments flake.
Decisions made on the noise are indistinguishable from coin flips, so quantify it.

### 6.1 The width of your uncertainty

For a suite of N independent tasks scored pass/fail with observed rate p, the standard error is sqrt(p(1-p)/N), and a rough 95 percent confidence interval is plus or minus two standard errors.
Anchor numbers worth memorizing, at p near 0.7:

- N = 50: interval roughly plus or minus 13 percentage points.
- N = 100: roughly plus or minus 9.
- N = 500: roughly plus or minus 4.
- N = 2000: roughly plus or minus 2.

A 100-task suite cannot resolve a 5-point improvement; observed deltas of that size are consistent with noise.
This single fact explains most eval whiplash: teams re-run a suite, see a 4-point swing, and hunt for a cause that does not exist.

### 6.2 Paired comparison: the free lunch

The interval above treats two suite runs as independent samples, but when comparing agent A and agent B on the same tasks, the per-task pairing carries most of the information.
Count only the discordant tasks: tasks where exactly one of A and B succeeded.
If B wins 15 discordant tasks and A wins 5, the appropriate test is a sign test (equivalently McNemar's test) on 15 versus 5, which is far more sensitive than comparing the two overall rates.
Paired evaluation on identical tasks with identical seeds is the cheapest statistical upgrade available and should be the default reporting format for any A-versus-B claim.

### 6.3 Per-task repetition and variance decomposition

Running each task n times decomposes variance: across-task variance (some tasks are harder) versus within-task variance (the agent is inconsistent on the same task).
Within-task variance is exactly what pass^k measures, so n greater than 1 is mandatory for reliability claims; n between 4 and 10 per task is a common budget compromise as of early 2026 practice.
Given a fixed compute budget of R total runs, allocating them as more tasks with fewer repetitions estimates the mean success rate more precisely, while fewer tasks with more repetitions estimates consistency more precisely; know which claim you are buying with the budget before spending it.

### 6.4 Multiple comparisons and peeking

Slicing a suite into 12 categories and celebrating the one category that improved significantly is a multiple-comparisons error; at a 5 percent false-positive rate, one spurious significant category per 20 slices is expected.
Similarly, re-running the suite until the delta looks good is peeking and invalidates the nominal error rate.
The lightweight defenses: pre-register which metric gates the decision, treat category slices as diagnostic rather than confirmatory, and if a surprising slice matters, confirm it on fresh tasks.

## 7. A worked eval harness

The following is a minimal but real harness exhibiting the chapter's structure: seeded environment, mock tools, outcome checks with negative assertions, trajectory invariants, per-task repetition, pass^k estimation, and paired comparison.
It uses only the Python standard library plus an abstract `run_agent` you supply; the agent interface is a function taking a task instruction and a tool-calling environment, which any provider SDK or framework can implement.
The code targets Python 3.11+, current as of early 2026.

```python
"""Minimal agent eval harness: environment, grading, repetition, statistics."""
from __future__ import annotations

import copy
import math
import random
import statistics
from dataclasses import dataclass, field
from typing import Callable

# ---------- Environment: seeded state + mock tools ----------

@dataclass
class Env:
    """Mock business environment. Rebuilt from seed before every run."""
    orders: dict[str, dict]
    refunds: list[dict] = field(default_factory=list)
    emails_sent: list[dict] = field(default_factory=list)
    tool_log: list[dict] = field(default_factory=list)

    # Tools exposed to the agent. Each logs itself for trajectory grading.
    def lookup_order(self, order_id: str) -> dict:
        self.tool_log.append({"tool": "lookup_order", "args": {"order_id": order_id}})
        return self.orders.get(order_id, {"error": "not_found"})

    def refund(self, order_id: str, amount_cents: int) -> dict:
        self.tool_log.append({"tool": "refund",
                              "args": {"order_id": order_id, "amount_cents": amount_cents}})
        if order_id not in self.orders:
            return {"error": "not_found"}
        self.refunds.append({"order_id": order_id, "amount_cents": amount_cents})
        return {"ok": True}

    def send_email(self, to: str, body: str) -> dict:
        self.tool_log.append({"tool": "send_email", "args": {"to": to}})
        self.emails_sent.append({"to": to, "body": body})
        return {"ok": True}


@dataclass
class Task:
    task_id: str
    instruction: str
    seed: dict                      # initial state for Env
    outcome_check: Callable[[Env], bool]
    max_tool_calls: int = 15        # trajectory invariant: cost ceiling
    forbidden_tools: frozenset[str] = frozenset()


def make_env(task: Task) -> Env:
    # Deep copy so runs never share mutable state.
    return Env(orders=copy.deepcopy(task.seed["orders"]))


# ---------- Example task with negative assertions ----------

def refund_task_check(env: Env) -> bool:
    """Outcome: exactly one refund, right order, right amount, no side effects."""
    if len(env.refunds) != 1:                       # negative: no double refund
        return False
    r = env.refunds[0]
    if r["order_id"] != "ORD-1002" or r["amount_cents"] != 4599:
        return False
    if any(e["to"] != "casey@example.com" for e in env.emails_sent):
        return False                                # negative: no email to wrong party
    return True


TASKS = [
    Task(
        task_id="refund-clutter-01",
        instruction=(
            "Hi, I'd like a refund for the headphones I bought last week. "
            "My email is casey@example.com."
        ),
        # Clutter: two orders for this customer, one refundable, plus a near-miss customer.
        seed={"orders": {
            "ORD-1001": {"email": "casey@example.com", "item": "usb cable",
                         "amount_cents": 899, "days_ago": 40},
            "ORD-1002": {"email": "casey@example.com", "item": "headphones",
                         "amount_cents": 4599, "days_ago": 6},
            "ORD-1003": {"email": "kasey@example.com", "item": "headphones",
                         "amount_cents": 4599, "days_ago": 5},
        }},
        outcome_check=refund_task_check,
        forbidden_tools=frozenset(),  # e.g. frozenset({"delete_order"})
    ),
]

# ---------- Running and grading ----------

@dataclass
class RunResult:
    task_id: str
    passed: bool
    failure_reason: str
    tool_calls: int

AgentFn = Callable[[str, Env], None]  # instruction, environment -> agent mutates env

def run_once(agent: AgentFn, task: Task, rng_seed: int) -> RunResult:
    random.seed(rng_seed)             # seed any harness-side randomness
    env = make_env(task)
    try:
        agent(task.instruction, env)
    except Exception as exc:          # agent crash is a failure, not a harness crash
        return RunResult(task.task_id, False, f"agent_exception: {exc!r}",
                         len(env.tool_log))
    # Trajectory invariants first: cheap, and they veto the outcome.
    used = [c["tool"] for c in env.tool_log]
    if any(t in task.forbidden_tools for t in used):
        return RunResult(task.task_id, False, "forbidden_tool_used", len(used))
    if len(used) > task.max_tool_calls:
        return RunResult(task.task_id, False, "tool_call_budget_exceeded", len(used))
    ok = task.outcome_check(env)
    return RunResult(task.task_id, ok, "" if ok else "outcome_check_failed", len(used))


def evaluate(agent: AgentFn, tasks: list[Task], n_trials: int) -> dict:
    per_task: dict[str, list[bool]] = {}
    failures: list[RunResult] = []
    for task in tasks:
        results = [run_once(agent, task, rng_seed=trial) for trial in range(n_trials)]
        per_task[task.task_id] = [r.passed for r in results]
        failures.extend(r for r in results if not r.passed)
    return {"per_task": per_task, "failures": failures}


# ---------- Statistics: pass^k, confidence interval, paired test ----------

def pass_hat_k(per_task: dict[str, list[bool]], k: int) -> float:
    """Mean over tasks of estimated p^k. Requires n_trials >= 1 per task."""
    vals = []
    for outcomes in per_task.values():
        p = sum(outcomes) / len(outcomes)
        vals.append(p ** k)
    return statistics.mean(vals)

def mean_and_ci(per_task: dict[str, list[bool]]) -> tuple[float, float]:
    """Suite mean success rate and approximate 95 percent CI half-width."""
    rates = [sum(o) / len(o) for o in per_task.values()]
    m = statistics.mean(rates)
    se = statistics.pstdev(rates) / math.sqrt(len(rates)) if len(rates) > 1 else 1.0
    return m, 1.96 * se

def paired_report(a: dict[str, list[bool]], b: dict[str, list[bool]]) -> str:
    """Discordant-task summary for agents A and B on identical tasks and trials."""
    a_wins = b_wins = 0
    for tid in a:
        pa = sum(a[tid]) / len(a[tid])
        pb = sum(b[tid]) / len(b[tid])
        if pa > pb: a_wins += 1
        elif pb > pa: b_wins += 1
    return f"discordant tasks: A better on {a_wins}, B better on {b_wins}"


if __name__ == "__main__":
    def naive_agent(instruction: str, env: Env) -> None:
        # Placeholder policy: refunds the first order it finds. Real agents call an LLM.
        for oid in list(env.orders):
            order = env.lookup_order(oid)
            if "error" not in order:
                env.refund(oid, order["amount_cents"])
                break

    report = evaluate(naive_agent, TASKS, n_trials=8)
    mean, ci = mean_and_ci(report["per_task"])
    print(f"mean pass rate: {mean:.2f} +/- {ci:.2f}")
    print(f"pass^8 estimate: {pass_hat_k(report['per_task'], k=8):.3f}")
    for f in report["failures"][:5]:
        print(f"FAIL {f.task_id}: {f.failure_reason} ({f.tool_calls} tool calls)")
```

The naive agent fails the clutter task by refunding the stale cable order, which is the point of clutter: the harness catches disambiguation failures a one-order fixture would hide.
What a production-grade harness adds beyond this skeleton: parallel execution with per-run isolation, transcript persistence for every run keyed by task and trial, structured failure taxonomies instead of reason strings, LLM-judge graders alongside programmatic ones, timeout handling, and result storage that supports longitudinal comparison; the skeleton's structure survives all of these additions.
One caveat on the code: `random.seed` controls only harness randomness, not model sampling; model-side nondeterminism is why n_trials exists at all, and per-trial variation should come from the model, not from varying fixtures.

## 8. Assembling the suite

The construction order that works in practice:

1. Write 10-30 tasks covering the core use case, sourced from real requests where any exist, with clutter in every seed.
2. Push every task down the grader ladder from Chapter 2; aim for programmatic outcome checks on the majority.
3. Add trajectory invariants only for genuine constraints: safety, side effects, cost ceilings.
4. Set n_trials to at least 4 and report mean rate with interval, plus pass^k for the k your product's reliability story implies.
5. Establish the paired-comparison workflow before the first model or prompt change, so the first decision the suite informs is made correctly.
6. Add user-simulated multi-turn tasks once single-turn scores stabilize, with simulator transcripts audited from day one.
7. Wire production failure ingestion (Chapter 7) so the suite grows from reality instead of imagination.

## Exercises

1. Extend the harness with a `delete_order` tool and a task whose forbidden-tools set includes it; write an agent policy that passes the outcome check while violating the invariant, and confirm the harness fails it for the right reason.
2. Add fault injection to `lookup_order` (a transient error on the first call with probability 0.5) and measure how the naive agent's pass rate and pass^8 change; then write the retry logic that restores them.
3. Derive pass@k and pass^k for p in {0.5, 0.8, 0.95, 0.99} and k in {1, 4, 16}; identify the smallest p for which pass^16 exceeds 0.9, and state what that implies for a 16-step-equivalent reliability target.
4. Simulate the paired-versus-unpaired comparison: generate synthetic per-task success probabilities for two agents differing by 3 points on 150 tasks, and measure how often each method detects the difference across 1000 simulated suite runs.
5. Write a user-simulator prompt for the refund task in the tau-bench style, with private ground truth (the customer knows the item but not the order ID) and a persona; run 10 episodes against any agent and audit the transcripts for simulator faults, classifying each episode as valid or invalid.
6. Take the 100-task suite you built in earlier exercises and compute the minimum detectable effect at 95 percent confidence; decide how many tasks you would need to add to detect a 3-point regression, and whether paired comparison changes the answer.

## Godhood check

You have internalized this chapter when you can do the following without reference.

- State the gate-on-outcome, constrain-on-trajectory, diagnose-with-trajectory composition and justify each clause.
- Design a mock-tool environment with seeded cluttered state and explain the fidelity-drift risk and its two mitigations.
- Write the five parts of a well-formed task and catch both specification failures: criteria tighter than the instruction and criteria looser than it.
- Reproduce the pass@k and pass^k formulas, compute both at p = 0.9 and k = 8 from memory, and explain which claim each supports and why the per-task-then-average estimator is required for pass^k.
- Explain what user simulation buys, its contamination risk in both directions, and the auditing discipline that contains it.
- Quote the approximate confidence interval half-widths at N = 100 and N = 500, explain why paired comparison on discordant tasks is more sensitive, and name the multiple-comparisons and peeking traps.
- Sketch the harness architecture from Section 7 on a whiteboard: environment rebuild, invariant-then-outcome grading, repetition, and the three statistics it reports.
