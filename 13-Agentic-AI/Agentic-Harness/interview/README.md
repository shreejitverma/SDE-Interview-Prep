---
type: moc
track: [ai-eng, sde]
level:
status: draft
last_reviewed:
sources: [https://github.com/shreejitverma/agents, https://github.com/shreejitverma/fleet-ops, https://github.com/shreejitverma/dotfiles-nix, https://github.com/shreejitverma/firstmate]
---

# Agentic Harness - interview pack

Interview material for presenting the personal agentic harness in a full-stack SWE final round (Global Markets technology).
Every claim is backed by a `repo/path:line` cite, a commit hash, a PR number, or a read-only command run on 2026-09-23.
The component deep dives live in [components](../components/firstmate.md) and are the primary source for everything here.

| Note | Use it for |
| --- | --- |
| [Pitches](Pitches.md) | 30-second, 2-minute, and 10-minute spoken pitches around one 12-box whiteboard |
| [JD-Mapping](JD-Mapping.md) | Each JD requirement: evidence, how to say it, honest gap, bridge |
| [Question-Bank](Question-Bank.md) | 49 likely questions with model answers |
| [STAR-Stories](STAR-Stories.md) | Three evidenced incidents plus two backups |
| [Cheat-Sheet](Cheat-Sheet.md) | One page to read before walking in |

## The one rule for this pack

Most harness components are open-source tools written upstream (mostly by Kun Chen, `kunchenguid/*`) that I forked.
My own work is the integration, the policy, and the operations: the routing policy, the `agents` manual generator and guard hooks, the `dotfiles-nix` fork additions, `fleet-ops`, six firstmate fork patches, and running the whole thing daily.
Say this early and plainly; it is both true and a stronger signal of engineering judgment than claiming authorship.
