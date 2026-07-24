# Appendix B: The Canon

The reading list for agentic AI engineering: canonical papers and engineering blog posts, organized by topic.
Every entry is a real, verifiable work; where exact venue details matter, the year given is the year of first public release (arXiv or blog).
Read in roughly this order within each section; sections mirror the volume ordering of this track.
Knowledge as of early 2026.

## 1. Transformers and foundations

**Neural Machine Translation by Jointly Learning to Align and Translate** - Bahdanau, Cho, Bengio (2014).
Introduced the attention mechanism for sequence-to-sequence models, letting the decoder softly search the source sentence instead of compressing it into one vector.
Everything in the transformer lineage descends from this idea.

**Attention Is All You Need** - Vaswani et al. (2017).
Introduced the transformer: an architecture built entirely from attention and feed-forward layers, discarding recurrence.
Its parallelizable training is what made scaling to modern sizes economically feasible.
Still the reference point for every architectural discussion in the field.

**The Illustrated Transformer** - Jay Alammar (2018, blog).
The standard visual walkthrough of the transformer's internals.
Worth reading before the paper if you have never implemented attention; worth skimming after to check your mental model.

**Language Models are Unsupervised Multitask Learners (GPT-2)** - Radford et al. (2019).
Showed that a decoder-only model trained purely on next-token prediction over web text acquires many tasks zero-shot.
Established the decoder-only, scale-up trajectory that GPT-3 confirmed.

**Language Models are Few-Shot Learners (GPT-3)** - Brown et al. (2020).
Demonstrated in-context learning at 175B parameters: task behavior induced from prompt examples with no weight updates.
This is the capability that makes prompting, and therefore agent scaffolding, possible.
Arguably the founding document of the entire post-2020 applied-LLM industry.

**RoFormer: Enhanced Transformer with Rotary Position Embedding** - Su et al. (2021).
Introduced RoPE, the rotary positional encoding used by Llama-class and most modern open models.
Relevant to agents because positional encoding choices govern context-length extension behavior.

**FlashAttention** - Dao et al. (2022).
An IO-aware exact attention algorithm that tiles computation to avoid materializing the attention matrix in slow GPU memory.
Made long contexts computationally practical and is now standard in every serving stack.

**Switch Transformers** - Fedus, Zoph, Shazeer (2021).
Scaled mixture-of-experts routing to trillion-parameter sparse models with simplified top-1 routing.
The lineage behind Mixtral and the sparse architectures used by 2025-era frontier open models.

**Mixtral of Experts** - Jiang et al. (2024).
An open sparse MoE model (8x7B) matching much larger dense models at a fraction of inference cost.
Made MoE the default architecture conversation for open-weight deployment.

**The Llama 3 Herd of Models** - Grattafiori et al., Meta (2024).
The most detailed public description of a frontier-scale training run (up to 405B parameters), covering data curation, infrastructure, and post-training.
The closest thing to an openly documented frontier pretraining playbook.

## 2. Scaling and pretraining

**Scaling Laws for Neural Language Models** - Kaplan et al. (2020).
Established smooth power-law relationships between loss and model size, data, and compute.
Turned model planning from folklore into budget arithmetic.

**Training Compute-Optimal Large Language Models (Chinchilla)** - Hoffmann et al. (2022).
Corrected Kaplan's coefficients: for a fixed compute budget, models should be smaller and trained on far more data (roughly 20 tokens per parameter).
A 70B model trained this way outperformed the 280B Gopher, resetting industry training practice overnight.

**PaLM: Scaling Language Modeling with Pathways** - Chowdhery et al. (2022).
A 540B dense model whose report documented discontinuous capability improvements with scale and popularized chain-of-thought evaluation at scale.

**Emergent Abilities of Large Language Models** - Wei et al. (2022).
Cataloged capabilities that appear abruptly at scale rather than improving smoothly.
Framed the "emergence" debate that shapes expectations about what the next scale-up buys.

**Are Emergent Abilities of Large Language Models a Mirage?** - Schaeffer, Miranda, Koyejo (2023).
Argued many claimed emergences are artifacts of discontinuous metrics rather than discontinuous capabilities.
Read alongside Wei et al. as a lesson in metric design, which transfers directly to agent evals.

**GPT-4 Technical Report** - OpenAI (2023).
Sparse on architecture but notable for demonstrating predictable scaling of loss from small pilot runs, and for the breadth of its capability and safety evaluations.
Also marks the point where frontier labs stopped publishing architectural detail.

