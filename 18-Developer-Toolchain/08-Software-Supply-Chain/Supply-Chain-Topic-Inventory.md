---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources:
  - "OpenSSF SLSA Steering Committee, Supply-chain Levels for Software Artifacts (SLSA) Specification v1.2, OpenSSF, 2026, https://slsa.dev/spec/v1.2/"
  - "Santiago Torres-Arias et al., in-toto Specification v1.0, in-toto Project, 2023, https://github.com/in-toto/specification/blob/master/in-toto-spec.md"
  - "in-toto Attestation Framework Contributors, in-toto Attestation Framework Specification v1.1 (v1.2.0), in-toto Project, 2026, https://github.com/in-toto/attestation/tree/main/spec/v1"
  - "Sigstore Project Authors, Sigstore Documentation (Cosign v3.1.3, Fulcio, Rekor, Gitsign), Linux Foundation, 2026, https://docs.sigstore.dev/"
  - "SPDX Workgroup, The System Package Data Exchange (SPDX) Specification Version 3.0.1, Linux Foundation, 2024, https://spdx.github.io/spdx-spec/v3.0.1/"
  - "OWASP CycloneDX Project, CycloneDX Authoritative Guide and Specification v1.7.2, OWASP Foundation, 2026, https://cyclonedx.org/specification/overview/"
  - "OpenSSF Scorecard Maintainers, OpenSSF Scorecard Checks Documentation v5.6.0, Open Source Security Foundation, 2026, https://github.com/ossf/scorecard/blob/main/docs/checks.md"
  - "Reproducible Builds Project, Reproducible Builds Documentation and Specification, Software in the Public Interest, 2026, https://reproducible-builds.org/docs/"
  - "Andres Freund, Backdoor in upstream xz/liblzma leading to SSH server compromise (CVE-2024-3094), oss-security, 2024, https://www.openwall.com/lists/oss-security/2024/03/29/4"
  - "Ayrton Sparling (FallingSnow), I don't know what to say. (event-stream Issue 116, repository of Dominic Tarr), GitHub, 2018, https://github.com/dominictarr/event-stream/issues/116"
  - "Cybersecurity and Infrastructure Security Agency, Emergency Directive 21-01 Mitigate SolarWinds Orion Code Compromise, CISA, 2020, https://www.cisa.gov/news-events/directives/ed-21-01-mitigate-solarwinds-orion-code-compromise-closed"
  - "Codecov Security Team, Bash Uploader Security Update, Codecov, 2021, https://about.codecov.io/security-update/"
  - "GitHub Advisory Database, tj-actions/changed-files runner memory exfiltration (CVE-2025-30066 / GHSA-mrrh-fwg8-r2c3), GitHub, 2025, https://github.com/advisories/GHSA-mrrh-fwg8-r2c3"
  - "PyTorch Team, Compromised PyTorch-nightly Dependency Chain (torchtriton), PyTorch Foundation, 2022, https://pytorch.org/blog/compromised-nightly-dependency/"
  - "Alex Birsan, Dependency Confusion: How I Hacked Into Apple, Microsoft and Dozens of Other Companies, Medium, 2021, https://medium.com/@alex.birsan/dependency-confusion-4a5d60ffec61"
  - "Cybersecurity and Infrastructure Security Agency, Widespread Supply Chain Compromise Impacting npm Ecosystem (Shai-Hulud), CISA, 2025, https://www.cisa.gov/news-events/alerts/2025/09/23/widespread-supply-chain-compromise-impacting-npm-ecosystem"
  - "Python Security Response Team, ctx Domain Takeover and Malicious PyPI Uploads, Python Software Foundation, 2022, https://python-security.readthedocs.io/pypi-vuln/index-2022-05-24-ctx-domain-takeover.html"
  - "Sansec Research, Polyfill Supply Chain Attack Disclosed (2024-06-25, domain acquired February 2024), Sansec B.V., 2024, https://sansec.io/research/polyfill-supply-chain-attack"
  - "Ultralytics Maintainers, Malicious Actions Execution in CI Workflows (Issue 18027 / Attack PR 18020), Ultralytics, 2024, https://github.com/ultralytics/ultralytics/issues/18027"
  - "GitHub Advisory Database, ua-parser-js Account Takeover and Malicious Version Publishing (CVE-2021-4229 / GHSA-pjwm-rvh2-c87w), GitHub, 2021, https://github.com/advisories/GHSA-pjwm-rvh2-c87w"
---

# Software Supply Chain Topic Inventory

Authoritative source-to-note mapping and depth targets for software supply chain security, cryptographic provenance, and artifact integrity.

