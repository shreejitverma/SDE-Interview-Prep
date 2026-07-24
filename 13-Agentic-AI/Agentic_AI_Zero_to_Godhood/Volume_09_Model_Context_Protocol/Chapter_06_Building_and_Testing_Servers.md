# Chapter 06 - Building and Testing Servers

## What you will master

- The official Python SDK's FastMCP decorator style and what it generates for you.
- The TypeScript SDK's McpServer registration style, and how the two SDKs differ philosophically.
- A complete worked Python server exposing tools, resources, and prompts together.
- Testing with the MCP Inspector, in-process testing, and a debugging playbook for the failure modes you will actually hit.
- Packaging, distribution, and the registry landscape as of early 2026.

API shapes shown here are the official modelcontextprotocol SDKs as of early 2026; pin versions, because both SDKs moved fast through 2025.

## 1. SDK landscape

Official SDKs existed as of early 2026 for TypeScript, Python, Java, Kotlin, C#, Go, Ruby, Rust, Swift, and PHP, maintained under the modelcontextprotocol organization, several in partnership with platform vendors (C# with Microsoft, Java with Spring, Go with Google).
This chapter works in Python and TypeScript because they cover most real servers and because their idioms transfer.

One naming clarification prevents an hour of confusion.
"FastMCP" names two things: the high-level decorator API inside the official Python SDK (mcp.server.fastmcp), which absorbed Jeremiah Lowin's original FastMCP 1.0, and the separate third-party FastMCP 2.x project that continued independently with a larger feature set (proxying, composition, hosted deployment tooling).
This chapter uses the official SDK's built-in FastMCP; when reading community code, check the import line to know which one you are looking at.

The SDKs' shared philosophy: you write typed functions, and the SDK derives the protocol surface - schemas from type hints, definitions from docstrings, dispatch, framing, and lifecycle - so that protocol correctness is not something every server author re-implements.
The trade-off is the usual one for high-level frameworks: the decorators cover the common 90 percent, and the low-level Server API remains available for the rest (custom capability wiring, exotic transports, fine control over notifications).

## 2. The Python SDK, FastMCP style

Install with the CLI extras: `uv add "mcp[cli]"` or `pip install "mcp[cli]"`.

The core pattern is a FastMCP instance plus decorators.

```python
from mcp.server.fastmcp import FastMCP

mcp = FastMCP("demo")

@mcp.tool()
def add(a: int, b: int) -> int:
    """Add two integers."""
    return a + b

if __name__ == "__main__":
    mcp.run()
```

What the decorator generates deserves spelling out, because it is the value proposition.
The function name becomes the tool name; the docstring becomes the description; the type hints become the JSON Schema inputSchema, with defaults marking parameters optional; Pydantic models and dataclasses as parameter or return types become nested object schemas; and a structured return type generates an outputSchema with the return value serialized into structuredContent alongside the text channel (output schema generation landed in the SDK alongside spec revision 2025-06-18).
Exceptions raised by the function are caught and converted into isError results, which implements the Chapter 02 rule - execution failures go to the model, not to the protocol - without you thinking about it.
mcp.run() defaults to stdio; mcp.run(transport="streamable-http") serves the HTTP transport instead, and the FastMCP constructor takes host and port settings for that case.

Resources and prompts follow the same shape.

```python
@mcp.resource("config://app")
def app_config() -> str:
    """Static application configuration."""
    return "retries=3\ntimeout_s=30"

@mcp.resource("users://{user_id}/profile")
def user_profile(user_id: str) -> str:
    """Profile for one user."""
    return lookup_profile(user_id)

@mcp.prompt()
def review_code(code: str) -> str:
    """Ask for a structured review of a code snippet."""
    return f"Review this code for correctness and style:\n\n{code}"
```

A URI containing braces registers a resource template rather than a concrete resource, with the placeholders bound to function parameters; this is the SDK making the Chapter 03 template concept nearly free.

The Context object is the bridge to everything session-scoped from Chapter 04.
Add a parameter annotated with the Context type to any tool, resource, or prompt function and the SDK injects it.

```python
from mcp.server.fastmcp import Context

@mcp.tool()
async def import_records(path: str, ctx: Context) -> str:
    """Import records from a CSV file, reporting progress."""
    rows = load_rows(path)
    for i, row in enumerate(rows):
        insert(row)
        await ctx.report_progress(i + 1, total=len(rows))
    await ctx.info(f"imported {len(rows)} rows")
    return f"imported {len(rows)} rows"
```

