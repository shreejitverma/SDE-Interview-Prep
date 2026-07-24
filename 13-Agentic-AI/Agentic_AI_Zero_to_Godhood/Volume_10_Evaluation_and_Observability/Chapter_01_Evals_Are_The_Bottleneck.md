# Chapter 01 - Evals Are The Bottleneck

## What you will master

- Why evaluation, not modeling or prompting, is the binding constraint on serious agent engineering.
- The demo-to-production gap: why an agent that works in a demo fails in production, quantified in terms of task distribution and reliability compounding.
- Eval-driven development as a workflow, contrasted with vibes-driven iteration, and the failure modes of each.
- The economics of evals: when to invest, how much a good eval suite is worth, and why underinvestment is the default failure.
- How to build an eval culture on a team, including ownership, review norms, and the social dynamics that kill eval suites.

## 1. The central claim

The hardest part of building an agent is not writing the prompt, choosing the model, or wiring the tools.
The hardest part is knowing whether the agent works, how often, on what, and whether your last change made it better or worse.
This claim sounds bureaucratic until you have shipped an agent, at which point it becomes the most obvious fact in the field.

The reason is structural.
Traditional software has a specification you can test against: given input X, the function returns Y, and a unit test freezes that contract.
An agent's contract is a distribution over open-ended tasks, executed by a stochastic policy, in an environment that itself changes.
There is no single input-output pair that certifies correctness.
There is only a measured success rate over a sample of tasks, and if you are not measuring it, you do not know it.

Every other part of the stack has matured faster than evaluation.
As of early 2026, models are strong, tool-use APIs are standardized, frameworks are plentiful, and inference is cheap relative to two years prior.
What has not been commoditized is knowing whether your specific agent does your specific job.
That knowledge cannot be bought from a model provider, because the provider has never seen your task distribution.
This is why evals are the highest-leverage artifact a team owns: they are the only part of the system that encodes what "good" means for your product.

### The asymmetry of iteration speed

Consider two teams building the same customer-support agent.
Team A has no eval suite; they test changes by chatting with the agent for ten minutes and shipping if it feels fine.
Team B has 300 scored tasks drawn from real tickets, runnable in twenty minutes, with per-category breakdowns.

Team A's iteration loop is fast per attempt but produces almost no information per attempt.
Ten manual conversations sample a tiny, biased slice of the task distribution, and the human doing the chatting habituates to the agent's quirks and stops noticing them.
Team B's loop costs more per run but each run yields a number that can be compared to yesterday's number.
Over a quarter, Team B compounds: every prompt change, model swap, and tool redesign is either kept or reverted based on evidence.
Team A oscillates: changes that fix one visible failure silently break three invisible ones, and the agent's quality performs a random walk.

The downside of Team B's approach is real and worth naming: building and maintaining the suite is unglamorous work, the suite is never fully representative, and a team can overfit to it.
But an imperfect measured target beats an unmeasured one in almost every regime, because the failure mode of a stale eval is detectable (audit the suite) while the failure mode of no eval is not.

## 2. The demo-to-production gap

Every agent demo you have ever seen was sampled from the survivorship distribution.
The presenter ran the flow several times, picked a task the agent handles well, and showed the good run.
This is not dishonesty; it is how demos work.
The problem is that teams then reason about production readiness from demo evidence, and the two distributions have almost nothing in common.

Three separate gaps compound.

### Gap 1: task distribution shift

The demo task is chosen by someone who knows what the agent can do.
Production tasks are chosen by users who do not, phrased ambiguously, containing typos, missing context, and mixing multiple requests.
A coding agent demoed on "add a logout button" meets production tasks like "the thing is broken again like last week" with no further detail.
The demo samples the center of the capability distribution; production samples the whole thing, including the tails.

### Gap 2: reliability compounding

An agent run is a chain of steps, and per-step reliability compounds multiplicatively.
If each of 20 steps succeeds with probability 0.98, the run succeeds with probability roughly 0.98^20, which is about 0.67.
A demo needs one good run; a product needs the run to succeed nearly every time for every user.
This is the single most underappreciated arithmetic fact in agent engineering.
It means that improvements invisible at the step level (0.98 to 0.995) are transformative at the run level (0.67 to about 0.90 for 20 steps), and that eyeballing single runs tells you almost nothing about run-level reliability.
Only repeated measurement over many tasks reveals where you sit on this curve.

### Gap 3: environment drift

The demo environment is frozen; production environments move.
APIs change response formats, websites redesign their DOM, databases accumulate edge-case rows, and the model provider ships a new snapshot.
An agent that worked in March can degrade by June with zero changes to your code.
Without continuous evaluation you discover this from angry users instead of from a dashboard.

The practical consequence of the three gaps: the honest question is never "does the agent work" but "what is the success rate, on what distribution, measured when."
Any statement about agent quality that lacks those three qualifiers is a vibe, not a fact.

