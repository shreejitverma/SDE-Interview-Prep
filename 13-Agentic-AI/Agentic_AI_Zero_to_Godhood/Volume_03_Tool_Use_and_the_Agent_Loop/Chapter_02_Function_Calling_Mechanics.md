# Chapter 02 - Function Calling Mechanics

## What you will master

- What actually happens at the token level when a model "calls a function", and why the model never executes anything.
- How tool definitions are injected into the prompt, what they cost, and how models were trained to emit structured calls.
- JSON Schema as the contract language for tool inputs, including the subset that matters and the parts to avoid.
- The complete Anthropic Messages API wire format for tool use: request, response, tool_result, and the stop-reason state machine.
- The complete OpenAI wire formats: Chat Completions function calling and the Responses API shape, with real JSON for both.
- tool_choice modes, forced calls, parallel tool calls, and strict schema validation on both providers.
- The prompt-caching and token-economics consequences of how tools are rendered.

All wire formats in this chapter are current as of early 2026 and are date-stamped because provider APIs drift; verify against live provider docs before building on a detail.

## 2.1 The mechanics, demystified

Function calling is the most mystified feature in the LLM API surface, so start with what it is not.

The model does not execute functions.
The model does not have a runtime, a network connection to your tools, or any awareness of whether you ran anything.
The provider does not call your endpoints.

What actually happens is a three-step text protocol between you and a next-token predictor.

Step one: you send the model a list of tool definitions alongside the conversation.
The provider renders those definitions into the model's context as text, in a format the model was trained to recognize, typically in a reserved region near the system prompt.

Step two: the model, instead of ending its response with prose, emits a span of tokens that the provider's serving stack recognizes as a structured call: a tool name plus a JSON object of arguments.
This span is just sampled tokens like any other output.
The model learned during post-training that when a task needs external action, emitting this format is the high-reward continuation.
The provider parses the span out of the raw output and hands it to you as structured data, along with a stop reason telling you the model is waiting.

Step three: you execute whatever the call means in your system, serialize the result to text or structured content, and append it to the conversation as a new message.
Then you call the model again with the grown conversation.
From the model's perspective, the tool result is simply more context that appeared, exactly as if a very fast collaborator had pasted it into the chat.

Every capability in this volume - agents, loops, recovery, control flow - is built on this one primitive: the model emits intent as structured text, you execute, you append the observation, you sample again.

Two consequences follow immediately and are worth internalizing now.

First, the API is stateless: each request carries the entire conversation, and "the agent's memory" is nothing more than the messages array you maintain.
Second, everything the model believes about the result of its actions comes from the text you return; the quality of that text is the quality of the model's perception, which is why Chapter 4 treats tool output design as a first-class discipline.

## 2.2 The token-level view

Understanding where the tokens go pays off in cost control and cache design, so descend one level.

When your request includes a `tools` array, the provider does not pass the JSON through untouched.
It renders each definition - name, description, and schema - into a textual representation in the model's expected format, and prepends it in a defined position.
For the Anthropic API the render order is tools, then system prompt, then messages, which matters for prompt caching because caching is a strict prefix match.

This rendering has three practical implications.

First, tool definitions cost input tokens on every single request, whether or not any tool is called.
A tool surface of a few dozen richly described tools can run to thousands of tokens, so the marginal tool has a real per-turn price, and large tool libraries motivate deferred loading and tool-search patterns covered later in the track.

Second, because tools render at the front of the prompt, adding, removing, or reordering a tool invalidates any prompt cache for the whole conversation.
Keep the tool list stable and deterministically ordered across a session; serialize it once and reuse the object.

Third, the model reads descriptions as text, not as code.
There is no compiler enforcing that a description matches behavior; every word is prompt engineering aimed at the decision "when should I emit a call to this".

How does the model reliably produce syntactically valid calls.
Two mechanisms stack.
Post-training teaches the format: models are fine-tuned on large volumes of tool-use trajectories, so the special tokens delimiting a call and the JSON inside are deeply learned distributions.
Constrained decoding hardens it: when you opt into strict mode, the serving stack compiles your JSON Schema into a grammar and masks invalid tokens at each sampling step, making schema-invalid output impossible rather than unlikely.
On the Anthropic API this is `strict: true` on the tool definition; on OpenAI it is `"strict": true` inside the function definition; both require `additionalProperties: false` and fully specified objects, and both are current as of early 2026.

