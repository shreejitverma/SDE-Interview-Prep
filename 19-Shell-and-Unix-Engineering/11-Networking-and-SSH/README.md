---
type: moc
track: [sde, distinguished, cloud-ai]
level:
status: draft
last_reviewed:
sources: []
---

# Networking and SSH from the Shell

The shell is the debugger for networks and the transport for remote automation.
These notes cover socket and DNS tools, TLS debugging, packet capture, and SSH in depth.

Part of [Shell and Unix Engineering](../README.md).

## Learning order

Read in this order; each note links its prerequisite and the next note.

1. [ip and ss](ip-and-ss.md)
2. [DNS from the Shell: dig and getent](DNS-from-the-Shell-dig-and-getent.md)
3. [openssl s_client and TLS Debugging](openssl-s_client-and-TLS-Debugging.md)
4. [netcat and socat](netcat-and-socat.md)
5. [tcpdump and tshark](tcpdump-and-tshark.md)
6. [SSH Protocol and Keys](SSH-Protocol-and-Keys.md)
7. [ssh-agent and Forwarding](ssh-agent-and-Forwarding.md)
8. [SSH Config and ProxyJump](SSH-Config-and-ProxyJump.md)
9. [SSH Multiplexing with ControlMaster](SSH-Multiplexing-ControlMaster.md)
10. [SSH Port Forwarding and Tunnels](SSH-Port-Forwarding-and-Tunnels.md)
11. [SSH Certificates](SSH-Certificates.md)
12. [SSH Remote Command Quoting](SSH-Remote-Command-Quoting.md)
13. [Fleet Fan-Out: pssh and ansible Ad Hoc](Fleet-Fan-Out-pssh-and-ansible-ad-hoc.md)
