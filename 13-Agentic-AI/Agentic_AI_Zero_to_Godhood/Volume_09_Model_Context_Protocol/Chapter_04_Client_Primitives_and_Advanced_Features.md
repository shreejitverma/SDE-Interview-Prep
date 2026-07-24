# Chapter 04 - Client Primitives and Advanced Features

## What you will master

- The three client primitives - sampling, roots, elicitation - where the server asks and the client answers.
- Sampling in depth: message flow, model preferences, human-in-the-loop points, and why it is architecturally important.
- Why sampling support lagged in real hosts, and what server authors do about it.
- Roots as workspace scoping, and elicitation as structured mid-operation user input.
- The utility layer: progress, cancellation, logging, and ping.

Spec details are stated as of early 2026 against revisions 2024-11-05, 2025-03-26, and 2025-06-18; elicitation exists only from 2025-06-18.

## 1. The direction reversal

Chapter 03 covered what clients ask of servers.
This chapter covers the reverse direction: requests the server sends to the client, which exist because the client sits next to two things the server lacks - the user and the model.

Roots: the client tells the server which filesystem-like locations are in scope.
Sampling: the server asks the client to run a model completion on its behalf.
Elicitation: the server asks the client to collect structured input from the user mid-operation.

All three are gated by capability negotiation: a server may only send them if the client declared the capability at initialize, and every one of them is an imposition on the host's user experience, which is exactly why host support arrived slowly.
A server that requires any of these must be designed to degrade when they are absent; that constraint shapes this entire chapter.

## 2. Roots

Roots answer the scoping question: out of everything this server could touch, what has the user actually opened?

A client declaring the roots capability answers roots/list with a list of root objects, each a file:// URI plus an optional name, and sends notifications/roots/list_changed when the set changes, as when the user opens a different project.

```json
{
  "roots": [
    { "uri": "file:///Users/dev/checkout/backend", "name": "backend" },
    { "uri": "file:///Users/dev/checkout/frontend", "name": "frontend" }
  ]
}
```

The semantics are advisory, and this must be said plainly: roots are information, not enforcement.
A well-behaved filesystem server queries roots at startup, constrains its operations to those subtrees, and re-queries on change notifications.
Nothing in the protocol prevents a server process from reading outside its roots if the operating system allows it; actual confinement comes from sandboxing the server process (containers, OS permissions), which Chapter 07 treats as mandatory for untrusted servers.
Roots are the cooperative half of a defense that must also have a coercive half.

Design intent: roots exist so that IDE-shaped hosts can communicate workspace boundaries in a standard way, letting one filesystem or git server serve every editor without per-editor configuration of paths.
Servers should treat operations outside the advertised roots as errors even when technically possible, because that behavior is what makes a server a good citizen worth trusting.

## 3. Sampling

Sampling inverts the protocol's usual flow: the server sends sampling/createMessage, and the client - which owns the model relationship - runs the completion and returns the result.
The name confuses everyone at first encounter; read it as "the server requests one model generation," from the statistics sense of sampling a distribution, not anything to do with audio or data sampling.

```json
{
  "jsonrpc": "2.0",
  "id": 12,
  "method": "sampling/createMessage",
  "params": {
    "messages": [
      {
        "role": "user",
        "content": { "type": "text", "text": "Summarize this changelog in three bullets:\n..." }
      }
    ],
    "systemPrompt": "You are a concise technical summarizer.",
    "maxTokens": 400,
    "modelPreferences": {
      "hints": [ { "name": "claude-3-5-sonnet" } ],
      "intelligencePriority": 0.5,
      "speedPriority": 0.8,
      "costPriority": 0.7
    }
  }
}
```

The client returns a single message: role, content, the model actually used, and a stopReason.
Model preferences are advisory: hints are substring-matched suggestions, and the three priority floats (cost, speed, intelligence, each 0 to 1) let the server express what it cares about while the client makes the final model choice, possibly mapping a hint to an equivalent model from a different provider.
This indirection is deliberate: servers must not need provider credentials or provider-specific code, so preferences are expressed abstractly and the client resolves them.