| Source heading group | Owning note | Depth target | Existing vault coverage |
|---|---|---|---|
| SLSA v1.2 Spec (Threats A-I, Build L0-L3, Source Track, VSAs) | [[SLSA-Framework-Build-and-Source-Tracks]] | Concept / L6 | [[17-Cloud-AI-Engineering/Volume_08_Security_Identity_and_the_Data_Perimeter/Chapter_05_Software_Supply_Chain_Security_SLSA_and_Sigstore.md\|Supply Chain Security SLSA and Sigstore]] |
| in-toto Spec & Attestation Framework v1.1/v1.2 (DSSE, Links) | [[In-Toto-Layouts-Link-Metadata-and-DSSE-Attestations]] | Concept / L6 | [[17-Cloud-AI-Engineering/Volume_08_Security_Identity_and_the_Data_Perimeter/Chapter_05_Software_Supply_Chain_Security_SLSA_and_Sigstore.md\|Supply Chain Security SLSA and Sigstore]] |
| Sigstore Docs (Cosign v3.1.3, Fulcio OIDC, Rekor, Gitsign) | [[Sigstore-Keyless-Signing-Cosign-Fulcio-and-Rekor]] | Concept / L6 | [[17-Cloud-AI-Engineering/Volume_08_Security_Identity_and_the_Data_Perimeter/Chapter_05_Software_Supply_Chain_Security_SLSA_and_Sigstore.md\|Supply Chain Security SLSA and Sigstore]] |
| SPDX v3.0.1 Spec (Core, Software, Security, Licensing, purl) | [[SBOM-Standards-SPDX-CycloneDX-and-VEX]] | Concept / L6 | [[11-Security-And-Cryptography/01-Common-Vulnerabilities/owasp_top_10.md\|OWASP Top 10]] |
| CycloneDX v1.7.2 Spec (Object model, components, VEX, CBOM) | [[SBOM-Standards-SPDX-CycloneDX-and-VEX]] | Concept / L6 | none |
| OpenSSF Scorecard v5.6.0 Checks & CI Hardening | [[OpenSSF-Scorecard-and-CI-CD-Pipeline-Hardening]] | Concept / L6 | [[19-Shell-and-Unix-Engineering/22-Shell-and-AI-Agents/Agent-Shell-Sandboxing-Seatbelt-and-Landlock.md\|Agent Shell Sandboxing]] |
| Registry Attacks & Dependency Confusion (Birsan, Typos) | [[Dependency-Confusion-Typosquatting-and-Registry-Attacks]] | Concept / L6 | [[19-Shell-and-Unix-Engineering/17-Security/Supply-Chain-Pinning-and-Verification.md\|Supply Chain Pinning]] |
| Reproducible Builds Docs (Determinism, diffoscope) | [[Reproducible-Builds-as-a-Security-Control]] | Concept / L6 | none |
| Enterprise Third-Party Ingestion & Quarantine Architecture | [[Enterprise-Third-Party-Code-Ingestion-and-Quarantine-Architecture]] | Concept / L8 | none |
| Supply Chain Architecture Decision Records | [[Supply-Chain-Decision-Records]] | Concept / L7 | none |
| Playbook: Respond to a Compromised Transitive Dependency | [[Respond-to-a-Compromised-Transitive-Dependency]] | Playbook / L7 | none |
| Playbook: Audit and Harden a GitHub Actions Pipeline | [[Audit-and-Harden-a-GitHub-Actions-Release-Pipeline]] | Playbook / L6 | [[19-Shell-and-Unix-Engineering/22-Shell-and-AI-Agents/Agent-Shell-Sandboxing-Seatbelt-and-Landlock.md\|Agent Shell Sandboxing]] |
| Drill: Detect Tarball vs Git Tag Backdoor | [[Drill-Supply-Chain-Detect-Tarball-vs-Git-Tag-Backdoor]] | Problem / L7 | none |
| Drill: Exploit and Remediate Dependency Confusion | [[Drill-Supply-Chain-Exploit-and-Fix-Dependency-Confusion]] | Problem / L6 | none |
| Drill: Verify and Reject Forged SLSA Provenance | [[Drill-Supply-Chain-Verify-and-Reject-Forged-SLSA-Provenance]] | Problem / L6 | none |
| Case Study: xz-utils Multi-Year Backdoor (CVE-2024-3094) | [[Case-Study-XZ-Utils-Backdoor-CVE-2024-3094]] | Case Study / L7 | none |
| Case Study: SolarWinds Orion SUNBURST Build-Time Source Swap | [[Case-Study-SolarWinds-Orion-SUNBURST]] | Case Study / L7 | none |
| Case Study: event-stream & Maintainer Burnout Exploitation | [[Case-Study-Event-Stream-and-Maintainer-Handoff]] | Case Study / L6 | none |
| Case Study: Codecov Bash Uploader & tj-actions Exfiltration | [[Case-Study-Codecov-Bash-Uploader-and-TJ-Actions]] | Case Study / L6 | none |
| Case Study: PyTorch torchtriton & Dependency Confusion | [[Case-Study-PyTorch-torchtriton-and-Dependency-Confusion]] | Case Study / L6 | none |
| Case Study: Shai-Hulud Self-Replicating npm Worm | [[Case-Study-Shai-Hulud-and-Self-Replicating-Registry-Worms]] | Case Study / L6 | none |

## Out of scope

- General transport layer security and mutual TLS service mesh configuration are owned by 17-Cloud-AI-Engineering.
- Dynamic web application vulnerability scanning and API penetration testing are owned by 11-Security-And-Cryptography.
- Build system graph scheduling and compilation caches are owned by 06-Build-Systems.
- Cryptographic hash collision attacks on Git object storage (such as identical-prefix collisions) are owned by 02-Version-Control.
- Foundational research papers on compilers and verification are housed exclusively in 10-Papers.
