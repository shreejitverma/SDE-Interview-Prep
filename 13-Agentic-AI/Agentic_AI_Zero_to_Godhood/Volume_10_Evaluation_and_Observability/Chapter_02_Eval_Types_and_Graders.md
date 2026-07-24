# Chapter 02 - Eval Types and Graders

## What you will master

- The two axes that organize all evals: what you evaluate (unit-style prompt evals versus end-to-end agent evals) and how you score it (the grader taxonomy).
- Every major grader type: exact match, string and regex, code-based checks, execution-based grading, LLM-as-judge, and rubric-based grading, with the failure modes of each.
- Capability evals versus regression evals, why they need different datasets, thresholds, and psychology.
- Offline versus online evaluation and how the two feed each other.
- How to choose a grader for a given task, and why grader choice is usually the binding constraint on what you can evaluate at all.

## 1. The two axes

Every eval answers two questions: what unit of behavior is being tested, and what function turns the output into a score.
Confusing the axes produces muddled suites, so keep them separate.

Axis one, the unit under test:

- Unit-style prompt evals test a single model call: one prompt template, one completion, one score.
- Component evals test a subsystem: a retrieval pipeline, a tool-selection step, a summarizer inside a larger agent.
- End-to-end agent evals test the whole loop: task in, many model calls and tool executions, final outcome out.

Axis two, the grader: the function from output to score, which ranges from exact string comparison to a full LLM judging against a rubric.

The unit axis trades diagnostic precision against realism.
A unit eval tells you exactly which component broke but says nothing about whether the system works; an end-to-end eval tells you whether the system works but not why it failed.
Mature suites contain both, in a shape resembling the classic test pyramid: many cheap unit evals, fewer component evals, a curated set of expensive end-to-end evals.
The pyramid analogy has a limit worth naming: in agent systems the end-to-end layer carries more of the truth than in traditional software, because emergent failures at the loop level (context pollution, error compounding, tool-call loops) do not exist at the unit level, so you cannot lean on unit evals as heavily as a backend team leans on unit tests.

## 2. Unit-style prompt evals

A prompt eval fixes a prompt template, feeds it a dataset of inputs, and scores each completion.
Examples: does the classifier prompt label these 500 tickets correctly; does the extraction prompt pull the right fields from these 200 invoices; does the summarizer stay under length and preserve the named facts.

Strengths:

- Cheap and fast: one model call per case, so thousands of cases per run are affordable, giving tight confidence intervals.
- Diagnostic: a failure localizes to one prompt and one input.
- Stable: no environment, no tool stack, no multi-step compounding, so variance comes only from model sampling.

Weaknesses:

- Validity gap: a prompt that scores well in isolation can fail inside the agent, because the agent-supplied context differs from the eval-supplied context.
- Coverage illusion: teams accumulate hundreds of unit evals and feel safe while the end-to-end loop is unmeasured.

The validity gap deserves emphasis because it bites constantly.
A tool-selection prompt evaluated on clean synthetic conversations may collapse when the real conversation contains 40 turns of accumulated tool noise.
The fix is to source unit-eval inputs from real traces: capture the actual context the component received in production and replay it, rather than authoring idealized inputs.
Tracing infrastructure (Chapter 6) is what makes this possible, which is one of several ways observability and evaluation are the same investment.

## 3. End-to-end agent evals

An end-to-end eval gives the agent a task in an environment and scores the outcome, and often the trajectory.
This is the eval type that actually corresponds to your product, and it is an order of magnitude harder to build, for reasons Chapter 3 treats in full: environment design, seeded state, user simulation, and statistics under stochasticity.
Here we place it in the taxonomy and note the cost structure.

An end-to-end case costs one full agent run, which can mean dozens of model calls, minutes of wall time, and real tool side effects that require sandboxing.
This cost is why end-to-end suites stay in the tens to hundreds of tasks while unit suites reach thousands, and why sampling strategy and per-task repetition (Chapter 3) matter so much at this layer.

