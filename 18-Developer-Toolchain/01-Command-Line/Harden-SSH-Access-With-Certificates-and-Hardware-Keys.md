---
type: playbook
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Harden SSH Access With Certificates and Hardware Keys

> [!summary] Scope
> Owns the production runbook for hardening corporate SSH infrastructure.
> Details procedures for establishing OpenSSH User and Host Certificate Authorities, issuing time-bounded signed certificates with principal restrictions, publishing Key Revocation Lists (KRL), eliminating agent forwarding via ProxyJump bastions, and configuring FIDO2 resident hardware keys.
> Reaches Use and Debug, putting SSH-In-Depth-Keys-Forwarding-Certificates-and-FIDO2.md into operational practice.

## Related

- Prerequisites: [[SSH-In-Depth-Keys-Forwarding-Certificates-and-FIDO2]]
- Next: [[Modal-Editing-LSP-Tree-Sitter-and-DAP]]
- See also: [[19-Shell-and-Unix-Engineering/11-Networking-and-SSH/SSH-Certificates|SSH Certificates]], [[19-Shell-and-Unix-Engineering/11-Networking-and-SSH/SSH-Config-and-ProxyJump|SSH Config and ProxyJump]]
- Lab: `code/01-Command-Line/ssh_fido2_krl/`
