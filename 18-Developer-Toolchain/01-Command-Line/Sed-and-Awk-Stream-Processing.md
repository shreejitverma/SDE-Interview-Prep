---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Sed and Awk Stream Processing

> [!summary] Scope
> Owns stream editor and pattern-scanning language execution models.
> Details sed cycle buffers (pattern space vs hold space), multiline command execution (N, D, P), and address branching; details gawk record (RS) and field (FS, OFS) parsing, associative array hash indexing, and coprocess two-way I/O.
> Reaches Use through Explain, handing structured record pipelines to Structured-Data-Wrangling-Jq-Yq-and-Coreutils.md.

## Related

- Prerequisites: [[Shell-Parsing-Tokenization-and-Expansion-Order]]
- Next: [[Structured-Data-Wrangling-Jq-Yq-and-Coreutils]]
- See also: [[15-Technical-Whitepapers/08-Developer-Tooling-and-Foundations/Developer-Tooling-and-SysAdmin|Developer Tooling and SysAdmin]], [[19-Shell-and-Unix-Engineering/07-Regex-and-Text-Processing/sed-Pattern-and-Hold-Space|sed Pattern and Hold Space]], [[19-Shell-and-Unix-Engineering/07-Regex-and-Text-Processing/sed-Addresses-and-Branching|sed Addresses and Branching]], [[19-Shell-and-Unix-Engineering/07-Regex-and-Text-Processing/awk-Language|awk Language]], [[19-Shell-and-Unix-Engineering/07-Regex-and-Text-Processing/awk-Associative-Arrays-and-Idioms|awk Associative Arrays and Idioms]]
- Lab: `code/01-Command-Line/sed_awk_pipeline/`
