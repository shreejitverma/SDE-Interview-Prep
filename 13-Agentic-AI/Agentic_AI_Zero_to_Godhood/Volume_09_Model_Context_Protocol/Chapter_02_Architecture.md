# Chapter 02 - Architecture

## What you will master

- The three-role architecture: hosts, clients, servers, and why the client is a distinct concept from the host.
- The one-client-per-server rule and its consequences for isolation and security.
- JSON-RPC 2.0 as the message layer: requests, results, errors, and notifications, with exact wire formats.
- The initialization lifecycle: version negotiation, capability negotiation, and the initialized notification.
- Date-based spec versioning, how a version is negotiated, and how it is pinned on HTTP transports.

Spec details are stated as of early 2026 against revisions 2024-11-05, 2025-03-26, and 2025-06-18.

## 1. Hosts, clients, and servers

MCP defines three roles, and the distinction between the first two is the part people get wrong.

The host is the AI application the user actually runs: Claude Desktop, Claude Code, Cursor, VS Code, ChatGPT, or your own agent harness.
The host owns everything user-facing and model-facing: the model connection, the conversation, consent prompts, permission policy, and the decision about which context from which server reaches the model.

The client is a protocol component inside the host that maintains a stateful connection to exactly one server.
It speaks JSON-RPC to that server, tracks the negotiated capabilities of that one connection, and mediates every message in both directions.

The server is a program that exposes capabilities: tools, resources, and prompts (Chapter 03).
A server can be a subprocess on the user's machine speaking stdio, or a remote HTTPS service; the primitives are identical either way.
Servers are intended to be small and focused: one system-of-record, one server, is the design intent, even though the ecosystem contains plenty of kitchen-sink servers.

The relationship is strictly one client per server connection.
A host that connects to five servers instantiates five clients, each holding one isolated session.
This rule is not bureaucratic; it carries the architecture's security and coherence properties.

Isolation: servers cannot see each other.
Server A never learns that server B exists, never sees B's tools, and never sees the conversation except the specific arguments and results that flow through its own connection.
The host is the only component with the full picture, which makes the host the natural enforcement point for permissions and the natural place to implement cross-server policy.

Independent negotiation: each client-server pair negotiates its own protocol version and capabilities, so one outdated server does not constrain what the host can do with the others.

Fault containment: a crashed or hung server takes down one client session, and the host can restart it without disturbing the rest.

The downside of the hub-and-spoke shape is that all composition happens in the host and therefore in the model's context.
If tool output from server A must reach server B, it travels through the model's context window by default, paying tokens both ways; this cost is the root motivation for the code-mode pattern in Chapter 07.

## 2. JSON-RPC 2.0 as the message layer

MCP messages are JSON-RPC 2.0, a deliberately boring choice.
JSON-RPC is transport-agnostic, human-readable, trivially parseable in every language, and proven at scale by LSP.
The cost of the choice is verbosity on the wire and no built-in binary payload support; binary content travels base64-encoded, which inflates size by roughly a third.

There are three message shapes you must know cold.

A request carries an id and expects exactly one response.

```json
{
  "jsonrpc": "2.0",
  "id": 7,
  "method": "tools/call",
  "params": {
    "name": "get_weather",
    "arguments": { "city": "Pune" }
  }
}
```

The id must be a string or number, must not be null, and must not be reused by the same sender within a session; both sides can issue requests, so ids are scoped per direction.

A successful response echoes the id and carries a result.

```json
{
  "jsonrpc": "2.0",
  "id": 7,
  "result": {
    "content": [ { "type": "text", "text": "31 C, clear" } ],
    "isError": false
  }
}
```

An error response echoes the id and carries an error object with a numeric code, a message, and optional data.

```json
{
  "jsonrpc": "2.0",
  "id": 7,
  "error": {
    "code": -32602,
    "message": "Unknown tool: get_wether",
    "data": { "suggestion": "get_weather" }
  }
}
```

The standard JSON-RPC codes apply: -32700 parse error, -32600 invalid request, -32601 method not found, -32602 invalid params, -32603 internal error.
Implementations may use codes above -32000 for application-defined errors.
A critical distinction lives here: a protocol error (the error object) means the machinery failed, while a tool execution failure is reported inside a successful result with isError set to true, so the model can see the failure text and react.
Confusing these two channels is a classic server bug: raising a protocol error for "file not found" hides the failure from the model, which then cannot recover.

A notification has no id and expects no response.

