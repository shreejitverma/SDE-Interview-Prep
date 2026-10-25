---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources:
  - "Scott Chacon and Ben Straub, Pro Git, 2nd Edition, Apress, 2014, https://git-scm.com/book/en/v2"
  - "Git Community, Git Reference Manual, Git 2.56.0, 2026, https://git-scm.com/docs"
  - "Git Community, Git Technical Documentation, Git 2.56.0, 2026, https://github.com/git/git/tree/master/Documentation/technical"
  - "Git Community, Git Breaking Changes, Git 2.56.0, 2026, https://github.com/git/git/blob/master/Documentation/BreakingChanges.adoc"
  - "Git Community, Git Release Notes (2.45.0 to 2.56.0), 2024-2026, https://github.com/git/git/tree/master/Documentation/RelNotes"
  - "Martin von Zweigbergk and Jujutsu Contributors, Jujutsu Documentation, v0.46.0, 2026, https://docs.jj-vcs.dev/latest/"
  - "Meta Open Source, Sapling SCM Documentation, v0.2.20260929, 2026, https://sapling-scm.com/docs/introduction/"
  - "Microsoft, Scalar Reference Manual, Git 2.56.0, 2026, https://git-scm.com/docs/scalar"
  - "Elijah Newren, git-filter-repo Manual, v2.47.0, 2024, https://htmlpreview.github.io/?https://github.com/newren/git-filter-repo/blob/docs/html/git-filter-repo.html"
  - "Git LFS Team, Git Large File Storage Specification (git-lfs v3.8.1), 2026, https://github.com/git-lfs/git-lfs/blob/main/docs/spec.md"
---

# Git and Version Control Topic Inventory

This inventory maps the authoritative documentation, internal technical specifications, and release roadmaps for version control systems to notes in 18-Developer-Toolchain.

