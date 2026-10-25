---
type: moc
track: [sde, distinguished, cloud-ai]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 15 - Containers, CI, and Cloud

Containers, CI runners, and cloud instances all start with a shell script.
This volume covers Dockerfile forms and PID 1, entrypoint patterns, Kubernetes, CI shells, cloud CLIs, SSH automation, and first-boot scripts.

## Prerequisites

- [Volume 00 - The Unix Process Model Under the Shell](../Volume_00_The_Unix_Process_Model/README.md)
- [Volume 09 - Robust and Defensive Scripting](../Volume_09_Robust_and_Defensive_Scripting/README.md)

## Chapters

1. [Dockerfile Shell Form versus Exec Form](Chapter_01_Dockerfile_Shell_Form_vs_Exec_Form.md) - The RUN, CMD, and ENTRYPOINT forms, the SHELL instruction, and layer behavior.
2. [PID 1, Signals, and Zombie Reaping in Containers](Chapter_02_PID_1_Signals_and_Zombie_Reaping_in_Containers.md) - Why PID 1 is special, tini and dumb-init, and exec in entrypoint scripts.
3. [Entrypoint Script Patterns](Chapter_03_Entrypoint_Script_Patterns.md) - Configuration from the environment, waiting for dependencies, and graceful shutdown.
4. [Kubernetes and the Shell](Chapter_04_Kubernetes_and_the_Shell.md) - Probes, init containers, kubectl output formats with jq, and kubectl debug.
5. [CI Shells: GitHub Actions and Others](Chapter_05_CI_Shells_GitHub_Actions_and_Others.md) - Default shells and flags, run step semantics, and quoting untrusted workflow context.
6. [Cloud CLIs: AWS CLI and gcloud Scripting](Chapter_06_Cloud_CLIs_AWS_CLI_and_Gcloud_Scripting.md) - Server-side filtering with `--query` and `--format`, pagination, retries, and credentials.
7. [SSH Automation and Remote Quoting](Chapter_07_SSH_Automation_and_Remote_Quoting.md) - How ssh joins its arguments into one remote command, connection multiplexing, and fleet-wide fan-out.
8. [cloud-init and Bootstrap Scripts](Chapter_08_Cloud_Init_and_Bootstrap_Scripts.md) - Boot stages, user-data scripts, and debugging first boot.

## Labs

Each chapter's worked example lives in [`labs/`](../labs/README.md) as a lab directory named `v15-cNN-<slug>`, with a bats test that runs under every shell in the matrix.