Strictness has a trade-off: grammars constrain syntax, not sense.
A strictly valid call can still name the wrong file, pass a plausible-but-fabricated ID, or satisfy the schema with semantically empty values.
Validation layers above the schema remain your job, and Chapter 5 covers them.

## 2.3 Tool schemas: JSON Schema as the contract

Both major providers use JSON Schema to describe tool inputs.
A definition has three parts: a name, a natural-language description, and an input schema.

```json
{
  "name": "get_weather",
  "description": "Get the current weather for a city. Call this when the user asks about present conditions; do not use it for forecasts.",
  "input_schema": {
    "type": "object",
    "properties": {
      "city": {
        "type": "string",
        "description": "City name, optionally with country, e.g. 'Paris, France'"
      },
      "unit": {
        "type": "string",
        "enum": ["celsius", "fahrenheit"],
        "description": "Temperature unit; defaults to celsius if omitted"
      }
    },
    "required": ["city"]
  }
}
```

The subset of JSON Schema that works reliably across providers as of early 2026: object, array, string, integer, number, boolean, null, `enum`, `const`, `required`, `description` at every level, and `anyOf` for unions.
String formats such as `date-time`, `email`, and `uuid` are accepted, with enforcement varying by provider and mode.

The parts to treat with suspicion: numeric range constraints (`minimum`, `maximum`), string length constraints, recursive schemas, and elaborate conditional schemas.
In strict modes these are frequently unsupported outright; outside strict modes they are rendered as hints the model may ignore.
Enforce such constraints in your executor and return violations as observations instead.

Schema design guidance that consistently pays off:

- Put a description on every property, not just the tool; the model reads them all when constructing arguments.
- Use `enum` wherever the value set is closed; it converts a hallucination surface into a multiple-choice question.
- Keep `required` honest: everything the executor cannot default must be required, and nothing else.
- Prefer flat objects over nesting; each level of nesting measurably increases malformed-argument rates on complex calls.
- Name parameters from the model's perspective, not your database's: `city` beats `loc_id_fk`.

## 2.4 The Anthropic wire format

The Anthropic Messages API expresses the whole protocol through content blocks in a single endpoint, `POST /v1/messages`.
The following is a complete, real round trip, current as of early 2026.

### Request 1: user question plus tools

```json
{
  "model": "claude-opus-4-8",
  "max_tokens": 1024,
  "tools": [
    {
      "name": "get_weather",
      "description": "Get the current weather for a city.",
      "input_schema": {
        "type": "object",
        "properties": {
          "city": {"type": "string", "description": "City name"}
        },
        "required": ["city"]
      }
    }
  ],
  "messages": [
    {"role": "user", "content": "What's the weather in Paris right now?"}
  ]
}
```

### Response 1: the model emits a tool call

```json
{
  "id": "msg_01Aq9w938a90dw8q",
  "type": "message",
  "role": "assistant",
  "model": "claude-opus-4-8",
  "content": [
    {
      "type": "text",
      "text": "I'll check the current weather in Paris."
    },
    {
      "type": "tool_use",
      "id": "toolu_01A09q90qw90lq91",
      "name": "get_weather",
      "input": {"city": "Paris"}
    }
  ],
  "stop_reason": "tool_use",
  "usage": {"input_tokens": 383, "output_tokens": 61}
}
```

Read the anatomy carefully, because your loop code branches on every field here.

`stop_reason: "tool_use"` is the signal that the model has paused and is waiting for results; your loop dispatches on this value.
`content` is an array of typed blocks, and text and tool_use blocks can interleave; the text is the model narrating, and you typically surface it to the user.
The `tool_use` block carries a unique `id` that you must echo back, a `name` to dispatch on, and `input` as an already-parsed JSON object.
Always treat `input` as parsed data; never string-match against its serialization, because escaping is not guaranteed stable across model versions.

### Request 2: return the result

You append the assistant message verbatim, then append a user message containing a `tool_result` block.