```json
{
  "jsonrpc": "2.0",
  "method": "notifications/resources/updated",
  "params": { "uri": "file:///logs/app.log" }
}
```

Notifications are fire-and-forget by construction; if delivery matters, the message should have been a request.
MCP uses notifications for lifecycle signals (initialized, cancelled), change events (list_changed for tools, resources, prompts, roots), progress updates, and log messages.

One historical note for reading old code: revision 2025-03-26 permitted JSON-RPC batch arrays, and revision 2025-06-18 removed batching entirely because it complicated streaming semantics for negligible benefit.
Current implementations send one message per JSON object.

The protocol is bidirectional and asymmetric.
Both sides send requests, but the method surface differs by direction: clients call server methods such as tools/call and resources/read, while servers call client methods such as sampling/createMessage, roots/list, and elicitation/create.
Requests can be in flight in both directions simultaneously, so both sides must be written as concurrent message routers, not as simple call-and-wait loops.

## 3. The initialization lifecycle

Every MCP session passes through three phases: initialize, operate, shutdown.
The handshake is one request-response pair plus one notification, and everything else in the protocol depends on what it establishes.

The client opens with an initialize request.

```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "initialize",
  "params": {
    "protocolVersion": "2025-06-18",
    "capabilities": {
      "roots": { "listChanged": true },
      "sampling": {},
      "elicitation": {}
    },
    "clientInfo": { "name": "my-host", "version": "1.4.0" }
  }
}
```

The server responds with its own version choice, capabilities, identity, and optional instructions.

```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "result": {
    "protocolVersion": "2025-06-18",
    "capabilities": {
      "tools": { "listChanged": true },
      "resources": { "subscribe": true, "listChanged": true },
      "prompts": { "listChanged": false },
      "logging": {},
      "completions": {}
    },
    "serverInfo": { "name": "weather-server", "version": "0.3.1" },
    "instructions": "Prefer get_forecast for future dates; get_weather is current conditions only."
  }
}
```

The client then sends notifications/initialized, and only after that may normal operation begin.
Before the handshake completes, the only traffic permitted is the initialize exchange itself plus ping.

Version negotiation is deliberately simple.
The client proposes the latest revision it supports.
If the server supports it, it echoes it back; otherwise it responds with the latest revision it does support.
If the client cannot accept the server's counter-offer, the client disconnects; there is no multi-round bargaining.
The simplicity trades away fine-grained compromise for implementations that are hard to get wrong.

Capability negotiation is declarative, not bargained.
Each side states what it implements, and each side must only use features the other declared.
A client must not call resources/subscribe if the server did not declare resources with subscribe true.
A server must not call sampling/createMessage if the client did not declare sampling.
Sub-capabilities refine the declaration: listChanged on a primitive means "I will send change notifications for this list," and subscribe on resources means per-resource update subscriptions are available.
The design consequence is graceful degradation: a server that wants sampling must be written to work, worse but correctly, when the client does not offer it, and this optionality is exactly why advanced features spread slowly through the ecosystem (Chapter 04).

The instructions field deserves attention because it is the server's one chance to inject usage guidance at session scope.
Hosts typically append it to the system prompt; keep it short, factual, and free of anything you would not want a security reviewer to read, because it is injected model-facing text and Chapter 07 will treat it as an attack surface.

Shutdown is transport-level, not protocol-level: there is no shutdown method.
On stdio, the client closes the server's stdin and waits for exit, escalating to signals if needed.
On HTTP, the session ends by explicit DELETE of the session or by expiry.

## 4. The operate phase: what flows where

During operation the message flow falls into a small number of patterns, and naming them makes the rest of the volume easier to hold.

Discovery: the client calls tools/list, resources/list, and prompts/get style methods, usually at startup and again on each list_changed notification, caching results in between.
Invocation: the client calls tools/call or resources/read on behalf of the model or the user.
Server-initiated work: the server calls roots/list, sampling/createMessage, or elicitation/create, each of which the client mediates and may refuse.
Utilities: ping in either direction for liveness, progress notifications tied to a progressToken supplied in a request's _meta field, cancellation via notifications/cancelled, and log messages via notifications/message after the client sets a level with logging/setLevel.

The _meta field appears throughout the protocol: requests, results, and most data objects can carry a _meta object for metadata that does not affect semantics, and the spec reserves prefixed keys for its own use.
Progress tokens and trace-propagation experiments live here; treat _meta as the protocol's extension escape hatch and do not overload it with application data the model needs to see.

