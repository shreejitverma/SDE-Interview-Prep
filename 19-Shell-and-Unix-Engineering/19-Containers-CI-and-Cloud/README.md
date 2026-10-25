---
type: moc
track: [sde, distinguished, cloud-ai]
level:
status: draft
last_reviewed:
sources: []
---

# Containers, CI, and Cloud

Containers, CI runners, and cloud instances all start with a shell script.
These notes cover Dockerfile forms and PID 1, entrypoints, Kubernetes, CI shells, cloud CLIs, and first-boot scripts.

Part of [Shell and Unix Engineering](../README.md).

## Learning order

Read in this order; each note links its prerequisite and the next note.

1. [Dockerfile Shell and Exec Form](Dockerfile-Shell-and-Exec-Form.md)
2. [PID 1 and Signal Forwarding](PID-1-and-Signal-Forwarding.md)
3. [tini and dumb-init](tini-and-dumb-init.md)
4. [Entrypoint Script Patterns](Entrypoint-Script-Patterns.md)
5. [Waiting for Dependencies](Waiting-for-Dependencies.md)
6. [kubectl and jq](kubectl-and-jq.md)
7. [Probes and Init Containers](Probes-and-Init-Containers.md)
8. [GitHub Actions Shells and Flags](GitHub-Actions-Shells-and-Flags.md)
9. [Untrusted Input in CI](Untrusted-Input-in-CI.md)
10. [AWS CLI Scripting](AWS-CLI-Scripting.md)
11. [gcloud Scripting](gcloud-Scripting.md)
12. [cloud-init and User Data](cloud-init-and-User-Data.md)

## See also in Developer Toolchain

- [[Ephemeral-Cloud-Development-Environments]]