Context exposes logging (ctx.debug, ctx.info, ctx.warning, ctx.error mapping to notifications/message), progress (ctx.report_progress using the request's progress token), resource access (ctx.read_resource), elicitation (ctx.elicit with a schema, returning the accept, decline, or cancel outcome), and the underlying session for sampling via ctx.session.create_message.
Every one of these is a capability-gated client feature, so production code checks for graceful degradation exactly as Chapter 04 prescribed; the SDK will surface the error if the client lacks the capability, but designing the fallback is still your job.
Async functions are supported throughout and are the right default for anything that does I/O, since the server multiplexes concurrent requests on one event loop and a blocking handler stalls its siblings.

For shared expensive state - connection pools, loaded models - the lifespan pattern gives you typed startup and shutdown.

```python
from contextlib import asynccontextmanager
from mcp.server.fastmcp import FastMCP

@asynccontextmanager
async def lifespan(server):
    pool = await open_pool()
    try:
        yield {"pool": pool}
    finally:
        await pool.close()

mcp = FastMCP("db-server", lifespan=lifespan)
```

The yielded value is reachable from handlers through the request context, which keeps globals out of your module and makes the server testable with a fake pool.

## 3. The TypeScript SDK

The TypeScript SDK (@modelcontextprotocol/sdk) expresses the same concepts with explicit registration and Zod schemas instead of decorators and type hints, because TypeScript's types vanish at runtime and cannot generate schemas by reflection.

```typescript
import { McpServer } from "@modelcontextprotocol/sdk/server/mcp.js";
import { StdioServerTransport } from "@modelcontextprotocol/sdk/server/stdio.js";
import { z } from "zod";

const server = new McpServer({ name: "demo", version: "1.0.0" });

server.registerTool(
  "add",
  {
    title: "Add",
    description: "Add two integers.",
    inputSchema: { a: z.number(), b: z.number() },
  },
  async ({ a, b }) => ({
    content: [{ type: "text", text: String(a + b) }],
  })
);

const transport = new StdioServerTransport();
await server.connect(transport);
```

registerResource and registerPrompt follow the same shape, with ResourceTemplate objects for parameterized URIs, and StreamableHTTPServerTransport replaces the stdio transport for remote serving, typically mounted inside an Express or similar HTTP app that owns session-id handling.
The philosophical difference from Python is visible in the handler signature: you construct the result object - content array and all - yourself, which is more ceremony and more control; the Python SDK guesses well from return types, and when it guesses wrong you drop to explicit result construction there too.
Older code uses server.tool() and the low-level Server class with setRequestHandler; the register* family is the current style, and reading both is necessary in the 2026 ecosystem because tutorials from 2024-2025 dominate search results.

Choose the SDK by deployment target more than taste: Python wins for data, ML, and script-shaped servers; TypeScript wins when the server ships inside an npm ecosystem, an Electron app, or an edge runtime, and npx-based distribution (section 6) is smoother for end users of desktop hosts.

## 4. A complete worked server

The following is a complete, runnable single-file server for a small issue tracker, exercising all three primitives plus context features; it is deliberately backed by an in-memory dict so every part is inspectable.

```python
"""issues_server.py - MCP server for a toy issue tracker."""
from dataclasses import dataclass, field
from mcp.server.fastmcp import FastMCP, Context

mcp = FastMCP("issue-tracker")

@dataclass
class Issue:
    id: int
    title: str
    body: str
    status: str = "open"
    labels: list[str] = field(default_factory=list)

DB: dict[int, Issue] = {
    1: Issue(1, "Crash on empty config", "Parser throws on zero-byte file.", labels=["bug"]),
    2: Issue(2, "Add CSV export", "Users want order history as CSV.", labels=["feature"]),
}

@mcp.tool()
def create_issue(title: str, body: str, labels: list[str] | None = None) -> dict:
    """Create a new issue and return it. Labels are optional free-form strings."""
    issue_id = max(DB) + 1 if DB else 1
    issue = Issue(issue_id, title, body, labels=labels or [])
    DB[issue_id] = issue
    return issue.__dict__

@mcp.tool()
def close_issue(issue_id: int, ctx: Context | None = None) -> dict:
    """Close an open issue. Fails if the issue does not exist or is already closed."""
    issue = DB.get(issue_id)
    if issue is None:
        raise ValueError(f"no issue with id {issue_id}; use search_issues to find valid ids")
    if issue.status == "closed":
        raise ValueError(f"issue {issue_id} is already closed")
    issue.status = "closed"
    return issue.__dict__

@mcp.tool()
def search_issues(query: str, status: str = "open") -> list[dict]:
    """Search issues by substring in title or body. status is 'open', 'closed', or 'all'."""
    if status not in ("open", "closed", "all"):
        raise ValueError("status must be 'open', 'closed', or 'all'")
    hits = [
        i.__dict__ for i in DB.values()
        if query.lower() in (i.title + " " + i.body).lower()
        and (status == "all" or i.status == status)
    ]
    return hits[:25]

@mcp.resource("issues://{issue_id}")
def issue_resource(issue_id: str) -> str:
    """Full text of one issue."""
    issue = DB.get(int(issue_id))
    if issue is None:
        raise ValueError(f"no issue with id {issue_id}")
    labels = ", ".join(issue.labels) or "none"
    return f"# {issue.title}\nstatus: {issue.status}\nlabels: {labels}\n\n{issue.body}"

@mcp.resource("issues://summary")
def summary_resource() -> str:
    """One-line-per-issue overview of the tracker."""
    return "\n".join(
        f"{i.id}: [{i.status}] {i.title}" for i in DB.values()
    )

@mcp.prompt()
def triage(issue_id: str) -> str:
    """Triage an issue: severity, root-cause hypotheses, next actions."""
    issue = DB.get(int(issue_id))
    if issue is None:
        raise ValueError(f"no issue with id {issue_id}")
    return (
        f"Triage the following issue.\n\n{issue.title}\n\n{issue.body}\n\n"
        "Give: severity (P0-P3) with justification, two root-cause hypotheses, "
        "and the single next diagnostic step for each hypothesis."
    )

if __name__ == "__main__":
    mcp.run()
```

Design notes worth internalizing from this small example.
Error messages tell the model what to do next ("use search_issues to find valid ids"), applying Chapter 03's error-text rule.
The search tool caps its result count, because unbounded results are a context-window incident waiting for a large database.
The same entity is exposed twice on purpose: as a tool result for model-driven flows and as a resource for application-driven attachment, which is the dual-exposure pragmatism Chapter 03 recommended.
The dict return types give the SDK enough to produce structured output; in a production server you would promote them to dataclass or Pydantic return types for a real outputSchema.

Wire it into a host with the standard config block, using absolute paths because hosts do not share your shell's working directory or PATH.

```json
{
  "mcpServers": {
    "issue-tracker": {
      "command": "/Users/dev/.venv/bin/python",
      "args": ["/Users/dev/servers/issues_server.py"]
    }
  }
}
```

## 5. Testing and debugging

The MCP Inspector is the standard interactive test harness: `npx @modelcontextprotocol/inspector python issues_server.py` launches your server under a web UI from which you connect, list tools, resources, and prompts, invoke them with typed argument forms, and watch the raw JSON-RPC in both directions.
The Inspector is a full client, so it also exercises the awkward features - notifications, progress, and (in later 2025 versions) elicitation and sampling round-trips - that are hard to reach through a real host on demand.
Two operational notes: recent Inspector versions bind to localhost with a session token specifically because of the 2025 CVE class covered in Chapter 05, so use the tokened URL it prints; and the Inspector validates against a specific spec revision, so keep it updated alongside your SDK.
The Python SDK also ships `mcp dev issues_server.py`, which wraps the same Inspector flow, and `mcp install`, which writes the Claude Desktop config block for you.

Automated testing has three tiers, and skipping the first is the common mistake.
Tier one: your handlers are plain functions, so unit-test them as functions - no protocol involved - covering the error paths that become isError results.
Tier two: in-process protocol tests; both SDKs let you connect a client session to the server object in memory (the Python SDK exposes an in-memory client-server pairing for exactly this), so you can assert that tools/list contains what you expect and that a tools/call round-trips with the right schema, without spawning processes.
Tier three: end-to-end against a real host, which is slow and manual but is the only tier that catches host-specific behavior like tool-name prefixing, approval prompts, and context-length pressure from your descriptions.

The debugging playbook, ordered by frequency observed in the wild.
Server connects then immediately disconnects on stdio: something wrote to stdout; find the stray print or chatty library and silence or redirect it, as Chapter 05 explained.
Tools never appear in the host: the host cached an old tool list, the server crashed before initialize completed (check stderr, which Claude Desktop and most hosts write to their log directory), or the config path is wrong - absolute paths again.
Tool appears but the model never calls it: not a protocol bug; your name and description are failing the model, so apply Volume 03's description discipline.
Tool calls fail with invalid params: the model is sending what your schema literally says, so read your generated schema in the Inspector rather than your assumptions about it - a `list[str] | None` versus `list[str]` difference is invisible in code review and glaring in the schema.
Everything works in the Inspector and fails in the host: capability mismatch (the host lacks sampling or elicitation) or the host's approval policy is silently blocking calls; check the negotiated capabilities in your initialize logging.
Log the initialize exchange at debug level in every server you ship; it is the single highest-value line of diagnostics you can add.

## 6. Packaging, distribution, and registries

Distribution shapes adoption more than code quality does, because your user's first experience is installation.

For local stdio servers the ecosystem standardized on runner-based launch: `npx -y package-name` for TypeScript and `uvx package-name` for Python, both of which fetch and run without a manual install step, which is why host config examples overwhelmingly use them.
The convenience has a security face - your users are executing whatever the registry serves at launch time - so version-pin in the config for anything sensitive, and Chapter 07 makes the general case for pinning.
Beyond raw configs, 2025 brought one-click paths: Claude Desktop extension bundles (.dxt, later renamed .mcpb) packaging a server plus manifest for double-click installation, Docker's MCP Catalog and toolkit running servers as containers (which buys the sandboxing Chapter 04 said roots cannot provide), and hosted remote servers where "installation" is an OAuth consent screen.
Remote-first distribution is where first-party vendors went (GitHub, Atlassian, Sentry, and others through 2025), because it eliminates local runtime problems entirely at the price of running a service.

Registries, as of early 2026.
The official MCP Registry (registry.modelcontextprotocol.io) entered preview in September 2025: an open catalog API where server authors publish metadata (package location, transport, verified namespace), designed to be consumed by downstream sub-registries rather than browsed by humans.
Around it sit community directories that predate it - Smithery, PulseMCP, Glama, mcp.so - plus curated vendor catalogs (Docker's, and host-specific directories in Claude, Cursor, and VS Code).
Listing in a registry is discoverability, not endorsement: none of these performed deep security review as of early 2026, and the registry ecosystem's trust model is essentially npm's, with the same supply-chain consequences.
Ship with versioned releases, a changelog that flags tool-definition changes (hosts re-prompt users on definition changes, and silent mutation is the rug-pull signature), and a README that states exactly which capabilities the server requires and which it degrades without - the documentation habit this volume has been building toward.

## Exercises

1. Run issues_server.py under the MCP Inspector, capture the raw initialize exchange, and annotate every capability field with the chapter that explained it.
2. Extend the server: give close_issue an elicitation-based confirmation when the issue has the label "release-blocker", handling accept, decline, and cancel distinctly; test all three paths in the Inspector.
3. Promote search_issues to structured output with a Pydantic return model, inspect the generated outputSchema, and confirm structuredContent appears in the raw result.
4. Port create_issue and search_issues to the TypeScript SDK with Zod schemas, and write down three concrete differences you hit that this chapter predicted.
5. Write tier-two in-process tests asserting the full tool list, a successful close_issue round-trip, and an isError result for a bad id; measure how long the suite takes versus one manual Inspector session.
6. Break your server four ways on purpose - stdout print, wrong config path, schema mismatch, exception in a resource handler - and write the observed symptom for each next to the playbook entry it matches.
7. Package the server for a colleague two ways - uvx-runnable package and a Docker container - and compare the security posture of each in three sentences.

## Godhood check

You have mastered this chapter when you can do the following without notes.

- Explain what the FastMCP tool decorator derives from a typed, docstringed function, including schemas, structured output, and exception-to-isError conversion.
- Name what Context provides and which client capability each feature depends on.
- Write a minimal but complete server exposing at least one tool, one resource template, and one prompt, from memory, in either SDK.
- Explain the two-FastMCP naming split and the register-versus-decorator philosophical difference between the SDKs, with the runtime-types reason.
- Run the three-tier testing model and the six-entry debugging playbook from memory, matching symptom to cause without experimentation.
- Describe the early-2026 registry landscape, the npx/uvx distribution convention and its supply-chain implication, and what belongs in a server's README for capability degradation.
