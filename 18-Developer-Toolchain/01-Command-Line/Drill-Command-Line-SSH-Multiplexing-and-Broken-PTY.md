---
type: problem
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Drill: SSH Multiplexing and Broken PTY

> [!summary] Scope
> Blind problem drill presenting a broken remote developer session with a hung ControlMaster master socket, a stale SSH_AUTH_SOCK within a reattached tmux session, and a corrupted raw-mode terminal displaying control garbage and suppressed newlines.
> Challenges the engineer to diagnose the master socket lockup, fix the agent socket symlink, and recover terminal line discipline without terminating the remote job.
> Reaches Debug, testing SSH-In-Depth-Keys-Forwarding-Certificates-and-FIDO2.md and Tmux-Architecture-and-Session-Persistence.md.

## Related

- Prerequisites: [[SSH-In-Depth-Keys-Forwarding-Certificates-and-FIDO2]], [[Tmux-Architecture-and-Session-Persistence]], [[TTYs-PTYs-and-Line-Discipline]]
- See also: [[19-Shell-and-Unix-Engineering/11-Networking-and-SSH/SSH-Multiplexing-ControlMaster|SSH Multiplexing ControlMaster]], [[19-Shell-and-Unix-Engineering/02-Terminals-and-Job-Control/termios-and-stty|termios and stty]]
- Lab: `code/01-Command-Line/drill_ssh_mux_pty/`
