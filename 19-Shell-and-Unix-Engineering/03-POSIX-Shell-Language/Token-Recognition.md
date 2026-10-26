---
type: concept
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources:
  - "https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/V1_chap03.html"
  - "https://pubs.opengroup.org/onlinepubs/9799919799/utilities/V3_chap02.html"
  - "https://pubs.opengroup.org/onlinepubs/9799919799/xrat/V4_xcu_chap01.html"
  - "https://austingroupbugs.net/view.php?id=249"
  - "https://austingroupbugs.net/view.php?id=1036"
  - "https://austingroupbugs.net/view.php?id=1055"
  - "https://austingroupbugs.net/view.php?id=1083"
  - "https://austingroupbugs.net/view.php?id=1085"
  - "https://git.savannah.gnu.org/cgit/bash.git/tree/parse.y?h=bash-5.3"
  - "https://git.kernel.org/pub/scm/utils/dash/dash.git/tree/src/parser.c"
  - "https://github.com/zsh-users/zsh/blob/master/Src/lex.c"
  - "https://git.busybox.net/busybox/tree/shell/ash.c"
  - "https://github.com/ksh93/ksh/blob/dev/src/cmd/ksh93/sh/lex.c"
  - "https://github.com/magicant/yash/blob/master/parser.c"
  - "https://salsa.debian.org/clint/posh"
  - "https://nvd.nist.gov/vuln/detail/CVE-2014-7187"
  - "https://ftp.gnu.org/gnu/bash/bash-4.3-patches/bash43-028"
  - "https://git.savannah.gnu.org/cgit/bash.git/tree/input.c?h=bash-5.3"
  - "https://git.kernel.org/pub/scm/utils/dash/dash.git/tree/src/input.c?h=v0.5.12"
  - "https://git.kernel.org/pub/scm/utils/dash/dash.git/tree/src/mksyntax.c?h=v0.5.12"
  - "https://github.com/zsh-users/zsh/blob/master/Src/input.c"
  - "https://github.com/ksh93/ksh/blob/dev/src/cmd/ksh93/include/io.h"
  - "https://github.com/ksh93/ksh/blob/dev/src/cmd/ksh93/sh/io.c"
  - "https://github.com/ksh93/ksh/blob/dev/src/lib/libast/include/sfio.h"
---

# Token Recognition

> [!summary] Claim
> Token recognition is the character-by-character lexical scan that breaks the raw shell input stream into discrete words and operators before grammar parsing, alias substitution, or any parameter, command, or arithmetic expansion takes place.

Part of [The POSIX Shell Language](README.md) in [Shell and Unix Engineering](../README.md).

## TL;DR

