---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# SSH in Depth: Keys, Forwarding, Certificates, and FIDO2

> [!summary] Scope
> Owns OpenSSH protocol mechanics, key types (Ed25519 vs RSA), connection multiplexing (ControlMaster, ControlPath, ControlPersist), and secure jump routing (ProxyJump vs ProxyCommand).
> Analyzes SSH agent forwarding compromise hazards, destination-constrained credentials (ssh-add -h), OpenSSH Certificate Authority architecture, and hardware-backed FIDO2 security keys (ed25519-sk).
> Reaches Use through Explain, handing production deployment procedures to Harden-SSH-Access-With-Certificates-and-Hardware-Keys.md.

## Related

- Prerequisites: none
- Next: [[Harden-SSH-Access-With-Certificates-and-Hardware-Keys]]
- See also: [[19-Shell-and-Unix-Engineering/11-Networking-and-SSH/SSH-Protocol-and-Keys|SSH Protocol and Keys]], [[19-Shell-and-Unix-Engineering/11-Networking-and-SSH/SSH-Config-and-ProxyJump|SSH Config and ProxyJump]], [[19-Shell-and-Unix-Engineering/11-Networking-and-SSH/SSH-Multiplexing-ControlMaster|SSH Multiplexing ControlMaster]], [[19-Shell-and-Unix-Engineering/11-Networking-and-SSH/SSH-Port-Forwarding-and-Tunnels|SSH Port Forwarding and Tunnels]], [[19-Shell-and-Unix-Engineering/11-Networking-and-SSH/ssh-agent-and-Forwarding|ssh agent and Forwarding]], [[19-Shell-and-Unix-Engineering/11-Networking-and-SSH/SSH-Certificates|SSH Certificates]]
- Lab: `code/01-Command-Line/ssh_ca_minting/`
