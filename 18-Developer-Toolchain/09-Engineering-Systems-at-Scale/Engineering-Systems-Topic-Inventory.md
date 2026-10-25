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
  - "Caitlin Sadowski et al., Lessons from Building Static Analysis Tools at Google, Communications of the ACM, Vol. 61, No. 4, pp. 58-66, 2018, https://doi.org/10.1145/3188720"
  - "Caitlin Sadowski et al., Modern Code Review: A Case Study at Google, ICSE-SEIP 2018, pp. 181-190, 2018, https://sback.it/publications/icse2018seip.pdf"
  - "CNCF TAG App Delivery, CNCF Platforms White Paper, v1.0, 2023, https://tag-app-delivery.cncf.io/whitepapers/platforms/"
---

# Engineering Systems at Scale Topic Inventory

This inventory maps the authoritative literature on developer productivity, platform engineering, static analysis platforms, and AI agent integration to notes in 18-Developer-Toolchain.

| Source heading group | Owning note | Depth target | Existing vault coverage |
| :--- | :--- | :--- | :--- |
| SWE at Google: Thesis (Time and Change, Scale and Efficiency, Hyrum's Law, Beyonce Rule) | [[Platform-Engineering-Paved-Roads-and-Deprecation]], [[Dependency-Management-Theory-and-Hyrums-Law]] | Explain, Decide at Scale | none |
| SWE at Google: Measuring Productivity (Goals-Signals-Metrics GSM, Qual/Quant Triangulation) | [[Developer-Productivity-Metrics-DORA-SPACE-and-GSM]] | Explain, Decide at Scale | [[04-System-Design/00-Concepts/SLOs-and-Observability.md\|SLOs-and-Observability]] |
| SWE at Google: Style Guides, Readability, Code Review Processes | [[Code-Review-and-Readability-Systems-at-Scale]] | Explain, Decide at Scale | [[09-Engineering-Leadership/03-Code-Review/checklist.md\|checklist]] |
| SWE at Google: Testing Overview, Flaky Tests, Presubmit Infrastructure | [[Build-and-Test-Latency-Economics-and-Flaky-Tests]] | Explain, Debug, Decide at Scale | none |
| SWE at Google: Technology Deprecation (Advisory vs Compulsory, Migration Budgets) | [[Platform-Engineering-Paved-Roads-and-Deprecation]] | Explain, Decide at Scale | none |
| SWE at Google: Source Code Search and Indexing Architecture | [[Code-Search-and-Semantic-Indexing-at-Scale]] | Explain, Decide at Scale | none |
| SWE at Google: Ephemeral Cloud Compute and Developer Environments | [[Ephemeral-Cloud-Development-Environments]] | Use, Explain, Decide at Scale | [[19-Shell-and-Unix-Engineering/19-Containers-CI-and-Cloud/README.md\|Containers-CI-and-Cloud]] |
| SWE at Google: Static Analysis Ecosystems and Review-Time Warnings | [[Static-Analysis-at-Scale-Tricorder-and-Autofix]] | Explain, Decide at Scale | none |
| SWE at Google: Dependency Management, Diamond Dependencies, Live at Head | [[Dependency-Management-Theory-and-Hyrums-Law]] | Explain, Decide at Scale | none |
| SWE at Google: Large-Scale Changes, Codemods, Sharding Across Ownership Boundaries | [[Large-Scale-Changes-and-Codemods-Across-Repos]] | Explain, Decide at Scale | none |
| DORA Capabilities: Core Delivery Metrics, Generative Learning Culture, Deployment Automation | [[Developer-Productivity-Metrics-DORA-SPACE-and-GSM]], [[Platform-Engineering-Paved-Roads-and-Deprecation]] | Explain, Decide at Scale | none |
| DORA Capabilities: Fast Feedback, Continuous Integration, Test Data Infrastructure | [[Build-and-Test-Latency-Economics-and-Flaky-Tests]] | Explain, Debug, Decide at Scale | none |
| SPACE Framework: Five Productivity Dimensions, Goodhart's Law Avoidance | [[Developer-Productivity-Metrics-DORA-SPACE-and-GSM]] | Explain, Decide at Scale | none |
| Tricorder & CACM: Workflow Integration, <10% False Positive Bar, Machine-Applicable Fixes | [[Static-Analysis-at-Scale-Tricorder-and-Autofix]] | Explain, Decide at Scale | none |
| Modern Code Review Paper: Change Size Bounds, Review Latency SLAs, Readability Standards | [[Code-Review-and-Readability-Systems-at-Scale]] | Explain, Decide at Scale | [[09-Engineering-Leadership/03-Code-Review/checklist.md\|checklist]] |
| CNCF Platforms White Paper: Internal Developer Platforms (IDP), Golden Paths, Platform-as-Product | [[Platform-Engineering-Paved-Roads-and-Deprecation]] | Explain, Decide at Scale | [[17-Cloud-AI-Engineering/Volume_05_Kubernetes_and_Platform_Engineering/Chapter_06_Internal_Developer_Platforms_and_Backstage.md\|Internal_Developer_Platforms]] |
| Autonomous Agent Toolchains: Worktree Isolation, CLI Ergonomics (AXI), Deterministic Gates | [[AI-Coding-Agents-Sandboxes-Worktrees-and-Ship-Gates]] | Use, Explain, Decide at Scale | [[13-Agentic-AI/Agentic-Harness/components/treehouse.md\|treehouse]], [[13-Agentic-AI/Agentic-Harness/components/axi.md\|axi]], [[19-Shell-and-Unix-Engineering/22-Shell-and-AI-Agents/Agent-Shell-Sandboxing-Seatbelt-and-Landlock.md\|Agent-Shell-Sandboxing]] |
| Autonomous Agent Toolchains: Multi-Agent Fleets, Supervisor Models, Quota-Aware Dispatch | [[Multi-Agent-Supervision-and-Quota-Aware-Routing]] | Explain, Decide at Scale | [[13-Agentic-AI/Agentic_AI_Zero_to_Godhood/Volume_13_Coding_Agents_and_Computer_Use/Chapter_06_Async_Agents_and_Fleets.md\|Async_Agents_and_Fleets]] |
| Operational Runbooks: Automated Flaky Test Quarantine and Deflaking Workflows | [[Playbook-Quarantine-and-Eliminate-Flaky-Tests]] | Use, Debug | none |
| Operational Runbooks: Large-Scale Breaking API Migration Across 1,000 Repositories | [[Playbook-Execute-a-1000-Repo-Breaking-API-Migration]] | Decide at Scale | [[19-Shell-and-Unix-Engineering/23-Shell-at-Organizational-Scale/Migrating-Shell-to-Real-Languages.md\|Migrating-Shell-to-Real-Languages]] |
| Operational Runbooks: Staff/Principal Toolchain Migration Design Doc (RFC) Template | [[Playbook-Write-a-Toolchain-Migration-RFC]] | Decide at Scale | [[19-Shell-and-Unix-Engineering/23-Shell-at-Organizational-Scale/Build-versus-Buy-for-Tooling.md\|Build-versus-Buy-for-Tooling]] |
| Diagnostic Drills: CI Merge Queue Throughput Collapse Troubleshooting | [[Drill-Engineering-Systems-Diagnose-CI-Merge-Queue-Collapse]] | Debug, Decide at Scale | none |
| Diagnostic Drills: Auditing and Correcting a Gamed Developer Productivity Dashboard | [[Drill-Engineering-Systems-Audit-a-Gamed-Productivity-Dashboard]] | Debug, Decide at Scale | none |

## Out of scope

- General engineering management, team leadership culture, and interpersonal communication (such as Chapters 2 through 6 of Software Engineering at Google) are out of scope because managerial leadership belongs to 09-Engineering-Leadership, not 18-Developer-Toolchain.
