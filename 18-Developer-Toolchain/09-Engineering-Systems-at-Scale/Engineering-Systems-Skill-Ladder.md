---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources:
  - "Titus Winters, Tom Manshreck, Hyrum Wright, Software Engineering at Google: Lessons Learned from Programming Over Time, O'Reilly Media, 2020, https://abseil.io/resources/swe-book"
  - "DORA, DORA Core Model and Capabilities Catalog, Google Cloud, 2026, https://dora.dev/capabilities/"
  - "Nicole Forsgren et al., The SPACE of Developer Productivity: There's more to it than you think, ACM Queue, Vol. 19, No. 1, 2021, https://doi.org/10.1145/3454122.3454124"
  - "Caitlin Sadowski et al., Tricorder: Building a Program Analysis Ecosystem, ICSE 2015, pp. 598-608, 2015, https://research.google/pubs/tricorder-building-a-program-analysis-ecosystem/"
---

# Engineering Systems at Scale Skill Ladder

This skill ladder establishes observable, testable engineering behaviors for platform, developer tooling, and engineering infrastructure roles from junior individual contributor (L4) through distinguished engineer and systems architect (L8+).

## L4

- Can write reproducible unit tests adhering to hermetic test size rules (small, medium, large) without accessing unisolated network services ([[Build-and-Test-Latency-Economics-and-Flaky-Tests]]).
- Can perform global symbol and regular expression searches across multi-million-line codebases using trigram search engines ([[Code-Search-and-Semantic-Indexing-at-Scale]]).
- Can configure and execute automated AST codemod scripts (using tools like Semgrep or ast-grep) for local repository refactorings ([[Large-Scale-Changes-and-Codemods-Across-Repos]]).
- Can configure and work within ephemeral development containers and cloud workstations without local environment drift ([[Ephemeral-Cloud-Development-Environments]]).
- Can resolve static analysis warnings and apply suggested automated fixes in review interfaces without dismissing valid alerts ([[Static-Analysis-at-Scale-Tricorder-and-Autofix]]).

## L5

- Can isolate, reproduce, and quarantine flaky test cases using stress tests and deterministic runtime tracing ([[Build-and-Test-Latency-Economics-and-Flaky-Tests]], [[Playbook-Quarantine-and-Eliminate-Flaky-Tests]]).
- Can author clean, reviewable pull requests under 200 lines adhering to team readability and style standards with high turnaround velocity ([[Code-Review-and-Readability-Systems-at-Scale]]).
- Can design and execute an automated codemod campaign across dozens of repositories with compatibility shims and canary verification ([[Large-Scale-Changes-and-Codemods-Across-Repos]]).
- Can integrate autonomous AI coding agents into local development workflows using isolated git worktrees and deterministic verification gates ([[AI-Coding-Agents-Sandboxes-Worktrees-and-Ship-Gates]]).
- Can author automated linter rules with actionable machine-applicable fixes and maintain effective false-positive rates below 10% ([[Static-Analysis-at-Scale-Tricorder-and-Autofix]]).

## L6

- Can diagnose and remediate CI merge queue throughput collapse by re-tuning batch sizes and isolating non-deterministic test suites ([[Drill-Engineering-Systems-Diagnose-CI-Merge-Queue-Collapse]]).
- Can audit an organization developer productivity metrics dashboard, identify Goodhart's Law gaming pathologies, and implement balanced SPACE and GSM frameworks ([[Drill-Engineering-Systems-Audit-a-Gamed-Productivity-Dashboard]], [[Developer-Productivity-Metrics-DORA-SPACE-and-GSM]]).
- Can execute a breaking API migration campaign across hundreds of repositories with automated pull request sharding and burndown tracking ([[Playbook-Execute-a-1000-Repo-Breaking-API-Migration]]).
- Can architect and maintain paved roads (golden paths) for service bootstrapping and CI/CD onboarding that reduce developer cognitive load ([[Platform-Engineering-Paved-Roads-and-Deprecation]]).
- Can implement multi-agent supervision harnesses with quota-aware routing and fresh-context verification to prevent review queue saturation ([[Multi-Agent-Supervision-and-Quota-Aware-Routing]]).

## L7

- Can author a comprehensive toolchain migration RFC including financial ROI modeling, failure mode risk matrices, and strangler-fig migration plans ([[Playbook-Write-a-Toolchain-Migration-RFC]]).
- Can formulate organizational dependency management strategy, enforcing the One-Version Rule and mitigating Hyrum's Law risks across Live-at-Head deployments ([[Dependency-Management-Theory-and-Hyrums-Law]]).
- Can design company-wide static analysis and presubmit review infrastructure supporting hundreds of analyzers with automated telemetry tracking developer satisfaction ([[Static-Analysis-at-Scale-Tricorder-and-Autofix]]).
- Can design automated code review routing algorithms and readability certification governance models for multi-thousand engineer organizations ([[Code-Review-and-Readability-Systems-at-Scale]]).
- Can architect an enterprise-grade semantic code search and indexing infrastructure supporting cross-language compiler graphs ([[Code-Search-and-Semantic-Indexing-at-Scale]]).

## L8+

- Can establish company-wide developer productivity measurement standards that steer executive resource allocation without perverse incentives ([[Developer-Productivity-Metrics-DORA-SPACE-and-GSM]]).
- Can architect the technological and cultural evolution of enterprise software engineering across thousands of services, bounding blast radius and managing technological debt ([[Platform-Engineering-Paved-Roads-and-Deprecation]], [[Dependency-Management-Theory-and-Hyrums-Law]]).
- Can design the architectural integration of autonomous multi-agent software engineering fleets into core engineering systems, establishing safety gates, economic bounds, and organizational verification capacity ([[AI-Coding-Agents-Sandboxes-Worktrees-and-Ship-Gates]], [[Multi-Agent-Supervision-and-Quota-Aware-Routing]]).
- Can align engineering executive leadership and lead multi-year platform transformations replacing obsolete version control, build, or deployment systems ([[Playbook-Write-a-Toolchain-Migration-RFC]]).
