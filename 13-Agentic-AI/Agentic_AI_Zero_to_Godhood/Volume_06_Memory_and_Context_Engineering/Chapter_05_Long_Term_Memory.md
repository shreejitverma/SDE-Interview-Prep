# Chapter 05 - Long-Term Memory

## What you will master

- The episodic, semantic, and procedural memory taxonomy and why it usefully carves agent memory.
- How production user memory systems work: ChatGPT memory and Claude memory as of 2025-2026.
- Memory extraction pipelines: deciding what to remember, distilling it, and consolidating it.
- Storage and retrieval design: flat stores, vector stores, and knowledge graphs, with honest trade-offs.
- Staleness and contradiction handling: supersession, temporal validity, and confidence decay.
- Privacy, consent, and governance obligations that memory creates.
- The framework landscape: the MemGPT-to-Letta lineage, Mem0, Zep, and when to build versus adopt.

## 5.1 From session memory to long-term memory

Chapter 04 gave agents durable storage within a project: scratchpads, memory files, a file tree.
Long-term memory is the next ring out: knowledge that spans sessions, tasks, projects, and often users, accumulated over months, and consulted by systems that were not running when the knowledge was written.
The qualitative shifts that justify a separate chapter:

- Volume: months of interactions produce far more candidate memories than any file tree an agent can navigate by listing; retrieval must be search, not browsing.
- Authorship distance: the session that reads a memory is not the session that wrote it, so context that made the memory obvious is gone; memories must be self-contained.
- Contradiction is normal: the world and the user change, so a long-term store without a supersession mechanism converges on being wrong.
- Stakes: user memories are personal data; storage, consent, and deletion stop being engineering conveniences and become obligations.

## 5.2 The taxonomy: episodic, semantic, procedural

Cognitive science's memory taxonomy transferred to agents remarkably well, and by 2025 it was the standard vocabulary across frameworks (LangGraph's memory docs, Letta's design, and academic surveys all use it).

Episodic memory: records of specific events; "on 2026-01-12 the user asked for a refactor of the billing module and rejected the first proposal for being too invasive."
Its native form is the transcript or a distilled event log; its native queries are "what happened when" and "have we tried this before."
It is cheap to write (append) and expensive to use raw (long, redundant), which is why pipelines distill it.

Semantic memory: facts abstracted away from the events that taught them; "the user prefers minimal diffs," "the billing module has no test coverage," "the user's timezone is IST."
Its native form is a set of statements with provenance; its native query is lookup by topic or entity.
It is what most people mean by "the agent remembers me," and it is where contradiction handling lives, because facts change while events do not.

Procedural memory: knowledge of how to do things; "to deploy this service, run these three commands in this order," or, in learned form, updated instructions and skills the agent applies without recalling any specific episode.
Its native forms are instruction files (the CLAUDE.md pattern of Chapter 04), runbooks, and skill libraries; as of early 2026 procedural memory is mostly curated rather than automatically learned, and automatic prompt-self-improvement remains research territory with real regression risk.

Why the taxonomy earns its keep in engineering terms: the three types have different write paths, different staleness behavior, and different retrieval patterns, so systems that jam all three into one vector store inherit the worst properties of each.
Events never become false, so episodic stores are append-only.
Facts become false, so semantic stores need supersession.
Procedures become dangerous when stale, so procedural stores need review gates.

## 5.3 Case studies: user memory in ChatGPT and Claude

The consumer assistants are the largest deployed memory systems and their designs are instructive; behavior described here is as of late 2025 to early 2026 and will drift.

ChatGPT memory (OpenAI) evolved in two stages.
Saved memories (rolled out broadly in 2024): the model detects memorable facts during chat or is told "remember X," writes short natural-language memory entries to a per-user store, and relevant entries are injected into future conversations; users can view, edit, and delete entries individually.
Reference chat history (announced April 2025): beyond curated entries, the assistant can draw on the user's past conversations wholesale, which trades user legibility (you cannot enumerate what it might recall) for coverage; both features are user-disableable, and a temporary-chat mode exists that bypasses memory entirely.

Claude memory (Anthropic) shipped in stages during 2025: first the ability to search and reference past chats on request, then a memory feature for team and enterprise plans and later consumer plans, notable for its emphasis on legibility and scoping.
Claude's design as launched: memory is project-scoped (memories from one project do not leak into another), users can view a summary of what Claude remembers and edit it in natural language, and an incognito mode exists for conversations that should leave no trace.
The project-scoping decision is the memorable engineering lesson: it accepts worse recall across contexts in exchange for containment, because a work-project memory surfacing in a personal conversation is both a quality bug and a privacy incident.

Common structure worth extracting from both: a curated semantic store with user-visible CRUD, an episodic layer over raw history, injection of a relevant subset rather than the whole store, and explicit off-switches.
Every one of these is a design obligation you inherit when you build memory into your own product.

## 5.4 The memory extraction pipeline