```json
{
  "model": "claude-opus-4-8",
  "max_tokens": 1024,
  "tools": [ ... same tools array ... ],
  "messages": [
    {"role": "user", "content": "What's the weather in Paris right now?"},
    {
      "role": "assistant",
      "content": [
        {"type": "text", "text": "I'll check the current weather in Paris."},
        {
          "type": "tool_use",
          "id": "toolu_01A09q90qw90lq91",
          "name": "get_weather",
          "input": {"city": "Paris"}
        }
      ]
    },
    {
      "role": "user",
      "content": [
        {
          "type": "tool_result",
          "tool_use_id": "toolu_01A09q90qw90lq91",
          "content": "18°C, overcast, light rain expected within the hour."
        }
      ]
    }
  ]
}
```

Three rules here generate most beginner bugs when violated.

You must append the assistant message with its content blocks unmodified; dropping the tool_use block or reformatting it produces a 400, because the tool_result would reference an id that no longer exists in context.
The `tool_use_id` must match exactly; this is how the model pairs results with calls when several are in flight.
The tool result arrives in a `user`-role message, because in this protocol the "user" turn is simply "everything the environment says to the model", which includes both humans and tool outputs.

### Response 2: the model answers

```json
{
  "id": "msg_01B7xk2m4n5p6q7r",
  "type": "message",
  "role": "assistant",
  "content": [
    {
      "type": "text",
      "text": "It's currently 18°C and overcast in Paris, with light rain expected within the hour."
    }
  ],
  "stop_reason": "end_turn",
  "usage": {"input_tokens": 471, "output_tokens": 34}
}
```

`stop_reason: "end_turn"` ends the exchange; the loop in Chapter 3 is little more than "while stop_reason is tool_use, execute and append".

### Errors as tool results

When execution fails, do not raise to the user and do not silently drop the call; return the failure as an observation with `is_error` set.

```json
{
  "type": "tool_result",
  "tool_use_id": "toolu_01A09q90qw90lq91",
  "content": "Error: unknown city 'Pariss'. Did you mean 'Paris'?",
  "is_error": true
}
```

The model reads this and typically self-corrects on the next turn, which is the foundational recovery pattern of Chapter 5.

### Other stop reasons you must handle

`max_tokens` means the response was truncated by your output cap, possibly mid-call.
`refusal` means safety systems declined; check `stop_reason` before reading content.
`pause_turn` appears with server-side tools when the provider's internal loop pauses; you resend the conversation to resume.
A loop that only knows `tool_use` and `end_turn` is a loop that will wedge in production.

## 2.5 tool_choice: who decides whether to call

By default the model chooses freely whether to answer in prose or call tools.
Both providers expose a control to override this.

Anthropic `tool_choice` values, as of early 2026:

```json
{"type": "auto"}
{"type": "any"}
{"type": "tool", "name": "get_weather"}
{"type": "none"}
```

`auto` is the default free choice.
`any` forces at least one tool call, with the model choosing which; use it when a prose answer is never acceptable, such as in extraction pipelines.
`tool` forces a specific tool, which turns the call into a structured-output mechanism: force a `record_answer` tool and you have schema-guaranteed extraction.
`none` forbids calls while leaving definitions visible in context, useful for a final summarization turn over a tool-using transcript.

Any of these may carry `"disable_parallel_tool_use": true`, which caps the model at one call per response.

OpenAI's equivalent parameter is also named `tool_choice`, with values `"auto"`, `"required"` (equivalent to Anthropic's `any`), `"none"`, and a forcing form `{"type": "function", "function": {"name": "get_weather"}}` in Chat Completions.
The semantic mapping is one-to-one; only the spelling differs.

One caution: forcing tools on models that also run extended thinking has provider-specific interactions and restrictions, so consult current provider docs when combining the two rather than assuming composability.

## 2.6 Parallel tool calls

Models emit multiple tool calls in a single response when the calls are independent, and your harness should be built for this from day one.

On the Anthropic API, parallelism appears as multiple `tool_use` blocks in one assistant message:

```json
{
  "role": "assistant",
  "content": [
    {"type": "tool_use", "id": "toolu_01AAA", "name": "get_weather", "input": {"city": "Paris"}},
    {"type": "tool_use", "id": "toolu_01BBB", "name": "get_weather", "input": {"city": "London"}}
  ],
  "stop_reason": "tool_use"
}
```

The contract for responding has one rule that is easy to violate and expensive when violated: return all results as `tool_result` blocks inside a single user message, in any order, keyed by id.

```json
{
  "role": "user",
  "content": [
    {"type": "tool_result", "tool_use_id": "toolu_01AAA", "content": "18°C, overcast"},
    {"type": "tool_result", "tool_use_id": "toolu_01BBB", "content": "15°C, drizzle"}
  ]
}
```

Splitting results across multiple user messages does not error; it does something worse, which is silently teach the model within the session that parallel calls come back awkwardly, degrading its willingness to parallelize.
Failed calls in a batch still get a result block with `is_error: true`; never omit one.

Whether you execute the batch concurrently is your choice; the protocol only requires that all results come back together.
Concurrent execution is safe for read-only tools and needs care for tools that mutate shared state, which is one of the arguments in Chapter 4 for telling the harness which tools are read-only.

## 2.7 The OpenAI wire formats

OpenAI shipped function calling in June 2023 and its Chat Completions shape became the de facto industry standard that most open-model serving stacks imitate.
As of early 2026 OpenAI maintains two API surfaces: Chat Completions, the legacy-but-ubiquitous form, and the Responses API, the recommended newer form.
You will encounter both; learn both.

### Chat Completions: request

```json
{
  "model": "gpt-5",
  "messages": [
    {"role": "user", "content": "What's the weather in Paris right now?"}
  ],
  "tools": [
    {
      "type": "function",
      "function": {
        "name": "get_weather",
        "description": "Get the current weather for a city.",
        "parameters": {
          "type": "object",
          "properties": {
            "city": {"type": "string", "description": "City name"}
          },
          "required": ["city"],
          "additionalProperties": false
        },
        "strict": true
      }
    }
  ],
  "tool_choice": "auto"
}
```

Note the differences from Anthropic: each tool is wrapped in `{"type": "function", "function": {...}}`, the schema key is `parameters` rather than `input_schema`, and `strict` lives inside the function object.

### Chat Completions: response

```json
{
  "id": "chatcmpl-abc123",
  "object": "chat.completion",
  "model": "gpt-5",
  "choices": [
    {
      "index": 0,
      "message": {
        "role": "assistant",
        "content": null,
        "tool_calls": [
          {
            "id": "call_9x7ab2",
            "type": "function",
            "function": {
              "name": "get_weather",
              "arguments": "{\"city\": \"Paris\"}"
            }
          }
        ]
      },
      "finish_reason": "tool_calls"
    }
  ]
}
```

The structural differences from Anthropic matter for harness code.

Calls live in a dedicated `tool_calls` array on the message, not interleaved as content blocks, and `content` is often null alongside them.
`arguments` is a JSON-encoded string, not a parsed object; you must `json.loads` it yourself, and you must handle the case where the string fails to parse, which strict mode eliminates but default mode does not.
The loop signal is `finish_reason: "tool_calls"` rather than a stop_reason.

### Chat Completions: returning results

Results go back as messages with the dedicated role `tool`, one message per call, each keyed by `tool_call_id`.

```json
{
  "messages": [
    {"role": "user", "content": "What's the weather in Paris right now?"},
    {
      "role": "assistant",
      "content": null,
      "tool_calls": [
        {"id": "call_9x7ab2", "type": "function",
         "function": {"name": "get_weather", "arguments": "{\"city\": \"Paris\"}"}}
      ]
    },
    {"role": "tool", "tool_call_id": "call_9x7ab2", "content": "18°C, overcast"}
  ]
}
```

Contrast with Anthropic: a distinct `tool` role instead of tool_result blocks inside a user message, and one message per result instead of one message carrying all results.
There is no `is_error` flag in this shape; the convention is to put error text in `content` and let the model infer, which is strictly less structured than Anthropic's design.

### The Responses API shape

The Responses API, OpenAI's recommended surface as of early 2026, flattens the conversation into a list of typed items and un-nests the tool definition.

