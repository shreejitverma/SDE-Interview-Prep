# Volume 09 - Model Context Protocol

MCP as of early 2026: the protocol that turned the NxM integration problem into N+M, read at spec level rather than tutorial level.
This volume covers the dated spec revisions 2024-11-05, 2025-03-26, and 2025-06-18 in depth, notes what the November 2025 revision added, and treats security and context economics as first-class engineering concerns rather than appendices.
It assumes the tool-use mechanics from Volume 03; here the subject is distribution, negotiation, transport, authorization, and the trade-offs of adopting a protocol at all.

## Chapters

| Chapter | Title | One-line summary |
|---------|-------|------------------|
| 01 | Why MCP Exists | The NxM problem and the N+M fix, the USB-C analogy and its four failure points, the November 2024 launch through 2025 competitor adoption and Linux Foundation stewardship, and what MCP is not. |
| 02 | Architecture | Hosts, clients, and servers with one client per server; JSON-RPC 2.0 requests, results, errors, and notifications; the initialize handshake with version and capability negotiation; dated spec revisions. |
| 03 | Server Primitives | Tools, resources, and prompts under the model-application-user controller model: schemas, structured output, annotations, templates, subscriptions, pagination, completion, and how to choose among the three. |
| 04 | Client Primitives and Advanced Features | Roots, sampling, and elicitation as server-to-client requests; why sampling matters architecturally and the four structural reasons host support lagged; progress, cancellation, logging, and ping. |
| 05 | Transports and Auth | stdio framing and its local-first advantage; streamable HTTP with sessions and Last-Event-ID resumability replacing HTTP+SSE; OAuth 2.1 with PKCE, RFC 9728/8414/7591/8707, and enterprise gateway patterns. |
| 06 | Building and Testing Servers | The Python SDK's FastMCP decorators and the TypeScript SDK's registration style, a complete worked server with tools plus resources plus prompts, the MCP Inspector, a debugging playbook, packaging and registries. |
| 07 | Security and Ecosystem Patterns | Tool poisoning, rug pulls, confused deputy, result injection, shadowing, and token theft; layered mitigations with their costs; gateways, tool overload, code mode, and the "MCP versus just write code" debate. |
