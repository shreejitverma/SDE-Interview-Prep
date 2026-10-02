---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lessons: [L11a, L11b]
environment: "Lima VM aos (Ubuntu 24.04 arm64)"
tags: [cs6210, cs6210/lab]
---

# lab-24-security: Protection in practice: capabilities, namespaces, seccomp, and an Andrew-style handshake

> [!info] Goal
> Make the protection mechanisms from L11a and the secure RPC concepts from L11b concrete by observing capabilities, isolating namespaces, applying a seccomp filter, and executing an Andrew-style handshake with nonces.

See [setup](../setup/README.md) for the VM.

## Prerequisites

- Lima VM `aos` running Ubuntu 24.04 arm64.
- `capsh` and `unshare` installed (part of standard util-linux and libcap2-bin).
- A C11 compiler to build the `seccomp_filter`.
- Python 3.12 with the `cryptography` library installed at `/opt/aos-venv/bin/python`.

## Run commands

To build and run all tests, executing the shell script, the seccomp C program, and the Python handshake:

```bash
make test
```

To regenerate the expected output file without failing on individual step errors, use:

```bash
../setup/run-in-vm.sh . capture
```

## What you should see

When running `make test`, you will see output for three distinct phases.
First, the namespace test drops you into a new user and mount namespace.
You will see your UID map to `root` (e.g., `uid=0(root)`) and the isolation of `/mnt/secret.txt`.
Second, the seccomp filter test confirms it can call `getpid()` (e.g., `PID: 13372`) but kills the process with SIGSYS (exit code 159) when `getuid()` is invoked.
Finally, the Andrew Secure RPC handshake completes a four-message exchange securely passing a session key and mitigating a simulated replay attack.

```
--- Testing Namespaces & Capabilities ---
...
Entering new user and mount namespace (sudo unshare -U -m -r)...
Inside namespace:
uid=0(root) gid=0(root) groups=0(root)
Mounting tmpfs to /mnt...
-rw-r--r-- 1 root root 14 Oct  1 20:39 /mnt/secret.txt
...
--- Testing Seccomp Filter ---
Setting up seccomp filter to deny getuid...
Calling getpid()...
PID: 13372
Calling getuid()... this should kill the process.
seccomp_filter successfully killed by SIGSYS (exit 159)
...
--- Testing Andrew Handshake ---
...
Attacker captures Msg 3 and replays it to Server...
Server rejects replayed message: Nonce mismatch.
Replay attack blocked.
PASS: lab-24-security
```

## How it works

1. **Capabilities and Namespaces**: Linux capabilities break up the monolithic root privileges.
We use `capsh --print` to inspect the current capabilities.
The `unshare -U -m -r` command creates new user and mount namespaces.
The `-r` flag maps the current user to root within the namespace, allowing administrative actions like mounting a `tmpfs` without affecting the host mount tree.
2. **Seccomp Filter**: Secure Computing mode (`seccomp`) allows a process to transition to a state where it can only make a restricted set of system calls.
Using raw BPF macros (`BPF_STMT` and `BPF_JUMP`), the filter reads the system call number and compares it against `SYS_getuid`.
If there is a match, it returns `SECCOMP_RET_KILL_PROCESS`, forcefully terminating the application.
3. **Andrew-style Handshake**: Implemented in Python, this models the Andrew Secure RPC protocol.
Nonces are used to prevent replay attacks by ensuring freshness.
The client generates a nonce and sends it.
The server responds with the incremented nonce and its own nonce, proving it is live and has the symmetric key.
The client then returns the incremented server nonce.
Finally, the server issues a session key.
A replay attack is foiled because the server forgets old nonces and only expects a valid response for its active session.

## Experiments

1. **Modify the seccomp filter**
   - **Prediction**: What happens if you change `SYS_getuid` to `SYS_write` in the filter, before the `printf` calls?
   - **Action**: Modify `seccomp_filter.c`, compile, and run it.
The process should be killed immediately when trying to print to stdout.
2. **Breach the namespace**
   - **Prediction**: If you omit the `-m` flag in the `unshare` command, will the `tmpfs` mount be visible outside the namespace?
   - **Action**: Change `unshare -U -m -r` to `unshare -U -r` in `caps_namespaces.sh` and observe if the host can see `/mnt/secret.txt`.
3. **Replay Message 2**
   - **Prediction**: What happens if the attacker captures message 2 and replays it to the client?
   - **Action**: Modify `andrew_handshake.py` to replay `msg2_ct` back to the client after the handshake.
The client should reject it since it is no longer waiting for a response to its initial nonce.

## Questions

<details>
<summary>Why must a process call <code>prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0)</code> before installing a seccomp filter?</summary>
This prevents the process or any of its children from gaining new privileges via <code>setuid</code> or <code>setgid</code> binaries.
Without this restriction, an attacker could load a malicious seccomp filter that subtly alters the behavior of a privileged program and then execute it.
</details>

<details>
<summary>How does the Andrew Secure RPC handshake prevent an attacker from reusing an old session key?</summary>
The handshake relies on nonces generated fresh for each interaction.
Because the final message containing the session key is tied to the current handshake's nonces, an attacker cannot replay a stale session key establishment message.
The client expects a specific response based on its newly generated nonce, making old messages invalid.
</details>

<details>
<summary>What is the primary advantage of user namespaces in containerization?</summary>
User namespaces allow a process to have root privileges inside the container while remaining an unprivileged user on the host system.
This minimizes the blast radius of a container breakout, as the escaping process will lack administrative rights on the underlying host kernel.
</details>
