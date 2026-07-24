# Chapter 06 - Keeping Up

## What you will master

- A triage system for the frontier firehose: which sources carry signal, what each is for, and how much attention each deserves.
- The curated source map as of early 2026: lab blogs, papers, newsletters, benchmark trackers, and community venues, with the shelf life of each stated honestly.
- How to read an ML paper efficiently as an engineer: the three-pass method adapted for LLM-era papers, and the specific sections where papers hide their weaknesses.
- Reproducing results cheaply: the scaling-down playbook, what transfers from toy scale and what does not.
- Separating capability announcements from deployed reality: a checklist of the standard inflation mechanisms and how to deflate them.
- Building your own eval suite as personal ground truth: why it is the single highest-leverage habit for staying calibrated, and how to maintain it.

## 1. The problem is filtering, not access

The frontier produces far more than anyone can read: hundreds of ML papers per day on arXiv, weekly model releases, and a commentary layer several times larger than the primary material.
The failure mode for working engineers is not ignorance but miscalibration: reading announcement-layer material (launch posts, demo videos, hot takes) and mistaking it for ground truth, or reading nothing and letting skills rot on a two-year-old mental model of what models can do.

Three principles organize everything in this chapter.

Pull, not push, for depth: let curators compress the field for breadth, but when something matters to your work, go to the primary source; the compression layer loses exactly the caveats you need.
Attention is a budget: allocate it like compute, with explicit tiers (skim, read, reproduce), because the default of reading everything shallowly produces confident wrongness.
Your own evals are the anchor: every external claim is someone else's distribution; the only claims you can fully trust are the ones you measured on your own tasks, which is why Section 6 is the load-bearing section of this chapter.

Date-stamp warning for the whole chapter: the principles here are stable, but the source map in Section 2 is early-2026 ephemera by construction; expect names to merge, pivot, and fade, and re-derive the map yearly using the selection criteria given rather than the list itself.

## 2. The source map, early 2026

### Lab and vendor primary sources

These are marketing and research simultaneously; read them for capability claims, API changes, and stated safety postures, and always ask what is omitted.

- Anthropic: research posts, engineering blog (agent-building posts have been unusually concrete), model cards, and system-card-style safety documentation.
- OpenAI: research posts, system cards, and API changelogs; the changelog is often more informative for engineers than the launch post.
- Google DeepMind: research blog and technical reports; historically the most detailed on world models, robotics, and search-based methods.
- Meta AI: open-weight releases and papers; the Llama technical reports have been reference documents for open training recipes.
- DeepSeek, Qwen (Alibaba), Moonshot, and the wider Chinese open-weight ecosystem: technical reports that through 2024-2025 were frequently more recipe-transparent than Western frontier labs; if you read only launch posts you will systematically underestimate this ecosystem.
- Inference and infrastructure vendors (together with the open-source stacks vLLM and SGLang): the practical layer where serving economics change; their engineering blogs are where cost-per-token reality lives.

### Papers and preprint infrastructure

- arXiv (cs.CL, cs.AI, cs.LG) is the primary venue; peer review lags the field by six to eighteen months, so treat conference acceptance as a lagging quality signal, not a freshness signal.
- Filtering layers: Hugging Face's daily papers page and alphaXiv-style discussion layers surface community attention; attention is a noisy but usable prior.
- Survey papers and "awesome" repositories: useful as maps when entering a subfield, always one refresh behind the frontier.

### Newsletters and curators

The compression layer; two or three of these, read consistently, outperform ten read sporadically.
Names current as of early 2026, selection criteria permanent (technical depth, primary-source citation, track record of correcting themselves):

- Interconnects (Nathan Lambert): post-training and RLHF/RLVR analysis with unusual technical honesty; the best single source for Chapter 01's territory.
- AI News (smol.ai): near-daily aggregation with links to primary sources; breadth instrument, not depth.
- Import AI (Jack Clark): weekly, policy-adjacent, strong on the capability-to-society interface.
- Latent Space: engineering-practitioner interviews and analysis; the best window into what builders (rather than researchers) are actually doing.
- ChinaTalk and similar: the China-ecosystem gap in Western coverage is real; deliberate coverage of it pays.

