# Chapter 05 - Transports and Auth

## What you will master

- The stdio transport: framing, lifecycle, and why local-first won the early ecosystem.
- Streamable HTTP in depth: the single endpoint, POST and GET semantics, SSE streams, sessions, and resumability.
- The deprecated HTTP+SSE transport it replaced, and why it was replaced.
- OAuth 2.1 authorization for remote servers: roles, PKCE, server metadata, dynamic client registration, and resource indicators.
- Enterprise deployment patterns: gateways, identity brokering, and the stateless-scaling tension.

Spec details are stated as of early 2026 against revisions 2024-11-05, 2025-03-26, and 2025-06-18; the OAuth model described is the 2025-06-18 form unless noted.

## 1. Two transports, one protocol

MCP defines the message layer (Chapter 02) independently of how bytes move, and standardizes two transports: stdio and streamable HTTP.
Custom transports are permitted - anything that carries JSON-RPC bidirectionally can work, and WebSocket experiments exist - but interoperability in practice means the standard two.
The transport choice is also a trust and operations choice: stdio means "code running as the user on the user's machine," HTTP means "a service with real authentication, deployment, and multi-tenancy concerns."
Most of this chapter is about what that second clause costs.

## 2. stdio

The stdio transport runs the server as a subprocess of the host.
The client writes JSON-RPC messages to the server's stdin and reads them from its stdout, one JSON object per line, UTF-8, newline-delimited; embedded newlines inside a message are therefore forbidden.
stderr is out of band: hosts typically capture it for logs, and it is the only safe place for a stdio server to print anything that is not protocol traffic.
A single stray print to stdout - a debug statement, a library banner, a progress bar - corrupts framing and produces the classic symptom of a server that "connects then immediately dies"; this is the single most common bug in first MCP servers.

Lifecycle is process lifecycle.
The host spawns the process with a configured command, arguments, and environment; the session lasts until the host closes stdin and the process exits, with signal escalation (SIGTERM, then SIGKILL on platforms that have them) as the backstop.
Configuration convention across hosts is a JSON block naming command, args, and env, as popularized by Claude Desktop's claude_desktop_config.json; secrets ride in as environment variables, which is convenient and is also why local server configs must be treated as credential stores.

Why stdio mattered strategically: zero deployment.
No TLS, no auth server, no hosting, no CORS; write a script, point the host at it, and you have a working integration in minutes, with the OS user's own permissions.
This is the property that seeded thousands of servers in 2024-2025 before remote deployment was well specified.
The trade-offs are the mirror image: the server runs with full user privileges unless you sandbox it, every user must install and update it locally, one instance serves one host, and there is no authorization layer at all - the act of configuring the server is the authorization.
stdio remains the right default for personal and developer tooling, and the wrong answer for anything multi-user or centrally governed.

A concrete session on the wire, so the framing is unambiguous.

```
-> {"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2025-06-18","capabilities":{"roots":{"listChanged":true}},"clientInfo":{"name":"host","version":"1.0"}}}
<- {"jsonrpc":"2.0","id":1,"result":{"protocolVersion":"2025-06-18","capabilities":{"tools":{"listChanged":true}},"serverInfo":{"name":"issue-tracker","version":"0.1.0"}}}
-> {"jsonrpc":"2.0","method":"notifications/initialized"}
-> {"jsonrpc":"2.0","id":2,"method":"tools/list","params":{}}
<- {"jsonrpc":"2.0","id":2,"result":{"tools":[...]}}
```

Each arrow is one line on the pipe, with no length prefix and no content-type header.
This differs from LSP, which uses HTTP-style Content-Length framing over the same pipes; MCP chose newline delimitation for simplicity, and the price is the forbidden-newline rule and the stdout fragility above.
If your server's JSON serializer pretty-prints by default, you have a bug that will not show up until a value happens to be long enough to wrap.

Three stdio-specific security points that get skipped in tutorials.
Environment-variable secrets are visible to anything that can read the process environment on the machine, and host config files containing them are frequently synced to cloud storage or committed by accident; prefer a credential helper or an OS keychain lookup at startup where the host permits it.
The host chooses the command line, so anyone who can write the host's config file achieves arbitrary code execution as the user - which makes those config files a high-value target and a legitimate item for endpoint management to monitor.
And the server inherits the user's ambient authority: filesystem, SSH agent, cloud CLI credentials, browser cookies on disk.
Nothing in the protocol constrains any of that, which is the practical argument for containerized stdio servers in Chapter 07.

## 3. Streamable HTTP

Streamable HTTP, introduced in revision 2025-03-26, is the remote transport, and its design is best understood through the transport it replaced.

