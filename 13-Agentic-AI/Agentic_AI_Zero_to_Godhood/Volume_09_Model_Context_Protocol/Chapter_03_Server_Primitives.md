# Chapter 03 - Server Primitives

## What you will master

- The three server primitives - tools, resources, prompts - and the control-model that distinguishes them.
- Tool definitions in depth: input schemas, output schemas, structured output, annotations, and error semantics.
- Resources in depth: URIs, templates, contents, subscriptions, and list-change notifications.
- Prompts in depth: arguments, completion, and multi-message templates.
- Pagination across all list operations.
- A decision procedure for choosing tool versus resource versus prompt, and why the ecosystem collapsed toward tools anyway.

Spec details are stated as of early 2026 against revisions 2024-11-05, 2025-03-26, and 2025-06-18.

## 1. Three primitives, three controllers

MCP's server surface is organized around who is meant to invoke each primitive, and this framing is the single most useful mental model in the protocol.

Tools are model-controlled: the model decides during generation that a tool should run, and the host executes it (with whatever human approval policy the host enforces).
Resources are application-controlled: the host decides which data to read and attach to context, whether by user selection, retrieval logic, or its own heuristics.
Prompts are user-controlled: the user explicitly picks a template, typically through a slash command or menu, and the host expands it into messages.

The controller framing explains design details that otherwise look arbitrary.
Tools carry rich descriptions because a model must choose among them from text alone.
Resources carry URIs and MIME types because applications need addressing and typing, not persuasion.
Prompts carry argument lists with completion support because humans fill them in interactively.

It also explains the ecosystem's actual shape as of early 2026: tools dominate because every host supports them, while resources and prompts have uneven host support, since they require host UI and host policy that many clients never built.
When a capability only works if the host built UI for it, server authors route around it by making everything a tool; this "everything becomes a tool" collapse is real, costs context tokens, and is worth resisting where your target hosts allow.

## 2. Tools

A server declares the tools capability, and the client discovers tools with tools/list.

```json
{
  "name": "query_orders",
  "title": "Query customer orders",
  "description": "Search orders by customer email and optional date range. Returns at most 50 orders, newest first.",
  "inputSchema": {
    "type": "object",
    "properties": {
      "email": { "type": "string", "description": "Customer email, exact match" },
      "since": { "type": "string", "format": "date", "description": "Earliest order date, inclusive" }
    },
    "required": ["email"]
  },
  "outputSchema": {
    "type": "object",
    "properties": {
      "orders": {
        "type": "array",
        "items": {
          "type": "object",
          "properties": {
            "id": { "type": "string" },
            "total_cents": { "type": "integer" },
            "status": { "type": "string" }
          },
          "required": ["id", "total_cents", "status"]
        }
      }
    },
    "required": ["orders"]
  },
  "annotations": {
    "readOnlyHint": true,
    "openWorldHint": false
  }
}
```

The inputSchema is JSON Schema (draft 2020-12 in current SDKs) and it is doing double duty: it is machine validation for the host and it is documentation the model reads.
Everything from Volume 03 about writing tool descriptions applies with full force: state units, bounds, defaults, side effects, and failure modes in the description, because the schema and description are the entire interface the model sees.
Names should be verb-object and unambiguous across all servers the host might attach, since the model sees a flattened namespace; hosts commonly prefix tool names with the server name to avoid collisions, which is another reason to keep names short.

Invocation is tools/call with a name and arguments, and the result has two parallel channels.

The content array is the model-facing channel: an ordered list of content blocks of type text, image, audio, resource_link, or embedded resource.
The structuredContent field, added in revision 2025-06-18, is the machine-facing channel: a JSON object that must validate against the declared outputSchema if one was declared.
Well-behaved servers set both, with the text content mirroring the structured data, because older clients only read content.

Structured output matters more than it first appears.
Before it, hosts that wanted to post-process tool results parsed prose, which broke constantly; with it, a host can route structuredContent to code (widgets, charts, downstream functions) while the model reads the text channel.
The cost is duplication on the wire: the same data travels twice, and for large results you should truncate the text channel aggressively while keeping structuredContent complete, or return a resource_link instead of either.

Execution errors use isError true with the failure described in content, as Chapter 02 established; reserve JSON-RPC errors for protocol-level failure such as unknown tool names.
Write error text for the model as a reader: "date must be YYYY-MM-DD, got 3/2/2026" lets the model retry correctly, while "ValidationError code 422" does not.