### Benchmark trackers and measurement organizations

- LMArena (the Chatbot Arena lineage): human-preference Elo; measures preference under arena conditions, which correlates imperfectly with task capability and is gameable by style; the 2025 leaderboard-gaming controversies are your standing reminder to read its numbers with care.
- Artificial Analysis: cost, latency, and capability tracking across providers; the quickest sanity check on price-performance claims.
- Epoch AI: compute trends, training-run tracking, and macro measurement; the best calibration source for scaling-trajectory claims.
- ARC Prize (ARC-AGI): a deliberately capability-resistant benchmark; its compute-cost-per-task disclosures set a transparency standard the field should be held to.
- SWE-bench leaderboards, tau-bench, GAIA, and the agentic-eval families from Volume 10: track the harness and the ruleset, not just the number, because harness changes move scores as much as model changes do.
- METR and comparable evaluation organizations: autonomous-capability and dangerous-capability evaluations; the closest thing to an independent auditor layer the field has as of early 2026.

### Community and code

- GitHub is a primary source: the issues and pull requests of major agent frameworks and inference stacks tell you what is breaking in practice months before any paper does.
- X/Twitter and its research-community pockets: highest-velocity and lowest-reliability layer; follow researchers who post negative results and corrections, mute the rest.
- Discords of major open-source projects: where replication failures surface first.

### A workable weekly budget

A concrete allocation that fits a working engineer's calendar: one breadth pass (aggregator or two newsletters, 30-60 minutes weekly), one depth item (a paper or technical report read properly, 60-90 minutes weekly), one hands-on item (try a release against your own evals, monthly), and one map refresh (revisit this source list, quarterly).
The specific hours matter less than the tiering; the failure mode this prevents is spending the entire budget at the breadth layer.

## 3. Reading ML papers efficiently as an engineer

You are reading as a practitioner deciding "does this change what I build," not as a reviewer deciding acceptance; that goal changes the method.

The three-pass structure, adapted for LLM-era papers:

- Pass one, five minutes: title, abstract, figure 1, the main results table, and the limitations section if it exists; output is a decision - discard, file the claim, or schedule pass two - and most papers should die here.
- Pass two, thirty minutes: method section for the actual mechanism (ignore the notation ceremony; find the loss function, the algorithm box, or the pipeline diagram), experimental setup for what was actually compared, and ablations for what actually mattered; output is a one-paragraph note in your own words, because if you cannot write the mechanism in a paragraph you have not understood it.
- Pass three, hours, rare: reproduce or reimplement at toy scale (Section 4); reserved for results you intend to build on.

Where LLM-era papers hide their weaknesses; check these before believing any headline claim:

- The baseline column: is the comparison against a well-tuned current baseline or a strawman with defaults, and is the baseline given the same inference budget (a sampling-heavy method compared against single-sample baselines is the classic sleight of hand, per Chapter 02)?
- The evaluation distribution: benchmark-only evidence with no held-out or contamination analysis; check whether the benchmark predates the model's training cutoff.
- The compute axis: gains reported without cost; always reconstruct the cost-per-solved-task framing.
- The variance: single-seed results and no confidence intervals on differences of a few points; a large fraction of small reported gains are within run-to-run noise.
- The scope of the claim versus the scope of the evidence: "improves reasoning" claimed, grade-school math measured; shrink every claim to its evidence before filing it.
- Author incentives: lab papers accompanying product launches, startup papers accompanying fundraises; not disqualifying, but a prior.

One more engineer-specific habit: read the appendix's prompt listings and hyperparameters when they exist, because that is where you discover the method's real ergonomics, and their absence is itself information about reproducibility.

## 4. Reproducing results cheaply

