---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources:
  - "OpenSSF SLSA Steering Committee, Supply-chain Levels for Software Artifacts (SLSA) Specification v1.2, OpenSSF, 2026, https://slsa.dev/spec/v1.2/"
  - "Sigstore Project Authors, Sigstore Documentation (Cosign v3.1.3, Fulcio, Rekor, Gitsign), Linux Foundation, 2026, https://docs.sigstore.dev/"
  - "OWASP CycloneDX Project, CycloneDX Authoritative Guide and Specification v1.7.2, OWASP Foundation, 2026, https://cyclonedx.org/specification/overview/"
  - "OpenSSF Scorecard Maintainers, OpenSSF Scorecard Checks Documentation v5.6.0, Open Source Security Foundation, 2026, https://github.com/ossf/scorecard/blob/main/docs/checks.md"
---

# Software Supply Chain Skill Ladder

Five-tier competency framework defining observable engineering behaviors from L4 to L8+ in software supply chain defense, cryptographic attestation, and artifact verification.

## L4

- Verifies cryptographic sha256 checksums on all third-party downloaded binaries and installer scripts before execution: [[SLSA-Framework-Build-and-Source-Tracks]], [[Case-Study-Codecov-Bash-Uploader-and-TJ-Actions]].
- Audits application dependency manifests against known vulnerability advisory databases using automated tooling: [[SBOM-Standards-SPDX-CycloneDX-and-VEX]], [[Case-Study-Event-Stream-and-Maintainer-Handoff]].
- Configures local package managers to reject unauthenticated package registries and untrusted HTTP sources: [[Dependency-Confusion-Typosquatting-and-Registry-Attacks]].
- Pins direct application dependencies to exact immutable versions in local lockfiles to prevent unreviewed transitive updates: [[Case-Study-Left-Pad-and-Registry-Immutability]].
- Recognizes dangerous package installation flags such as arbitrary preinstall lifecycle hook execution: [[Case-Study-Shai-Hulud-and-Self-Replicating-Registry-Worms]].

## L5

- Generates compliant CycloneDX 1.7 or SPDX 3.0 Software Bill of Materials during continuous integration builds: [[SBOM-Standards-SPDX-CycloneDX-and-VEX]].
- Configures package manager resolution to enforce strict repository scoping and prevent public dependency confusion: [[Dependency-Confusion-Typosquatting-and-Registry-Attacks]], [[Drill-Supply-Chain-Exploit-and-Fix-Dependency-Confusion]].
- Verifies keyless Sigstore container and artifact signatures using `cosign` and transparency log Merkle inclusion proofs: [[Sigstore-Keyless-Signing-Cosign-Fulcio-and-Rekor]], [[Drill-Supply-Chain-Verify-and-Reject-Forged-SLSA-Provenance]].
- Hardens continuous integration workflows by restricting `GITHUB_TOKEN` permissions and pinning third-party actions to full commit SHAs: [[OpenSSF-Scorecard-and-CI-CD-Pipeline-Hardening]], [[Audit-and-Harden-a-GitHub-Actions-Release-Pipeline]].
- Triages false-positive security vulnerabilities in dependency trees using Vulnerability Exploitability eXchange statements: [[SBOM-Standards-SPDX-CycloneDX-and-VEX]].

## L6

- Implements automated SLSA Build Level 2 and Level 3 provenance generation using isolated hosted builders: [[SLSA-Framework-Build-and-Source-Tracks]], [[Audit-and-Harden-a-GitHub-Actions-Release-Pipeline]].
- Authors in-toto supply chain layouts, configures authorized functionaries, and validates step link metadata across pipelines: [[In-Toto-Layouts-Link-Metadata-and-DSSE-Attestations]].
- Performs forensic source-to-binary diffing on release tarballs against public Git tags using `diffoscope`: [[Reproducible-Builds-as-a-Security-Control]], [[Drill-Supply-Chain-Detect-Tarball-vs-Git-Tag-Backdoor]].
- Conducts differential code reviews on third-party crate updates using `cargo-vet` and federated audit registries: [[Enterprise-Third-Party-Code-Ingestion-and-Quarantine-Architecture]].
- Mitigates CI runner memory exfiltration and environment scraping attacks by isolating privileged build and publish environments: [[Case-Study-Codecov-Bash-Uploader-and-TJ-Actions]], [[OpenSSF-Scorecard-and-CI-CD-Pipeline-Hardening]].

## L7

- Executes organization-wide incident response when an upstream transitive dependency is backdoored or hijacked: [[Respond-to-a-Compromised-Transitive-Dependency]], [[Case-Study-XZ-Utils-Backdoor-CVE-2024-3094]].
- Designs enterprise quarantine perimeters with mandatory package publication cooldowns and sandbox detonation queues: [[Enterprise-Third-Party-Code-Ingestion-and-Quarantine-Architecture]], [[Case-Study-Shai-Hulud-and-Self-Replicating-Registry-Worms]].
- Enforces bit-for-bit reproducible build invariants across multi-language compiler pipelines using `SOURCE_DATE_EPOCH` and path remapping: [[Reproducible-Builds-as-a-Security-Control]], [[Case-Study-SolarWinds-Orion-SUNBURST]].
- Evaluates architectural decision records regarding SBOM formats, keyless Sigstore versus HSM signing, and source rebuilds: [[Supply-Chain-Decision-Records]].
- Audits and remediates subtle GitHub Actions injection vulnerabilities including untrusted `pull_request_target` event triggers: [[OpenSSF-Scorecard-and-CI-CD-Pipeline-Hardening]], [[Audit-and-Harden-a-GitHub-Actions-Release-Pipeline]].

## L8+

- Conceives and directs enterprise zero-trust supply chain architectures that isolate the organization from public package registry risks: [[Enterprise-Third-Party-Code-Ingestion-and-Quarantine-Architecture]].
- Formulates corporate policies governing third-party software ingestion, internal source rebuilds, and automated gate enforcement: [[Supply-Chain-Decision-Records]], [[Enterprise-Third-Party-Code-Ingestion-and-Quarantine-Architecture]].
- Designs cryptographic provenance evaluation engines capable of detecting sophisticated compiler-swap and build-system compromises: [[SLSA-Framework-Build-and-Source-Tracks]], [[Case-Study-SolarWinds-Orion-SUNBURST]].
- Represents the organization in international supply chain standards bodies including OpenSSF, in-toto, and SLSA steering committees: [[SLSA-Framework-Build-and-Source-Tracks]].
