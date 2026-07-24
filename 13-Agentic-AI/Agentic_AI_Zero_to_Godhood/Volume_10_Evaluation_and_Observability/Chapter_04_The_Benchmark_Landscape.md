# Chapter 04 - The Benchmark Landscape

## What you will master

- The major public agent benchmarks as of early 2026: SWE-bench and its Verified and Pro variants, tau-bench and tau2-bench, GAIA, WebArena, OSWorld, Terminal-bench, AgentBench, BrowseComp, and Humanity's Last Exam.
- What each benchmark actually measures, which is often narrower than its name and marketing suggest.
- The known flaws of each: specification bugs, weak tests, simulator faults, environment fragility, and saturation.
- Contamination and overfitting: the mechanisms by which leaderboard numbers inflate, and how to detect them.
- Why leaderboard deltas usually do not transfer to your task, and how to use public benchmarks correctly anyway.

Everything in this chapter is date-stamped as of early 2026; benchmark scores and leaderboard positions are the fastest-rotting facts in the field, so this chapter teaches the anatomy and failure modes of each benchmark rather than current numbers.

## 1. How to read any benchmark

Before the catalog, the five questions that dissect any benchmark:

1. What is the task distribution, and who chose it? Every benchmark encodes its authors' idea of what matters, and the selection process (scraped, hand-authored, filtered) determines what the score means.
2. What is the grader? Execution-based, programmatic state checks, LLM judge, or exact match; the grader sets the ceiling on how much you can trust the score (Chapter 2).
3. What is the harness, and is it standardized? Many "model" comparisons are actually harness comparisons, because scaffolding (retry logic, context management, tool design) moves agent scores by large margins.
4. Is the test set public? Public test sets leak into training corpora and into developer iteration loops; private or rolling test sets resist this at the cost of reproducibility.
5. Is it saturating? A benchmark where frontier systems cluster near the ceiling no longer differentiates them, and remaining deltas are mostly noise and grader artifacts.

Carry these five questions through the catalog below.

## 2. Coding: SWE-bench and its variants

SWE-bench (Princeton, 2023) scrapes real GitHub issues from popular Python repositories and asks the agent to produce a patch, graded by running the repository's test suite: fail-to-pass tests must pass, pass-to-pass tests must not break.
It became the de facto standard for coding-agent claims because it uses real tasks and execution-based grading, the strongest grader type.

What it measures: repository-scale bug fixing in mature Python codebases, given an issue description, under execution grading.
What it does not measure, despite being cited as "coding ability": greenfield development, other languages (the original set is Python-only), design work, code review, or anything requiring interaction with an issue reporter.

Known flaws, each documented publicly:

- Task quality: audits found a nontrivial fraction of original tasks unsolvable or underspecified (issue text missing information the tests require, broken environments, tests that encode the exact patch); SWE-bench Verified (OpenAI collaboration, 2024) is a human-filtered 500-task subset created specifically to fix this, and scores on Verified run meaningfully higher than on the full set because the noise floor was removed.
- Weak tests: some tasks pass with patches that do not actually fix the issue, because the tests are shallow; execution grading is only as strong as the hidden tests.
- Contamination: the repositories and their commit histories, including the actual fix commits, are on GitHub and therefore in pretraining corpora; a model may have memorized the fix.
- Overfitting through iteration: because the test set is public, scaffold developers iterate directly against it, and harness tricks that exploit benchmark regularities (for example, locating the fix by searching for recently modified files) inflate scores without generalizing.

SWE-bench Pro (Scale AI, 2025) responds to saturation and contamination: harder, longer-horizon tasks across more languages, with a partially held-out private set and copyleft-licensed repositories chosen to reduce the chance of inclusion in training data; frontier scores on Pro sit far below Verified scores, which is the intended reset of headroom.
Related variants worth knowing by name: SWE-bench Multimodal (issues with screenshots), SWE-bench Live and similar rolling variants (continuously refreshed tasks postdating training cutoffs, targeting contamination directly), and Multi-SWE-bench (multi-language).

How to use it: SWE-bench Verified remains the cleanest public signal for repository-scale Python bug fixing as of early 2026, provided you compare runs under the same harness and treat single-digit deltas as noise.

## 3. Customer-facing tool use: tau-bench and tau2-bench

tau-bench (Sierra, 2024) evaluates agents in simulated customer-service domains (retail, airline): the agent talks to an LLM-simulated user, calls tools against a mocked business database, and must follow written domain policy; grading compares final database state against the annotated goal, plus checks on required communications.
Its two lasting contributions are the pass^k metric (Chapter 3) and the demonstration that agents with acceptable pass@1 collapse under consistency measurement.

What it measures: multi-turn tool use under policy constraints and social pressure, plus consistency.
Known flaws: the user simulator is part of the instrument, and simulator faults contaminated a nontrivial fraction of original episodes; some tasks have multiple defensible final states that the single-goal-state grading marks wrong; the domains are only two, so the task distribution is narrow.
tau2-bench (2025) added a telecom domain, dual-control tasks where user and agent must coordinate actions (the user performs device steps the agent cannot), and cleaner task specifications addressing the simulator-fault analysis.