A point about statefulness that shapes deployment: an MCP session is stateful by design.
Negotiated versions and capabilities, subscriptions, and in-flight requests all live in the session.
This is natural on stdio, where the session is the process lifetime, and it is the awkward part of remote deployment, where load balancers and serverless platforms prefer stateless requests; Chapter 05 covers how streamable HTTP sessions and resumability address this, and what it costs operationally.

## 5. Versioning by dated revisions

MCP versions the specification with dates, in the form YYYY-MM-DD, rather than semantic versions.
A revision string names the complete state of the spec on that date; the revisions you must know are 2024-11-05, 2025-03-26, and 2025-06-18, with a further revision in November 2025 whose additions (asynchronous tasks, an extension mechanism) were still bedding in as of early 2026.

Why dates instead of semver?
Semver encodes a compatibility promise (major breaks, minor adds) that a fast-moving multi-party spec cannot honestly make; dated revisions promise nothing except identity, and the negotiation handshake handles compatibility explicitly at runtime.
The downside is that a date communicates nothing about the size of the change: 2025-06-18 was a large revision and its date looks no different from a trivial one.
Implementers must read changelogs; there is no shortcut encoded in the version string.

Two mechanics matter in practice.
First, negotiation happens in initialize as described above, and SDKs typically support a range of revisions simultaneously, translating internally.
Second, on HTTP transports, revision 2025-06-18 added a requirement that after initialization the client sends the negotiated version on every request in an MCP-Protocol-Version header, because an HTTP server cannot otherwise associate a bare request with the handshake that preceded it; absence of the header defaults to 2025-03-26 behavior for backward compatibility.

Changes within the lifetime of a revision are limited to clarifications; behavior changes get a new date.
For an implementer the operational rule is: pin your SDK, read the changelog before upgrading, and test against at least one older-revision peer, because the ecosystem's long tail updates slowly and your server will meet 2025-03-26 clients for years.

## 6. Architectural judgments to carry forward

Three judgments summarize this chapter's design analysis.

The host-centric trust model is the architecture's best property: exactly one component sees everything, so policy, consent, and audit have a single home; any deployment pattern that blurs the host's mediating role (for example, servers calling each other directly out of band) forfeits the model's core guarantee.

JSON-RPC's blandness is a feature: the interesting parts of MCP are the primitives and the negotiation, and putting them on a boring message layer means every language had a working transport on day one; the token and byte overhead of JSON is the accepted price.

Stateful sessions are the architecture's most contested choice: they make subscriptions, sampling, and progress natural, and they make horizontal scaling of remote servers genuinely harder than a stateless REST API; you should be able to defend both halves of that sentence in a design review.

## Exercises

1. Write, by hand and without an SDK, the full JSON for an initialize request, its response, and the initialized notification for a client supporting sampling and roots connecting to a server supporting tools and subscribable resources.
2. Implement a minimal message router in Python that reads newline-delimited JSON-RPC from stdin, answers ping, responds to initialize correctly, and returns -32601 for everything else; verify it against a real client or the MCP Inspector.
3. A server declares resources with subscribe false and listChanged true; enumerate exactly which resource-related messages are legal on that session, in each direction.
4. Explain to a colleague why "file not found" from a tool must be an isError result rather than a JSON-RPC error, and what the model experiences in each case.
5. Diagram the clients a host must instantiate for a setup with three servers, and annotate which component would enforce a rule like "results from the web-search server may never be passed as arguments to the shell server."
6. Read the 2025-06-18 changelog entry on the MCP-Protocol-Version header and write down the exact failure scenario on a stateless HTTP deployment that the header prevents.
7. Argue for and against dated revisions versus semver for a protocol with independent client and server release cycles; commit to a position.

## Godhood check

You have mastered this chapter when you can do the following without notes.

- Draw the host-client-server diagram for a multi-server host and state the one-client-per-server rule and all three properties it buys.
- Write syntactically valid JSON-RPC for a request, a success result, an error, and a notification, and name the five standard error codes.
- Walk through the initialization handshake message by message, including version negotiation fallback and the point at which normal traffic becomes legal.
- Explain capability negotiation as declaration rather than bargaining, and give one client-side and one server-side example of a capability that gates a method.
- State why sessions are stateful, what that enables, and what it costs at deployment time.
- Name the three core dated revisions with their headline changes, and explain the MCP-Protocol-Version header's purpose on HTTP.