The original remote transport (2024-11-05) was HTTP+SSE: the client opened a long-lived GET to an SSE endpoint, the server pushed all its messages down that stream, and the client sent its messages as POSTs to a separate endpoint the server announced over the stream.
It worked, and it had three structural problems.
The permanent open connection made stateless and serverless deployment awkward, since every session pinned a stream to a live process.
A dropped stream lost messages with no recovery mechanism.
And the two-endpoint design complicated infrastructure - proxies, load balancers, auth middleware - that wants one resource to reason about.
Revision 2025-03-26 deprecated it; as of early 2026 you still meet it in older servers, and SDK clients commonly implement fallback: try streamable HTTP, and on failure retry in the legacy mode.

Streamable HTTP collapses everything onto a single endpoint, conventionally /mcp, used with three HTTP methods.

POST carries every client-to-server message.
For notifications and responses, the server answers 202 Accepted with no body.
For requests, the server chooses its response mode per request: either a plain application/json body with the single response, or a text/event-stream body - an SSE stream - on which it can send progress notifications, its own server-to-client requests (sampling, elicitation), and finally the response, then close the stream.
The client advertises both content types in Accept, and this per-request choice is the feature that gives the transport its name: a simple tool call can be one cheap request-response, while a slow tool call can stream progress, without any standing connection.

GET on the same endpoint opens an optional standing SSE stream for server-initiated traffic that is not tied to any in-flight request: list_changed notifications, resource update notifications, unsolicited server requests.
Servers may respond 405 Method Not Allowed if they never initiate anything, which is a perfectly valid minimal implementation.

DELETE terminates the session explicitly, where sessions exist.

The exchange for a single streamed tool call looks like this, with headers that matter shown and the rest elided.

```
POST /mcp HTTP/1.1
Accept: application/json, text/event-stream
Content-Type: application/json
Mcp-Session-Id: 8f2c...e91
MCP-Protocol-Version: 2025-06-18
Authorization: Bearer eyJhbGciOi...

{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"export","arguments":{"scope":"all"},"_meta":{"progressToken":"p4"}}}

HTTP/1.1 200 OK
Content-Type: text/event-stream

id: 1
data: {"jsonrpc":"2.0","method":"notifications/progress","params":{"progressToken":"p4","progress":40,"total":100}}

id: 2
data: {"jsonrpc":"2.0","id":4,"result":{"content":[{"type":"text","text":"exported 100 records"}]}}
```

Read that carefully, because four chapters converge in it: the progress token from Chapter 04 rides in _meta, the session and version headers come from this chapter, the bearer token from section 4, and the result shape from Chapter 03.
Note that the SSE event ids are per-stream sequence numbers, which is what makes Last-Event-ID replay well defined, and that the server closes the stream after the response - the stream exists for this request only.

Backwards compatibility deserves a concrete recipe, because you will need it for years.
A client wanting to support both transports POSTs an initialize request to the server URL; a 200 with either content type means streamable HTTP, while a 404 or 405 means the server is probably legacy, so the client falls back to opening a GET expecting a text/event-stream with an endpoint event naming the POST URL.
A server wanting to support both keeps the legacy SSE and POST endpoints alive alongside /mcp, which costs little and is what several first-party remote servers did through 2025.
Both SDKs implement the client half of this; know that it exists so you can read the extra round trip in your logs and not mistake it for a bug.

Sessions are the transport's answer to MCP's statefulness meeting HTTP's statelessness.
A server that wants a session returns an Mcp-Session-Id header on the initialize response; the client must then echo that header on every subsequent request in the session, the server may expire sessions and answer 404 to force re-initialization, and the client may DELETE to end cleanly.
Session ids must be cryptographically unguessable, must not encode anything sensitive, and - critically - are session identity, not user identity: authorization comes from the OAuth layer below, and a server that treats possession of a session id as authentication has built a bearer-token system with none of the protections.
Revision 2025-06-18 additionally requires the negotiated MCP-Protocol-Version as a header on post-initialize requests, as Chapter 02 covered, because the server cannot otherwise map a bare HTTP request to handshake state.

Resumability is built on SSE mechanics.
A server may attach an id to each SSE event; after a dropped connection, the client reconnects with the standard Last-Event-ID header, and the server replays messages the client missed from that stream.
This closes the message-loss hole that HTTP+SSE had, at the cost of the server buffering recent per-stream messages; how much to buffer is an implementation decision with a direct memory cost, and SDKs expose it as an event-store abstraction (the in-memory reference implementations are explicitly not production-grade).