## 3. Vibes versus measurement

"Vibes" is the industry term for evaluating by interacting with the model and forming an impression.
Vibes are not worthless; they are a legitimate exploratory instrument, and experienced practitioners detect real regressions by feel.
The failure is using vibes as the decision instrument for shipping.

Why vibes fail as a decision instrument:

- Sample size: a human forms an impression from 5-20 interactions; agent quality differences of 5 percentage points, which are commercially decisive, are statistically invisible at that sample size.
- Selection bias: humans test what they think to test, which correlates with what the agent already handles well.
- Recency and salience bias: one vivid failure outweighs ten quiet successes, so vibes systematically overweight dramatic errors and underweight dull ones like slightly wrong numbers.
- Non-stationarity of the observer: your standards drift as you use the system, so "feels better than last week" compares against a moving baseline.
- Non-transferability: a vibe lives in one person's head, cannot be reviewed in a pull request, and leaves the company when they do.

Why measurement alone is also insufficient, stated honestly:

- Every eval is a proxy, and Goodhart's law applies: optimize the proxy hard enough and it decouples from the target.
- Evals lag reality; the suite encodes last quarter's failure modes, and users invent new ones.
- Some qualities, such as tone, are expensive to score well, so measured suites underweight them exactly because they are hard to measure.

The mature position is a loop, not a choice: use vibes and production feedback to discover failure modes, convert discoveries into eval cases, and use the eval suite to make ship decisions.
Vibes are the discovery instrument; evals are the decision instrument.
Teams that use vibes for decisions ship regressions; teams that use evals for discovery ossify.

## 4. Eval-driven development

Eval-driven development (EDD) is test-driven development transposed to a stochastic system.
The workflow:

1. Before building a capability, write the eval: a set of tasks with success criteria that define what the capability means.
2. Run the eval against the current system to establish a baseline, which is often surprisingly nonzero or surprisingly zero, and both surprises are informative.
3. Build the capability, running the eval as the inner-loop feedback signal.
4. Ship when the eval clears the bar; keep the eval running forever as a regression gate.

The transposition changes several things relative to TDD.
A unit test passes or fails; an eval yields a rate, so "passing" means clearing a threshold like 85 percent, and the threshold is a product decision, not a technical one.
A unit test is deterministic; an eval of a stochastic system needs multiple samples per task and confidence-interval thinking, covered quantitatively in Chapter 3.
A unit test is cheap; a full agent eval can cost real money in tokens and minutes in wall time, so eval design includes a cost budget and a tiering strategy: a fast cheap subset for the inner loop, the full suite for the merge gate.

### Writing the eval first is the point

The discipline of writing the eval before the feature forces the team to define success precisely, and this definition work is where most of the value lives.
"The agent should handle refunds" becomes 40 concrete refund scenarios with expected outcomes, and writing them surfaces every ambiguity in the product spec: partial refunds, out-of-window requests, refunds to expired cards, users who ask for a refund but actually want an exchange.
Teams routinely discover that they disagree about what the agent should do long before any model is involved.
An eval suite is a precise, executable product spec, and that is its deepest value.

The downside of EDD is upfront cost and the risk of premature freezing: if you write evals for a capability you have not explored, you may encode the wrong success criteria and then optimize toward them.
Mitigate this by treating early evals as drafts, reviewing them after the first real build iteration, and versioning criteria changes explicitly so scores remain comparable.

## 5. The economics of evals

Eval investment is chronically undersupplied because its costs are immediate and visible while its returns are diffuse and counterfactual.
The costs: engineer time to build harnesses, ongoing token spend, dataset curation, and grader maintenance.
The returns: regressions not shipped, model migrations executed in days instead of months, debugging sessions shortened because failures are reproducible, and the ability to say no to a tempting change with evidence.
Nobody gets promoted for a regression that did not happen, which is exactly why leadership has to fund evals explicitly rather than expecting them to emerge.

Rules of thumb that hold as of early 2026, stated as heuristics rather than laws:

- A team shipping an agent to real users should expect to spend a large fraction, plausibly a quarter to a half, of total engineering effort on evaluation and observability across the product's life.
- The eval suite becomes decisively valuable at the first model migration: teams with suites re-baseline and ship in days, teams without effectively rebuild their product knowledge from scratch.
- The marginal value of eval cases is front-loaded: going from 0 to 50 well-chosen tasks changes how the team operates; going from 500 to 550 rarely does.
- Grader quality matters more than dataset size: 100 tasks with trustworthy grading beat 1000 tasks with noisy grading, because noisy grading caps the resolution at which you can see real differences.

The build-versus-buy question: eval platforms (covered in Chapter 6 alongside observability tooling) sell harness infrastructure, dashboards, and dataset management, and buying that layer is usually sensible.
What cannot be bought is the dataset and the success criteria, because they are your product spec.
Treat vendor tooling as plumbing and your task set plus graders as core IP.