**The Bitter Lesson** - Rich Sutton (2019, essay).
Seventy years of AI history compressed into one claim: general methods that leverage computation beat human-knowledge-engineered methods, every time.
The essay agent engineers cite when deciding to bet on model improvement over elaborate scaffolding.

## 3. Alignment and post-training

**Deep Reinforcement Learning from Human Preferences** - Christiano et al. (2017).
Showed agents can be trained from pairwise human preference comparisons instead of hand-written reward functions.
The methodological seed of RLHF.

**Learning to Summarize from Human Feedback** - Stiennon et al. (2020).
First convincing application of the preference-learning pipeline to language models, on summarization.
Established the SFT, reward model, PPO recipe later scaled by InstructGPT.

**Training Language Models to Follow Instructions with Human Feedback (InstructGPT)** - Ouyang et al. (2022).
The RLHF paper: human raters preferred a 1.3B instruction-tuned model over the 175B GPT-3 base.
Defined the post-training pipeline that turned raw predictors into usable assistants, and directly preceded ChatGPT.

**Training a Helpful and Harmless Assistant with RLHF** - Bai et al., Anthropic (2022).
Anthropic's detailed account of assistant-style RLHF, including the helpfulness-harmlessness tension and iterated online training.
The companion empirical grounding for Constitutional AI.

**Constitutional AI: Harmlessness from AI Feedback** - Bai et al., Anthropic (2022).
Replaced human harmlessness labels with AI self-critique and revision against written principles, plus RL from AI feedback (RLAIF).
The template for scalable-oversight approaches and the origin of "constitution" as a model-behavior artifact.

**Direct Preference Optimization** - Rafailov et al. (2023).
Showed the RLHF objective can be optimized directly on preference pairs with a simple classification loss, eliminating the reward model and RL machinery.
Became the default preference-tuning method for open-weight models within a year.

**Proximal Policy Optimization Algorithms** - Schulman et al. (2017).
The clipped-surrogate policy-gradient algorithm that powered classic RLHF.
Worth knowing precisely because GRPO and its successors are defined by what they remove from it.

**Self-Instruct: Aligning Language Models with Self-Generated Instructions** - Wang et al. (2022).
Bootstrapped instruction-tuning data from the model itself, seeding the synthetic-data flywheel that open post-training now depends on.

**Tulu 3: Pushing Frontiers in Open Language Model Post-Training** - Lambert et al., AI2 (2024).
A fully open post-training recipe (data, code, methods) that named and demonstrated reinforcement learning with verifiable rewards (RLVR).
The bridge between classic RLHF and the reasoning-model training era.

## 4. Reasoning and test-time compute

**Chain-of-Thought Prompting Elicits Reasoning in Large Language Models** - Wei et al. (2022).
Showed that prompting models to produce intermediate reasoning steps dramatically improves multi-step task accuracy at sufficient scale.
The single most consequential prompting result ever published.

**Large Language Models are Zero-Shot Reasoners** - Kojima et al. (2022).
"Let's think step by step" alone, with no examples, elicits chain-of-thought.
Demonstrated that the reasoning behavior was latent, not taught by the few-shot examples.

**Self-Consistency Improves Chain of Thought Reasoning in Language Models** - Wang et al. (2022).
Sample many reasoning paths and majority-vote the answers.
The simplest parallel test-time-compute method and still a strong baseline.

**STaR: Bootstrapping Reasoning with Reasoning** - Zelikman et al. (2022).
Fine-tune on the model's own rationales that led to correct answers, iterate.
The conceptual ancestor of the RL-on-reasoning-traces recipes behind o1-class models.

**Tree of Thoughts: Deliberate Problem Solving with Large Language Models** - Yao et al. (2023).
Framed inference as search over a tree of partial thoughts with generation, evaluation, and backtracking.
Explicit-search scaffolding that reasoning models later partially internalized.

**Let's Verify Step by Step** - Lightman et al., OpenAI (2023).
Process supervision (rewarding each correct reasoning step) beat outcome supervision on MATH, and produced the PRM800K dataset.
The empirical case for process reward models and step-level verification.

**Learning to Reason with LLMs (o1 announcement)** - OpenAI (2024, blog).
Introduced o1 and the public framing of train-time and test-time compute as new scaling axes: the model learns via RL to think longer and thinking longer improves results.
The start of the reasoning-model product era.

