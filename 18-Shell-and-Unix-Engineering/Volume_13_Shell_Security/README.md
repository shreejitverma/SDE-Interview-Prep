---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 13 - Shell Security

Shell is a language where data becomes code by default.
This volume covers every injection surface, the environment and filesystem attacks specific to scripts, privilege handling, secrets, supply chain risk, and two landmark incidents: Shellshock and the xz-utils backdoor.

## Prerequisites

- [Volume 02 - The POSIX Shell Command Language](../Volume_02_The_POSIX_Shell_Command_Language/README.md)
- [Volume 09 - Robust and Defensive Scripting](../Volume_09_Robust_and_Defensive_Scripting/README.md)

## Chapters

1. [A Threat Model for Shell Scripts](Chapter_01_Threat_Model_for_Shell_Scripts.md) - Sources of untrusted input, trust boundaries, and privilege.
2. [Command and Code Injection](Chapter_02_Command_and_Code_Injection.md) - eval, `sh -c`, ssh remote commands, and safe quoting with `printf %q`.
3. [Argument Injection and Hostile Filenames](Chapter_03_Argument_Injection_and_Hostile_Filenames.md) - Leading dashes and the double-dash convention, newlines and glob characters in file names, and `find -print0`.
4. [Environment Attacks: PATH, IFS, and Startup Files](Chapter_04_Environment_Attacks_PATH_IFS_and_Startup_Files.md) - Hijacking command search, manipulating IFS, and the `BASH_ENV` and `ENV` variables.
5. [Shellshock](Chapter_05_Shellshock.md) - CVE-2014-6271 and its follow-up CVEs, the function import mechanism that caused them, and the fix.
6. [Temporary Files and TOCTOU](Chapter_06_Temporary_Files_and_TOCTOU.md) - Predictable names, symlink attacks, mktemp, and private directories.
7. [Privilege: setuid, sudo, and Capabilities](Chapter_07_Privilege_setuid_sudo_and_Capabilities.md) - Why setuid scripts are unsafe, sudoers rules and secure_path, and dropping privileges.
8. [Secrets in Shell](Chapter_08_Secrets_in_Shell.md) - Exposure through argv, the environment, `/proc`, and history, and safe handling patterns.
9. [Supply Chain: Piped Installers](Chapter_09_Supply_Chain_Piped_Installers.md) - The risks of piping a downloaded script into a shell, verification, and pinning.
10. [Case Study: The xz-utils Backdoor](Chapter_10_Case_Study_The_xz_Utils_Backdoor.md) - CVE-2024-3094, its build-time shell stages, and the lessons for reviewing build scripts.
11. [Restricted Shells and Sandboxing](Chapter_11_Restricted_Shells_and_Sandboxing.md) - rbash, chroot, namespaces, and their limits.

## Labs

Each chapter's worked example lives in [`labs/`](../labs/README.md) as a lab directory named `v13-cNN-<slug>`, with a bats test that runs under every shell in the matrix.