Annotations, added in revision 2025-03-26, are hints about tool behavior: readOnlyHint (does not modify state), destructiveHint (may perform irreversible updates, meaningful only when not read-only), idempotentHint (repeat calls with same arguments have no additional effect), and openWorldHint (interacts with external entities beyond a closed system, such as web search).
Two hard rules govern their use.
First, hosts may use annotations for UX, such as auto-approving read-only tools and demanding confirmation for destructive ones.
Second, annotations are unverified claims from the server and must never be treated as a security boundary; a malicious server will happily mark a destructive tool read-only, so trust in annotations must be rooted in trust in the server itself (Chapter 07).

Tool lists change: servers that declare listChanged send notifications/tools/list_changed when tools appear, disappear, or change definition, and clients re-fetch.
Dynamic tool lists enable good patterns, such as exposing login-gated tools only after authentication, and they enable the rug-pull attack, where definitions mutate after approval; both live in Chapter 07.

## 3. Resources

Resources expose data for the application to read: files, database schemas, documents, logs, anything addressable and readable.

Each resource has a URI, a human-readable name, an optional description, an optional MIME type, and optionally a size in bytes.

```json
{
  "uri": "postgres://analytics/schema/orders",
  "name": "orders table schema",
  "description": "DDL and column statistics for the orders table",
  "mimeType": "text/plain"
}
```

URI schemes are open: file://, https://, git://, postgres://, or custom schemes; the URI is an identifier within the server's namespace, and clients must not assume they can dereference it outside the protocol.
Reading is resources/read with the URI, and the result carries contents: one or more items each bearing the uri, mimeType, and either text or a base64 blob.
One request may return multiple contents; reading a directory-like URI can return the files within it.

Resource templates handle parameterized spaces that cannot be enumerated.
A template is an RFC 6570 URI template such as file:///logs/{date}.log or github://repos/{owner}/{repo}/issues, discovered via resources/templates/list.
Templates are what make resources feel like a queryable surface rather than a fixed list, and argument completion (section 5) attaches to template parameters so hosts can offer autocompletion while a user fills one in.

Two change mechanisms exist, and they answer different questions.
The listChanged capability with notifications/resources/list_changed says "the set of resources changed."
The subscribe capability says "this particular resource's content changed": the client sends resources/subscribe with a URI, the server later sends notifications/resources/updated for it, and the client re-reads if it cares; resources/unsubscribe ends it.
Subscriptions are the protocol's mechanism for live context - a log file that updates, a document being edited - and they are among the least-supported features in real hosts, so design servers to be useful without them.

The judgment call in resource design is granularity.
Fine-grained resources (one per table, one per file) give the application precise, cheap attachment but produce huge lists; coarse resources (one per database) keep lists small but force over-fetching into context.
There is no universal answer: optimize for the attachment patterns your target hosts actually have, and remember that every byte read lands in a context window, so resources should be sized in kilobytes, not megabytes, with pagination or narrowing parameters for anything larger.

## 4. Prompts

Prompts are named, parameterized message templates the user invokes deliberately.

```json
{
  "name": "review_pr",
  "title": "Review a pull request",
  "description": "Structured code review with severity-ranked findings",
  "arguments": [
    { "name": "pr_number", "description": "Pull request number", "required": true },
    { "name": "focus", "description": "Optional area of focus, e.g. security", "required": false }
  ]
}
```

Discovery is prompts/list; expansion is prompts/get with argument values, and the result is a description plus a messages array of role-tagged messages whose content can be text, images, audio, or embedded resources.
That last point is the power feature: a prompts/get implementation can fetch the diff, embed it as a resource inside a user message, and return a fully grounded multi-message conversation seed, not just interpolated text.

Prompts encode workflow expertise on the server side.
The team that owns the deployment system ships a "diagnose failed deploy" prompt that embeds the right logs and asks the right questions, and every host user gets that expertise through a slash-command-like gesture.
Hosts typically surface prompts as slash commands or menu entries; Claude Code, for example, exposes server prompts as slash commands.

The limitation is symmetrical: because prompts are user-controlled, the model will not invoke them autonomously, so a workflow that must be model-triggerable needs to be a tool instead, and a workflow that should be both ends up implemented twice.
This asymmetry is a real design wart, acknowledged in ecosystem discussion, and no clean resolution existed as of early 2026.

