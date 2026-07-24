# Chapter 05 - The Rest of the Field

Knowledge in this chapter is current as of early 2026.
Each section gives one framework the same treatment: philosophy, mechanics, sweet spot, and weaknesses, stated plainly.
The goal is not encyclopedic coverage but calibrated judgment: after this chapter you should be able to hear a framework pitch and locate it on the map in seconds.

## What you will master

- CrewAI's role-based model and the Flows correction to it.
- The AutoGen lineage: the AG2 fork, the 0.4 rewrite, and the Microsoft Agent Framework convergence with Semantic Kernel.
- smolagents and the code-acting thesis.
- Pydantic AI and the type-safety thesis.
- Google ADK and its place in the Vertex and A2A ecosystem.
- Mastra and the Vercel AI SDK, the two poles of the TypeScript ecosystem.
- A cross-cutting comparison you can defend in an architecture review.

## CrewAI: role-based crews, then flows

Philosophy: agents are teammates.
You define each agent by role, goal, and backstory, group them into a crew, give the crew tasks, and a process (sequential or hierarchical with a manager) coordinates who works when.
CrewAI began on top of LangChain and was rewritten as a standalone framework, a detail that matters because it signals the project's bet that its abstraction, not its plumbing, is the product.

```python
# Python, CrewAI, shape current as of 2025.
from crewai import Agent, Crew, Task, Process

researcher = Agent(
    role="Senior Research Analyst",
    goal="Find accurate, current information on the assigned topic",
    backstory="A meticulous analyst who verifies every claim.",
)
writer = Agent(role="Writer", goal="Produce a clear brief", backstory="A concise technical writer.")

research = Task(description="Research {topic}.", agent=researcher, expected_output="Bullet findings")
brief = Task(description="Write a one-page brief.", agent=writer, expected_output="One page")

crew = Crew(agents=[researcher, writer], tasks=[research, brief], process=Process.sequential)
result = crew.kickoff(inputs={"topic": "agent framework lock-in"})
```

The role/goal/backstory triple is prompt engineering with a schema: it compiles into system prompts, and the anthropomorphic framing makes agent design legible to non-specialists, which is a real reason for CrewAI's large adoption among newcomers and business-adjacent builders.
Flows, added in 2024, are the framework's self-correction: an event-driven, lower-level API with typed state, conditional routing, and deterministic steps, into which crews can be embedded.
Read Flows as CrewAI conceding the same point LangGraph made: production systems need explicit control flow, and pure role-play autonomy is not enough.

Sweet spot: rapid prototyping of multi-step content, research, and analysis pipelines; teams that want an accessible mental model; workloads where a plausible draft is the goal and a human reviews output.
Weaknesses: the role-play abstraction hides the loop, so debugging means digging through generated prompts you did not write; token consumption in hierarchical mode is high because coordination itself burns model calls; the persona framing invites magical thinking (a "Senior Analyst" backstory does not create competence, only style); and the framework's velocity has historically outrun its documentation.
The deeper critique to carry from Volume 07: crew-style designs multiply agents where one agent with better tools and context often wins, and CrewAI makes the multiplying easy.

## AutoGen, AG2, and the Microsoft Agent Framework

Philosophy of the original AutoGen (Microsoft Research, 2023): multi-agent systems as conversation; agents are conversable entities that solve tasks by talking, including group chats where a manager selects the next speaker.
AutoGen was the research community's workhorse for multi-agent experiments and the origin of much of Volume 07's vocabulary.

The lineage then split, and you need the map more than the details.

- AG2: in late 2024, AutoGen's original creators forked the project under the name AG2, continuing the 0.2-style conversational API as a community-driven project.
- AutoGen 0.4: in January 2025, Microsoft shipped a ground-up rewrite: an actor-style, event-driven, asynchronous core with typed messages, cross-language support, and a layered API, fixing the 0.2 era's scaling and observability problems at the cost of breaking everyone.
- Microsoft Agent Framework: announced in late 2025, this converges AutoGen's orchestration research with Semantic Kernel's enterprise plumbing (connectors, compliance posture, .NET citizenship) into one supported product line for Python and .NET, with graph-style workflows plus conversational agents, and positions itself as the successor both communities should migrate to.

Semantic Kernel deserves its one paragraph: it was Microsoft's earlier enterprise SDK for LLM apps (skills, planners, connectors), strong in .NET shops and weak in mindshare against Python-native rivals; the convergence is Microsoft consolidating two overlapping bets into one.

Sweet spot: enterprise Microsoft estates (Azure, .NET, compliance requirements) where a vendor-supported framework with a long support horizon beats a hipper library; research-flavored multi-agent conversation patterns; teams already on Semantic Kernel or AutoGen needing a supported path forward.
Weaknesses: the lineage itself is the warning, since users absorbed a fork, a rewrite, and a convergence within roughly two years, which is the churn tax at maximum rate; conversational multi-agent designs inherit all of Volume 07's failure modes (speaker-selection flakiness, token burn, error cascade); and the enterprise framing brings enterprise weight, with more concepts and configuration than the small frameworks in this chapter.