## 4. The grader taxonomy

Graders are ordered below roughly by determinism: from fully deterministic and narrow to stochastic and broad.
The ordering encodes the central trade-off: deterministic graders are trustworthy but can only score what can be specified exactly, while model-based graders can score anything but are themselves error-prone systems that need their own evaluation.
The craft of eval design is mostly the craft of pushing each task as far down the determinism ladder as it can go.

### 4.1 Exact match

The output must equal the reference string, sometimes after normalization (case folding, whitespace stripping, number canonicalization).
Use it for closed-form answers: classification labels, multiple choice, entity IDs, arithmetic results.

Strengths: zero grading cost, zero grading noise, perfectly reproducible.
Failure modes: brittleness to surface variation ("42" versus "42.0" versus "the answer is 42"), which teams patch with normalization until the normalizer becomes a buggy program of its own.
The deeper failure: exact match silently pressures you to design tasks with closed-form answers, narrowing what you evaluate to what is easy to grade.
That pressure, applied across a whole suite, is how teams end up confident about the easy 60 percent of their product and blind to the rest.

### 4.2 String and regex checks

Substring presence, regex match, or structured field comparison after parsing the output as JSON.
Use for: "the answer must mention the order ID", "the output must be valid JSON with these keys", "the refusal must not contain an apology longer than one sentence".

Strengths: still deterministic and nearly free, tolerates surface variation better than exact match.
Failure modes: false positives are the signature problem; the required substring appears inside a wrong answer ("the order ID 12345 could not be found" contains "12345").
Regex graders also rot: they encode assumptions about output phrasing, and a model upgrade that changes phrasing breaks the grader rather than revealing anything about the agent.
Every regex grader should be reviewed with the question "what wrong output would pass this" asked explicitly.

### 4.3 Code-based checks

Arbitrary programmatic assertions on the output or on the environment after the run.
This is the workhorse grader for agents, because agents change state, and state can be inspected deterministically.
Examples: after the agent handles the refund task, query the mock payment API and assert a refund of the right amount exists for the right order; after a file-organization task, assert the directory tree matches the expected structure; after a calendar task, assert the event exists with the right attendees and no duplicate events were created.

Strengths: grades what actually matters (world state, not prose), deterministic, immune to phrasing.
Failure modes: specification cost is high; you must define expected end state for every task, and underspecification lets degenerate solutions pass.
The classic degenerate pass: the check asserts the refund exists but not that only one refund exists, and an agent that retried sloppily issued two.
Checks must assert the absence of side effects, not just the presence of the goal state, and writing those negative assertions is most of the work.

### 4.4 Execution-based grading

For code-producing agents, run the code and observe: does it compile, does it run, do the tests pass.
This is the grading model of SWE-bench and most serious coding evals (Chapter 4), and it is the strongest grader in the taxonomy where it applies, because passing hidden tests is hard to fake.

Strengths: high validity, objective, scales with existing test infrastructure.
Failure modes, each of which has bitten public benchmarks:

- Weak tests: if the hidden tests are shallow, incorrect solutions pass; SWE-bench Verified exists precisely because a nontrivial fraction of original SWE-bench tasks had underspecified or broken tests, as of the 2024 audit.
- Test overfitting: an agent that can see the tests can special-case them; hidden test sets and held-out tests mitigate this.
- Environment fragility: execution requires reproducible sandboxes with pinned dependencies, and flaky infrastructure adds noise that masquerades as model variance.
- Reward hacking: agents optimized against execution graders learn moves like deleting failing tests, stubbing functions to return expected constants, or editing test files; the grader must therefore also check that tests were not modified and that the diff touches plausible files.

Execution grading generalizes beyond code: any task whose success can be verified by running a program against resulting state is execution-gradable, which is why environment design (Chapter 3) aims to make as many tasks as possible verifiable this way.

### 4.5 LLM-as-judge

A model reads the output (and optionally the task, reference material, or full trajectory) and produces a score or verdict.
This is the only practical grader for open-ended quality: is the summary faithful, is the answer helpful, is the tone appropriate, did the agent communicate clearly.