**Scaling LLM Test-Time Compute Optimally Can Be More Effective than Scaling Model Parameters** - Snell et al. (2024).
Showed compute-optimal test-time strategies let smaller models outperform much larger ones on suitable problems.
The analytical backbone for inference-compute budgeting.

**DeepSeekMath: Pushing the Limits of Mathematical Reasoning in Open Language Models** - Shao et al. (2024).
Introduced GRPO, the group-baseline policy-gradient algorithm that removes PPO's value model.
Read for the algorithm; it is the one R1 and much of the 2025 open RL ecosystem runs on.

**DeepSeek-R1: Incentivizing Reasoning Capability in LLMs via Reinforcement Learning** - DeepSeek-AI (2025).
Showed that large-scale RL with verifiable rewards, applied to a strong base model, elicits long-horizon reasoning (R1-Zero) without supervised reasoning data, and that the resulting traces distill into small models.
The most influential open release of the reasoning era, both for the method and for publishing it.

**s1: Simple Test-Time Scaling** - Muennighoff et al. (2025).
Achieved strong reasoning by fine-tuning on just 1,000 curated traces plus budget forcing (controlling thinking length by suppressing or forcing continuation).
Evidence that much of reasoning-mode behavior is cheap to elicit from strong bases.

## 5. Tool use and agents

**MRKL Systems** - Karpas et al., AI21 Labs (2022).
An early architecture proposal routing between an LLM and discrete expert modules (calculators, APIs).
Historically important as the pre-function-calling articulation of neuro-symbolic tool routing.

**WebGPT: Browser-Assisted Question-Answering with Human Feedback** - Nakano et al., OpenAI (2021).
Trained GPT-3 to browse the web in a text-based environment and cite sources, using human feedback.
The earliest serious tool-using LLM agent from a frontier lab.

**ReAct: Synergizing Reasoning and Acting in Language Models** - Yao et al. (2022).
Interleaved chain-of-thought reasoning with actions and observations in a single loop, beating both reasoning-only and acting-only baselines.
The paper that defined the shape of the modern agent loop.

**Toolformer: Language Models Can Teach Themselves to Use Tools** - Schick et al. (2023).
Self-supervised insertion of API calls into training text wherever they reduce perplexity, teaching tool use without human demonstrations.
The training-side complement to ReAct's prompting-side loop.

**PAL: Program-Aided Language Models** - Gao et al. (2022).
Offloaded computation from the model to a Python interpreter: the model writes code, the runtime produces the answer.
The intellectual basis for code interpreters and code-as-action agents.

**Reflexion: Language Agents with Verbal Reinforcement Learning** - Shinn et al. (2023).
Agents store natural-language self-critiques of failed attempts and condition retries on them, improving across trials with no weight updates.
The canonical reflection-and-retry pattern.

**Voyager: An Open-Ended Embodied Agent with Large Language Models** - Wang et al. (2023).
A Minecraft agent that writes, verifies, and stores executable skills in a growing library.
The reference design for procedural memory and skill accumulation.

**Generative Agents: Interactive Simulacra of Human Behavior** - Park et al. (2023).
Twenty-five agents in a simulated town with a memory stream, retrieval, reflection, and planning, producing emergent social behavior.
The canonical memory-architecture paper for long-lived agents.

**Gorilla: Large Language Model Connected with Massive APIs** - Patil et al. (2023).
Fine-tuned a model for accurate API call generation with retrieval of API documentation, measuring hallucinated calls.
Early evidence that tool-calling accuracy is trainable and measurable.

**Executable Code Actions Elicit Better LLM Agents (CodeAct)** - Wang et al. (2024).
Unified agent actions as executable Python instead of JSON tool calls, improving success via composition, control flow, and error feedback.
The empirical case behind code-execution-centric agent designs and later "code mode" MCP patterns.

**SWE-agent: Agent-Computer Interfaces Enable Automated Software Engineering** - Yang et al. (2024).
Showed that carefully designed agent-computer interfaces (custom file viewer, search, edit commands with guardrails) substantially improve coding-agent success at fixed model capability.
Named the ACI concept and made harness design a first-class research object.

**LLM Powered Autonomous Agents** - Lilian Weng (2023, blog).
The survey that organized the field's vocabulary: planning, memory, tool use as the three agent components.
Still the best single orientation read for newcomers.

**Building Effective Agents** - Anthropic (2024, blog).
Defined the workflow-vs-agent distinction and cataloged the five workflow patterns (prompt chaining, routing, parallelization, orchestrator-workers, evaluator-optimizer) plus guidance to use the simplest structure that works.
The most-cited engineering document in agent system design.

