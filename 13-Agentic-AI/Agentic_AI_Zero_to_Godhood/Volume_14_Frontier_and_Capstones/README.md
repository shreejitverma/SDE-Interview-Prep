# Volume 14 - Frontier and Capstones

The edge of the field and the proof that you can build at it.
Chapters 01-05 survey the research directions that are actively changing what agents can do, Chapter 06 gives you the habits for tracking a field that outpaces any book, and Chapter 07 closes the track with three graded capstone projects that integrate Volumes 01-13.

Content is current to early 2026.
This volume rots faster than any other in the track by construction, so model names, benchmark states, and ecosystem claims are date-stamped, speculation is marked as speculation, and Chapter 06 exists specifically so you can re-derive the picture yourself rather than trusting this one indefinitely.

## Chapters

- [Chapter 01 - RL For Agents](Chapter_01_RL_For_Agents.md): Why RL returned to the center of LLM training, RL with verifiable rewards, PPO and GRPO at working depth, agentic RL in multi-turn tool environments, environment design as the new data engineering, and reward hacking in agent training.
- [Chapter 02 - Test-Time Compute](Chapter_02_Test_Time_Compute.md): Inference compute as the third scaling axis: sequential thinking, parallel sampling and the selection problem, search methods and why tree search underdelivered, outcome versus process reward models, compute-optimal allocation, and the intelligence-per-dollar-per-second framing.
- [Chapter 03 - Self-Improvement](Chapter_03_Self_Improvement.md): Where the improvement signal actually comes from: synthetic data pipelines and their filters, distillation economics, trajectory harvesting flywheels, why the self-play analogy breaks for open-ended tasks, AlphaEvolve-class evolutionary search, DSPy-style scaffold optimization, and a calibrated hype map.
- [Chapter 04 - Beyond Text](Chapter_04_Beyond_Text.md): Agency outside the text channel: cascaded versus native speech-to-speech voice agents and their latency floors, turn taking and barge-in, world models, vision-language-action models and the sim-to-real gap, video understanding, and a deployment-reality map.
- [Chapter 05 - Continual Learning and Personalization](Chapter_05_Continual_Learning_and_Personalization.md): Adaptation without weight updates: in-context learning as the substrate and its four walls, memory systems as pseudo-learning, per-tenant LoRA and multi-adapter serving economics, the online-learning threat catalog, evaluating personalization, and why continual weight updates remain rare.
- [Chapter 06 - Keeping Up](Chapter_06_Keeping_Up.md): Staying calibrated on a moving field: a triage system and source map for labs, papers, newsletters, and benchmark trackers, the three-pass method for reading papers as an engineer, cheap scaled-down reproduction, deflating capability announcements, and building a private eval suite as personal ground truth.
- [Chapter 07 - Capstone Projects](Chapter_07_Capstone_Projects.md): Three graded capstones with milestones, verification criteria, rubrics, failure catalogs, and godhood-bar stretch goals: a deep-research agent with citation verification, a production-grade coding agent with permissions and budgets, and an ops agent over real tools with approval gates; plus the closing statement on what mastery here actually means.

## How to read this volume

Chapters 01 through 03 form a sequence about where capability comes from (training signal, inference compute, and generated data) and are best read in order.
Chapter 04 and Chapter 05 are independent surveys and can be read whenever their subject becomes relevant to you.
Chapter 06 is the most durable chapter in the volume and should be read early rather than last, because its habits are what keep the rest of the volume from expiring on you.
Chapter 07 is not reading, it is work; budget weeks, not an evening.

## Related volumes

- Volume 01 covers pretraining, post-training, and reasoning models, which Chapters 01 through 03 extend.
- Volume 07 covers multi-agent systems, the orchestration substrate for Capstone A.
- Volume 10 covers evaluation and observability, which every chapter here depends on and which Capstones A, B, and C each stress differently.
- Volume 11 covers safety, security, and alignment, the source of the guardrails, permissions, and approval gates the capstones require.
- Volume 12 covers production engineering, the cost, latency, reliability, and operations layer the capstones are graded on.
- Volume 13 covers coding agents and computer use, the direct prerequisite for Capstone B.
