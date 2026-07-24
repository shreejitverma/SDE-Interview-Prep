# Appendix C: Benchmark Index

A reference index of benchmarks relevant to agent engineering: what each measures, its format, and its known caveats.
No scores are listed; leaderboard numbers rot in months, so consult the official leaderboard or the model card of interest and check the harness version.
Knowledge as of early 2026.

## How to read any benchmark claim

Check five things before trusting a number.
First, the harness: agent benchmarks measure model plus scaffold, and scaffold differences move scores by tens of points.
Second, the metric: pass@1 versus pass@k versus pass^k answer different questions (typical, ceiling, reliable).
Third, contamination: was the benchmark public before the model's training cutoff.
Fourth, the subset: "SWE-bench" may mean full, Lite, or Verified, which are different populations.
Fifth, the grader: execution-based grading is trustworthy, LLM-judge grading inherits judge bias, and simulated-user grading inherits simulator noise.

## 1. Coding agents

### SWE-bench (2023, Princeton)

- Measures: whether an agent can resolve real GitHub issues by producing a repository patch.
- Format: 2,294 issue-codebase pairs from 12 popular Python repositories; the agent gets the repo and issue text; grading runs held-out fail-to-pass and pass-to-pass tests.
- Variants: SWE-bench Lite (300 instances, easier and cheaper to run); SWE-bench Verified (500 instances human-validated by OpenAI in 2024 after finding many full-set tasks underspecified or unfairly graded); SWE-bench Multimodal (issues with visual elements).
- Caveats: Python-only and 12 repos limit generalization; solutions to many issues exist in public git history, so contamination is structural; Verified is the only subset worth quoting for capability claims; scores mix model and harness contributions inseparably.

### SWE-bench Pro (2025, Scale AI)

- Measures: harder, contamination-resistant repository engineering, including tasks from commercial codebases.
- Format: SWE-bench-style issue resolution with held-out tests, on repositories selected to be harder and partly non-public.
- Caveats: newer and less independently audited than SWE-bench Verified; partly private data means external replication is limited.

### SWE-Lancer (2025, OpenAI)

- Measures: economic value of software agents via real freelance work.
- Format: over 1,400 real Upwork software engineering tasks with associated dollar payouts; graded by end-to-end tests (implementation tasks) or choice accuracy (management tasks); reports total dollars earned.
- Caveats: single client codebase (Expensify) dominates; dollar-weighted scoring rewards a different distribution than uniform task-weighted scoring.

### Terminal-Bench (2025, Stanford and Laude Institute)

- Measures: agent competence in a terminal: builds, sysadmin tasks, data processing, debugging inside a sandboxed shell.
- Format: dockerized tasks with instruction text; grading by execution checks in the container; harness-agnostic (bring your own agent).
- Caveats: young benchmark with evolving task set and versioning; results across versions are not comparable; task authorship skews toward developer-tool domains.

### HumanEval (2021, OpenAI) and LiveCodeBench (2024)

- Measures: function-level code synthesis from docstrings (HumanEval); contest-style coding with continuously refreshed problems (LiveCodeBench).
- Format: execution against unit tests; pass@k.
- Caveats: HumanEval is saturated and contaminated, useful only as a smoke test; LiveCodeBench mitigates contamination by dating problems after model cutoffs, so always check the evaluation window; neither measures repository-scale or agentic work.

### Aider Polyglot (2024, aider project)

- Measures: code editing across multiple languages through a real coding-assistant harness.
- Format: 225 hard Exercism exercises across languages; graded by test execution; measures both correctness and edit-format compliance.
- Caveats: tied to the aider harness and its edit formats; exercises are small and self-contained, unlike production repositories.

### MLE-bench (2024, OpenAI)

- Measures: machine-learning engineering: training models, preparing data, and hitting leaderboard thresholds.
- Format: 75 Kaggle competitions run offline; graded against human leaderboard medal thresholds.
- Caveats: compute budget strongly affects results; Kaggle-style problems overrepresent tabular and vision tasks relative to production ML work.

## 2. Conversational tool agents

### tau-bench (2024, Sierra)

- Measures: tool-using customer-service agents that must converse with a user, follow domain policy, and mutate a database correctly.
- Format: airline and retail domains; an LLM simulates the user; grading compares final database state to ground truth plus required utterances; metrics are pass@1 and pass^k over independent trials.
- Caveats: the user simulator is itself an LLM and its variance contaminates measurement; two domains only; policy documents are long, so it partly measures long-instruction adherence; pass^k results show reliability far below capability, which is the point.

### tau2-bench (2025, Sierra)

- Measures: the same, plus dual-control settings where the user also acts on the environment and the agent must guide them.
- Format: adds a telecom domain; improved simulator and task verification over tau-bench.
- Caveats: same simulator-noise class of issues; newer, smaller body of comparative results.

## 3. General assistant and research agents

### GAIA (2023, Meta AI, HuggingFace and collaborators)

- Measures: general assistant ability: questions easy for humans but requiring browsing, tool use, multimodality, and multi-step reasoning.
- Format: 466 questions in three difficulty levels; answers are short strings graded by quasi-exact match; a private test split guards against overfitting.
- Caveats: web-dependent tasks decay as the live web changes; exact-match grading punishes correct-but-differently-formatted answers; leaderboard entries vary enormously in scaffold sophistication.

