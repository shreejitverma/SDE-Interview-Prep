---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources:
  - "Scott Chacon and Ben Straub, Pro Git, 2nd Edition, Apress, 2014, https://git-scm.com/book/en/v2"
  - "Git Community, Git Reference Manual, Git 2.56.0, 2026, https://git-scm.com/docs"
  - "Martin von Zweigbergk and Jujutsu Contributors, Jujutsu Documentation, v0.46.0, 2026, https://jj-vcs.github.io/jj/latest/"
  - "Meta Open Source, Sapling SCM Documentation, v0.2.20260929, 2026, https://sapling-scm.com/docs/introduction/"
---

# Git and Version Control Skill Ladder

This skill ladder establishes observable, testable engineering behaviors for version control systems from junior individual contributor (L4) through distinguished engineer and systems architect (L8+).

## L4

- Can stage partial file hunks, create well-structured atomic commits, and inspect revision history using graph logs ([[Git-Object-Model-and-Content-Addressing]], [[Git-Revision-Selection-and-Graph-Walks]]).
- Can configure a personal production-grade Git environment with sensible diff, merge, and rebase defaults ([[Git-Expert-Config]]).
- Can resolve two-way and three-way textual merge conflicts cleanly using standard conflict markers and abort conflicting merges safely ([[Git-Three-Way-Merge-ORT-and-Diff-Algorithms]]).
- Can navigate remote tracking branches, create local feature branches, and push branches without destructive force flags ([[Git-Refs-Reflog-and-Reftable]]).
- Can locate historical edits and identify when a specific line changed using git blame with whitespace and move flags ([[Git-Archaeology-Log-Blame-and-Bisect]]).

## L5

- Can manage stacked branch series using interactive rebase, autosquash, and modern branch reference updating ([[Git-Rebase-Internals-and-Stacked-Branches]]).
- Can rescue accidentally detached, reset, or lost commits and dropped stashes within 10 minutes using reflogs and fsck ([[Recover-Lost-Git-Work]], [[Drill-Git-Detached-Rebase-and-Dangling-Tree-Recovery]]).
- Can automate binary regression searches across complex history using git bisect run with automated test scripts handling untestable revisions ([[Git-Archaeology-Log-Blame-and-Bisect]], [[Drill-Git-Automated-Bisect-on-Non-Linear-History]]).
- Can configure and maintain Git LFS attributes and clean and smudge filters for large binary assets ([[Git-Submodules-Subtrees-LFS-and-Attributes]]).
- Can configure cryptographic signing of commits and tags using SSH keys or Sigstore gitsign and verify commit provenance ([[Git-Hooks-and-Cryptographic-Signing]]).
- Can work effectively across blobless and treeless partial clones with sparse checkouts in large repositories ([[Git-Transfer-Protocol-and-Partial-Clone]]).

## L6

- Can diagnose and repair low-level object store corruption, broken commit graphs, and truncated multi-pack indexes without data loss ([[Repair-a-Corrupted-Git-Repository]], [[Drill-Git-Corrupted-Packfile-and-MIDX-Rescue]]).
- Can execute a zero-downtime repository secrets purge and history rewrite across all branches and tags using git-filter-repo ([[Remove-Secrets-and-Rewrite-Git-History]]).
- Can optimize repository maintenance using geometric repacking, incremental MIDX chains, and pseudo-merge reachability bitmaps ([[Git-GC-Maintenance-MIDX-and-Commit-Graph]]).
- Can diagnose slow status and checkout operations in large working trees using fsmonitor, sparse-index, and Trace2 telemetry streams ([[Git-Index-Stat-Cache-and-Fsmonitor]], [[Git-Trace2-Telemetry-and-Diagnostics]]).
- Can operate and evaluate next-generation change-based version control systems such as Jujutsu and Sapling alongside existing Git repos ([[Beyond-Git-Jujutsu-Sapling-and-Patch-Theory]]).
- Can implement a working prototype of a Git-compatible object store and packfile parser in Python or Rust ([[Rebuild-Mini-Git-Lab]]).

## L7

- Can design and deploy high-throughput server-side merge queues and speculative build trains using worktree-free git replay (EXPERIMENTAL as of Git 2.56) and merge-tree primitives ([[Git-Replay-and-Server-Side-History-Rewriting]], [[Git-Branching-Models-Merge-Queues-and-Code-Review]]).
- Can evaluate trade-offs and author an organizational decision record choosing between merge commits, rebase fast-forward, and squash-merge strategies ([[Decision-Record-Merge-vs-Rebase-and-Branching]]).
- Can migrate high-churn enterprise repositories to modern block-indexed reftable backends and configure server-side reference transaction logging ([[Git-Refs-Reflog-and-Reftable]]).
- Can tune wire protocol v2 capabilities and CDN bundle-URI offloading to protect internal Git server infrastructure from CI clone storms ([[Git-Transfer-Protocol-and-Partial-Clone]]).
- Can design repository review architectures, CODEOWNERS sharding, and automated branch governance for multi-thousand engineer organizations ([[Git-Branching-Models-Merge-Queues-and-Code-Review]]).

## L8+

- Can architect multi-billion-line enterprise monorepo infrastructure leveraging virtualized file systems like Scalar, VFS for Git, and EdenFS ([[Git-At-Scale-VFS-Scalar-and-Monorepos]]).
- Can author the definitive organizational decision record for monorepo versus polyrepo architecture, including financial modeling and multi-year migration paths ([[Decision-Record-Monorepo-vs-Polyrepo]]).
- Can formulate cryptographic migration strategies for repository fleets transitioning from SHA-1 to SHA-256 and post-quantum hash functions while preventing collision attacks ([[Case-Study-SHAttered-SHA1-Collision-and-Git-Hardening]]).
- Can evaluate patch algebra and distributed consensus models across Jujutsu, Darcs, and Pijul to steer company-wide source control tooling strategy a decade ahead ([[Beyond-Git-Jujutsu-Sapling-and-Patch-Theory]]).