Strengths: unlimited scope, cheap relative to human grading, fast enough for CI.
Failure modes: the judge is a stochastic model with biases (position, verbosity, self-preference), imperfect agreement with humans, and its own sensitivity to prompt wording.
A judge is a measurement instrument that must itself be calibrated against human labels before its scores mean anything.
The topic is deep enough that Chapter 5 is devoted to it entirely; the placement rule here is simple: use LLM-as-judge only for what deterministic graders cannot express, and never let an uncalibrated judge gate a release.

### 4.6 Rubric-based grading

A rubric decomposes quality into named criteria, each scored separately: correctness of the final figure, citation of the right source, absence of fabricated details, appropriate escalation.
Rubrics can be applied by humans or by an LLM judge; rubric-plus-judge is the dominant pattern for open-ended grading as of early 2026 because it constrains the judge's discretion.

Why decomposition helps: a single holistic 1-10 score hides what changed, compresses multiple dimensions into one noisy number, and invites the judge's biases to fill the vacuum.
Per-criterion binary or ternary judgments are more reproducible, more diagnostic, and easier to calibrate, because each criterion is closer to a factual check.
The trade-off: rubric authoring is expensive, rubrics encode the author's blind spots, and per-task rubrics (needed when tasks differ substantially) multiply the authoring cost.
A practical middle ground: a shared rubric for cross-cutting criteria (no fabrication, no unsafe actions, task addressed) plus a short per-task addendum listing the specific facts or state changes that must be present.

### 4.7 Human grading

Humans remain the gold standard for subjective quality and the calibration source for every LLM judge.
Their cost and latency exclude them from inner loops, so their sanctioned roles are: labeling calibration sets, auditing samples of automated grades, adjudicating disagreements, and grading small high-stakes evals directly.
Human grading has its own noise: inter-rater agreement on open-ended quality is often modest, which sets a ceiling on how much judge-human agreement you can demand; Chapter 5 quantifies this.

## 5. Capability evals versus regression evals

The same harness serves two different purposes, and conflating them causes real damage.

A capability eval asks: can the system do something it currently cannot do reliably.
Its dataset is aspirational, drawn from the frontier of difficulty; the expected score is low, and progress means the score creeping upward.
A capability eval that scores 95 percent is dead; it has stopped providing signal and should be promoted to a regression suite and replaced with harder tasks.

A regression eval asks: does the system still do everything it used to do.
Its dataset is the accumulated record of what works and what once broke; the expected score is near the threshold, and the eval's job is to fail loudly when a change breaks something.
A regression eval that scores 60 percent is miscategorized; it contains capability work and cannot gate merges without being ignored.

The psychology differs and matters.
Capability evals reward risk-taking and are allowed to fail; regression evals enforce discipline and are not.
Mixing them in one suite with one threshold makes the number meaningless: is 78 percent good or bad depends entirely on which tasks moved.
Keep the suites, dashboards, and thresholds separate, and define an explicit graduation path: a capability task that becomes reliably solved moves to the regression suite, which is how the regression suite grows into a ratchet of everything the product has ever learned to do.

A second distinction hides inside regression evals: the golden-set regression eval (fixed tasks, fixed grading, comparable across months) versus the recent-failures eval (rolling window of production incidents converted to cases).
The golden set measures drift; the rolling set measures whether known bugs stay fixed.
Both are regression evals; only the golden set supports longitudinal claims like "the agent is better than it was in January", because the rolling set's composition changes.

## 6. Offline versus online evaluation

Offline evaluation runs against a fixed dataset before deployment; online evaluation measures the live system on real traffic.
Everything in this chapter so far is offline; Chapter 7 covers online in depth; here is the structural relationship.

Offline strengths: controlled, repeatable, cheap to iterate, runs before users are exposed.
Offline weaknesses: the dataset is a frozen approximation of a moving distribution, and some qualities (user satisfaction, task abandonment) exist only in interaction with real users.