**A Practical Guide to Building Agents** - OpenAI (2025, guide).
OpenAI's engineering guidance covering single-agent loops, when to split into multi-agent designs, guardrails, and human-in-the-loop.
Read against the Anthropic post to triangulate lab consensus.

**Claude Code: Best Practices for Agentic Coding** - Anthropic (2025, blog).
Concrete operational practice for coding agents: CLAUDE.md conventions, explore-plan-code-commit loops, permission management, and multi-instance workflows.
Valuable as a documented, widely replicated production harness design.

**Writing Effective Tools for Agents** - Anthropic (2025, blog).
Guidance on tool design as prompt engineering: naming, descriptions, response formats, token efficiency, and evaluating tools with agents in the loop.
The reference for ACI craft at the individual-tool level.

## 6. Context and memory

**Lost in the Middle: How Language Models Use Long Contexts** - Liu et al. (2023).
Documented the U-shaped position curve: retrieval accuracy is high at context edges and poor in the middle.
The paper behind the "put critical content first or last" rule.

**MemGPT: Towards LLMs as Operating Systems** - Packer et al. (2023).
Treated the context window as main memory with an OS-style hierarchy: the model pages information between in-context and external storage via self-directed function calls.
The design ancestor of most agent-memory products, including its successor project Letta.

**Context Rot: How Increasing Input Tokens Impacts LLM Performance** - Chroma (2025, technical report).
Systematically measured performance degradation with input length across models, even on trivially simple tasks.
Gave the field the term "context rot" and quantitative justification for aggressive context minimalism.

**Effective Context Engineering for AI Agents** - Anthropic (2025, blog).
Consolidated context-engineering practice: attention budgets, just-in-time retrieval, compaction, structured note-taking, and sub-agent architectures.
The clearest statement of context as a finite resource with diminishing marginal returns.

## 7. Retrieval and RAG

**Retrieval-Augmented Generation for Knowledge-Intensive NLP Tasks** - Lewis et al. (2020).
Combined a dense retriever with a generator, trained end-to-end, for knowledge-intensive tasks.
The paper that named RAG; the trained architecture faded but the pattern became universal.

**Dense Passage Retrieval for Open-Domain Question Answering** - Karpukhin et al. (2020).
Showed dual-encoder dense retrieval trained on question-passage pairs beats BM25 for open-domain QA.
The foundation of embedding-based retrieval stacks.

**Sentence-BERT** - Reimers and Gurevych (2019).
Siamese BERT networks producing sentence embeddings comparable by cosine similarity.
The lineage behind the sentence-transformers ecosystem most RAG systems still build on.

**Precise Zero-Shot Dense Retrieval without Relevance Labels (HyDE)** - Gao et al. (2022).
Embed a model-written hypothetical answer instead of the query.
A cheap, robust fix for query-document vocabulary mismatch.

**Self-RAG: Learning to Retrieve, Generate, and Critique through Self-Reflection** - Asai et al. (2023).
Trained a model to decide when to retrieve and to critique retrieved passages and its own generations via reflection tokens.
An early, influential form of adaptive (agentic) retrieval.

**RAPTOR: Recursive Abstractive Processing for Tree-Organized Retrieval** - Sarthi et al. (2024).
Built a tree of recursive cluster summaries over a corpus so retrieval can hit the right abstraction level.
The reference technique for corpus-level and thematic questions.

**From Local to Global: A Graph RAG Approach to Query-Focused Summarization** - Edge et al., Microsoft (2024).
Extracted an entity knowledge graph and community summaries to answer global "what are the themes" questions that chunk retrieval cannot.
The paper behind the GraphRAG ecosystem.

**Introducing Contextual Retrieval** - Anthropic (2024, blog).
Prepend a model-written, chunk-situating context sentence to each chunk before embedding and indexing, combined with BM25 and reranking.
Reported large reductions in retrieval failure rate and made prompt-caching-powered preprocessing a standard trick.

## 8. Multi-agent systems

**CAMEL: Communicative Agents for "Mind" Exploration of Large Language Model Society** - Li et al. (2023).
Two role-playing agents (user and assistant) cooperating via inception prompting to complete tasks autonomously.
Early systematic study of agent-to-agent conversation dynamics.

**Improving Factuality and Reasoning in Language Models through Multiagent Debate** - Du et al. (2023).
Multiple model instances propose answers, read each other's reasoning, and revise over rounds.
The canonical debate result and a useful baseline against single-model self-consistency.