Deployment consequences, stated plainly.
Stateless-friendly: a server that declines sessions and never initiates messages is nearly a plain HTTP API and scales like one - any replica can answer any request, serverless works, and you give up subscriptions, server-initiated sampling, and resumability.
Stateful: sessions plus standing streams give you the full protocol and re-create the classic sticky-routing problem - you need session affinity at the load balancer or a shared backing store (the Redis-backed event store pattern) so any replica can serve any session.
This tension is not a flaw to engineer away; it is a dial, and you should decide where your server sits on it before writing code, because retrofitting statefulness onto a serverless deployment is far harder than the reverse.

Transport security basics predate any OAuth discussion: HTTPS is mandatory for non-localhost, and servers must validate the Origin header on incoming requests and bind development servers to 127.0.0.1 rather than 0.0.0.0, because a browser page can otherwise POST to a localhost MCP server - the DNS-rebinding and drive-by-localhost class of attack that hit several local MCP tools in 2025 (the mcp-remote and MCP Inspector CVEs among them).

A short comparison to fix the choice in mind.

| Property | stdio | Streamable HTTP, stateless | Streamable HTTP, stateful |
|---|---|---|---|
| Deployment | none, subprocess | any HTTP host, serverless fine | needs affinity or shared store |
| Authorization | ambient user authority | OAuth 2.1 bearer | OAuth 2.1 bearer |
| Multi-user | no, one process per user | yes | yes |
| Server-initiated messages | yes, free | no | yes, via GET stream |
| Subscriptions and sampling | yes | no | yes |
| Resumability | not applicable | not applicable | yes, Last-Event-ID |
| Main risk | full user privileges | token handling | token handling plus session state |

The row that decides most designs is the third: the moment more than one human uses the server, stdio is off the table, and everything in section 4 becomes mandatory rather than optional.

## 4. Authorization: OAuth 2.1 for remote servers

stdio servers inherit the user's local authority and use environment-variable credentials; the spec's authorization framework applies to HTTP transports.
The framework arrived in revision 2025-03-26 and was significantly restructured in 2025-06-18; learn the 2025-06-18 shape, because it is the one the ecosystem converged on.

The 2025-03-26 version made the MCP server both authorization server and resource server: it issued its own tokens, hosted its own OAuth endpoints, and in practice pushed every server author into the identity business, which nobody wanted and few did safely.
Revision 2025-06-18 reclassified the MCP server as a pure OAuth 2.1 resource server: it accepts and validates access tokens, and token issuance belongs to a separate authorization server - your corporate IdP, Auth0, Okta, or a purpose-built broker.
This separation is the single most important fact in the section: MCP servers verify tokens; they should not mint them.

The discovery chain, end to end, for a client meeting a protected server cold.

The client calls the server without a token and receives 401 Unauthorized with a WWW-Authenticate header pointing at protected resource metadata.
The client fetches that metadata - RFC 9728, at a well-known URI such as /.well-known/oauth-protected-resource - which names the resource identifier and the authorization servers that protect it.
The client fetches the authorization server's own metadata - RFC 8414, /.well-known/oauth-authorization-server - learning the authorization, token, and registration endpoints and supported grant types.
If the client is not yet registered, it may use dynamic client registration - RFC 7591 - to POST its metadata and receive a client id on the spot.
The client then runs the OAuth 2.1 authorization code flow with PKCE: browser redirect, user authenticates and consents at the authorization server, code returns to the client's redirect URI, client exchanges code plus PKCE verifier for tokens.
The client retries the MCP request with Authorization: Bearer token, and the server validates the token on every request - signature or introspection, expiry, and audience.

Each piece exists for a stated reason; know the reasons, not just the acronyms.
OAuth 2.1 rather than 2.0 consolidates hard-won practice into requirements: PKCE mandatory for all clients, the implicit and password grants gone, exact redirect URI matching.
PKCE matters here specifically because MCP clients are public clients - desktop apps and CLIs that cannot hold a client secret - and PKCE is what makes the code flow safe without one.
Dynamic client registration matters because the client-server pairing is open-world: any host may meet any server for the first time at runtime, and pre-registering every host at every authorization server does not scale; DCR trades an admission-control point away for zero-friction first contact, and enterprises often disable it and pre-register known clients precisely to get that control back.

