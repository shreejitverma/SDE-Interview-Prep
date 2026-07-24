# Volume 04 - Agent Architectures

This volume covers the design space between a single model call and a fully autonomous agent: the composable patterns, the historical lineage, the deliberation and self-correction mechanisms, the runtime formalisms, the harness that carries it all, and a decision framework for choosing among them.

## Chapters

- [Chapter 01 - Workflows Versus Agents](Chapter_01_Workflows_Versus_Agents.md): the Anthropic "Building Effective Agents" taxonomy (prompt chaining, routing, parallelization, orchestrator-workers, evaluator-optimizer) with pseudocode, the workflow/agent control-flow distinction, and the simplicity-first ladder.
- [Chapter 02 - ReAct and Its Descendants](Chapter_02_ReAct_and_Its_Descendants.md): the reason-plus-act interleaving loop, thought-action-observation trace anatomy and failure taxonomy, MRKL and Toolformer context, and how native tool use and thinking modes subsumed explicit ReAct prompting.
- [Chapter 03 - Planning](Chapter_03_Planning.md): plan-then-execute, plan data structures (todo lists, DAGs, hierarchies), replanning policy, Tree of Thoughts and search, LLM-Modulo verifier-guided planning, plan mode in coding agents, and when explicit planning helps versus ossifies.
- [Chapter 04 - Reflection and Self-Critique](Chapter_04_Reflection_and_Self_Critique.md): self-consistency, Self-Refine, Reflexion, critic and verifier loops, evaluator-optimizer in production depth, the empirical limits of intrinsic self-correction, and LLM-judge pathologies including rubber-stamping.
- [Chapter 05 - State Machines and Graphs](Chapter_05_State_Machines_and_Graphs.md): modeling agents as state machines and graphs, why LangGraph reified control flow, checkpointing and durable execution with their obligations, and the honest conditions under which the graph abstraction beats plain code.
- [Chapter 06 - Harness Design](Chapter_06_Harness_Design.md): the harness as the real product, four-layer system prompt architecture, prompts-as-code discipline, tool surface curation as agent UX, environment and feedback-channel design, the harness/model co-evolution loop, and lessons from Claude Code.
- [Chapter 07 - Choosing an Architecture](Chapter_07_Choosing_An_Architecture.md): a decision framework on complexity, verifiability, and risk, cost ceilings and latency budgets as pruning constraints, four worked case studies mapped to architectures, and revisit triggers for dated decisions.