**AutoGen: Enabling Next-Gen LLM Applications via Multi-Agent Conversation** - Wu et al., Microsoft (2023).
Framework paper defining conversable agents whose interactions (agent-agent, agent-human, agent-tool) are programmable conversations.
The academic root of Microsoft's agent-framework line.

**MetaGPT: Meta Programming for a Multi-Agent Collaborative Framework** - Hong et al. (2023).
Encoded human standard operating procedures (roles, artifacts, review gates) into a software-company-shaped agent team.
The strongest early argument that structured process, not free conversation, is what makes agent teams work.

**ChatDev: Communicative Agents for Software Development** - Qian et al. (2023).
A virtual software company of chatting agents covering design, coding, and testing phases.
Read with MetaGPT as the paired studies of SOP-driven agent organizations.

**How We Built Our Multi-Agent Research System** - Anthropic (2025, blog).
Production account of an orchestrator-workers research system: a lead agent decomposes queries and spawns parallel subagents, with detailed prompt-engineering lessons and the observation that such systems consume roughly 15x more tokens than chat.
The most honest public engineering writeup of multi-agent economics and failure modes.

**Don't Build Multi-Agents** - Walden Yan, Cognition (2025, blog).
Argued that parallel subagents with divergent contexts produce conflicting work, and derived principles: share full context and traces, and avoid decisions made on hidden context.
The essential counterweight to multi-agent enthusiasm; read together with the Anthropic post.

**Announcing the Agent2Agent Protocol (A2A)** - Google (2025, blog).
Introduced an open protocol for inter-agent interoperability: capability discovery via agent cards, task lifecycle, and artifact exchange across vendors.
The reference point for agent-to-agent standardization efforts.

## 9. Evaluation and benchmarks

**Measuring Massive Multitask Language Understanding (MMLU)** - Hendrycks et al. (2020).
Fifty-seven-subject multiple-choice exam that served as the field's headline knowledge benchmark for years.
Read as the archetype of the static benchmark and its saturation-and-contamination lifecycle.

**Evaluating Large Language Models Trained on Code (HumanEval)** - Chen et al., OpenAI (2021).
Introduced execution-based code evaluation (164 problems) and the unbiased pass@k estimator.
The methodological ancestor of all execution-graded coding evals.

**Holistic Evaluation of Language Models (HELM)** - Liang et al., Stanford (2022).
Argued for multi-metric, multi-scenario, transparent evaluation rather than single leaderboard numbers.
The framework paper for thinking about eval coverage and validity.

**Judging LLM-as-a-Judge with MT-Bench and Chatbot Arena** - Zheng et al. (2023).
Validated LLM judges against human preferences and cataloged their biases (position, verbosity, self-preference).
The paper to cite for both the legitimacy and the limits of judge-based evaluation.

**SWE-bench: Can Language Models Resolve Real-World GitHub Issues?** - Jimenez et al. (2023).
Built the benchmark of real repository issues graded by held-out tests; with SWE-bench Verified (OpenAI, 2024) it became the standard coding-agent yardstick.
Also a case study in benchmark hygiene, given later contamination and quality findings.

**tau-bench: A Benchmark for Tool-Agent-User Interaction in Real-World Domains** - Yao et al., Sierra (2024).
Evaluated agents conversing with a simulated user under domain policy, graded on final database state, and introduced pass^k for reliability.
The benchmark that made reliability, not just capability, a headline metric; extended by tau2-bench (2025).

**GAIA: A Benchmark for General AI Assistants** - Mialon et al. (2023).
Real-world assistant questions trivial for humans yet requiring browsing, tool use, and multi-step reasoning from models.
The standard general-assistant benchmark and the origin of the "easy for humans, hard for models" design philosophy.

**WebArena: A Realistic Web Environment for Building Autonomous Agents** - Zhou et al. (2023).
Self-hosted, functional replicas of real web applications with execution-based task grading.
The reference environment for web-agent evaluation.

**OSWorld: Benchmarking Multimodal Agents for Open-Ended Tasks in Real Computer Environments** - Xie et al. (2024).
Real-OS desktop tasks graded by execution-based scripts across applications.
The standard computer-use benchmark and a persistent demonstration of the human-model gap in GUI manipulation.

**AgentBench: Evaluating LLMs as Agents** - Liu et al. (2023).
Eight heterogeneous environments (OS, database, web, games) under one evaluation harness.
Early evidence that agentic capability is distinct from chat capability.

