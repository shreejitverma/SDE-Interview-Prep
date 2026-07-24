# Volume 10 - Evaluation and Observability

How to know whether an agent works: eval design, graders, benchmarks, judges, tracing, and the production feedback loop.
Claims that rot (tool landscapes, benchmark states) are date-stamped as of early 2026.

## Chapters

- [Chapter 01 - Evals Are The Bottleneck](Chapter_01_Evals_Are_The_Bottleneck.md): Why evaluation is the hardest and highest-leverage part of agent engineering, the demo-to-production gap, vibes versus measurement, eval-driven development, and building an eval culture.
- [Chapter 02 - Eval Types and Graders](Chapter_02_Eval_Types_and_Graders.md): The two-axis taxonomy of evals and the grader ladder from exact match through execution-based checks to LLM judges, plus capability versus regression and offline versus online.
- [Chapter 03 - Building Agent Evals](Chapter_03_Building_Agent_Evals.md): Trajectory versus outcome evaluation, environment and task design, pass@k versus pass^k, tau-bench-style user simulation, eval statistics, and a worked Python harness.
- [Chapter 04 - The Benchmark Landscape](Chapter_04_The_Benchmark_Landscape.md): SWE-bench and its variants, tau-bench, GAIA, WebArena, OSWorld, Terminal-bench, AgentBench, BrowseComp, and HLE: what each measures, known flaws, contamination, and why leaderboard deltas rarely transfer.
- [Chapter 05 - LLM As Judge](Chapter_05_LLM_As_Judge.md): The judge as a calibrated instrument: prompt anatomy, pairwise versus pointwise, the bias catalog, calibration against human labels, judge selection, structural failures, and meta-evaluation.
- [Chapter 06 - Tracing and Observability](Chapter_06_Tracing_and_Observability.md): Spans and traces for agent runs, OpenTelemetry GenAI conventions, the tooling landscape, what to log, privacy in trace stores, and the systematic trace-debugging workflow.
- [Chapter 07 - Production Evaluation](Chapter_07_Production_Evaluation.md): Implicit feedback signals, A/B testing agents, tiered CI regression gates, canary deployments for prompt and model changes, drift monitoring, and the data flywheel that turns failures into eval cases.