Long-term memory systems are ETL pipelines whose source is conversation and whose warehouse is the memory store.
The canonical stages, present in Mem0, Zep, and most in-house designs as of 2025-2026:

1. Candidate detection: decide which spans of a session contain memorable content; triggers include explicit user requests ("remember that"), preference statements, corrections, stable facts about entities, and task outcomes.
2. Extraction: an LLM pass distills candidates into atomic, self-contained statements; atomicity matters because retrieval, deduplication, and supersession all operate per-statement, and a compound memory ("prefers Python and lives in Chennai") fails all three when half of it changes.
3. Consolidation: compare each new statement against the existing store and decide add, update, supersede, or discard; this is the stage that separates a memory system from an append-only log, and Mem0's published design makes exactly this add/update/delete decision loop its core.
4. Storage with metadata: persist the statement with provenance (source session, timestamp), scope (user, project, organization), confidence, and validity fields.
5. Retrieval-time assembly: at each new session or turn, select the relevant subset, budget it (Chapter 02), and inject it with provenance labels.

Timing choices for stages 1-3, with their trade-offs: inline during the session (freshest, but adds latency and tokens to live turns), at session end (the common default; one batch pass over the transcript), or offline batch consolidation (cheapest per memory, tolerates hours of staleness, and enables cross-session dedup); mature systems run session-end extraction plus periodic offline consolidation, mirroring the fast/slow split of human memory consolidation.

Quality controls that experience shows are not optional:

- Extraction precision beats recall: a forgotten preference costs a repeat question; a wrong or over-general memory ("user hates tests," extracted from one frustrated message) poisons every future session until found.
- Never extract from untrusted content: memories mined from tool output or retrieved documents are a persistent prompt-injection channel (the poisoned-note attack of Chapter 04, now with unbounded lifetime); extract from user statements and verified outcomes only, and label provenance.
- Keep the raw episode pointer on every memory so any statement can be re-checked against what was actually said.

## 5.5 Storage and retrieval design

Three storage shapes cover the field; the choice is a real fork with real consequences.

Flat store (a list of memory statements, filtered by scope, injected by recency or simple relevance): trivially simple, fully legible, adequate up to a few hundred memories; this is roughly the shape of ChatGPT saved memories, and its ceiling is retrieval quality as the store grows.

Vector store (statements embedded, retrieved by semantic similarity to the current context): scales to large stores and finds paraphrase matches; inherits every RAG failure mode of Volume 05: similarity is not relevance, top-k truncates arbitrarily, and near-duplicate memories crowd the result list, which is why consolidation-time dedup is load-bearing.

Knowledge graph (entities and relations, with memories attached to nodes and edges): supports multi-hop queries ("what does the user's employer use for CI") and principled contradiction handling on edges; costs an extraction step that must get entity resolution right, and errors compound structurally.
Zep's Graphiti engine is the flagship graph design as of 2025: a temporal knowledge graph where each edge carries validity intervals, so "user works at X" is closed with an end date when "user works at Y" arrives, giving supersession and time-travel queries as first-class operations rather than bolted-on flags.

Retrieval-time assembly rules that hold across all three shapes:

- Retrieve against a composed query (current message plus active task summary), not the last message alone.
- Budget memory injection like any other section (Chapter 02); a dozen crisp statements nearly always beats fifty mediocre ones, by the attention economics of Chapter 01.
- Inject with provenance and age ("remembered from 2025-11, user-stated"), so the model can weigh trust and the user can contest.
- Prefer injecting semantic statements and pointers to episodes, letting the agent JIT-retrieve full episodes only when the task demands detail (the hybrid rule of Chapter 04).

## 5.6 Staleness and contradiction

Facts decay; a memory system without a theory of time is a misinformation system with good recall.
The working toolkit:

- Temporal validity: store observed-at and, where possible, valid-from and valid-until on each statement; supersession closes the old interval rather than deleting the row, preserving history ("moved from Bangalore to Chennai") instead of erasing it.
- Supersession over deletion for facts, deletion reserved for user-requested removal and extraction errors; the difference is auditability.
- Contradiction resolution at consolidation time: newer user-stated beats older user-stated; user-stated beats inferred; high-specificity beats generalization; when signals tie, keep both flagged as conflicting and let the assistant ask rather than guess, because silently picking wrong is the worst outcome.
- Confidence decay by category: identity facts decay slowly, preferences decay moderately, situational facts ("is traveling this week") should expire in days; category-aware TTLs are crude and effective.
- Periodic re-validation for high-impact memories: cheap batch passes that ask "is this still consistent with recent episodes" and downgrade confidence when evidence thins.

The downside of all this machinery is that every knob is a policy choice a user cannot see; the mitigation, demonstrated by the consumer systems, is exposing the store: viewable, editable, deletable memories convert silent policy into correctable state.

## 5.7 Privacy, consent, and governance

Memory converts a stateless service into a dossier-holding one, and the obligations are concrete as of 2026:

- Consent and control: memory should be visibly on-or-off, per-scope where possible, with an incognito path; both major assistants ship all three, which is now the baseline user expectation.
- Right to erasure: GDPR-style deletion requests must reach every derivative: the statement, its embeddings, graph edges, caches, and any copies in downstream contexts; design the delete path before launch, because retrofitting deletion onto a graph with derived edges is grim.
- Data minimization: extraction precision (Section 5.4) is also a compliance property; do not store sensitive categories (health, credentials, financial identifiers) unless the product genuinely requires them, and gate such categories behind explicit rules in the extraction prompt plus mechanical filters.
- Scoping as containment: Claude's project-scoped design generalizes; organizational memory must respect the access-control boundaries of its sources, or memory becomes a privilege-escalation channel where an agent launders restricted documents into freely-retrieved facts.
- Auditability: log memory reads and writes like any other data access; when an agent says something surprising about a user, "which memory, from which episode" must be answerable.

## 5.8 Frameworks: the lineage and the landscape

MemGPT (Packer et al., October 2023) is the ancestor of the modern designs: it framed the LLM as an operating system managing a memory hierarchy, with main context (the window) and external context (storage), and crucially let the model edit its own memory via function calls: self-editing memory blocks for core facts, archival storage for the long tail, and interrupts to page data in and out.
Nearly everything in Chapters 03-05 is visible in embryo in that paper: the window-as-RAM framing, tool-driven memory operations, and paging as retrieval.
The project evolved into Letta (renamed 2024), an agent server where agents carry persistent memory blocks and message history in a database, with the model continuing to manage memory through tools; the Anthropic memory tool of Chapter 04 is the same idea standardized at the platform level with the model trained on the interface.

Mem0 (open source, prominent from 2024-2025) packages the extraction-consolidation pipeline of Section 5.4 as a service: add-session, extract atomic memories, run the add/update/delete decision against the store, retrieve by relevance; its published evaluations (on benchmarks like LOCOMO, per its 2025 paper) claim accuracy gains and large token savings versus full-history baselines; treat vendor-published numbers as directional and re-run on your workload.

Zep (with its Graphiti engine, open sourced 2024-2025) is the temporal-knowledge-graph position described in Section 5.5, aimed at contradiction-heavy, entity-rich domains.

Build versus adopt, honestly: adopt a framework when your needs match its shape (user-assistant products with standard preference memory fit Mem0-like pipelines; entity-heavy enterprise data fits graph designs) and when you can live with its consolidation policies, because those policies are the product and they are opinionated.
Build when memory is your differentiator, when your contradiction or compliance rules are unusual, or when file-shaped memory from Chapter 04 plus a search index already clears your bar, which for single-project coding and operations agents it very often does.
The trap to avoid in 2026 is resume-driven memory infrastructure: a knowledge graph serving a use case that a constraints file and grep would have served better.

## Exercises

1. Classify fifty real memory candidates from your own assistant transcripts into episodic, semantic, and procedural, and specify for each class its write path, staleness policy, and retrieval pattern in your design.
2. Build the minimal extraction pipeline: an LLM pass that turns a session transcript into atomic statements with provenance and scope, plus a consolidation pass implementing add/update/supersede/discard against a flat store; measure extraction precision by hand-auditing 100 extracted memories.
3. Extend the store with temporal validity and implement the contradiction policy of Section 5.6; unit-test the supersession cases (move, job change, preference reversal) and the tie case where the system must flag rather than pick.
4. Run a poisoning red-team: seed a session with tool output containing a plausible false fact about the user, verify whether your pipeline extracts it, then add provenance gating and demonstrate the attack fails.
5. Implement the full deletion path: delete one user memory and prove by inspection that the statement, its embedding, and any injected copies in active sessions are gone; write down what your design would have to change if a graph store had derived edges from it.
6. Evaluate one framework (Mem0, Zep, or Letta) against your hand-built pipeline on twenty multi-session scenarios with planted preferences and one planted contradiction; report recall, precision, contradiction handling, and tokens per session.

## Godhood check

You have mastered this chapter when you can do the following without notes.

- Define episodic, semantic, and procedural memory, and derive from the definitions why their stores need different mutation policies (append-only, supersession, review-gated).
- Describe the memory architectures of ChatGPT and Claude as of 2025-2026, and argue for or against project-scoped memory using both quality and privacy reasoning.
- Draw the five-stage extraction pipeline and explain why atomicity, precision-over-recall, and provenance gating are each load-bearing.
- Choose among flat, vector, and graph storage for three described products, naming the failure mode you accept in each choice.
- Specify a staleness-and-contradiction policy with temporal validity, supersession, and category TTLs, and explain why user-visible memory CRUD is the mitigation for policy opacity.
- Trace the MemGPT-to-Letta lineage, state what Mem0 and Zep each actually do, and give the build-versus-adopt decision rule with its trap.
