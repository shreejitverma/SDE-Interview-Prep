---
type: problem
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Drill: Verify and Reject Forged SLSA Provenance Attestations

> [!summary] Scope
> Investigative exercise inspecting a signed binary accompanied by an in-toto DSSE attestation claiming SLSA L3 build provenance.
> Guides the engineer through using cosign and slsa-verifier to inspect Fulcio certificate subject alternative names, decode Rekor log entries, detect an unauthorized builder workflow identity, and reject the artifact at deployment time.
> Teaches cryptographic provenance verification without relying on blind trust.
> Provides sample forged and legitimate bundles for comparative inspection.

## Related

- Prerequisites: [[SLSA-Framework-Build-and-Source-Tracks]], [[Sigstore-Keyless-Signing-Cosign-Fulcio-and-Rekor]]
- Next: [[Case-Study-XZ-Utils-Backdoor-CVE-2024-3094]]
- Lab: `code/08-Software-Supply-Chain/slsa_verification/`