The spec builds human oversight into the flow: the client should be able to show the user the prompt before it runs and the completion before it returns, and may edit or refuse either.
The server also does not see the client's conversation; it sees only what it put in its own request, plus an includeContext field (allServers, thisServer, none) whose honoring is entirely at the client's discretion.
Sampling is therefore doubly mediated: context flows in only if the client allows it, and output flows back only after the client (and possibly the user) approves.

Why sampling matters architecturally is worth stating carefully, because it is the protocol's most under-appreciated idea.
Without sampling, a server that needs intelligence - summarize before returning, classify a record, decide which of three queries to run - must embed its own model access: an API key, a provider dependency, a billing relationship, and a security surface, per server.
With sampling, intelligence becomes something the host provisions once and servers borrow through a mediated channel.
The user's model choice, spend controls, and audit trail all stay in one place; a server needing a small summarization step does not become an AI vendor to provide it.
Sampling also enables agentic servers: a server tool can loop - call model, act on result, call model again - effectively running a sub-agent whose every model call passes through the host's oversight, which is the protocol-native alternative to servers secretly shipping their own agent loops.

Now the lag, and its causes, because the gap between design elegance and deployed reality is the lesson.
For roughly the first year of MCP's life, most major hosts - including, for a long stretch, Claude Desktop - did not implement sampling; support began appearing in more clients through 2025 (VS Code's MCP client was an early notable implementer), and as of early 2026 sampling still cannot be assumed.

The causes are structural, not accidental.
First, cost accountability: sampling spends the host's tokens at the server's request, and hosts were reluctant to let third-party code initiate spend, however mediated.
Second, UX burden: a meaningful implementation needs review-and-approve surfaces for prompts and completions, which is real product work, and a low-friction implementation that auto-approves everything turns the feature into a prompt-injection amplifier.
Third, incentive ordering: server authors could not depend on sampling, so they shipped servers with their own API keys, so hosts saw little demand, a classic chicken-and-egg that only broke slowly.
Fourth, trust: a sampling request's content is attacker-influencable text entering a model with includeContext potentially exposing conversation data, and hosts needed the security thinking of Chapter 07 to mature before enabling it broadly.

Practical guidance for server authors as of early 2026: treat sampling as an optimization tier.
Check the negotiated capability; if present, use it for the intelligence step; if absent, either degrade to a non-intelligent behavior, return raw data and let the host's model do the work in-context, or accept an optional user-supplied API key as a fallback - and say which you chose in your documentation, because each fallback has a different cost and privacy profile.

## 4. Elicitation

Elicitation, added in revision 2025-06-18, lets a server pause mid-operation and ask the user for structured input through the client.

```json
{
  "jsonrpc": "2.0",
  "id": 19,
  "method": "elicitation/create",
  "params": {
    "message": "Confirm deployment target",
    "requestedSchema": {
      "type": "object",
      "properties": {
        "environment": { "type": "string", "enum": ["staging", "production"] },
        "notify_channel": { "type": "string", "description": "Slack channel for the deploy notice" }
      },
      "required": ["environment"]
    }
  }
}
```

The requested schema is deliberately restricted to flat objects with primitive properties - strings (with optional formats and enums), numbers, booleans - so that every host can render a simple form without a full JSON Schema form engine.
The response distinguishes three outcomes, and handling all three is the mark of a correct server: accept (with validated content), decline (the user explicitly said no), and cancel (the user dismissed without deciding).
Decline and cancel are different signals - a decline may mean "never," a cancel may mean "ask later" - and collapsing them loses information.

Elicitation fixes a real pre-2025-06-18 pattern failure: tools needed every possibly-relevant parameter up front, forcing either bloated schemas or failed calls that bounced back through the model ("please ask the user which environment").
With elicitation, a tool can start with minimal arguments and ask for what it turns out to need, when it needs it.

Two boundaries keep it safe and usable.
The spec forbids using elicitation to request sensitive information such as passwords or API keys; secrets belong in the authorization layer (Chapter 05), and hosts are told to display which server is asking so users are not phished by a look-alike request.
And elicitation interrupts the user, so it shares sampling's structural problem in milder form: hosts must build the form UI, support arrived through late 2025 unevenly, and servers must handle absence - typically by falling back to the bloated-schema pattern elicitation was meant to replace.

## 5. Utilities: progress, cancellation, logging, ping