Tool definition: `{"type": "function", "name": "get_weather", "description": ..., "parameters": {...}, "strict": true}` with no inner `function` wrapper.
A call arrives as an output item: `{"type": "function_call", "call_id": "call_9x7ab2", "name": "get_weather", "arguments": "{\"city\": \"Paris\"}"}`.
You reply by appending an input item: `{"type": "function_call_output", "call_id": "call_9x7ab2", "output": "18°C, overcast"}`.
The Responses API can also manage conversation state server-side via a `previous_response_id`, relieving you of resending history, which is a genuine architectural difference from both Chat Completions and the Anthropic API rather than a spelling difference.

### Provider comparison table

| Concern | Anthropic Messages API | OpenAI Chat Completions | OpenAI Responses API |
|---|---|---|---|
| Tool definition | flat: name, description, input_schema | nested under type: function | flat, with parameters key |
| Call in response | tool_use content block, parsed input | tool_calls array, arguments as JSON string | function_call item, arguments as JSON string |
| Result message | tool_result block in a user message | role: tool message per call | function_call_output item per call |
| Loop signal | stop_reason: tool_use | finish_reason: tool_calls | function_call item present in output |
| Error signaling | is_error flag on tool_result | convention only | convention only |
| Parallel results | all blocks in one user message | one tool message per call | one output item per call |
| State | stateless, client resends history | stateless | optional server-side state |

If you build a provider-abstraction layer, these are the seven rows it must normalize, and the arguments-as-string versus parsed-object row is where the first bug will live.

## 2.8 Token economics and caching

Close the chapter where it started: everything above is tokens, and tokens are money and latency.

Tool definitions are paid on every request in the loop, so an agent that takes thirty turns pays for its tool surface thirty times; prompt caching exists precisely to make those repeated tokens nearly free, and stable prefixes are what make caching work.
The practical rules: freeze the tools array for the life of a session, keep it deterministically serialized, place cache breakpoints after the stable prefix, and verify cache hits by reading the usage fields on responses rather than assuming.

Tool results are input tokens on every subsequent turn, forever, because the transcript is resent each time.
A single verbose 20,000-token tool result does not cost you once; it costs you on every remaining turn of the trajectory, which is the economic argument behind Chapter 4's obsession with concise observations and truncation.

Argument emission is output tokens, which are the expensive kind.
Tools that force the model to emit large payloads, such as a write_file tool taking full file content as an argument, concentrate cost and latency in output; designs that let the model emit a diff instead of a file are cheaper for the same effect.

These three flows - definitions in, observations in, arguments out - are the complete cost model of tool use, and you should be able to estimate each before deploying an agent.

## Exercises

1. Write the full JSON for a four-message Anthropic conversation in which the model calls two tools in parallel, one succeeds, one fails with is_error, and the model recovers; validate your message structure against the rules in section 2.4.
2. Translate that exact conversation into Chat Completions format, then into Responses API format, and list every field you had to change.
3. Design an input schema for a `search_flights` tool with origin, destination, date, and cabin class, using enums and per-property descriptions; then list which constraints you deliberately left to executor-side validation and why.
4. Using curl or an HTTP client, run a real forced-tool-choice request against a provider and confirm the response shape matches this chapter; note anything that differs and check the provider changelog for it.
5. Estimate the total input-token cost of a 25-turn trajectory with a 3,000-token tool surface and average 500-token results, with and without an effective prompt cache, showing your arithmetic.
6. Strict mode guarantees schema validity; write down five semantically wrong calls that strict mode cannot prevent for your search_flights tool, and the validation layer that catches each.

## Godhood check

You have mastered this chapter when you can do the following without reference material.

- Narrate the three-step protocol precisely enough that a colleague could implement a harness from your narration alone.
- Write a syntactically correct Anthropic tool-use round trip, including error results and parallel calls, from memory.
- Write the Chat Completions equivalent from memory and name every structural difference between the two, including the arguments-as-string trap.
- Explain the mechanism and the limits of strict schema enforcement, and give examples of failures it cannot prevent.
- Choose the correct tool_choice mode for extraction, agentic, and summarization workloads, and explain each choice.
- State the three token flows of tool use and predict which one dominates cost for a given tool design before running it.