Resource indicators - RFC 8707, made mandatory in 2025-06-18 - deserve their own paragraph because they encode a real attack.
The client must include a resource parameter naming the specific MCP server in both the authorization and token requests, and the authorization server binds the token's audience to it; the server must reject tokens whose audience is not itself.
Without audience binding, a token issued for server A works at server B, and a malicious server that receives a broadly scoped token can replay it against other services - the token passthrough and confused deputy family.
The same revision therefore also forbids token passthrough outright: an MCP server must never forward the client's inbound token upstream; if it needs to call an upstream API, it obtains its own token for that API (token exchange, or a separate client credential) and maintains the two trust domains separately.
The cost of all this correctness is real: the full chain is many round trips, DCR plus per-resource consent produces consent fatigue that users click through, and 2025 ecosystem experience showed most server authors reaching for hosted auth providers or gateways rather than implementing the chain themselves - which is the sane outcome the 2025-06-18 restructuring was designed to permit.

The chain in concrete HTTP, abbreviated but faithful in shape.

```
POST /mcp                                   -> 401 Unauthorized
   WWW-Authenticate: Bearer resource_metadata="https://srv.example/.well-known/oauth-protected-resource"

GET /.well-known/oauth-protected-resource   -> 200
   {"resource":"https://srv.example/mcp","authorization_servers":["https://idp.example"]}

GET https://idp.example/.well-known/oauth-authorization-server -> 200
   {"authorization_endpoint":"...","token_endpoint":"...","registration_endpoint":"...",
    "code_challenge_methods_supported":["S256"]}

POST https://idp.example/register            -> 201  (dynamic client registration, if allowed)
   {"client_id":"c_9f31","redirect_uris":["http://127.0.0.1:33418/callback"]}

browser -> https://idp.example/authorize?response_type=code&client_id=c_9f31
           &code_challenge=...&code_challenge_method=S256
           &resource=https%3A%2F%2Fsrv.example%2Fmcp&redirect_uri=...

POST https://idp.example/token               -> 200
   grant_type=authorization_code&code=...&code_verifier=...&resource=https%3A%2F%2Fsrv.example%2Fmcp

POST /mcp  Authorization: Bearer ...          -> 200
```

The resource parameter appearing in both the authorize and token requests is the RFC 8707 binding; drop it and the whole audience protection silently disappears, because everything still works.
That is the general hazard of this layer: nearly every correctness rule here fails open, so absence of errors is not evidence of correctness, and the only way to know is to test negatively - present a token minted for another resource and confirm your server rejects it.

Server-side validation checklist, in the order a request should be checked.
Is there a bearer token at all, and is it in the Authorization header rather than a query parameter (query-string tokens leak into logs and referrers, and the spec forbids them)?
Does the signature verify against the authorization server's published keys, or does introspection succeed?
Is it unexpired, and is the issuer the one your metadata advertises?
Is the audience exactly this server's resource identifier?
Do the scopes cover the specific method and tool being invoked, checked per call rather than once per session?
And is the acting user's identity - not the session id - what your authorization decision and audit log record?
A server that stops after step two has implemented authentication and skipped authorization, which is the most common remote-server defect found in 2025 reviews.