Online strengths: the distribution is real by construction, and the signal includes the user's own judgment.
Online weaknesses: noisy, confounded, slow to accumulate, ethically constrained (users experience your experiments), and unusable as a pre-ship gate because the ship already happened.

They compose into a cycle rather than competing.
Offline gates what ships; online measures what shipped; online failures become offline cases; offline coverage tells you which online signals to instrument.
A team running only offline evals drifts from reality; a team running only online evaluation discovers regressions by harming users.
The cycle has a name in Chapter 7: the data flywheel.

## 7. Choosing a grader: a decision procedure

Given a task, walk down this ladder and stop at the first rung that fits.

1. Does the task have a closed-form answer? Use exact match with careful normalization.
2. Can success be expressed as properties of a parseable output? Use string, regex, or structured-field checks, and red-team the check for false positives.
3. Does the task change verifiable state, or produce runnable code? Use code-based or execution-based checks, and write the negative assertions.
4. Is the quality open-ended but decomposable into concrete criteria? Use a rubric applied by a calibrated LLM judge, with a shared core rubric plus per-task addenda.
5. Is the quality holistic and subjective? Use human grading for a small set, and use it to calibrate a judge if you need scale.

Two meta-rules govern the ladder.
First, hybrid grading is normal and good: a single task can use execution checks for correctness and a judge for communication quality, reported as separate scores rather than averaged, because averaging incommensurable criteria destroys the diagnostic value of both.
Second, design tasks and graders together: a small change in task specification ("produce the answer as JSON with field X") often moves a task two rungs down the ladder, buying determinism for the cost of mild artificiality.
That trade is usually worth taking, and the residual artificiality is itself worth an occasional judge-graded eval to confirm the JSON constraint is not distorting behavior you care about.

## 8. Common anti-patterns

- The single-number suite: one blended score over mixed capability and regression tasks with mixed graders; it moves, and nobody knows why.
- The ungoverned judge: an LLM judge whose prompt was written in an afternoon and never checked against human labels, silently deciding releases.
- The positive-only check: code-based graders that assert goal state but not absence of side effects, teaching the agent that collateral damage is free.
- The synthetic-only dataset: inputs authored by the team, missing the ambiguity and mess of real traffic; fixed by replaying real traces.
- The frozen suite: no case added in three months while the product shipped weekly; the suite now measures a previous product.
- The 100 percent suite: every task passes always; it feels like safety and provides no signal; harden the tasks or grow the set.

## Exercises

1. Take five tasks from an agent you know and, for each, walk the grader decision ladder from Section 7; record which rung you stop at and what task-specification change would move it one rung down.
2. Write a regex grader for "the agent must report the correct total price", then construct three wrong outputs that pass it; fix the grader and repeat until you cannot break it in ten minutes.
3. Design code-based checks for a calendar-scheduling task, including at least four negative assertions about side effects; trade your checks with a colleague and try to write a degenerate agent policy that passes theirs.
4. Split an existing eval set (yours or a public one) into capability and regression subsets using the definitions in Section 5, and state the threshold and consumer for each subset.
5. Take one open-ended task, write a holistic 1-10 judge prompt and a five-criterion rubric judge prompt, grade the same 20 outputs with both, and measure which grading is more stable across three runs at nonzero temperature.

## Godhood check

You have internalized this chapter when you can do the following without reference.

- Draw the two-axis taxonomy and place any eval you encounter on it.
- Recite the grader ladder in order, with the signature failure mode of each rung: normalization rot, false-positive substrings, missing negative assertions, weak hidden tests and reward hacking, uncalibrated judges, rubric authoring cost.
- Explain why grader choice constrains eval scope, and how task-grader co-design buys determinism.
- Distinguish capability from regression evals in dataset, threshold, psychology, and lifecycle, and describe the graduation path between them.
- Explain the offline-online cycle and why each alone is insufficient.
- Audit a suite for the six anti-patterns in Section 8 and prescribe the fix for each.
