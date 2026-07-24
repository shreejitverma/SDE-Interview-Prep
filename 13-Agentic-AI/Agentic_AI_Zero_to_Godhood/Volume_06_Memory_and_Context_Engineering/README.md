# Volume 06 - Memory and Context Engineering

This volume treats the context window as a scarce, rotting, billable resource and builds the full engineering stack around it: what goes in the window, what gets evicted, what lives outside it, and what it all costs.
Claims tied to specific providers, APIs, and prices are date-stamped as of early 2026.

## Chapters

- [Chapter 01 - Context Engineering as a Discipline](Chapter_01_Context_Engineering_As_A_Discipline.md): the shift from prompt to context engineering, context rot and the attention budget, and the "smallest set of high-signal tokens" principle with the design rules it generates.
- [Chapter 02 - Anatomy of a Context](Chapter_02_Anatomy_Of_A_Context.md): dissection of a real agent context (system prompt layers, tool definitions, history, tool results, retrieved content, scratchpads), token accounting in practice, and position effects.
- [Chapter 03 - Compaction and Summarization](Chapter_03_Compaction_and_Summarization.md): truncation versus summarization versus compaction, the contract of what must survive, tool-result clearing, a five-stage pipeline with code, Claude Code as case study, and context-editing APIs.
- [Chapter 04 - Scratchpads and External Memory](Chapter_04_Scratchpads_and_External_Memory.md): note-taking, todo lists as attention anchors, the CLAUDE.md/AGENTS.md memory-file pattern, file-system-as-memory, structured note schemas, JIT retrieval versus pre-loading, and the agentic memory tool.
- [Chapter 05 - Long-Term Memory](Chapter_05_Long_Term_Memory.md): episodic/semantic/procedural taxonomy, ChatGPT and Claude user memory, extraction and consolidation pipelines, storage shapes, staleness and contradiction handling, privacy, and the MemGPT/Letta, Mem0, and Zep landscape.
- [Chapter 06 - State and Persistence](Chapter_06_State_and_Persistence.md): session versus durable state, checkpointing and crash-resumability, the side-effect ledger, event-sourced designs with the transcript as source of truth, multi-session continuity, and schema versioning.
- [Chapter 07 - Caching and Context Economics](Chapter_07_Caching_and_Context_Economics.md): KV cache and prompt-caching mechanics, structuring for cache hits (stable prefix, append-only history), cost modeling for long-running agents, measuring hit rates, and when caching dictates architecture.
