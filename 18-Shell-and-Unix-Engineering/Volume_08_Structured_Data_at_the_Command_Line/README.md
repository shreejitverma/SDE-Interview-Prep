---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 08 - Structured Data at the Command Line

JSON, YAML, CSV, and HTTP APIs are now the common case, and line-oriented text tools mishandle all of them.
This volume covers the structured tools that replace them, and the signs that a pipeline should become a program.

## Prerequisites

- [Volume 06 - Regular Expressions and Text Processing](../Volume_06_Regular_Expressions_and_Text_Processing/README.md)

## Chapters

1. [jq I: The Language](Chapter_01_Jq_The_Language.md) - Filters, paths, generators, and jq programs as functions from one JSON value to a stream of values.
2. [jq II: Advanced Programs and Streaming](Chapter_02_Jq_Advanced_Programs_and_Streaming.md) - reduce, foreach, path expressions, `--stream` for huge inputs, and alternative jq implementations.
3. [YAML, TOML, and XML at the Command Line](Chapter_03_YAML_TOML_and_XML_at_the_Command_Line.md) - The yq variants, dasel, and xmlstarlet, and the hazards of parsing YAML.
4. [CSV and Tabular Data](Chapter_04_CSV_and_Tabular_Data.md) - Miller, xsv and qsv, csvkit, and CSV quoting edge cases.
5. [Databases as Shell Citizens: sqlite3 and DuckDB](Chapter_05_Databases_as_Shell_Citizens_sqlite3_and_DuckDB.md) - Querying files with SQL from the shell, and when to switch tools.
6. [HTTP and APIs from the Shell](Chapter_06_HTTP_and_APIs_from_the_Shell.md) - curl in depth: options, retries, timeouts, authentication, and response handling.
7. [When to Stop Using Text Tools](Chapter_07_When_to_Stop_Using_Text_Tools.md) - The signs that a pipeline needs a structured language, and how to migrate it.

## Labs

Each chapter's worked example lives in [`labs/`](../labs/README.md) as a lab directory named `v08-cNN-<slug>`, with a bats test that runs under every shell in the matrix.
