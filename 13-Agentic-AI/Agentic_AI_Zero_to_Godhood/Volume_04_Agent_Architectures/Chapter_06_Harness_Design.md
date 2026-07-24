# Chapter 06 - Harness Design

## What you will master

- The harness as the real product: why two teams with the same model ship agents of wildly different quality, and where that quality actually lives.
- System prompt architecture: the identity, instructions, tool guidance, and examples layers, and the engineering discipline of prompts-as-code with versioning and review.
- Tool surface curation: why fewer, better tools beat many mediocre ones, and how tool design is agent UX design.
- Environment setup as a harness responsibility: sandboxes, working directories, ambient context, and feedback channels.
- The harness/model co-evolution loop, the bitter-lesson discipline of deleting scaffolding as models improve, and concrete lessons from Claude Code's harness design.

## 1. The harness is the product

Define the harness as everything around the model weights that shapes an agent's behavior: the system prompt, the tool definitions and their implementations, the context-assembly logic, the environment the agent acts in, the permission and safety gates, and the loop mechanics (retries, budgets, interrupts).
The model is a commodity you rent; the harness is the artifact you own, iterate, and differentiate on.

The evidence for taking this framing seriously accumulated across 2024-2026 and is not subtle.
Different harnesses over the same frontier model produce large swings on agentic benchmarks; SWE-bench submissions with identical underlying models have varied by tens of points depending on scaffold quality, which is a larger effect than a typical model-generation jump.
Coding-agent products in the 2025 market competed almost entirely on harness: the same few frontier models sat under most of them, yet user-perceived quality diverged sharply.
And harness bugs present as model stupidity: a truncated tool result, a stale file cache, or a contradictory system prompt makes the finest model in the world look incompetent, and users blame the model every time.

The consequence for how you allocate engineering effort: treat the harness with the seriousness you would give a compiler or an operating system, because it is the layer where your decisions actually change outcomes.

## 2. System prompt architecture