- Mechanism: The shell ingests input bytes through `read(2)` into an internal line buffer and scans each character in turn through a ten-rule decision ladder (`(POSIX.1-2024 XCU 2.3)`) to delimit tokens as either words or operators without performing any parameter, command, or arithmetic substitutions.
- Rule: Quoting characters (`\`, `'`, `"`, `$'...'`) and expansion prefixes (`$`, `${`, `$(`, `` ` ``, `$((`) maintain word integrity across internal delimiters, so the resulting token preserves enclosing and embedded quotes literally until execution-time quote removal (`(POSIX.1-2024 XCU 2.3 Rule 4 and Rule 5)`).
- Trap: A backslash before newline inside a comment (`# comment \` followed by code) does not continue the comment onto the next line; the newline immediately terminates the comment and the subsequent line executes unconditionally (`(POSIX.1-2024 XCU 2.3 Rule 9 and XRAT C.2.3)`, where the rationale still calls it rule (10); lab test 1, item 1).
- Portability fact: In POSIX shells each `complete_command` is tokenized, parsed, and executed before the next command is tokenized (`(POSIX.1-2024 XCU 2.3, Austin Group Defect 1055)`), whereas `zsh -c` and `posh -c` pre-parse the entire input string into an abstract syntax tree prior to execution, preventing earlier commands from executing if a syntax error exists later in the string.
- When not to use it: Never rely on lexical tokenization tricks or dynamic alias injection inside non-interactive scripts to modify syntax; use shell functions, standard control structures, or pass structured arguments directly to external tools.

## Mental model

```mermaid
flowchart TD
  In["Raw Byte Stream<br/>read(2) into line buffer"] --> R1{"End of Input?<br/>(Rule 1)"}
  R1 -- "Yes" --> EOI["Delimit Token / Return EOI"]
  R1 -- "No" --> R2{"In Operator?<br/>(Rules 2-3)"}
  R2 -- "Can extend operator" --> OpExt["Append to Operator Token"]
  R2 -- "Cannot extend operator" --> OpDelim["Delimit Operator Token"]
  R2 -- "Not in operator" --> R4{"Quoting Prefix?<br/>\\, ', \", $'<br/>(Rule 4)"}
  R4 -- "Yes" --> QScan["Scan to Matching Quote<br/>Preserve raw bytes verbatim"]
  R4 -- "No" --> R5{"Expansion Prefix?<br/>$, ${, $(, `, $((<br/>(Rule 5)"}
  R5 -- "Yes" --> EScan["Recursive Construct Scan<br/>Track nested quotes & here-docs"]
  R5 -- "No" --> R6{"New Operator Char?<br/>&, |, ;, <, >, (, ), newline<br/>(Rule 6)"}
  R6 -- "Yes" --> StartOp["Delimit Current Token<br/>Start New Operator Token"]
  R6 -- "No" --> R7{"Unquoted Blank?<br/>space, tab<br/>(Rule 7)"}
  R7 -- "Yes" --> DelimBlank["Delimit Current Token<br/>Discard Blank"]
  R7 -- "No" --> R8{"In Word?<br/>(Rule 8)"}
  R8 -- "Yes" --> AppendW["Append Char to Word Token"]
  R8 -- "No" --> R9{"Hash '#'?<br/>(Rule 9)"}
  R9 -- "Yes" --> Comment["Discard to Next Newline<br/>Newline is NOT discarded"]
  R9 -- "No" --> StartW["Start New Word Token<br/>(Rule 10)"]
  OpDelim --> Classify["Lexical Classification<br/>(XCU 2.10.1)"]
  DelimBlank --> Classify
  StartOp --> Classify
  Classify --> Tokens["Grammar Tokens<br/>TOKEN, WORD, NAME, IO_NUMBER, Operators"]
```

The flowchart traces the ten sequential rules of POSIX token recognition (`(POSIX.1-2024 XCU 2.3)`).
Raw input arrives through `read(2)` into an unbounded line buffer.
The lexer applies the first matching rule to each character in turn.
If the character continues an active operator, Rule 2 greedily appends it; if not, Rule 3 delimits the operator.
Quoting prefixes branch to Rule 4, consuming text until the matching quote while preserving all characters and quotes unmodified without performing substitutions.
Unquoted expansion prefixes branch to Rule 5, reading sufficient characters recursively to determine the construct boundary while keeping embedded syntax verbatim.
New operator characters trigger Rule 6, delimiting any pending token and starting an operator.
Unquoted blanks trigger Rule 7, delimiting the active token and discarding the whitespace.
If an existing word is open, Rule 8 appends the character.
At token start, an unquoted hash triggers Rule 9, discarding all following characters up to the next newline.
Finally, Rule 10 opens a new word token.
Delimited tokens pass to the grammar classifier (`(POSIX.1-2024 XCU 2.10.1)`), which maps digit sequences before redirections to `IO_NUMBER` and other units to `TOKEN` or operators before grammar parsing.

## How it works

Token recognition is the earliest lexical phase of the shell command execution pipeline.
Before the grammar can assemble pipelines, loops, or simple commands, the shell must partition raw character input into atomic tokens.

### From system call to input buffer

The shell reads input from a terminal, a script file, or a `-c` string argument.
For script files and standard input, the shell issues `read(2)` system calls into a heap-allocated buffer.
In `dash` (`src/input.c`), `preadfd()` reads up to `IBUFSIZ - 1` bytes at a time, where `IBUFSIZ` is defined as `BUFSIZ + 1`, and hands characters to the lexer through `pgetc()` ([dash 0.5.12 input.c](https://git.kernel.org/pub/scm/utils/dash/dash.git/tree/src/input.c?h=v0.5.12)).
`BUFSIZ` is 8192 under glibc and 1024 under musl and macOS (measured in the shell matrix, as of 2026-10), so the chunk size depends on the C library.
In GNU `bash` (`parse.y`), `shell_getc()` reads input lines through bash's own buffered-input layer in `input.c`, whose `MAX_INPUT_BUFFER_SIZE` is 8192 bytes (8172 when bash is built with its own malloc), capped at the file size for seekable files ([bash 5.3 input.c](https://git.savannah.gnu.org/cgit/bash.git/tree/input.c?h=bash-5.3)).
In `zsh` (`Src/input.c`), `ingetc()` coordinates line reading with interactive zle line editing or file buffers; non-interactive script input is read by `shingetline()` into a `SHINBUFSIZE` (8192-byte) buffer ([zsh input.c](https://github.com/zsh-users/zsh/blob/master/Src/input.c)).
In `ksh93u+m`, `sh_iostream()` (`src/cmd/ksh93/sh/io.c`) buffers script input in an AT&T `sfio` stream of `IOBSIZE` bytes, defined as `SFIO_BUFSIZE` (8192) times `sizeof(char*)`, so 65536 bytes on 64-bit builds ([ksh93u+m io.h](https://github.com/ksh93/ksh/blob/dev/src/cmd/ksh93/include/io.h)).
POSIX explicitly mandates that input lines can be of unlimited length (`(POSIX.1-2024 XCU 2.3)`).
The shell dynamically resizes its line buffer using `realloc(3)` whenever an incoming line exceeds initial allocation limits.

### The ten tokenizing rules

When not reading the body of an active here-document, the shell applies the first applicable rule below to each character in turn (`(POSIX.1-2024 XCU 2.3)`):

1. Rule 1 (End of input): If end of input is recognized, the current token (if any) is delimited.
   If there is no current token, the end-of-input indicator is returned as the token.
2. Rule 2 (Operator extension): If the previous character was used as part of an operator, and the current character is not quoted and can combine with previous characters to form an operator, it is appended to that operator token.
   This implements greedy longest-match operator parsing.
3. Rule 3 (Operator termination): If the previous character was used as part of an operator and the current character cannot combine with it to form an operator, the operator token containing the previous character is delimited.
4. Rule 4 (Quoting): If the current character is an unquoted `<backslash>`, single-quote, double-quote, or the start of an unquoted `$'...'` sequence, it affects quoting for subsequent characters up to the matching quote (`(POSIX.1-2024 XCU 2.2)`).
   Crucially, during token recognition no substitutions are performed, and the resulting token contains exactly the raw input characters unmodified, including enclosing or embedded quotes.
   The token is not delimited by the end of the quoted construct.
5. Rule 5 (Candidate expansions): If the current character is an unquoted `$` or `` ` ``, the shell identifies candidate parameter expansions (`$var`, `${...}`), command substitutions (`$(...)`, `` `...` ``), or arithmetic expansions (`$((...))`).
   The shell reads sufficient input to determine the end of the construct, parsing nested quotes, expansions, and here-documents recursively.
   All characters from the start of the substitution to its end are retained verbatim in the result token.
   The token is not delimited by the end of the substitution.
6. Rule 6 (New operator): If the current character is not quoted and can serve as the first character of a new operator, the current token (if any) is delimited.
   The current character begins the next operator token.
7. Rule 7 (Unquoted blank): If the current character is an unquoted blank (`<space>` or `<tab>`), any token containing the previous character is delimited and the current character is discarded.
8. Rule 8 (Word continuation): If the previous character was part of a word, the current character is appended to that word.
9. Rule 9 (Comments): If the current character is a `#`, it and all subsequent characters up to, but excluding, the next `<newline>` are discarded as a comment.
   The trailing `<newline>` is not part of the comment and remains to delimit the line.
10. Rule 10 (Word start): The current character begins a new word token.

### Lexical classification and IO_NUMBER

Once a token is delimited, `(POSIX.1-2024 XCU 2.10.1)` dictates its lexical classification based on immediate character context:

- Operator: If the token matches a control operator or redirection operator, the specific operator identifier is produced.
- IO_NUMBER: If the token consists solely of decimal digits and the immediate delimiter character is `<` or `>`, the token identifier `IO_NUMBER` results.
  For example, in `2>file`, `2` is classified as `IO_NUMBER`, redirecting file descriptor 2.
  However, in `"2">file` or `\2>file`, Rule 4 marks the character as quoted, preventing `IO_NUMBER` classification; the token remains a `WORD` operand, and standard output (file descriptor 1) is redirected instead.
- IO_LOCATION: POSIX.1-2024 permits, but does not require, a `{location}redir-op word` form such as `{varname}>file`.
  If the string contains at least three characters, begins with `{`, ends with `}`, and its delimiter is `<` or `>`, the token identifier `IO_LOCATION` may result; otherwise it is `TOKEN` (`(POSIX.1-2024 XCU 2.10.1)`).
  If a shell supports the form, its behavior is implementation-defined (`(POSIX.1-2024 XCU 2.7)`); `bash` 4.4+, `ksh93`, and native `zsh` use it to open an unused descriptor of 10 or above and store its number in the variable (measured in the shell matrix, as of 2026-10).
- TOKEN: Any other character sequence yields type `TOKEN`, which is subsequently refined into `WORD`, `NAME`, `ASSIGNMENT_WORD`, or a reserved keyword depending on grammar context.

### Control and redirection operators

POSIX defines two families of operators (`(POSIX.1-2024 XBD 3.85, 3.304)`):

- Control operators: `&`, `&&`, `(`, `)`, `;`, `;;`, `;&`, `<newline>`, `|`, `||`, and the internal end-of-input indicator.
  POSIX.1-2024 formally standardizes `;&` for `case` statement fall-through execution.
- Redirection operators: `<`, `>`, `>|`, `<<`, `>>`, `<&`, `>&`, `<<-`, `<>`.

Because Rule 2 applies greedy longest match, `>>` is always recognized as a single append operator rather than two separate redirection tokens.
Extensions such as `|&`, `<<<` (here-string), and `((` (arithmetic command) are recognized as operators only in shells that implement those extensions.
The same `|&` means different things: in `bash` 4.0+ and `zsh` it is shorthand for `2>&1 |`, while in `ksh93` and `mksh` it starts a co-process, the KornShell meaning the rationale cites (`(POSIX.1-2024 XRAT C.2.3)`).
`((` is an arithmetic command in `bash`, `ksh93`, `mksh`, and `zsh`, and `<<<` is accepted by those four and by `yash` (measured in the shell matrix, as of 2026-10).
`dash`, `posh`, and the matrix builds of BusyBox `ash` 1.37 reject both `|&` and `<<<` with a syntax error, `yash` rejects `|&`, and `bash` 3.2 rejects `|&` (measured in the shell matrix, as of 2026-10).
In shells without the arithmetic command (`dash`, `busybox ash`, `yash`, `posh`), `((0))` is tokenized as two `(` operators, the word `0`, and two `)` operators, so it runs nested subshells that try to execute the non-existent utility `0` and exit with status 127 (lab test 3).

### Line continuation before tokenization

Line continuation via `<backslash><newline>` is processed at the physical character stream level before token recognition begins (`(POSIX.1-2024 XCU 2.2.1)`).
Whenever an unquoted backslash immediately precedes a newline, both bytes are discarded from the stream.
In POSIX-compliant shells (`dash`, `bash`), writing `false |\` followed by `| true` strips the backslash and newline, allowing Rule 2 to assemble the two pipe symbols into the `||` control operator.
However, `zsh` (native and `--emulate sh`) and `ksh93` do not join the two `|` characters across the escaped newline and fail with a syntax error on the second line (lab test 2).

### Alias substitution during token recognition

Alias substitution is an integral phase of token recognition (`(POSIX.1-2024 XCU 2.3.1)`).
When a token is delimited and classified as type `TOKEN`, it is immediately checked for alias expansion if all of the following conditions are met:
1. The token does not contain any quoting characters.
2. The token is a valid alias name (`(POSIX.1-2024 XBD 3.10)`).
3. An alias with that name is currently active.
4. The token did not result from an earlier alias substitution of the same name (recursion loop prevention).
5. The token occupies the command position of a simple command, or follows an alias whose replacement value ended with an unquoted blank.

If an alias value ends with a trailing space or tab (for example, `alias sudo="sudo "`), condition 5 together with the trailing-blank paragraph of XCU 2.3.1 directs the tokenizer to check the subsequent token for alias substitution as well (lab test 1, item 6).
Quoting any character in the command name (such as `\ls` or `"ls"`) violates condition 1, cleanly bypassing alias expansion without invoking `command`.

### Pipelined execution of complete commands

POSIX specifies that when a shell parses its input as a program, each `complete_command` recognized by the grammar must be executed before the subsequent `complete_command` is tokenized and parsed (`(POSIX.1-2024 XCU 2.3, Austin Group Defect 1055)`).
This architectural requirement ensures three critical properties:
1. Scripts can test for the presence of shell features dynamically and exit cleanly before reaching non-standard syntax that would otherwise trigger a fatal syntax error.
2. Self-extracting archives (such as shell scripts concatenated with raw binary tarballs) can parse the decompression header and terminate execution before the tokenizer encounters binary payloads.
3. Long scripts stream incrementally with minimal resident memory overhead because the shell does not construct a monolithic abstract syntax tree for the entire file.

While `dash`, `bash`, `ksh93`, `mksh`, `yash`, and `BusyBox ash` strictly follow this streaming model across all invocation forms, `zsh` and `posh` deviate under `-c` invocations by parsing the entire input string into an abstract syntax tree prior to executing the first statement (lab test 7); both still run line 1 of a script file before reporting a syntax error on a later line, and `posh` also streams standard input (measured in the shell matrix, as of 2026-10).

### Implementation anatomy

Real shell implementations divide token recognition across distinct lexical routines:

- GNU `bash` (`parse.y`): `read_token()` acts as the primary parser interface, calling `read_token_word()` to accumulate characters into a dynamically growing string buffer `token`.
  `parse_matched_pair()` handles recursive nesting for `${...}`, `$((...))`, and quoted strings, while in bash 5.3 `$(...)` goes to `parse_comsub()`, which calls `yyparse()` recursively on the substitution body.
- `dash` (`src/parser.c`): A handwritten recursive-descent parser drives the lexer functions `readtoken()`, `xxreadtoken()`, and `readtoken1()`.
  Character classification uses the lookup tables `basesyntax`, `dqsyntax`, `sqsyntax`, and `arisyntax`, generated at build time by `src/mksyntax.c` and indexed by character value.
- `zsh` (`Src/lex.c`): `gettok()` scans the input stream, accumulating words in `gettokstr()` and dispatching double-quoted text to `dquote_parse()`.
  Alias expansion occurs in-place via `checkalias()` and `exalias()`.
- `BusyBox ash` (`shell/ash.c`): Inherits the Almquist shell architecture from NetBSD/dash, utilizing `readtoken()` and `readtoken1()` with compact static syntax tables.
- `ksh93` (`src/cmd/ksh93/sh/lex.c`): Implements `sh_lex()` as a state machine driven by the per-mode character-class tables `sh_lexstates[]`.

## Numbers and limits

The following limits govern token recognition across POSIX specifications and real implementations (as of 2026-10):

| Metric / Parameter | Value | Standard / Specification | Implementation Behavior |
| :--- | :--- | :--- | :--- |
| Maximum input line length | Unlimited | `(POSIX.1-2024 XCU 2.3)` | Shells dynamically reallocate line buffers via `realloc(3)` |
| Maximum single token size | Unlimited | `(POSIX.1-2024 XCU 2.3)` | Lab verified 100,000 bytes in a single word token |
| Standard control operators count | 10 symbols | `(POSIX.1-2024 XBD 3.85)` | `&`, `&&`, `(`, `)`, `;`, `;;`, `;&`, `<newline>`, `\|`, `\|\|` |
| Standard redirection operators count | 9 symbols | `(POSIX.1-2024 XBD 3.304)` | `<`, `>`, `>\|`, `<<`, `>>`, `<&`, `>&`, `<<-`, `<>` |
| Default script read buffer | 1024 to 65536 bytes | Implementation and libc | `dash` reads `BUFSIZ` (8192 under glibc, 1024 under musl and macOS); `bash` 8192 (`MAX_INPUT_BUFFER_SIZE`); `zsh` 8192 (`SHINBUFSIZE`); `ksh93u+m` 65536 on 64-bit (`IOBSIZE`) |
| Longest standard operator | 3 bytes (`<<-`) | `(POSIX.1-2024 XBD 3.85, 3.304)` | Rule 2 decides one character at a time whether the next character extends the current operator, so no extra lookahead is needed |
| Alias trailing space lookahead | 1 token | `(POSIX.1-2024 XCU 2.3.1)` | Inspects immediate successor token when replacement ends in blank |
| Bash `word_lineno` array size | 128 entries (`MAX_CASE_NEST`) | CVE-2014-7187 | Off-by-one write past the array in bash through 4.3 patch 26; [bash43-028](https://ftp.gnu.org/gnu/bash/bash-4.3-patches/bash43-028) enlarged it to `MAX_CASE_NEST+1` |
| Semicolon fall-through operator | 2 bytes (`;&`) | `(POSIX.1-2024 XCU 2.9.4.3)`, `SEMI_AND` in XCU 2.10.2 | Supported in `bash` 4.0+ (measured 4.4 to 5.3), `ksh93`, `mksh`, `yash`, `zsh`; rejected in `bash 3.2`, `dash`, `posh`, `BusyBox ash` |
| Allocated descriptor for `IO_LOCATION` | Not specified | `(POSIX.1-2024 XCU 2.7)`: behavior implementation-defined | Measured: `bash 4.4+` and `ksh93` yield 10, native `zsh` yields 11 on Linux and macOS |
| Posh alias support | 0 builtins | `alias` utility, `(POSIX.1-2024 XCU)` | `posh 0.14.1` omits `alias` and `unalias` entirely (`command -v alias` fails) |

## Portability

The following matrix records token recognition semantics across POSIX.1-2024 and major shell families (measured in the lab matrix as of 2026-10).
The multi-line operator, `((0))`, `$'...'`, and pipelined-execution columns are asserted by lab tests 2, 3, 4, and 7; the `IO_LOCATION` and `;&` columns were measured by hand in the same shell matrix.

| Shell / Environment | Multi-line Operator (`false \|\<nl>\| b`) | Arithmetic Operator `((0))` | ANSI-C `$'\\t'` Quoting | `IO_LOCATION` `{fd}>file` | Semicolon Fall-through `;&` | Pipelined `complete_command` Execution |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| POSIX.1-2024 `sh` | Joins into `\|\|` (`b` runs) | Unspecified (`(XRAT C.2.3)`) | Supported ([[POSIX-2024-Changes\|Defect 249]]) | Optional, implementation-defined (XCU 2.7, 2.10.1) | Supported (`;&`) | Mandatory ([[Shell-Grammar\|Defect 1055]]) |
| macOS `bash 3.2` | Joins into `\|\|` (`b` runs) | Evaluates arithmetic 0 (`exit=1`) | Supported | Not supported (`exec: {fd}: not found`) | Not supported (syntax error) | Executed sequentially |
| Current `bash 5.3` | Joins into `\|\|` (`b` runs) | Evaluates arithmetic 0 (`exit=1`) | Supported | Supported (`fd=10`) | Supported | Executed sequentially |
| `zsh 5.9` (native) | Syntax error near `\|` | Evaluates arithmetic 0 (`exit=1`) | Supported | Supported (`fd=11` on Linux and macOS) | Supported | Whole input pre-parsed under `-c` (streams script files) |
| `zsh 5.9` (`--emulate sh`) | Syntax error near `\|` | Evaluates arithmetic 0 (`exit=1`) | Supported | Not supported (`command not found: {fd}`) | Supported | Whole input pre-parsed under `-c` (streams script files) |
| `dash 0.5.12` | Joins into `\|\|` (`b` runs) | Nested subshells running `0` (`exit=127`) | Not supported (literal `$a\tb`) | Not supported (`exec: {fd}: not found`) | Not supported (syntax error) | Executed sequentially |
| `ksh93u+` / `ksh93u+m` | Syntax error near `\|` | Evaluates arithmetic 0 (`exit=1`) | Supported | Supported (`fd=10`) | Supported | Executed sequentially |
| `mksh R59` | Joins into `\|\|` (`b` runs) | Evaluates arithmetic 0 (`exit=1`) | Supported | Not supported (`{fd}: inaccessible or not found`) | Supported | Executed sequentially |
| `yash 2.58` | Joins into `\|\|` (`b` runs) | Nested subshells running `0` (`exit=127`) | Supported | Not supported | Supported | Executed sequentially |
| `posh 0.14` | Joins into `\|\|` (`b` runs) | Nested subshells running `0` (`exit=127`) | Not supported (literal `$a\tb`) | Not supported | Not supported (syntax error) | Whole input pre-parsed under `-c` (streams script files) |
| `BusyBox ash 1.37` | Joins into `\|\|` (`b` runs) | Nested subshells running `0` (`exit=127`) | Supported (in matrix builds) | Not supported | Not supported | Executed sequentially |

## Worked example

The automated test harness under [lab](../labs/03-token-recognition/) executes `token-recognition-probe.sh` to demonstrate core POSIX token recognition mechanisms:

```sh
#!/bin/sh
# token-recognition-probe.sh: probe POSIX.1-2024 XCU 2.3 Token Recognition semantics.
set -eu

if [ -n "${BASH_VERSION-}" ]; then
  # shellcheck disable=SC3044 # bash-specific option enabled only under bash
  shopt -s expand_aliases
fi

# 1. Comment non-continuation across backslash-newline (XCU 2.3 Rule 9 and XRAT C.2.3)
t1_raw=$(printf '# comment \\\necho run1\n')
t1_out=$(eval "${t1_raw}")
printf '1_comment_nl: <%s>\n' "${t1_out}"

# 2. Comment hash in word interior vs token boundary (XCU 2.3 Rule 8 vs Rule 9)
t2_mid=$(eval 'echo foo#bar')
t2_sep=$(eval 'echo foo #bar')
printf '2_comment_mid: word=<%s> separated=<%s>\n' "${t2_mid}" "${t2_sep}"

# 3. Quoted vs bare digit redirection (XCU 2.10.1 IO_NUMBER)
tmp_dir=$(mktemp -d)
# shellcheck disable=SC2317 # executed in cleanup trap
trap 'rm -rf "${tmp_dir}"' EXIT

# Bare digit before '>' yields IO_NUMBER (0 positional parameters to set)
set -- 1>"${tmp_dir}/out1"
c1=$#

# Quoted digit immediately before '>' yields WORD token '1' (1 positional parameter).
# The code is an eval string so that a formatter cannot insert a blank before '>',
# which would make the digit a separate word whether or not quoting suppresses IO_NUMBER.
# shellcheck disable=SC2016 # expanded by eval
eval 'set -- "1">"${tmp_dir}/out2"'
c2=$#
a2=$1

# Escaped digit immediately before '>' yields WORD token '1' (1 positional parameter)
# shellcheck disable=SC2016 # expanded by eval
eval 'set -- \1>"${tmp_dir}/out3"'
c3=$#
a3=$1

printf '3_quoted_redir: bare=%d quoted=%d escaped=%d (a2=%s, a3=%s)\n' \
  "${c1}" "${c2}" "${c3}" "${a2}" "${a3}"

# 4. Token boundary preservation across quotes and parameter expansion (XCU 2.3 Rule 4 & 5)
var='d'
# shellcheck disable=SC2086,SC2248 # testing token boundary assembly without splitting
set -- a'b'"c"${var}
printf '4_token_boundary: argc=%d <%s>\n' "$#" "$1"

# 5. Greedy multi-character operator assembly (XCU 2.3 Rule 2)
printf 'x' >>"${tmp_dir}/op_out"
printf 'y' >>"${tmp_dir}/op_out"
op_content=$(cat "${tmp_dir}/op_out")
printf '5_operator_greedy: <%s>\n' "${op_content}"

# 6. Alias substitution with trailing blank next-word chaining (XCU 2.3.1)
# 7. Quoted alias token bypass (XCU 2.3.1)
if command -v alias >/dev/null 2>&1; then
  alias pfx='echo '
  alias tgt='expanded'
  t6_out=$(eval 'pfx tgt')
  printf '6_alias_blank: <%s>\n' "${t6_out}"

  alias myprobe_cmd='echo aliased'
  t7_ctrl=$(eval 'myprobe_cmd plain')
  if [ "${t7_ctrl}" = "aliased plain" ]; then
    if (eval '\myprobe_cmd plain') >/dev/null 2>&1; then
      t7_bypass='no'
    else
      t7_bypass='yes'
    fi
  else
    t7_bypass='unsupported'
  fi
  printf '7_alias_bypass: bypassed=%s\n' "${t7_bypass}"
else
  printf '6_alias_blank: <alias_not_supported>\n'
  printf '7_alias_bypass: bypassed=alias_not_supported\n'
fi

# 8. Here-document queuing across single line (XCU 2.3)
t8_raw=$(
  cat <<EOF1 && cat <<EOF2
lineA
EOF1
lineB
EOF2
)
t8_flat=$(printf '%s\n' "${t8_raw}" | tr '\n' ' ' | sed 's/ $//')
printf '8_here_doc_interleave: <%s>\n' "${t8_flat}"
```

When executed under standard POSIX-compliant shells with alias support (`dash`, `bash`, `ksh93`, `mksh`, `yash`, `busybox`, `zsh`), the script outputs:

```
1_comment_nl: <run1>
2_comment_mid: word=<foo#bar> separated=<foo>
3_quoted_redir: bare=0 quoted=1 escaped=1 (a2=1, a3=1)
4_token_boundary: argc=1 <abcd>
5_operator_greedy: <xy>
6_alias_blank: <expanded>
7_alias_bypass: bypassed=yes
8_here_doc_interleave: <lineA lineB>
```

Under `posh 0.14` (which omits the `alias` builtin entirely), the probe detects missing alias support and outputs:

```
1_comment_nl: <run1>
2_comment_mid: word=<foo#bar> separated=<foo>
3_quoted_redir: bare=0 quoted=1 escaped=1 (a2=1, a3=1)
4_token_boundary: argc=1 <abcd>
5_operator_greedy: <xy>
6_alias_blank: <alias_not_supported>
7_alias_bypass: bypassed=alias_not_supported
8_here_doc_interleave: <lineA lineB>
```

## Pitfalls

> [!warning] Comment continuation illusion across backslash-newline
> Trigger: Writing a backslash at the end of a comment line (`# comment \`) expecting subsequent lines to remain commented out as in C macro definitions or Python code.
> Symptom: The shell treats the newline as the immediate end of the comment according to Rule 9; the subsequent physical line executes as live shell code, often causing catastrophic unintended operations.
> Fix: Place an explicit unquoted `#` character at the beginning of every commented physical line (`(POSIX.1-2024 XRAT C.2.3)`).

> [!warning] Quoted file descriptor redirection suppression
> Trigger: Wrapping descriptor digits in quotes or escaping them prior to a redirection operator (`"2">err.log` or `\2>err.log`).
> Symptom: Quoting prevents the lexer from classifying the digit string as `IO_NUMBER` under XCU 2.10.1; the shell passes `"2"` as a command argument and redirects standard output (descriptor 1) to `err.log` instead of standard error.
> Fix: Keep file descriptor digits completely bare and unquoted immediately adjacent to `<` or `>`; XCU 2.7 gives `echo \2>a` as the example that writes the character 2 into `a` (lab test 1, item 3).

> [!warning] Same-line alias evaluation failure
> Trigger: Defining an alias and invoking it within the same line, compound command, or non-interactive `-c` string (`alias foo="echo bar"; foo`).
> Symptom: The command fails with `foo: command not found` because token recognition for the entire line completes before the `alias` command executes.
> Fix: Separate alias definition and invocation across distinct `complete_command` boundaries using newlines, or use standard POSIX functions instead (`(POSIX.1-2024 XCU 2.3.1)`).

> [!warning] Unspaced nested subshell arithmetic collision
> Trigger: Writing a single-command nested subshell without spaces between opening parentheses: `((0))` or `((cmd))`.
> Symptom: In shells with Korn arithmetic support (`bash`, `ksh93`, `mksh`, `zsh`), the tokenizer treats `((` as an arithmetic command operator, evaluating `((0))` as arithmetic false (`exit=1`) or failing on non-arithmetic syntax like `((echo a))`, whereas in shells without arithmetic commands (`dash`, `busybox ash`, `yash`, `posh`) it parses as nested subshells executing command `0` or `cmd` (`exit=127`).
> Fix: Insert mandatory whitespace between adjacent subshell parentheses: `( (0) )` or `( (cmd) )`, as the normative text requires of a conforming application (`(POSIX.1-2024 XCU 2.9.4.1, XRAT C.2.3)`).

> [!warning] Multi-line operator line break failure in zsh
> Trigger: Splitting a multi-character operator across lines with an escaped newline (`false |\` newline `| true`).
> Symptom: While POSIX shells discard `<backslash><newline>` prior to token recognition, `zsh` and `ksh93` tokenize the first `|` immediately, aborting with a parse error on the second line.
> Fix: Always complete the operator on the first line, or break lines after the complete operator without a backslash.

> [!warning] Premature command substitution termination on comment parenthesis in posh
> Trigger: Including a closing parenthesis within a comment inside a `$(...)` command substitution (`out=$(cmd # note (closing paren))`).
> Symptom: In `posh 0.14`, the lexical scanner for `$(...)` treats any closing parenthesis inside a comment as the terminator of the command substitution instead of parsing the subshell program recursively, aborting with ``posh: syntax error: `)' unexpected`` (exit status 1, lab test 8).
> Fix: Avoid unquoted closing parentheses in comments inside `$(...)`, or write comments on separate lines outside the substitution construct.

> [!warning] Full-string AST pre-parsing under -c strings in zsh and posh
> Trigger: Relying on sequential `complete_command` execution in `sh -c`, `zsh -c`, or `posh -c` strings to gate extended syntax or binary payloads behind a runtime check on line 1.
> Symptom: While POSIX shells (`dash`, `bash`, `ksh93`, `mksh`, `yash`, `BusyBox ash`) stream and execute each complete command sequentially, `zsh` and `posh` pre-parse the entire `-c` argument into an abstract syntax tree prior to execution; any subsequent syntax error aborts the script before line 1 runs.
> Fix: Stream code from standard input (`sh <<'EOF'`) or a script file, where `posh` and standard shells execute incrementally, or invoke a separate shell child process for feature-gated syntax.

> [!warning] Omission of alias builtins in minimalist shells
> Trigger: Invoking `alias` or `unalias` inside scripts executed by minimalist shells such as `posh 0.14`.
> Symptom: The command fails with `alias: not found` (exit status 127), terminating execution under `set -e`.
> Fix: Do not use aliases in shell scripts; define standard POSIX shell functions (`func() { ...; }`) instead.

## Security and robustness

Token recognition provides the fundamental syntactic barrier that protects Unix shells from command injection.
Because the shell tokenizes input strictly before performing parameter expansion, command substitution, or arithmetic expansion (`(POSIX.1-2024 XCU 2.3, 2.6)`), unquoted variables containing characters like `;`, `&`, `|`, or `>` can never produce new operators during standard execution.

For example, given:

```sh
param="; rm -rf /"
printf '%s\n' $param
```

The lexer scans the second line into three word tokens, the command word `printf`, the quoted word `'%s\n'`, and the expansion candidate `$param`, followed by the newline operator.
Later, when word expansion and field splitting occur, the contents of `$param` split into fields passed as individual `argv` entries to `printf`.
The semicolon remains data within an argument; it is never recognized as a control operator.

The most common way to break this architecture is `eval` (any construct that hands data back to the parser, such as `sh -c "$param"`, has the same effect):

```sh
eval "printf '%s\n' $param"
```

Calling `eval` re-submits the fully expanded string to token recognition and grammar parsing, allowing injected metacharacters to create new control operators and execute arbitrary commands.

Parser robustness also extends to nested construct handling.
Historical vulnerabilities such as CVE-2014-7187 in GNU `bash` arose from an off-by-one error in `read_token_word()` (`parse.y`) during deeply nested loop parsing, leading to an out-of-bounds write past the static `word_lineno` line-number array ([NVD](https://nvd.nist.gov/vuln/detail/CVE-2014-7187); fixed in [bash43-028](https://ftp.gnu.org/gnu/bash/bash-4.3-patches/bash43-028)).
Hardened production scripts maintain shallow syntactic nesting and reject untrusted data that could be passed to dynamic evaluators.

## The principal's view

At distinguished engineering scale, shell token recognition represents a non-negotiable architectural boundary: syntax is immutable once parsed, whereas data expands dynamically at execution time.

Engineers encounter recurring production outages when they conflate these two phases:
1. Aliases must never be used in production scripts, automation, or library code.
   Aliases rely on lexical replacement during token recognition and introduce erratic ordering bugs across subshells, functions, and `-c` strings.
   Furthermore, minimalist shells like `posh 0.14` omit the `alias` builtin entirely.
   POSIX functions (`name() { ...; }`) provide clean runtime dispatch with standard lexical scoping.
2. Avoid vendor-specific operator extensions (`|&`, `<<<`, `((...))`) in deployment hooks, container entrypoints, and infrastructure automation.
   Unspaced `((0))` evaluates as arithmetic false (`exit=1`) in `bash`, `ksh93`, `mksh`, and `zsh`, but parses as nested subshells attempting to execute utility `0` (`exit=127`) in `dash`, `busybox ash`, `yash`, and `posh`.
   Defaulting to POSIX standard operators guarantees that scripts execute identically across Alpine Linux (`BusyBox ash`), Debian/Ubuntu (`dash`), and macOS (`bash 3.2` or `zsh`).
3. Be vigilant of execution model differences under `-c`.
   While POSIX mandates pipelined execution of complete commands, `zsh -c` and `posh -c` pre-parse the entire input string into an abstract syntax tree prior to executing line 1.
   Any dynamic syntax gating intended to run across multiple shells must be loaded from separate files or executed in a subshell rather than packed into a single `-c` string.
4. Recognize the structural limits of the shell grammar.
   If a task requires dynamic command assembly, complex lexing, or structured nested objects, do not string-escape shell commands into `eval`.
   Migrate immediately to Python, Go, or Rust, where arguments pass as discrete string arrays directly to `execve(2)` without invoking a shell tokenizer.

## Interview questions

> [!question] L5: What does the command `echo foo#bar` print compared to `echo foo #bar`, and which POSIX tokenizing rules explain the difference?
>
> > [!success]- Answer
> > The command `echo foo#bar` outputs `foo#bar`, whereas `echo foo #bar` outputs `foo`.
> > In `echo foo#bar`, the `#` character occurs within an active word token.
> > POSIX.1-2024 XCU 2.3 Rule 8 dictates that characters following a word character are appended to that word.
> > In `echo foo #bar`, the space character triggers Rule 7, delimiting `foo` and discarding the blank.
> > The `#` character then appears at the beginning of a new token, triggering Rule 9.
> > Rule 9 discards the `#` and all subsequent characters up to the next newline as a comment.
> >
> > Follow-up: Does an escaped newline inside a comment (`# comment \` followed by code) continue the comment onto the next line?

> [!question] L5: Predict the output and redirection behavior of `set -- 1>out` versus `set -- "1">out`.
>
> > [!success]- Answer
> > `set -- 1>out` leaves `$#` equal to 0, whereas `set -- "1">out` leaves `$#` equal to 1 with `$1` set to `1`.
> > Under POSIX.1-2024 XCU 2.10.1, a string consisting solely of unquoted digits immediately preceding `<` or `>` is classified as `IO_NUMBER`.
> > In `set -- 1>out`, `1` becomes `IO_NUMBER`, redirecting standard output (file descriptor 1) to `out` and passing 0 operands to `set`.
> > In `set -- "1">out`, Rule 4 applies because the digit is enclosed in double quotes.
> > Quoted strings cannot be classified as `IO_NUMBER`, so `"1"` remains a regular `WORD` operand passed to `set`.
> > The redirection `>out` then uses the default descriptor (file descriptor 1).
> >
> > Follow-up: What happens if the digit is escaped with a backslash as in `set -- \1>out`?

> [!question] L6: Predict the exit status of executing the command `((0))` across bash, ksh93, dash, yash, and posh, and explain the grammatical difference.
>
> > [!success]- Answer
> > In `bash`, `ksh93`, `mksh`, and `zsh`, `((0))` yields exit status 1, whereas in `dash`, `busybox ash`, `yash`, and `posh`, it yields exit status 127.
> > Shells with Korn arithmetic support recognize `((` as a compound arithmetic command operator.
> > The enclosed expression `0` evaluates to integer zero (false), setting the exit status to 1.
> > Standard POSIX shells without this extension do not recognize `((` as an operator.
> > Under POSIX.1-2024 XCU 2.3 Rule 6, the lexer tokenizes `((0))` as two opening subshell operators `(` and `(` followed by the word `0` and two closing subshell operators `)` and `)`.
> > The subshell attempts to execute an external utility named `0`.
> > Because no executable named `0` exists in `PATH`, the command fails with command not found (exit status 127).
> >
> > Follow-up: How does POSIX.1-2024 XRAT C.2.3 advise writing portable nested subshells to avoid this collision?

> [!question] L6: Predict the output of the following script across POSIX shells:
> ```sh
> alias mycmd="echo "
> alias target="success"
> mycmd target
> alias mycmd2="echo"
> mycmd2 target
> ```
>
> > [!success]- Answer
> > The script outputs `success` on the first line and `target` on the second line.
> > Under POSIX.1-2024 XCU 2.3.1, alias substitution checks the first word of a simple command during token recognition.
> > In the first call, `mycmd` expands to `echo `.
> > Because the replacement value ends with an unquoted blank, the trailing-blank paragraph of XCU 2.3.1 requires checking the next token for alias substitution.
> > The next token `target` is recognized as an alias and expands to `success`.
> > In the second call, `mycmd2` expands to `echo` without a trailing blank.
> > The next token `target` is in operand position and is not checked for alias substitution, printing `target` literally.
> >
> > Follow-up: Why does alias expansion fail if both lines are combined into a single line without a newline?

> [!question] L6: Debug this script: A developer writes a multi-line deployment script containing:
> ```sh
> # Temporarily disable cleanup routine \
> rm -rf "${DEPLOY_TMPDIR}"
> ```
> Why does `rm -rf` execute despite the developer intending to comment it out?
>
> > [!success]- Answer
> > The command executes because POSIX shell comments cannot be continued across physical lines using backslashes.
> > Under POSIX.1-2024 XCU 2.3 Rule 9 and XRAT C.2.3, the search for the end of a comment discards all characters up to the next newline.
> > The specification explicitly states that escaped newlines are not considered specially during comment scanning.
> > The newline immediately terminates the comment regardless of the preceding backslash.
> > The shell then tokenizes the subsequent line as active shell commands.
> > To fix this, an explicit `#` must precede every commented physical line.
> >
> > Follow-up: How does this rule differ from macro definitions in C and string line continuations in Python?

> [!question] L6: Explain why an unquoted variable containing `; rm -rf /` does not execute as a separate command when expanded in `echo $var`.
>
> > [!success]- Answer
> > The command does not execute because token recognition occurs strictly prior to word expansion.
> > In POSIX.1-2024 XCU 2.3, the shell breaks the command line into words and operators during initial lexical analysis.
> > At token recognition time, `echo` is recognized as a command word and `$var` is recognized as an expansion candidate token.
> > Control operators like `;` can only be recognized during the token recognition phase.
> > Word expansions ([[Parameter-Expansion]], [[Command-Substitution]], [[Arithmetic-Expansion]]) run later in the execution phase.
> > The resulting expansion undergoes field splitting and quote removal, but is never re-submitted to token recognition.
> > The semicolon is delivered as an argument string to `echo`, preserving syntax integrity.
> >
> > Follow-up: Under what circumstances can expanded data produce new control operators?

> [!question] L7: Explain Austin Group Defect 1055 and describe why the POSIX requirement for incremental `complete_command` execution is essential for self-extracting shell archives.
>
> > [!success]- Answer
> > Austin Group Defect 1055 clarified POSIX.1-2024 XCU 2.3 to require that each `complete_command` must be executed before the subsequent `complete_command` is tokenized and parsed.
> > In historical shells, tokenization and parsing stream incrementally rather than reading the entire input file into memory.
> > Self-extracting archives exploit this by concatenating a shell decompression script with a raw binary payload.
> > The shell script extracts the payload (often using `tail` or `sed` referencing an offset) and terminates with `exit 0`.
> > Because execution occurs before tokenizing subsequent lines, the shell exits cleanly before its tokenizer encounters arbitrary binary bytes.
> > If a shell were to parse the entire file into an AST ahead of execution, binary payload bytes would trigger fatal syntax errors.
> > This rule also allows scripts to perform runtime environment checks before executing syntax extensions.
> >
> > Follow-up: How do `zsh -c` and `posh -c` deviate from this streaming execution requirement?

> [!question] L7: What was the root cause of CVE-2014-7187 in GNU Bash, and how did it exploit the parser's lexical state tracking?
>
> > [!success]- Answer
> > CVE-2014-7187 was an off-by-one error in `read_token_word()` in GNU Bash's `parse.y`, affecting bash through 4.3 patch 26 (NVD).
> > In `parse.y`, the lexical analyzer records the line number of each nested `for`, `select`, or `case` word in the static array `word_lineno`, indexed by `word_top`.
> > The array held `MAX_CASE_NEST` (128) entries, but the index check in `read_token_word()` let `word_top` reach 128, one past the last element.
> > An attacker supplying deeply nested loop constructs caused the parser to write past the bounds of `word_lineno` into adjacent static memory.
> > NVD describes the impact as a denial of service (out-of-bounds array access and crash) or possibly unspecified other impact.
> > The fix in `bash43-028`, which also fixed the here-document `redir_stack` overflow (CVE-2014-7186), enlarged the array to `MAX_CASE_NEST+1` entries rather than adding dynamic reallocation.
> >
> > Follow-up: Why is lexical state tracking in shell parsers inherently more complex than in context-free languages?

> [!question] L7: How does token recognition resolve multiple here-documents specified on a single physical line, such as `cat <<EOF1 && cat <<EOF2`?
>
> > [!success]- Answer
> > Under POSIX.1-2024 XCU 2.3, the shell operates in two primary modes: ordinary token recognition and here-document processing.
> > When the grammar recognizes an `io_here` redirection token, it continues ordinary token recognition until encountering the next `NEWLINE` token.
> > All non-newline tokens encountered on that line (including additional `io_here` tokens) are saved in an internal queue.
> > Upon recognizing the `NEWLINE`, the shell switches to here-document processing mode.
> > The lines immediately following the `NEWLINE` form the body of the first here-document up to its delimiter.
> > The body of the second here-document begins immediately on the line following the first delimiter.
> > Once all queued here-documents are consumed, ordinary token recognition resumes for subsequent commands.
> >
> > Follow-up: What is the behavior if a command substitution terminating parenthesis occurs on the same line as an `io_here` token?

## Related

Prerequisites: [[Reading-the-POSIX-Standard]]

Next: [[Shell-Grammar]]

See also:
- [[Aliases]]
- [[Quoting-Backslash-Single-and-Double]]
- [[Dollar-Single-Quotes]]
- [[Expansion-Order]]
- [[Parameter-Expansion]]
- [[Command-Substitution]]
- [[Arithmetic-Expansion]]
- [[Field-Splitting-and-IFS]]
- [[Here-Documents]]
- [[Redirection-Order-and-Semantics]]
- [[Simple-Commands-and-Assignment-Prefixes]]
- [[POSIX-2024-Changes]]

## Further reading

- [POSIX.1-2024 Shell Command Language (XCU Chapter 2)](https://pubs.opengroup.org/onlinepubs/9799919799/utilities/V3_chap02.html): The authoritative specification defining the ten tokenizing rules and grammar classifications.
- [POSIX.1-2024 Rationale (XRAT C.2.3 Token Recognition)](https://pubs.opengroup.org/onlinepubs/9799919799/xrat/V4_xcu_chap01.html): Historical design context explaining comment non-continuation, streaming execution, and alias spacing.
- [Austin Group Defect 1055](https://austingroupbugs.net/view.php?id=1055): Specification clarification establishing that complete commands must be executed before tokenizing subsequent input.
- [Austin Group Defect 249](https://austingroupbugs.net/view.php?id=249): The formal standard defect adding `$'...'` ANSI-C quoting to POSIX.1-2024 token recognition Rule 4.
- [GNU Bash Parser Implementation (`parse.y`)](https://git.savannah.gnu.org/cgit/bash.git/tree/parse.y?h=bash-5.3): Production source code showing `read_token()`, `read_token_word()`, and state-driven lexical analysis.
- [DASH Parser Implementation (`src/parser.c`)](https://git.kernel.org/pub/scm/utils/dash/dash.git/tree/src/parser.c): Fast, minimal recursive-descent shell lexer demonstrating table-driven syntax scanning.
- [ZSH Lexer Implementation (`Src/lex.c`)](https://github.com/zsh-users/zsh/blob/master/Src/lex.c): Comprehensive multi-mode lexer handling interactive editing, alias expansion, and history substitution.
- [Yash Parser Implementation (`parser.c`)](https://github.com/magicant/yash/blob/master/parser.c): Modern POSIX-compliant parser demonstrating strict standard compliance without non-standard compound commands.
- [Posh Source Repository](https://salsa.debian.org/clint/posh): Debian's Policy-compliant Ordinary Shell illustrating minimalist builtins and AST pre-parsing under `-c`.
- [NVD Record for CVE-2014-7187](https://nvd.nist.gov/vuln/detail/CVE-2014-7187): Vulnerability analysis of parser memory corruption within Bash's word accumulation routines.