## 5. Cross-cutting machinery: pagination and completion

All four list operations - tools/list, resources/list, resources/templates/list, prompts/list - paginate the same way.
The server may return a nextCursor alongside results; the client passes it back as cursor to get the next page, and absence of nextCursor means the end.
Cursors are opaque tokens: clients must not parse or fabricate them, and servers are free to encode offsets, keys, or snapshots however they like; page size is server-chosen.
The design mirrors modern REST cursor pagination and exists because thousand-tool servers and million-resource stores are real; clients must implement it even though small servers never emit cursors, because the client that ignores nextCursor silently sees a truncated world.

Completion is autocompletion for humans, not models.
A server declaring the completions capability answers completion/complete requests referencing either a prompt argument or a resource template parameter, with the partial value typed so far, and returns up to 100 candidate values plus an optional total and hasMore flag.
Revision 2025-06-18 added a context field carrying previously resolved arguments, so completing repo can depend on the owner already chosen.
Completion exists because prompts and templates are user-facing: a picker over live values (branch names, table names, ticket ids) is the difference between a usable prompt and an ignored one.
It is also, like subscriptions, unevenly supported by hosts, so treat it as progressive enhancement.

## 6. Choosing between tool, resource, and prompt

The decision procedure follows from the controller model; apply the questions in order.

Does the action have side effects, or must the model be able to decide to invoke it mid-task?
Then it is a tool; nothing else is model-triggerable.

Is it data that an application or user should attach to context, addressable by an identifier, with no side effects on read?
Then it is a resource; you get URIs, MIME types, subscriptions, and you avoid spending a tool slot and its per-request token cost on what is really a read.

Is it a reusable interaction pattern a human should deliberately start?
Then it is a prompt; you get arguments, completion, and explicit user intent.

Then apply the corrections that experience adds to the clean theory.

If your target hosts do not surface resources in a usable way, and the model genuinely needs the data mid-task, a read-only tool wrapping the resource is the pragmatic answer; mark it readOnlyHint true and keep the resource too, so capable hosts can do better.
Do not model queries with large parameter spaces as resource lists; that is what templates or a search tool are for.
Do not ship a tool per trivial variation ("get_open_orders", "get_closed_orders"); one tool with an enum parameter costs fewer tokens and less model confusion.
And keep the total tool count per server small - single digits is a good default - because the host may be aggregating ten servers, and Chapter 07's tool-overload discussion starts exactly here.

The honest summary of the ecosystem as of early 2026: tools are universally supported and overused, resources are underused relative to their design intent, and prompts are the sleeper feature that teams discover when they start encoding workflows.
Design for the controller model, degrade gracefully toward tools, and measure the token cost of whatever you ship.

## Exercises

1. Take a REST API you know well and partition its endpoints into tools, resources, resource templates, and prompts; justify every borderline call in one sentence each.
2. Write the full JSON tool definition, including outputSchema and annotations, for a tool that cancels an order; decide each annotation value and defend it.
3. Design the resource URI scheme for a server exposing a Postgres database: what is a resource, what is a template, what is deliberately not exposed, and why.
4. Write a prompts/get response, as raw JSON, for an incident-triage prompt that embeds a log excerpt as an embedded resource in a user message.
5. Implement cursor pagination for a 10,000-item resource list in pseudocode twice: once with offset-encoded cursors and once with keyset cursors, and state which failure mode each has under concurrent inserts.
6. Find one popular community server that models read-only data as tools; estimate the per-request token cost of its tool definitions and sketch the resource-based redesign.
7. Specify the completion behavior for a review_pr prompt's pr_number argument, including what the server queries and how it uses the 2025-06-18 context field.

## Godhood check

You have mastered this chapter when you can do the following without notes.

- State the controller model - model, application, user - and derive from it two design details of each primitive.
- Write a complete tool definition from memory, including input schema, output schema, and all four standard annotations, and explain why annotations are not a security boundary.
- Explain the two channels of a tool result, why both exist, and when to send a resource_link instead.
- Distinguish list_changed notifications from resource subscriptions and give a use case for each.
- Explain why prompts cannot be model-invoked, what that asymmetry costs, and the double-implementation workaround.
- Run the tool-versus-resource-versus-prompt decision procedure on a novel capability out loud, including the host-support pragmatics that bend the clean answer.