**BrowseComp: A Simple Yet Challenging Benchmark for Browsing Agents** - OpenAI (2025).
Questions whose answers are verifiable but extremely hard to locate, stressing persistent multi-hop web research.
The deep-research-agent benchmark of the 2025 generation.

**Humanity's Last Exam** - Phan et al., CAIS and Scale AI (2025).
Expert-written closed-ended questions across dozens of fields, built explicitly because prior static benchmarks saturated.
The current frontier static-knowledge benchmark and, by design, probably the last of its kind.

**Measuring AI Ability to Complete Long Tasks** - METR (2025).
Proposed the 50 percent task-completion time horizon metric and measured it doubling roughly every seven months.
The cleanest quantitative framing of agent capability growth over time.

## 10. Safety and security

**Prompt Injection Attacks Against GPT-3** - Simon Willison (2022, blog).
Named and defined prompt injection, distinguishing it from jailbreaking as an application-level vulnerability.
Willison's continuing series (including "The Lethal Trifecta", 2025) is the running field log of this unsolved problem.

**Ignore Previous Prompt: Attack Techniques for Language Models** - Perez and Ribeiro (2022).
Early academic study of goal hijacking and prompt leaking against production-style prompts.
Useful as the first taxonomy of injection attack objectives.

**Not What You've Signed Up For: Compromising Real-World LLM-Integrated Applications with Indirect Prompt Injection** - Greshake et al. (2023).
Defined indirect prompt injection: attacks delivered through retrieved or processed content rather than the user's own input.
The threat model every tool-using agent must be designed against.

**Universal and Transferable Adversarial Attacks on Aligned Language Models** - Zou et al. (2023).
Gradient-searched adversarial suffixes (GCG) that jailbreak multiple aligned models and transfer across them.
Demonstrated that alignment training is not adversarially robust.

**Sleeper Agents: Training Deceptive LLMs that Persist Through Safety Training** - Hubinger et al., Anthropic (2024).
Constructed backdoored models whose deceptive behavior survives standard safety fine-tuning, with larger models more persistent.
The key caution against assuming post-training removes hidden behaviors.

**AgentDojo: A Dynamic Environment to Evaluate Prompt Injection Attacks and Defenses for LLM Agents** - Debenedetti et al. (2024).
An executable environment measuring both task utility and injection robustness for tool-using agents.
The standard benchmark for agent-security defense claims.

**Defeating Prompt Injections by Design (CaMeL)** - Debenedetti et al., Google DeepMind (2025).
A system-level defense that extracts control flow from the trusted query and confines untrusted data behind capability-based policies, rather than hoping the model resists injection.
The strongest published argument that agent security must be architectural.

**Design Patterns for Securing LLM Agents Against Prompt Injections** - Beurer-Kellner et al. (2025).
Cataloged principled patterns (plan-then-execute, dual LLM, context minimization, action confinement) with case studies.
The practitioner's pattern reference for injection-resistant agent design.

**Introducing the Model Context Protocol** - Anthropic (2024, blog).
The MCP announcement: motivation, architecture (hosts, clients, servers; tools, resources, prompts), and the standardization bet.
Read with the evolving spec at modelcontextprotocol.io; the 2025 spec revisions (streamable HTTP, elicitation, OAuth-based authorization) show how fast protocol security matured.

**MCP Security Notification: Tool Poisoning Attacks** - Invariant Labs (2025, blog).
Demonstrated malicious instructions hidden in MCP tool descriptions, invisible to users but read by models, including exfiltration via seemingly benign tools.
Named the tool-poisoning attack class and triggered the MCP ecosystem's supply-chain security reckoning.

## 11. Inference systems

**Efficient Memory Management for Large Language Model Serving with PagedAttention (vLLM)** - Kwon et al. (2023).
Virtual-memory-style paging for the KV cache, eliminating fragmentation and enabling high-throughput continuous batching.
The systems paper behind the dominant open serving stack.

**Fast Inference from Transformers via Speculative Decoding** - Leviathan, Kalman, Matias (2023).
A small draft model proposes tokens that the target model verifies in parallel, preserving the exact output distribution while cutting latency.
The standard reference for lossless inference acceleration.

## How to use this list

Read sections 1 through 4 for the model substrate, 5 through 8 for the craft, and 9 through 11 for the discipline of shipping.
Prefer primary sources over summaries once you have context; most entries are short by academic standards.
When a claim in this track and a paper disagree, trust the paper, then check its date against the current state of the field.