## smolagents: the code-acting minimalist

Philosophy: most agent frameworks are too big, and JSON tool calling is the wrong action format.
smolagents (Hugging Face, released January 2025) keeps its core to roughly a thousand lines and centers on the CodeAgent, whose actions are Python snippets the model writes, executed in a sandboxed interpreter, with tool calls being ordinary function calls inside that code.
The intellectual ancestry is the CodeAct line of research from Volume 04: expressing actions as code lets one action compose several tools, loop, branch, and transform data, where JSON tool calling needs one model round trip per step.

```python
# Python, smolagents, shape current as of 2025.
from smolagents import CodeAgent, WebSearchTool, InferenceClientModel

agent = CodeAgent(tools=[WebSearchTool()], model=InferenceClientModel())
agent.run("How many seconds would it take a leopard at top speed to cross Pont des Arts?")
```

The benchmark-flavored claim, order of magnitude only: the code-acting papers report meaningfully fewer steps to solve multi-tool tasks than JSON tool calling, because composition happens inside one action; smolagents inherits this and its GAIA-style demos lean on it.
Model-agnosticism is genuine: it targets Hugging Face models, local models, and closed APIs alike, which fits its role as the open ecosystem's teaching and research harness.

Sweet spot: research and experimentation on agent behavior; tasks with heavy data transformation between tool calls, where code actions shine; learning, since you can read the entire framework in a sitting, which aligns with this track's doctrine.
Weaknesses: executing model-written code is the largest attack surface an agent can have, and the sandboxing options (local interpreter restrictions, remote executors) demand the full Volume 11 treatment before production; the minimalism means production concerns (durable state, human-in-the-loop, deployment) are yours; and code actions require strong code-generating models, degrading harder on weak models than JSON calling does.

## Pydantic AI: the type-safety thesis

Philosophy: agent development should feel like normal, rigorously typed Python, and the FastAPI development experience is the standard to meet.
Built by the Pydantic team, whose validation library already sits inside nearly every Python agent framework, Pydantic AI makes the agent generic over two types: its dependencies and its output.

```python
# Python, Pydantic AI, shape current as of 2025.
from dataclasses import dataclass
from pydantic import BaseModel
from pydantic_ai import Agent, RunContext

@dataclass
class Deps:
    db: DatabaseConn

class Verdict(BaseModel):
    risk_level: int
    escalate: bool

agent = Agent("anthropic:claude-sonnet-4-5", deps_type=Deps, output_type=Verdict)

@agent.tool
async def customer_balance(ctx: RunContext[Deps], customer_id: str) -> float:
    return await ctx.deps.db.balance(customer_id)

result = await agent.run("Assess risk for customer 42.", deps=Deps(db=db))
assert isinstance(result.output, Verdict)
```

The load-bearing ideas: outputs are validated Pydantic models, with validation failures fed back to the model for retry, so downstream code consumes types, not prose; dependencies are injected through the run context rather than global state, which makes agents unit-testable with fake deps; and the whole surface type-checks, so mypy catches a mis-wired tool at development time rather than in production.
Instrumentation flows naturally to Logfire, the team's observability product, which is the same open-core pattern as everywhere else in this volume.

Sweet spot: production Python teams with a typing culture; agents embedded in larger applications where the agent is a component with a contract, not a chat; workloads where structured output correctness matters more than orchestration exotica.
Weaknesses: multi-agent orchestration is intentionally thin, with the project pointing graph-shaped needs to its separate graph library or plain code; the framework is young enough that patterns are still settling; and the typing rigor that senior teams love adds ceremony that prototypers feel immediately.

## Google ADK: the ecosystem play

Philosophy: agents as hierarchical compositions, shipped with the Google Cloud estate attached.
The Agent Development Kit (released April 2025, Python first with other languages following) structures applications as trees of agents: LLM agents for reasoning, workflow agents (sequential, parallel, loop) for deterministic composition, with tools, session state, and evaluation built in, plus a CLI and web UI for local development.
ADK is also the reference implementation environment for the A2A protocol from Volume 07 and deploys naturally to Vertex AI Agent Engine, which is the managed runtime lock-in point.

Sweet spot: teams on Google Cloud and Gemini, where the integration (Vertex deployment, Google search grounding, enterprise controls) is the point; workloads that decompose into workflow-plus-LLM hybrids, which the explicit workflow agents express cleanly.
Weaknesses: outside the Google ecosystem its pull weakens, since every capability has a stronger-communitied equivalent elsewhere; the abstraction count is high for the volume of documentation available; and betting on Google developer products carries a well-known continuity risk that enterprise buyers price in explicitly.

## Mastra: the TypeScript native

Philosophy: TypeScript teams deserve a first-class agent framework, not a port of Python ideas.
Mastra (from the founders of Gatsby, emerged 2024-2025) bundles agents, tool calling, workflow graphs with suspend-and-resume, memory, RAG primitives, and evals into one TypeScript package with strong Zod-based typing and a local development playground.
Its workflow layer gives the durable, step-based execution story (pause for human input, resume later) that this volume keeps returning to, expressed in idiomatic TypeScript.