How to use it: the closest public analog to support-agent products; more useful as a design template for your own user-simulated evals than as a leaderboard, precisely because its environment-plus-policy-plus-simulator construction is what Chapter 3 tells you to build for your own domain.

## 4. General assistance: GAIA and BrowseComp

GAIA (Meta, HuggingFace and collaborators, 2023) contains real-world assistant questions requiring multi-step reasoning, web browsing, file handling, and multimodality, with unambiguous short-form answers graded by exact match after normalization.
Its design principle: easy for humans, hard for models, trivial to grade; human respondents score very high while early agent systems scored low, and the gap has closed substantially since, moving GAIA toward saturation at its easier levels as of early 2026.
Known flaws: exact-match grading rejects correct answers phrased unexpectedly; some tasks depend on live web resources that change or vanish, so scores drift with the web itself; the public test answers have circulated, raising contamination concerns the private leaderboard set only partially contains.

BrowseComp (OpenAI, 2025) targets deep web research: questions whose short factual answers are deliberately hard to locate, requiring persistent multi-hop browsing, with answers chosen to be easy to verify once found.
What it measures: search persistence and creative query strategy, not synthesis quality; its inverted-construction method (authors start from an obscure fact and build a hard-to-search question) means tasks are adversarially unfindable rather than representative of real research needs.
Known flaws: the inverted construction rewards exhaustive search over judgment; it does not grade the long-form reports real research products produce; and live-web dependence makes runs non-reproducible.

## 5. GUI and environment control: WebArena, OSWorld, Terminal-bench

WebArena (CMU, 2023) provides self-hosted replicas of realistic websites (e-commerce, forum, code hosting, CMS) and grades tasks by programmatic checks on final site state or answer strings.
Self-hosting solves the live-web reproducibility problem and permits state-based grading; the cost is that the frozen replicas age relative to the modern web, and the environment stack is heavy to stand up correctly.
Known flaws: a meaningful fraction of task specifications and graders had bugs identified by later audits (tasks marked failed for correct behavior and vice versa); harness variance across published runs is large; and top agents now score high enough that its headroom is limited, as of early 2026.
VisualWebArena extends it to visually grounded tasks.

OSWorld (2024) scales the idea to full operating systems: real Ubuntu, Windows, and macOS VMs, hundreds of tasks across real applications (office suites, browsers, IDEs), graded by execution-based post-run state checks.
What it measures: end-to-end computer use from screenshots and accessibility trees, including cross-application workflows.
Known flaws: environment fragility at VM scale (timing sensitivity, app version drift) makes runs flaky; some grading scripts encode one valid solution path; human performance is far above agent performance, which is honest headroom but also means small score deltas ride on a low base.
The OSWorld-Verified refresh (2025) fixed hundreds of task and grader issues and standardized infrastructure, the same Verified pattern seen with SWE-bench, which should teach you something: every ambitious agent benchmark ships with a meaningful defect rate, and audits find them only after the leaderboard has been cited for a year.

Terminal-bench (Stanford and Laude Institute, 2025) evaluates agents in a sandboxed terminal: real command-line tasks (builds, data processing, server configuration, debugging) in Docker environments with execution-based grading.
What it measures: the terminal-native slice of computer use, which is the natural habitat of coding agents.
Known flaws: young benchmark, task set still evolving across versions, and container-based grading inherits the usual environment-pinning fragility; version-to-version task churn means scores are not comparable across releases.

## 6. Breadth suites and frontier ceilings: AgentBench and Humanity's Last Exam

AgentBench (Tsinghua and collaborators, 2023) was the first broad multi-environment agent suite: eight environments spanning OS interaction, databases, knowledge graphs, card games, puzzles, household simulation, web shopping, and web browsing.
Its historical contribution is establishing that agent ability varies wildly across environment types and correlates imperfectly with chat quality; its current limitation is age, since its environments are simpler than the 2024-2025 generation and frontier systems have outgrown much of it, as of early 2026.
Treat it as a historical reference and a source of environment-design ideas rather than a live differentiator.

Humanity's Last Exam (CAIS and Scale AI, 2025) is not an agent benchmark: it is a few-thousand-question expert-authored academic exam (text plus images, heavy on graduate-level science and math) built explicitly because MMLU-class exams saturated.
It appears in this chapter because agent products cite it constantly, and you should know what it does and does not claim: it measures closed-form academic knowledge and reasoning at the frontier of difficulty, graded against reference answers, with a partially private set to resist contamination.
Known flaws acknowledged publicly: expert disagreement on some reference answers, a calibration focus that models game poorly, and the general critique that esoteric exam difficulty is not the same axis as practical capability; search-augmented agents also score very differently from bare models, so quoted numbers must specify tooling.
Frontier scores rose fast through 2025, which is the standard life cycle: a benchmark built to be unsaturatable begins saturating within a year or two of release.

## 7. Contamination and overfitting

Contamination is the benchmark's test data influencing the system under test through training; overfitting is the developer iterating against the benchmark until the harness exploits its regularities.
Both inflate scores without improving the capability the benchmark claims to measure, and both are endemic.

The mechanisms, in increasing subtlety:

- Direct inclusion: public test sets, including answers, are crawled into pretraining corpora; for GitHub-derived benchmarks, the actual fix commits are in the corpus by construction.
- Paraphrase leakage: blog posts, papers, and solution writeups about benchmark tasks teach the tasks even when the raw set is excluded from training by hash filtering.
- Selection contamination: model developers choose checkpoints, and scaffold developers choose designs, partly by benchmark scores, so the benchmark shapes the system even with zero data leakage; this is Goodhart pressure, not cheating, and it is universal.
- Harness overfitting: retry counts, prompt phrasing, and tool designs tuned on the public set exploit its idiosyncrasies (task length distribution, repository set, grader quirks).

Detection signals you can apply from outside:

- A model scores far better on a benchmark's public split than on its private or post-cutoff split; rolling benchmarks like the live SWE-bench variants exist to expose exactly this gap.
- Performance drops sharply on lightly perturbed tasks (renamed variables, reworded issues) while human difficulty is unchanged; perturbation studies through 2024-2025 repeatedly found such gaps.
- A system's rank order across two benchmarks measuring supposedly similar skills is inconsistent, suggesting at least one score is artifactual.

The defenses benchmark authors deploy: private held-out sets, rolling task refresh with post-cutoff data, canary strings that let trainers filter the set out of corpora, and Verified-style audits.
None is complete: private sets prevent reproduction, rolling sets break longitudinal comparison, canaries depend on trainer cooperation, and audits fix specification bugs but not Goodhart pressure.

## 8. Why leaderboard deltas do not transfer

The practical question is never "which system tops the leaderboard" but "which system is best at my task", and the mapping between the two is weak for compounding reasons:

- Distribution mismatch: your task distribution overlaps a benchmark's distribution far less than the shared vocabulary suggests; "coding" as measured by Python bug-fixing under issue descriptions is a narrow slice of what your coding workload contains.
- Harness mismatch: leaderboard runs use scaffolds tuned for the benchmark, not the scaffold you will run; since scaffolding moves scores by margins comparable to model generations, the leaderboard's model ranking may not survive transplantation into your harness.
- Grader mismatch: benchmarks grade what is gradeable at scale; your product's success criteria include qualities (communication, judgment about when to ask, cost discipline) the benchmark never scored.
- Saturation compression: near a benchmark's ceiling, deltas between top systems are within noise and grader-artifact range, yet marketing treats a 1-point gap as a ranking.
- Selective reporting: vendors publish the benchmarks they win, with the harness configuration that wins them, and independent reproduction regularly lands lower than launch-post numbers once harness and sampling settings are normalized.

The correct use of public benchmarks, then:

- As a screening filter: a model far below the frontier on execution-graded coding benchmarks is unlikely to be your best coding-agent base, so benchmarks prune the candidate list.
- As a design library: tau-bench's simulator construction, WebArena's state-based grading, OSWorld's environment checks, and SWE-bench's fail-to-pass test discipline are reusable blueprints for your private evals, which is their highest value.
- As a trend instrument: year-over-year movement on a stable benchmark family says something real about the field even when point-in-time deltas between vendors do not.
- Never as a ship decision: the decision instrument for your product is your private suite on your task distribution (Chapters 1-3), and the private suite always overrides the leaderboard when they disagree, which they will.

## Exercises

1. Pick two benchmarks from this chapter and answer the five questions from Section 1 for each from their papers, not from summaries; note where the paper is silent, since silences are usually where the flaws live.
2. Take one SWE-bench Verified task, read the issue, the gold patch, and the fail-to-pass tests, and write down what a weak-test exploit would look like for that specific task; then check whether the tests would actually catch it.
3. Design a contamination probe for a benchmark of your choice: specify a perturbation scheme that preserves human difficulty, and state what score drop you would interpret as contamination evidence versus brittleness evidence, and why the two are hard to distinguish.
4. Your team is choosing between two frontier models whose published scores differ by 2 points on SWE-bench Verified and 6 points on tau2-bench; write the one-page memo explaining what these deltas do and do not imply for your support-agent product, and what 30-task private eval you would run before deciding.
5. Sketch a rolling private benchmark for your own product in the style of the live SWE-bench variants: task source, refresh cadence, how you keep longitudinal comparability despite churn, and what you give up relative to a frozen golden set.

## Godhood check

You have internalized this chapter when you can do the following without reference.

- For each of SWE-bench (plus Verified and Pro), tau-bench and tau2, GAIA, BrowseComp, WebArena, OSWorld, Terminal-bench, AgentBench, and HLE: state in one sentence what it measures, its grader type, and its best-known flaw.
- Explain why the Verified pattern (post-hoc human audit of an ambitious benchmark) has recurred across SWE-bench and OSWorld, and what that implies about any new benchmark's first-year numbers.
- Distinguish the four contamination and overfitting mechanisms and name a detection signal for each.
- Argue the five reasons leaderboard deltas fail to transfer, and state the four legitimate uses of public benchmarks.
- Given a vendor's launch-post benchmark table, list the questions you would ask before believing any row: harness, sampling settings, public versus private split, and reproduction status.