Two practical notes on client-side flows.
Local clients need a redirect URI, and the convention is a loopback listener on an ephemeral port (http://127.0.0.1:PORT/callback), which OAuth 2.1 permits for native apps specifically because it avoids custom URI schemes that other local apps can hijack.
Refresh tokens must be stored in an OS keychain or equivalent rather than a plaintext config file, and rotating refresh tokens (issued fresh on every use, with reuse detection) are the recommended default because a stolen static refresh token is indefinite access.

## 5. Enterprise deployment patterns

Enterprises deploying MCP at scale in 2025-2026 converged on a small set of patterns; each answers a governance question the raw protocol leaves open.

The MCP gateway: a reverse proxy speaking MCP on both faces, sitting between all hosts and all servers.
It centralizes authentication against the corporate IdP, authorization policy per user per server per tool, audit logging of every call, rate limiting, and often catalog control - only gateway-registered servers are reachable at all.
The trade-offs are the classic ones for any chokepoint: a single point of failure and added latency, plus the gateway must track protocol revisions faithfully or it becomes the ecosystem's compatibility bottleneck; Chapter 07 returns to gateways as a security control.

Identity brokering: hosts authenticate users to the gateway with corporate SSO; the gateway holds or exchanges per-upstream credentials, so end users never handle API keys for the systems behind their tools, and offboarding a user at the IdP severs every MCP capability at once.
This pattern is why the resource-server restructuring mattered: it made "IdP issues tokens, servers verify them" the spec-blessed shape rather than a workaround.

Placement tiers, in increasing order of operational weight: local stdio servers under MDM-managed configuration for developer tooling; centrally hosted streamable HTTP servers inside the VPN for shared internal systems; and vendor-hosted remote servers (GitHub's, Atlassian's, and similar first-party servers, which became common through 2025) consumed across the internet with OAuth.
Most real estates run all three tiers simultaneously, and the honest operational summary is that the transport is the easy part; the register-of-record questions - who may add a server, who reviews its tools, who sees the audit log - are organizational, and the gateway is simply the place where their answers get enforced.

Operational specifics that separate a working deployment from a demo.
Timeouts: agents make long tool calls, so proxies and load balancers with default 30- or 60-second idle timeouts will sever SSE streams mid-call; raise idle timeouts and disable response buffering on the path, because a buffering proxy converts a streaming response into a single delayed blob and silently defeats progress reporting.
Session storage: if you run stateful replicas, decide early between sticky routing (simple, but rebalancing and deploys drop sessions) and externalized session plus event state (survives deploys, adds a dependency and serialization cost).
Deploys: rolling a stateful MCP fleet terminates sessions, and clients must be able to re-initialize transparently on a 404; test this deliberately, because a client that surfaces a hard error on session expiry makes every deploy user-visible.
Observability: log per call the server name, tool name, user identity, latency, result size in tokens, and error class, since the last two feed directly into the context-economics work in Chapter 07.
Capacity: tool calls are usually I/O-bound and long-tailed, so size for concurrency rather than throughput, and put per-user rate limits at the gateway because a looping agent will otherwise find your slowest tool and call it a thousand times.

Finally, the residual risk this chapter cannot fix.
Transport security and OAuth answer "who is calling and may they call," and they say nothing about whether the model should be making that call at all - the model may be acting on injected instructions with a perfectly valid token, and every layer here will approve it.
That gap is the entire subject of Chapter 07, and it is why the authorization work in this chapter is necessary but never sufficient.

## Exercises

1. Write a minimal stdio framing layer in Python - read newline-delimited JSON from stdin, write responses to stdout, log to stderr - and demonstrate the corruption bug by adding one print statement, capturing what a real client reports.
2. Trace a tools/call over streamable HTTP twice, listing every HTTP request, method, header, and body: once against a stateless server answering plain JSON, once against a stateful server that streams two progress notifications first; include Mcp-Session-Id and MCP-Protocol-Version where required.
3. A streamable HTTP client's SSE stream drops mid-tool-call; write the reconnection sequence with Last-Event-ID and state exactly what the server must have retained for replay to work, and what memory bound you would put on it.
4. Diagram the full cold-start authorization chain - 401, RFC 9728, RFC 8414, RFC 7591, PKCE code flow, bearer retry - and mark which steps disappear when the enterprise pre-registers clients and disables DCR.
5. Explain the attack that RFC 8707 resource indicators prevent, with a concrete two-server scenario, and then explain separately why token passthrough is forbidden even with audience-bound tokens.
6. Design the deployment for an internal "customer data" MCP server for 500 employees: choose transport, session mode, token issuer, and gateway placement; name the failure mode of each choice you rejected.
7. Audit one real local MCP server you use for the localhost-binding and Origin-validation issues; report what you find and fix what you can.
8. Implement the six-step server-side token validation checklist against a test IdP, then verify negatively: present an expired token, a token for a different resource, and a token with insufficient scope, confirming three distinct rejections.
9. Deliberately break a stateful deployment: kill the replica holding a session mid-call and record what the client does, then implement transparent re-initialization on 404 and repeat.
10. Configure a proxy in front of a streaming MCP server with a 30-second idle timeout and response buffering enabled, observe both failure modes, and write the exact configuration lines that fix each.

## Godhood check

You have mastered this chapter when you can do the following without notes.

- Specify stdio framing exactly, including the stderr rule, and diagnose the connects-then-dies symptom on sight.
- Explain the three defects of HTTP+SSE and how streamable HTTP's single endpoint, per-request response modes, and Last-Event-ID resumability address each.
- Describe session establishment, the Mcp-Session-Id and MCP-Protocol-Version headers, and the stateless-versus-stateful deployment dial with its scaling consequences.
- Walk the 2025-06-18 authorization chain end to end, naming what RFCs 9728, 8414, 7591, and 8707 each contribute and why PKCE is mandatory.
- State why the MCP server is a resource server and not an authorization server, what the 2025-03-26 design got wrong, and why token passthrough is banned.
- Sketch the enterprise gateway and identity-brokering patterns and argue their trade-offs as chokepoints.
- Recite the six-step token validation order and name the step most servers omit.
- List the four operational hazards - proxy idle timeouts, response buffering, session loss on deploy, unbounded agent retry - and the fix for each.
- State precisely what authorization does not protect against, and hand off correctly to Chapter 07.