### BrowseComp (2025, OpenAI)

- Measures: persistent deep web research: locating hard-to-find, entangled facts.
- Format: 1,266 questions with short verifiable answers, built to be hard to find but easy to check; graded by answer match.
- Caveats: deliberately unrepresentative of typical user queries (favors needle-hunting over synthesis); English and public-web centric; live-web dependence means difficulty drifts over time.

### AgentBench (2023, Tsinghua and collaborators)

- Measures: broad agentic ability across heterogeneous environments.
- Format: eight environments (OS shell, database, knowledge graph, card game, puzzles, household, web shopping, web browsing) under one harness; environment-specific success metrics.
- Caveats: age means substantial contamination exposure and dated task design; per-environment quality varies; mostly superseded for frontier comparisons but useful for breadth.

### METR time-horizon methodology (2025, METR)

- Measures: not a task suite score but the length of task (in human-expert time) that a model completes with 50 percent reliability.
- Format: diverse timed tasks from minutes to many hours; logistic fit of success versus human task duration.
- Caveats: task portfolio composition drives the estimate; 50 percent reliability is far below deployment thresholds, and 80 percent horizons are much shorter; extrapolations of the doubling trend are projections, not measurements.

## 4. Web and computer use

### WebArena (2023, CMU)

- Measures: autonomous task completion on realistic, functional websites.
- Format: 812 tasks over self-hosted replicas (e-commerce, forum, GitLab, CMS, map); graded by functional outcome checks, not action matching.
- Caveats: self-hosted sites are stable but stylistically dated; some task checks have known false negatives; VisualWebArena extends to visually grounded tasks; human performance is high, so headroom claims should cite the human baseline.

### OSWorld (2024, HKU and collaborators)

- Measures: computer use on a real operating system: file management, office apps, browsers, multi-app workflows.
- Format: 369 tasks in VM-hosted real OS environments (mostly Ubuntu); agents act via screenshots plus mouse and keyboard (or accessibility tree); graded by execution-based state checks.
- Caveats: screenshot-based agents are sensitive to resolution and rendering details; some tasks admit shortcut solutions via terminal that bypass the intended GUI skill; an updated OSWorld-Verified pass (2025) fixed many flaky tasks, so version matters when comparing.

## 5. Security

### AgentDojo (2024, ETH Zurich)

- Measures: both utility and injection robustness of tool-using agents in one framework.
- Format: realistic task suites (email, banking, travel) with hundreds of injection test cases; reports task success without attack, attack success rate, and utility under attack.
- Caveats: defenses tuned to its attack distribution may not generalize to adaptive attackers; utility-security trade-off means single-number summaries mislead, so always quote both axes.

## 6. Static benchmarks for contrast

These measure knowledge or single-response reasoning, not agency; they are listed because agent papers still cite them and because their lifecycle (release, climb, saturation, contamination) is the cautionary tale agent benchmarks are trying to escape.

### MMLU (2020) and MMLU-Pro (2024)

- Measures: multi-domain knowledge via multiple choice (57 subjects; MMLU-Pro hardens with ten options and more reasoning-heavy items).
- Caveats: saturated at the frontier; documented label errors in several subjects; heavy contamination exposure; choice-format gaming (answer-only shortcuts) inflates scores.

### GPQA (2023)

- Measures: graduate-level science questions written to be "Google-proof"; the Diamond subset (198 questions) is the commonly quoted split.
- Caveats: small, so confidence intervals are wide; expert human baselines are themselves modest, complicating "superhuman" claims; frontier reasoning models have largely saturated Diamond.

### Humanity's Last Exam (2025, CAIS and Scale AI)

- Measures: frontier academic knowledge and reasoning across dozens of disciplines, roughly 2,500 expert-written closed-ended questions.
- Caveats: designed to resist saturation but not immune; multimodal and text subsets differ in difficulty; calibration is poor (models are confidently wrong), so accuracy alone understates the problem; with search tools enabled, scores measure retrieval plus reasoning, not parametric knowledge.

### MATH (2021) and AIME-style sets

- Measures: competition mathematics with checkable final answers.
- Caveats: MATH is effectively solved at the frontier; AIME sets are tiny (30 problems per year), so single-year scores are noisy; answer-matching can miss equivalent forms without careful normalization.

### Chatbot Arena / LMArena (2023-)

- Measures: human pairwise preference over anonymized model responses, aggregated to Elo-style ratings.
- Caveats: measures preference, not correctness; style and verbosity influence votes; prompt distribution is arena-user-shaped, not enterprise-shaped; ratings say little about tool use or long-horizon agency.

## 7. Choosing benchmarks for your own claims

Match the benchmark to the claim: coding agents to SWE-bench Verified or Terminal-Bench, conversational tool use to tau2-bench, research to GAIA or BrowseComp, computer use to OSWorld, security to AgentDojo.
Report the harness, model version, date, metric, and subset alongside any number.
For deployment decisions, weight pass^k-style reliability metrics over pass@k capability metrics.
And treat your own domain eval as the benchmark that actually matters; public benchmarks are for orientation and regression, not for predicting your product's success.