The utility layer is unglamorous and is where production quality lives.

Progress attaches to any request via a progressToken the sender places in the request's _meta.
The receiver may then emit notifications/progress carrying the token, a monotonically increasing progress value, an optional total, and since 2025-03-26 an optional human-readable message.
Progress is optional in both directions - tokens may be ignored, totals may be absent or change - so consume it as advisory UI fuel, never as flow control.
For a slow tool (a large export, a long build), progress is the difference between a host showing a live status line and a host showing a frozen spinner that users kill at second twenty.

Cancellation is the notification notifications/cancelled, carrying the id of an in-flight request and an optional reason.
Because it is a notification racing against the work itself, the semantics are best-effort by construction: the receiver should stop work and should not respond to the cancelled request, but the canceller must tolerate a response that was already in flight when the notification landed.
The initialize request must never be cancelled.
Server authors have one obligation here that SDKs cannot fully hide: long-running handlers must actually check for cancellation and stop burning resources, because a protocol that delivers the notification to a handler that never yields has delivered nothing.

Logging gives servers a structured channel to the host's diagnostics.
A server declaring the logging capability accepts logging/setLevel and emits notifications/message with a severity from the RFC 5424 ladder (debug, info, notice, warning, error, critical, alert, emergency), an optional logger name, and arbitrary JSON data.
The rule that saves stdio servers: never print diagnostics to stdout, because stdout is the protocol channel and a stray print corrupts framing; log over the protocol, or to stderr, which stdio hosts typically capture.
Log messages are also server-authored text that may be shown to users or fed to models, so they are part of your injection surface; log facts, not instructions.

Ping is a request valid in both directions at any time after initialization begins; the receiver answers promptly with an empty result, and either side may use timeouts on ping to decide a peer is dead.
Trivial to implement, and the first thing to check when a session silently hangs.

## 6. The pattern behind the chapter

Step back and the client primitives share one architecture: the server states a need - workspace scope, intelligence, user input - and the client satisfies it under host policy, with the user visible at every step.
This is the protocol's answer to keeping many mutually distrustful servers usable inside one application: capabilities flow through the trust hub rather than around it.
The recurring cost is that every mediated capability demands host-side product work, so each one shipped later and spread slower than the server primitives; the recurring benefit is that no server ever holds the user's model keys, conversation, or unmediated attention.
Design servers assuming the capabilities are absent, use them when present, and you will be right in every host.

## Exercises

1. Write the full message sequence, as raw JSON, for a server that receives a tools/call, issues an elicitation for a missing parameter, receives an accept, and returns the tool result; include ids and all three possible elicitation outcomes as branches.
2. Implement a sampling fallback ladder in pseudocode for a summarize_thread tool: capability present, capability absent with host-model degradation, and capability absent with user-key fallback; annotate each rung with its cost and privacy implications.
3. Explain why roots without process sandboxing is not a security control, then list the exact sandbox mechanism you would use on your platform for an untrusted stdio server.
4. Design the elicitation schema for a database migration tool that needs environment, confirmation of downtime acceptance, and an optional maintenance-window string; justify every required flag.
5. Add correct cancellation handling to a long-running tool handler in Python: show where the cancellation check sits in the loop and what cleanup runs; state what happens if the result was already sent.
6. A host reports your server as hung; write the diagnostic sequence using ping, logging levels, and progress tokens that distinguishes a dead process from a slow tool from a deadlocked handler.
7. Argue the strongest case that hosts were right to delay sampling support, then the strongest case that the delay damaged the ecosystem; end with your own verdict in three sentences.

## Godhood check

You have mastered this chapter when you can do the following without notes.

- Name the three client primitives, who initiates each, and the capability that gates each.
- Write a sampling/createMessage request from memory, explain model preferences and includeContext, and identify both human-in-the-loop checkpoints.
- Give the four structural reasons sampling support lagged and the three-rung fallback strategy a server should implement.
- Explain roots as advisory scoping and state where real enforcement must live.
- Distinguish accept, decline, and cancel in elicitation and explain the flat-schema restriction and the no-secrets rule.
- Describe progress tokens, best-effort cancellation semantics, the RFC 5424 log levels, and the stdout rule for stdio servers, and say which of these you would check first for a hanging session.