Full reproduction of frontier results is capital-intensive by design; the engineering skill is scaled-down reproduction: testing whether a mechanism exists and behaves as described, at a budget of hours and tens of dollars.

The playbook:

- Shrink the model: use the smallest open-weight model where the phenomenon could plausibly appear (1-8B class for most post-training and scaffolding claims); the open replication culture around DeepSeek-R1 in 2025 (Open-R1, TinyZero-class projects) demonstrated RLVR emergence phenomena for a few hundred dollars, which is your proof that this playbook works even for headline results.
- Shrink the task: a few hundred examples from the benchmark, or a synthetic task family with the same structure; you are estimating direction and rough effect size, not leaderboard position.
- Prefer the authors' code when it exists, but budget for the standard finding that it does not run as shipped; the delta between paper and repository is itself a signal about the result's robustness.
- Reproduce the ablation, not the headline: the headline number needs their full stack; the ablation ("with versus without component X") often transfers to small scale and is the actual knowledge you wanted.
- Log everything into your eval harness (Section 6) so the reproduction leaves a permanent artifact rather than a vibe.

What transfers from toy scale and what does not, stated as calibration rather than rules: mechanism existence (does the loss go down, does the behavior emerge) usually transfers; effect magnitudes usually do not; rankings between two methods sometimes invert with scale, which is the known emergent-capability caveat; and infrastructure claims (throughput, cost) transfer only if you match the serving configuration, which you usually cannot.
State your reproduction's scope honestly in your notes, or you will re-inflate the claim you just carefully deflated.

## 5. Announcements versus deployed reality

Every capability announcement passes through standard inflation mechanisms before reaching you; deflating them is mechanical once you know the list.

- Demo selection: launch demos are the best of many attempts; the base rate is not shown; assume best-of-n until stated otherwise, and recall from Chapter 02 that the selector may have used ground truth unavailable in production.
- Benchmark-harness coupling: agentic scores depend on the scaffold as much as the model; a score under the vendor's proprietary harness does not transfer to your harness, which Volume 10 taught you and vendor decks rely on you forgetting.
- Compute concealment: results at undisclosed or extreme inference budgets (the ARC-AGI o3 high-compute configuration from December 2024 remains the canonical dated example); always locate the compute axis before being impressed.
- The capability-reliability gap: a 60 percent solve rate is a marvel in a paper and a support-ticket generator in a product; announcements report capability, deployment requires reliability, and the gap between pass@1 and pass^k-style repeated-success metrics (the tau-bench framing) is exactly where products die.
- Availability lag and quiet nerfs: announced is not shipped, shipped is not GA, and the deployed model behind an endpoint changes under load and over time; only your own periodic re-measurement (Section 6) detects this.
- The missing denominators: "agent completed a 10-hour task" without the human-intervention count, the retry count, or the cost; insist on all three denominators before updating.

A useful discipline that costs one minute per announcement: write the falsifiable version of the claim ("model X achieves Y on Z under harness H at budget B"), then note which of those five variables the announcement actually specified; the count of unspecified variables is a serviceable inflation index, and writing it down builds the calibration this chapter is for.
The deepest reason to bother: your professional value as an agent engineer is precisely the size of your gap between announced and real; if your beliefs equal the press releases, you are replaceable by the press releases.

## 6. Your own eval suite as personal ground truth

The single highest-leverage habit in this chapter: maintain a private eval suite - your tasks, your harness, your grading - and run every model and technique that matters to you through it.
This is Volume 10's machinery repurposed from product QA to personal epistemics.

Why it dominates public benchmarks for your purposes: it cannot be contaminated (unpublished by construction), it cannot be gamed (nobody optimizes for it but reality), it measures your distribution (the only one you are paid to care about), and it converts every model release from a stream of opinions into a number you can generate in an hour.

Composition, drawing on everything since Volume 10:

- 30 to 100 tasks: large enough for signal, small enough to maintain; weight toward tasks where models currently fail, because ceiling tasks stop discriminating.
- Mix graded types: verifiable tasks (code with tests, extraction with exact answers) for hard signal, rubric-judged tasks for the open-ended work you actually do, and a few adversarial tasks (injection resistance, instruction-conflict handling) as your personal safety canary.
- Include full agentic trajectories, not just single turns: a small SWE-style task in a container, a tool-use workflow with a mock API; these are the tasks where announcement-versus-reality gaps are largest, so they are where your private measurement earns most.
- Version it like code: tasks, harness, prompts, and grading rubrics in git; every eval run recorded with model identifier, date, cost, and latency, because the longitudinal series is the asset - it is your private record of what actually improved, at what price, across two years of releases.

Maintenance rules that keep it honest: retire saturated tasks and add new failure-mode tasks quarterly; never fix a task because a model you like fails it; keep a held-out subset you run rarely, as your own contamination control against overfitting your prompts to your own suite; and re-run the full suite on a schedule (monthly, and on every provider model-version change you can detect), which is what catches quiet nerfs and silent improvements alike.

The compounding payoff, stated plainly: after a year of this practice you possess something genuinely rare - a private, longitudinal, cost-annotated capability record on tasks you care about - and it upgrades every decision this track has taught you to make: model selection (Volume 02), scaffold design (Volumes 03-08), test-time budget allocation (Chapter 02), and the build-versus-wait timing calls that separate teams that surf capability improvements from teams that are drowned by them.

## Exercises

1. Build your source map: pick two newsletters, three lab blogs, and two benchmark trackers from Section 2 (or current equivalents), subscribe, and write one paragraph per source stating what claim types you will trust it for and what you will always verify elsewhere; calendar the quarterly refresh.
2. Run the three-pass method on two papers this week: one launch-adjacent lab paper and one academic paper claiming a training or scaffolding improvement; produce the pass-two paragraph for each, plus the weakness checklist from Section 3 with a verdict per item.
3. Perform one cheap reproduction: pick a post-training or scaffolding claim (a Chapter 02 selection method or a Chapter 03 optimizer is ideal), reproduce its central ablation on a small open-weight model at under 50 dollars of compute, and write the scope-honest conclusion of what your reproduction does and does not establish.
4. Take three capability announcements from the past six months and write the falsifiable version of each claim, scoring the inflation index (unspecified variables out of five: metric, benchmark, harness, budget, availability); then find any independent measurement of each and record the announced-versus-independent delta.
5. Build the first version of your personal eval suite: 30 tasks minimum, at least ten verifiable, at least five agentic trajectories, at least three adversarial; run two current models through it, record cost and latency per task, and commit the whole thing to a private repository with the run log.
6. Schedule and execute one full re-run of your suite a month later, diff the results, and write the two-paragraph memo you would send yourself: what changed, what it costs, and what, if anything, you should now build differently.

## Godhood check

You are at godhood level for this chapter when you can do the following without notes.

- State the three organizing principles (pull for depth, attention as budget, own evals as anchor) and the weekly tiered budget that implements them.
- Reconstruct the source map's categories and selection criteria from scratch, name current-as-of-early-2026 exemplars in each, and explain which category answers which kind of question.
- Execute the three-pass reading method on any paper and recite the six-item weakness checklist, including the inference-budget-matching trap and the claim-versus-evidence shrink.
- Describe the cheap-reproduction playbook, what transfers from toy scale and what does not, and why reproducing the ablation beats reproducing the headline.
- List the six announcement-inflation mechanisms with a dated example, and produce the falsifiable-claim rewrite with its five-variable inflation index in under a minute.
- Specify a personal eval suite's composition, versioning, and maintenance rules, including the held-out contamination control and the re-run schedule that catches quiet model changes.
- Explain, in one paragraph, why the private longitudinal eval record compounds into a career asset, and connect it to the model-selection, scaffolding, and timing decisions from earlier volumes.