Sweet spot: full-stack TypeScript product teams building agentic features into web applications, who want one coherent toolkit instead of stitching libraries; Node and serverless deployment targets.
Weaknesses: it is the youngest framework given a full section here, so API stability and community depth trail the Python incumbents; the all-in-one scope risks kitchen-sink drift, the same disease early LangChain had; and the research ecosystem publishes in Python, so cutting-edge patterns arrive in TypeScript with lag.

## Vercel AI SDK: the application layer, not the agent brain

Philosophy: the model call belongs in the web framework, streamed to the UI.
The Vercel AI SDK is the dominant TypeScript library for LLM features in web apps, with a provider-abstraction core (generateText, streamText, generateObject) across many model vendors, UI hooks (useChat) for React and friends, and, in its 2025 major version, an agent loop primitive: multi-step tool calling controlled by stopping conditions.

```typescript
// TypeScript, Vercel AI SDK v5-era shape, current as of 2025.
import { generateText, tool, stepCountIs } from "ai";
import { z } from "zod";

const result = await generateText({
  model: "anthropic/claude-sonnet-4-5",
  tools: {
    weather: tool({
      description: "Get weather for a city",
      inputSchema: z.object({ city: z.string() }),
      execute: async ({ city }) => fetchWeather(city),
    }),
  },
  stopWhen: stepCountIs(8),
  prompt: "Compare the weather in Pune and Bengaluru.",
});
```

Sweet spot: streaming chat and copilot UIs, where its UI integration is unmatched; product teams that need a good-enough agent loop embedded in a Next.js route rather than an orchestration platform; provider portability at the model-call level.
Weaknesses: it is an application SDK, not an orchestration framework, so multi-agent topologies, durable execution, and complex state are out of scope by design; its gravitational field pulls toward Vercel's hosting platform; and the provider abstraction pays the lowest-common-denominator tax that Chapter 06 dissects.

## Cross-cutting comparison

| Framework | Core bet | Buy it for | Its tax |
|-----------|----------|------------|---------|
| CrewAI | Agents as role-playing teammates | Accessible multi-step pipelines | Opaque prompts, token burn, persona magical thinking |
| MS Agent Framework | Enterprise consolidation of AutoGen + SK | Microsoft estates, support horizon | Churned lineage, enterprise weight |
| smolagents | Code actions beat JSON actions | Research, learning, tool-dense tasks | Sandboxing burden, thin production story |
| Pydantic AI | Types are the interface | Production Python components | Thin orchestration, young patterns |
| Google ADK | Hierarchical agents in the GCP estate | Gemini + Vertex shops | Weak pull outside Google, platform risk |
| Mastra | TS deserves a native framework | Full-stack TS product teams | Youth, kitchen-sink risk |
| Vercel AI SDK | The model call lives in the web app | Streaming UIs, embedded loops | Not an orchestrator, LCD abstraction |

Two synthesis observations.
First, every framework in this chapter converged on the same correction between 2024 and 2026: whatever the founding ideology (roles, conversations, chains), each added explicit, typed, workflow-style control flow, because production demanded determinism where it matters; the ideologies differ, the destination is shared.
Second, every commercially backed framework attaches to a platform (Logfire, Vertex, Azure, Vercel, CrewAI's own enterprise offering), so the Chapter 01 lock-in analysis is not optional homework, it is the actual selection decision.

## Exercises

1. Implement the same two-step research-and-summarize task in CrewAI and in plain Python with two raw API calls; count tokens, wall-clock time, and lines of code, and write a paragraph on what the crew abstraction bought and cost.
2. Extract and read the actual system prompts CrewAI generates from role, goal, and backstory; rewrite them by hand in the plain-Python version and compare output quality.
3. Take one multi-tool question and run it through smolagents' CodeAgent and through a JSON-tool-calling loop; count model round trips for each and connect the difference to the code-acting thesis.
4. Build a Pydantic AI agent with an injected fake database dependency and write a unit test that runs it fully offline with a test model; this exercise is the type-safety thesis in miniature.
5. Sketch (on paper, no code) how you would express one supervisor-plus-two-specialists workload in Mastra, in ADK, and in the Microsoft Agent Framework; note which concepts map one-to-one and which do not exist in each.
6. For any two frameworks here, find the exact configuration that logs the raw request payload to the model, applying Chapter 01's transparency criterion; report how long each took to find.

## Godhood check

Answer these cold before moving on.

- What does Flows' existence concede about CrewAI's founding abstraction, and which other framework made the same concession earlier?
- Reconstruct the AutoGen lineage (original, AG2, 0.4, Agent Framework) with approximate dates and what each transition broke.
- State the code-acting thesis in one sentence and name its security cost.
- What two type parameters make a Pydantic AI agent generic, and what does each buy you at development time?
- Which two frameworks in this chapter are application-layer rather than orchestration-layer, and why does the distinction change how you evaluate them?
- Name the shared correction every framework converged on by 2026, and explain why production workloads forced it.