## 6. Building an eval culture

Tooling does not create an eval culture; norms do.
The observable behaviors of a team with a real eval culture:

- Every user-visible change to prompts, tools, models, or agent logic links an eval result in its pull request, the same way code changes link tests.
- Production failures are triaged into eval cases as a matter of routine, with a named owner and a service-level expectation, not as a best effort.
- Eval datasets and graders go through code review with the same rigor as production code, because a wrong grader silently corrupts every future decision.
- Someone owns the suite: its coverage, its cost, its runtime, and the deprecation of stale cases; unowned suites rot within a quarter or two.
- The team distinguishes capability work (raising the ceiling, measured by hard evals that mostly fail) from reliability work (raising the floor, measured by regression evals that must pass), and staffs both.

### The social failure modes

Eval suites die socially before they die technically, and the patterns repeat across teams.

Goodharting under pressure: a launch deadline approaches, the suite blocks it, and the team edits the threshold or the grader instead of the agent.
Once this happens twice, the suite is decorative.
The countermeasure is governance: changing a threshold or grader requires review by someone outside the launching team, exactly like changing a test to make it pass requires justification.

The flaky-eval death spiral: an eval with high variance fails randomly, engineers learn to re-run until green, and then real failures hide inside the noise.
The countermeasure is treating eval flakiness as a P1 defect, the same discipline mature teams apply to flaky tests, and using enough samples per task that variance is quantified rather than mysterious.

Ownership diffusion: the suite belongs to everyone, meaning nobody, and the first quarter with a big launch starves it.
The countermeasure is a named owner with the suite in their performance goals.

Demo-driven leadership: leadership evaluates the agent by personal use, and the team optimizes for the executive's ten favorite prompts.
This is Team A's methodology with organizational power behind it, and it produces agents that impress in reviews and fail in the field.
The countermeasure is reporting eval dashboards, not demos, as the primary status artifact, while still doing demos for what they are good at: qualitative discovery and communication.

### The cold-start problem

Teams delay evals because they have no data, and delay collecting data because they have no product; the deadlock is broken by starting small and synthetic.
Twenty hand-written tasks representing the core use case, graded by simple checks, runnable in five minutes, built in one afternoon, is a real eval suite.
It will be embarrassingly incomplete, and it will still be the most informative artifact the team owns, because the alternative is zero measured tasks.
Every production failure thereafter grows it, which is the data flywheel covered in Chapter 7.

## 7. What the rest of this volume covers

- Chapter 2 builds the taxonomy: eval types, grader types, capability versus regression, offline versus online.
- Chapter 3 goes deep on agent-specific evaluation: trajectories versus outcomes, environment design, pass@k versus pass^k, user simulation, statistics, and a worked Python harness.
- Chapter 4 surveys the public benchmark landscape as of early 2026 and explains why leaderboard deltas rarely transfer to your task.
- Chapter 5 is a full treatment of LLM-as-judge, the most powerful and most dangerous grader type.
- Chapter 6 covers tracing and observability: spans, OpenTelemetry GenAI conventions, the tooling landscape, and debugging from traces.
- Chapter 7 closes the loop with production evaluation: online signals, A/B tests, CI gates, canaries, drift, and the data flywheel.

The through-line: evaluation is not a phase that follows building; it is the instrument panel you build first and fly by afterward.

## Exercises

1. Take an agent you have built or used and write down its success rate on its core task; if you cannot state a number with a source, write down the experiment that would produce one, including task count, success criteria, and cost.
2. Compute run-level success rates for per-step reliabilities of 0.95, 0.98, and 0.995 across chains of 5, 20, and 50 steps; identify which regime your current or planned agent occupies and what per-step reliability your product would require.
3. Write 20 eval tasks for a refund-handling support agent before designing the agent; for each task, record the expected outcome and note every product ambiguity the writing process surfaced.
4. Interview someone who uses an LLM product daily and collect their last 10 real prompts; compare them against what a demo of that product would show, and categorize the gaps using the three-gap framework from Section 2.
5. Draft a one-page eval charter for a hypothetical team: who owns the suite, what gates a merge, who can change thresholds, and the SLA for converting production failures into cases; identify which social failure mode from Section 6 your charter is weakest against.

## Godhood check

You have internalized this chapter when you can do the following without reference.

- State the three demo-to-production gaps and give a concrete example of each from a system you know.
- Reproduce the reliability compounding arithmetic and explain why step-level improvements invisible to eyeballing are decisive at run level.
- Argue both directions of vibes versus measurement: why vibes fail as a decision instrument and why measurement alone ossifies, and state the discovery-versus-decision division of labor.
- Explain why an eval suite is an executable product spec, and why writing evals before building surfaces product disagreements.
- Name the four social failure modes of eval culture and the countermeasure for each.
- Given a team with no evals and no data, lay out the first afternoon of work that breaks the cold-start deadlock.
