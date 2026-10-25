---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Tmux Architecture and Session Persistence

> [!summary] Scope
> Owns terminal multiplexing architecture based on tmux.
> Explains client-server UNIX domain socket communication, session, window, and pane PTY hierarchy, and environment variable propagation (update-environment, stale SSH_AUTH_SOCK refresh).
> Details headless automation via send-keys, capture-pane, wait-for, and control mode (-C).
> Reaches Use through Explain, handing underlying pty kernel mechanics to 00-Program-Execution/TTYs-PTYs-and-Line-Discipline.md.

## Related

- Prerequisites: [[TTYs-PTYs-and-Line-Discipline]]
- Next: [[SSH-In-Depth-Keys-Forwarding-Certificates-and-FIDO2]]
- See also: [[19-Shell-and-Unix-Engineering/02-Terminals-and-Job-Control/Terminal-Multiplexers-tmux-and-screen|Terminal Multiplexers tmux and screen]]
- Lab: `code/01-Command-Line/tmux_control_script/`
