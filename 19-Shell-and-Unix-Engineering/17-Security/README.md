---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Shell Security

Shell is a language where data becomes code by default.
These notes cover every injection surface, environment and filesystem attacks, privilege, secrets, supply chain, and the landmark incidents.

Part of [Shell and Unix Engineering](../README.md).

## Learning order

Read in this order; each note links its prerequisite and the next note.

1. [Shell Threat Model](Shell-Threat-Model.md)
2. [Command Injection](Command-Injection.md)
3. [eval and sh -c](eval-and-sh-c.md)
4. [Argument Injection and Double Dash](Argument-Injection-and-Double-Dash.md)
5. [Hostile Filenames](Hostile-Filenames.md)
6. [PATH Hijacking](PATH-Hijacking.md)
7. [IFS Attacks](IFS-Attacks.md)
8. [BASH_ENV and ENV](BASH_ENV-and-ENV.md)
9. [Shellshock](Shellshock.md)
10. [Temporary File Attacks](Temporary-File-Attacks.md)
11. [sudo and sudoers](sudo-and-sudoers.md)
12. [Dropping Privileges](Dropping-Privileges.md)
13. [Secrets in argv, the Environment, and History](Secrets-in-argv-env-and-History.md)
14. [Piped Installers](Piped-Installers.md)
15. [Supply Chain Pinning and Verification](Supply-Chain-Pinning-and-Verification.md)
16. [The xz-utils Backdoor](xz-Utils-Backdoor.md)
17. [Restricted Shells](Restricted-Shells.md)
18. [Sandboxing Scripts](Sandboxing-Scripts.md)
