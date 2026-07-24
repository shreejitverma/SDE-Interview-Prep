# Authoring Style for Agentic AI: Zero to Godhood

These rules govern every chapter in this track.

## Prose
- One full sentence per physical line.
- No emojis anywhere.
- No em dashes; use a plain dash "-" instead.
- Dense, factual, technically precise; no motivational filler.
- Explain the why behind every design decision, not just the what.
- Name trade-offs explicitly, including the downside of the recommended option.

## Structure
- Each chapter file: `Chapter_NN_Title_In_Snake_Case.md`.
- Each chapter starts with a short "What you will master" block.
- Each chapter ends with "Exercises" and a "Godhood check" section.
- Volumes have a `README.md` listing chapters with one-line summaries.

## Code
- Python for runnable examples; pseudocode for provider-neutral concepts.
- Examples must be minimal but real; no invented APIs.
- When an API is provider-specific, say which provider and roughly when the API shape was current.

## Accuracy
- The field moves fast; date-stamp claims that will rot (model names, benchmark scores, framework versions).
- Distinguish stable principles (the agent loop, context economics) from ephemera (leaderboard positions).
- Never fabricate benchmark numbers; give orders of magnitude and cite the benchmark name instead.
