---
type: problem
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Drill: Catastrophic Backtracking and Stream Bottleneck

> [!summary] Scope
> Timed problem drill presenting a real-time log ingestion pipeline that hangs under production traffic due to catastrophic regex backtracking in a parsing filter combined with libc stdio block-buffering deadlocks.
> Challenges the engineer to detect CPU pinning, isolate the polynomial backtracking pattern, rewrite the regex for linear execution or replace it with ripgrep, and unbuffer stream pipes using stdbuf.
> Reaches Debug, validating Regex-Engines-Automata-Backtracking-and-ReDoS.md and Pipe-Buffering-and-Stdbuf.md.

## Related

- Prerequisites: [[Regex-Engines-Automata-Backtracking-and-ReDoS]], [[Pipe-Buffering-and-Stdbuf]], [[Ripgrep-Internals-and-Fast-Text-Search]]
- Next: [[Drill-Command-Line-SSH-Multiplexing-and-Broken-PTY]]
- See also: [[19-Shell-and-Unix-Engineering/08-Coreutils-and-the-Userland/stdio-Buffering-and-stdbuf|stdio Buffering and stdbuf]], [[19-Shell-and-Unix-Engineering/07-Regex-and-Text-Processing/ReDoS|ReDoS]]
- Lab: `code/01-Command-Line/drill_redos_stream/` (requires: linux)