The system prompt is the harness's constitution: the always-present context that defines who the agent is, what it may do, and how.
Mature agent system prompts (Claude Code's is a public, studied example; most serious products converged on similar anatomy by 2025) are thousands of tokens of structured, layered instruction, and their architecture matters more than their wording.

The four layers, in the order they should appear and be reasoned about:

- **Identity**: what the agent is, who it serves, and the register it speaks in; one tight paragraph, not aspiration ("You are X, a Y that helps users Z").
  Identity does real work: it anchors refusal behavior, tone under ambiguity, and the agent's default assumption about what the user wants.
- **Instructions**: the behavioral rules, grouped by concern (task policy, communication style, safety constraints, escalation rules), stated as directives with their conditions ("When the task is ambiguous, ask one clarifying question before acting").
  The discipline that separates professional prompts from piles: every rule earns its tokens, rules are grouped so related rules can be found and audited together, and conflicting rules are treated as bugs, because the model will resolve conflicts arbitrarily and inconsistently.
- **Tool guidance**: not the tool schemas themselves (those travel in the API's tool definitions) but the policy layer over them: when to prefer tool A over B, tools that must not be combined, budget expectations ("prefer one targeted search over broad enumeration"), and error-handling norms ("if a tool fails twice, report rather than retry a third time").
  This layer exists because schemas say what a tool does, and only the prompt can say when it is wise.
- **Examples**: worked demonstrations of the hard judgment calls; few, curated, and chosen to pin down behaviors that rules alone leave underdetermined (the tricky refusal, the correct level of output verbosity, the right way to present a plan).
  Examples are the most expensive layer per token and the most powerful per instance; rotate them as the observed failure distribution changes.

Cross-cutting rules learned repeatedly and expensively across the industry:

- Contradiction is the deadliest prompt bug; as prompts grow by accretion (every incident adds a rule), rules eventually collide, and the symptom is nondeterministic policy behavior that no one can reproduce; periodic full-prompt audits are the only cure.
- Position and emphasis matter but decay: models attend most reliably to clear, unconditional statements; deeply nested conditionals ("unless X, except when Y") underperform flat rules with explicit conditions.
- The prompt is not the place for facts that change (tool inventories that vary, dates, user data); inject those as structured context per-session, and keep the constitution stable so caching (Volume 02) and behavioral regression testing both work.
- Length is a real cost but not the dominant one; incoherence is; a 5,000-token coherent prompt outperforms a 1,500-token self-contradictory one, and prompt caching has made the marginal token cheap as of 2025-era pricing models.

## 3. Prompts as code

The prompt lifecycle discipline that production teams converged on, stated as a checklist because every item is regularly skipped and regretted:

- Prompts live in version control, not dashboards or database rows edited live; a prompt change is a deploy.
- Prompt changes go through review, and reviewers are asked the same question as for code: what behavior changes, and what might regress.
- Every prompt version is tied to an eval run (Volume 10); "it reads better" is not evidence, and single-anecdote validation is how regressions ship.
- Prompts are templated with explicit variables, and the assembly function that builds the final context is unit-tested code, because context-assembly bugs (wrong ordering, missing sections, double-injection) are among the most common production agent defects.
- Rollback is one revert; this rules out architectures where the live prompt is assembled from mutable sources that cannot be pinned.
- Model migrations get a full re-eval of the prompt suite; instructions tuned around one model's quirks (over-emphatic repetition, workaround phrasing) become dead weight or active harm on the next model, which is the co-evolution loop of section 6 in miniature.

The trade-off to name: this discipline adds friction to iteration speed, and early-stage products legitimately trade some of it away; the mistake is not choosing speed early, it is failing to install the discipline before the prompt becomes load-bearing for revenue.

## 4. Tool surface curation

Chapter 01 said the sophistication lives in the tools; here is the design discipline.

The core insight, argued publicly by Anthropic's tool-design writing (2025) and confirmed by every serious harness team: tools are the agent's user interface to the world, and designing them is UX design where the user is a model.
The model's attention, working memory, and error patterns are the ergonomic constraints, replacing human ones.

Principles that consistently pay:

- **Curate ruthlessly**: every tool costs context tokens for its definition and adds a branch to every action decision; large tool inventories measurably degrade selection accuracy, and the practical ceiling before selection quality decays is far lower than teams expect (dozens, not hundreds, as of early-2026 models).
  Prefer a few high-leverage tools over exhaustive coverage; merge tools that always co-occur; delete tools the traces show are unused or misused.
- **Name and describe for the model, not the codebase**: the description is a micro-prompt, and the best ones state what the tool does, when to use it over its neighbors, and what it returns, in the same register as the system prompt; internal API names (`svc_query_v2`) are self-inflicted wounds.
- **Design return values as context, not payloads**: a tool that dumps 40 KB of JSON into the trace has externalized its cost onto every subsequent decision; return the distilled, relevant form, offer verbosity parameters, truncate with explicit markers and escape hatches ("first 100 of 2,340 rows; refine the query or pass full=true").
- **Make errors instructive**: the error string is the model's only debugging signal; "invalid argument" wastes a loop iteration, while "date must be YYYY-MM-DD, got 03/04/2026" repairs in one; every tool error message should be written as a hint toward the corrected call.
- **Match tool granularity to intent**: too-fine tools (five calls to do one conceptual thing) burn loop iterations and multiply error surface; too-coarse tools (one mega-tool with a mode enum) blur selection; the target is one tool per coherent intention the agent regularly has.
- **Bake in safety asymmetry**: read tools should be cheap and permissive, mutation tools explicit and gated; a tool surface where reading and writing look identical forfeits the cheapest safety structure available (Chapter 03's plan-mode asymmetry is this principle at harness scale).

A general-purpose escape hatch changed this calculus in 2024-2026: giving the agent code execution (a sandboxed interpreter or shell) collapses many special-purpose tools into one composable action space, which is the CodeAct lineage of Chapter 02 industrialized.
The trade is real on both sides: code execution buys expressiveness and composition, and costs sandboxing infrastructure, harder static safety review, and less structured observability; most strong 2026-era harnesses carry both a small curated tool set and an execution tool, and route by task.

## 5. Environment setup and feedback channels

The harness also owns the world the agent acts in, and environment design is regularly the difference between a reliable agent and a flaky one.

- **Sandboxing and blast radius**: decide what the agent can touch before deciding what it can do; filesystem scoping, network egress policy, credential injection with least privilege, and disposable execution environments are harness features, not deployment afterthoughts (Volume 11 covers the security side; here the point is architectural: the environment is part of the design).
- **Ambient context**: the harness front-loads what the agent would otherwise spend iterations discovering: working directory, repository layout, current date, available services, user identity and preferences; every fact reliably injected is a tool call the loop does not spend, and the discipline is to inject stable, cheap, high-frequency facts and let the agent fetch the long tail.
- **Feedback channel quality**: Chapter 02 established that the loop is only as good as its observations; harness work here means fast, deterministic, high-signal feedback: test commands that run in seconds, linters wired in, execution output captured cleanly, diffs rendered readably; a harness that makes verification cheap gets an agent that verifies, and one that makes it slow gets an agent that guesses.
- **Statefulness decisions**: whether the environment persists across sessions (a durable workspace) or resets (hermetic runs) is a real trade: persistence accumulates useful state and accumulates corruption; hermetic environments reproduce cleanly and pay setup cost every run; coding agents generally choose persistent workspaces with cheap reset commands, evals choose hermetic.

## 6. The harness/model co-evolution loop

Harness engineering is not a one-time design task; it is a loop coupled to model progress, and managing that coupling is a senior-engineer responsibility.

The loop, as practiced by strong teams:

1. Observe failures in production traces and evals.
2. Classify each: model capability gap, or harness defect (missing context, bad tool ergonomics, contradictory instructions).
3. Patch the binding constraint; capability gaps get scaffolding (more structure, more gates, decomposition), harness defects get fixed directly.
4. On each model upgrade, re-run the eval suite and actively hunt for scaffolding to delete, because structure built to compensate for the old model's weaknesses is now dead weight that constrains the new model.

Step 4 is the one teams skip, and it has a name worth using: the bitter-lesson discipline, after Sutton's essay, applied to harnesses.
Scaffolding that hard-codes how to do the task (rigid decompositions, forced tool orders, elaborate per-step prompts) depreciates with every model generation, while harness investment in what the task is (clean tools, good feedback channels, honest evals, safety gates) appreciates.
The 2023-2026 record is unambiguous: multi-step prompt pipelines built to babysit 2023 models became the legacy code of 2025, and the teams that shipped fastest on new models were the ones whose harnesses were thin where models were improving and thick where models were not (permissions, environment, verification).

Design rule of thumb that falls out: before adding scaffolding, label it "compensating for model weakness" or "encoding task truth"; build the first kind cheap and expect to delete it, and build the second kind well because it compounds.

## 7. Lessons from Claude Code's harness

Claude Code (Anthropic's coding agent, 2025-2026 era) is this volume's best-documented industrial harness, through Anthropic's own engineering writing and extensive public dissection; the transferable lessons, stated as principles rather than product trivia:

- **A thin loop over a strong model**: the architecture is a single main agent loop with tools, not a multi-agent planner hierarchy; complexity was spent on tool quality and context management rather than orchestration topology, a direct application of Chapter 01's doctrine.
- **The permission system as the trust spine**: every action classifies as read or mutate, mutation requires user consent by default, and consent granularity (once, session, always) is a first-class UX surface; safety came from harness structure, not from hoping the model behaves.
- **Plan mode as a harness state**: Chapter 03 covered it; the harness enforces read-only exploration before an approved plan unlocks mutation, which operationalizes cheap-plan-errors versus expensive-trajectory-errors as a permission asymmetry.
- **Ambient context via convention files**: project- and user-level instruction files (CLAUDE.md) let users extend the system prompt declaratively per scope; the harness merges them, which turned system prompt architecture into a user-extensible surface with a defined precedence order.
- **Verification is wired in, not hoped for**: the harness runs the user's own build and test commands, and the agent's loop is steered toward checking its work with real executions; the feedback-channel investment of section 5, made central.
- **Context economics as a harness job**: automatic compaction of long histories, tool-result truncation, and sub-agent delegation for context-heavy exploration are all harness mechanisms protecting the model's working memory (Volume 06's subject, surfacing here as harness design).
- **The todo list as visible working state**: the plan artifact is maintained by the harness as a structure the user watches update, serving simultaneously as goal-restatement for the model and progress transparency for the human.

The meta-lesson: essentially every mechanism in that list is model-agnostic harness engineering, which is why the product improved with each model swap rather than being invalidated by it.

## 8. Claims that will rot

Product specifics (Claude Code's features, CLAUDE.md conventions, permission UX) describe the 2025 to early-2026 versions and will drift; treat them as design lessons, not as documentation.
Numeric flavor (tool-count ceilings before selection degrades, harness-induced benchmark spreads) reflects early-2026 models and scaffolds; re-measure on current models before citing.
The stable content is the four-layer prompt architecture, prompts-as-code discipline, tool-as-UX principles, environment-as-design, and the co-evolution loop with its bitter-lesson discipline; these have already survived multiple model generations and are the chapter's durable core.

## Exercises

1. Obtain any serious agent system prompt (your own product's, or a published one) and audit it: map every passage to the four layers, list every pair of rules in tension, and propose the minimal edit that resolves each conflict.
2. Take a tool inventory you have access to and run a curation pass: for each tool, find its usage frequency and misuse rate in traces, then merge, rename, rewrite descriptions, or delete; write the one-paragraph rationale per change.
3. Rewrite the error messages of one real tool so that every failure names the violated constraint and the shape of a corrected call; measure repair-in-one-retry rate before and after over 30 induced failures.
4. Design the ambient context block for an agent operating in your main repository: choose the facts worth injecting every session, estimate their token cost, and defend each against the alternative of letting the agent discover it.
5. Inventory one existing agent's scaffolding and label every element "compensating for model weakness" or "encoding task truth"; predict which elements the next model generation should delete, and design the eval that would prove it.
6. Specify the prompts-as-code pipeline for a team of five: repository layout, review checklist, eval gate, rollback procedure, and the model-migration protocol; identify the single most likely point of process decay and its countermeasure.

## Godhood check

You have mastered this chapter when you can:

- Argue with evidence that the harness, not the model, is where an agent team's differentiation lives, and name the three ways harness bugs masquerade as model stupidity.
- Structure a system prompt into identity, instructions, tool guidance, and examples from a blank page, and audit an existing one for contradictions systematically.
- Apply the tool-as-UX principles to a concrete tool surface and justify each curation decision in terms of model ergonomics.
- Explain the co-evolution loop and the bitter-lesson discipline, with one historical example of scaffolding that depreciated and one harness investment that compounded.
- Extract the model-agnostic principle from any shipped agent product's harness choices, as section 7 did for Claude Code, rather than cataloguing its features.