| Source heading group | Owning note | Depth target | Existing vault coverage |
| :--- | :--- | :--- | :--- |
| Git Reference: Main Porcelain - Getting and Creating Projects (`init`, `clone`) | [[Git-Object-Model-and-Content-Addressing]], [[Git-Transfer-Protocol-and-Partial-Clone]] | Use, Explain, Rebuild | none |
| Git Reference: Main Porcelain - Basic Snapshotting (`add`, `status`, `diff`, `commit`, `restore`, `reset`, `rm`, `mv`) | [[Git-Index-Stat-Cache-and-Fsmonitor]], [[Git-Three-Way-Merge-ORT-and-Diff-Algorithms]] | Use, Explain, Debug | none |
| Git Reference: Main Porcelain - Branching and Merging (`branch`, `checkout`, `switch`, `merge`, `mergetool`, `log`, `stash`, `tag`, `worktree`) | [[Git-Refs-Reflog-and-Reftable]], [[Git-Transfer-Protocol-and-Partial-Clone]] | Use, Explain, Decide at Scale | [[13-Agentic-AI/Agentic-Harness/components/treehouse.md\|treehouse]] |
| Git Reference: Main Porcelain - Sharing and Updating Projects (`fetch`, `pull`, `push`, `remote`, `submodule`) | [[Git-Transfer-Protocol-and-Partial-Clone]], [[Git-Submodules-Subtrees-LFS-and-Attributes]] | Use, Explain, Decide at Scale | none |
| Git Reference: Main Porcelain - Inspection and Comparison (`show`, `log`, `diff`, `difftool`, `range-diff`, `shortlog`, `describe`) | [[Git-Archaeology-Log-Blame-and-Bisect]] | Use, Explain, Debug | none |
| Git Reference: Main Porcelain - Patching (`apply`, `cherry-pick`, `rebase`, `revert`) | [[Git-Rebase-Internals-and-Stacked-Branches]] | Use, Explain, Debug | none |
| Git Reference: Main Porcelain - Debugging (`bisect`, `blame`, `grep`) | [[Git-Archaeology-Log-Blame-and-Bisect]] | Use, Explain, Debug | none |
| Git Reference: Main Porcelain - Repository Upkeep (`gc`, `maintenance`) | [[Git-GC-Maintenance-MIDX-and-Commit-Graph]] | Use, Explain, Decide at Scale | none |
| Git Reference: Main Porcelain - Email and Patch Communication (`am`, `format-patch`, `send-email`, `request-pull`) | [[Git-Email-and-Patch-Workflows]] | Use, Explain | none |
| Git Reference: Ancillary Manipulators (`config`, `pack-refs`, `prune`, `reflog`, `refs`, `repack`, `replace`) | [[Git-Packfiles-and-Delta-Compression]], [[Git-GC-Maintenance-MIDX-and-Commit-Graph]], [[Git-Expert-Config]] | Use, Explain, Decide at Scale | none |
| Git Reference: Ancillary Interrogators (`blame`, `fsck`, `merge-tree`, `rerere`, `verify-commit`, `verify-tag`) and the EXPERIMENTAL low-level interrogator `last-modified` (added in 2.52) | [[Git-Archaeology-Log-Blame-and-Bisect]], [[Repair-a-Corrupted-Git-Repository]], [[Git-Hooks-and-Cryptographic-Signing]] | Use, Debug, Rebuild | none |
| Git Reference: Plumbing Manipulation (`apply`, `checkout-index`, `commit-graph`, `commit-tree`, `hash-object`, `index-pack`, `merge-file`, `multi-pack-index`, `pack-objects`, `read-tree`, `replay` (EXPERIMENTAL), `symbolic-ref`, `update-index`, `update-ref`, `write-tree`) | [[Git-Object-Model-and-Content-Addressing]], [[Git-Replay-and-Server-Side-History-Rewriting]], [[Rebuild-Mini-Git-Lab]] | Explain, Rebuild, Decide at Scale | none |
| Git Reference: Plumbing Interrogation (`cat-file`, `diff-files`, `diff-index`, `diff-tree`, `for-each-ref`, `ls-files`, `ls-remote`, `ls-tree`, `merge-base`, `rev-list`, `rev-parse`, `show-index`, `verify-pack`) | [[Git-Object-Model-and-Content-Addressing]], [[Git-Revision-Selection-and-Graph-Walks]] | Explain, Debug, Rebuild | none |
| Git Reference: File Formats - Bundle (`gitformat-bundle(5)`) | [[Git-Transfer-Protocol-and-Partial-Clone]] | Explain, Decide at Scale | none |
| Git Reference: File Formats - Chunk Format (`gitformat-chunk(5)`) | [[Git-GC-Maintenance-MIDX-and-Commit-Graph]] | Explain, Rebuild | none |
| Git Reference: File Formats - Commit-Graph (`gitformat-commit-graph(5)`) | [[Git-GC-Maintenance-MIDX-and-Commit-Graph]] | Explain, Rebuild | none |
| Git Reference: File Formats - Index Format (`gitformat-index(5)`) | [[Git-Index-Stat-Cache-and-Fsmonitor]] | Explain, Rebuild | [[19-Shell-and-Unix-Engineering/10-Files-Filesystems-and-Permissions/inotify-and-fswatch.md\|inotify-and-fswatch]] |
| Git Reference: File Formats - Packfile and Pack Index (`gitformat-pack(5)`) | [[Git-Packfiles-and-Delta-Compression]] | Explain, Rebuild | none |
| Git Reference: File Formats - Cryptographic Signature Formats (`gitformat-signature(5)`) | [[Git-Hooks-and-Cryptographic-Signing]] | Explain, Decide at Scale | [[19-Shell-and-Unix-Engineering/11-Networking-and-SSH/SSH-Protocol-and-Keys.md\|SSH-Protocol-and-Keys]] |
| Git Reference: Protocols - Wire Protocol v2 (`gitprotocol-v2(5)`) | [[Git-Transfer-Protocol-and-Partial-Clone]] | Explain, Decide at Scale | none |
| Git Technical Docs: Reachability Bitmaps and Cruft Packs (`bitmap-format`, `pack-heuristics`, and the cruft packs section of `gitformat-pack(5)`) | [[Git-Packfiles-and-Delta-Compression]], [[Git-GC-Maintenance-MIDX-and-Commit-Graph]] | Explain, Decide at Scale | none |
| Git Technical Docs: Block-Indexed Reference Backend (`reftable`) | [[Git-Refs-Reflog-and-Reftable]] | Explain, Decide at Scale | none |
| Git Technical Docs: Cryptographic Transition (`hash-function-transition`, `BreakingChanges`) | [[Git-Object-Model-and-Content-Addressing]], [[Case-Study-SHAttered-SHA1-Collision-and-Git-Hardening]] | Explain, Decide at Scale | [[15-Technical-Whitepapers/05-Offensive-Security-and-Exploitation/Binary-Exploitation-and-Reverse-Eng.md\|Binary-Exploitation-and-Reverse-Eng]] |
| Git Technical Docs: Bundle URIs and Partial Clones (`bundle-uri`, `partial-clone`) | [[Git-Transfer-Protocol-and-Partial-Clone]] | Explain, Decide at Scale | none |
| Git Technical Docs: Sparse Index and Cone Mode (`sparse-checkout`, `sparse-index`) | [[Git-Index-Stat-Cache-and-Fsmonitor]], [[Git-At-Scale-VFS-Scalar-and-Monorepos]] | Explain, Decide at Scale | none |
| Git Technical Docs: Merge Strategies and Rename Detection (`remembering-renames`, `directory-rename-detection`) | [[Git-Three-Way-Merge-ORT-and-Diff-Algorithms]] | Explain, Rebuild | none |
| Git Technical Docs: Telemetry Infrastructure (`api-trace2`) | [[Git-Trace2-Telemetry-and-Diagnostics]] | Explain, Decide at Scale | [[19-Shell-and-Unix-Engineering/20-Observability-and-Incident-Response/Log-Triage-Pipelines.md\|Log-Triage-Pipelines]] |
| Pro Git 2nd Ed: Chapter 3 Git Branching & Workflows | [[Git-Branching-Models-Merge-Queues-and-Code-Review]], [[Decision-Record-Merge-vs-Rebase-and-Branching]] | Use, Decide at Scale | [[09-Engineering-Leadership/03-Code-Review/checklist.md\|checklist]] |
| Pro Git 2nd Ed: Chapter 7 Advanced Merging, Rerere, and Revision Selection | [[Git-Revision-Selection-and-Graph-Walks]], [[Git-Three-Way-Merge-ORT-and-Diff-Algorithms]] | Use, Explain, Debug | none |
| Pro Git 2nd Ed: Chapter 8 Customizing Git (Config, Attributes, Hooks) | [[Git-Expert-Config]], [[Git-Submodules-Subtrees-LFS-and-Attributes]], [[Git-Hooks-and-Cryptographic-Signing]] | Use, Explain | none |
| Pro Git 2nd Ed: Chapter 10 Git Internals (Plumbing, Objects, References, Packfiles) | [[Git-Object-Model-and-Content-Addressing]], [[Git-Refs-Reflog-and-Reftable]], [[Git-Packfiles-and-Delta-Compression]], [[Recover-Lost-Git-Work]] | Explain, Debug, Rebuild | none |
| Jujutsu Documentation: Architecture, Conflicts, Operation Log, Revsets (v0.46.0) | [[Beyond-Git-Jujutsu-Sapling-and-Patch-Theory]] | Use, Explain, Decide at Scale | none |
| Sapling SCM Documentation: Smartlog, Commits, Mononoke, EdenFS (v0.2.20260929) | [[Git-At-Scale-VFS-Scalar-and-Monorepos]], [[Beyond-Git-Jujutsu-Sapling-and-Patch-Theory]] | Explain, Decide at Scale | none |
| Scalar Reference Manual: Enlistments, Background Maintenance, Sparse-Checkout Cone Mode (`scalar(1)`) | [[Git-At-Scale-VFS-Scalar-and-Monorepos]] | Use, Explain, Decide at Scale | none |
| git-filter-repo Manual: History Rewriting, Secret Scrubbing, Ref Filtering (`git-filter-repo(1)`) | [[Remove-Secrets-and-Rewrite-Git-History]] | Use, Debug, Decide at Scale | [[11-Security-And-Cryptography/README.md\|Security-And-Cryptography]] |
| Git LFS Specification: Pointer Files, Clean and Smudge Filter Protocol, Batch API (`docs/spec.md`) | [[Git-Submodules-Subtrees-LFS-and-Attributes]] | Explain, Decide at Scale | none |

## Out of scope

- Main Porcelain External Systems (`git-svn`, `git-p4`, `fast-import` legacy bridges) are out of scope because legacy Subversion and Perforce client bridges are superseded by native modern version control coverage and git-filter-repo.
- Pro Git Chapter 6 GitHub is out of scope because forge-specific web user interface tutorials and web settings do not belong in core toolchain mechanics; CLI and GitHub Actions workflows are owned by 01-Command-Line and CI systems.
- Git internal C helper APIs (`api-parse-options`, `api-error-handling`, `api-simple-ipc`) are out of scope because internal C helper macros are git.git contributor internals rather than developer toolchain engineering concepts.
