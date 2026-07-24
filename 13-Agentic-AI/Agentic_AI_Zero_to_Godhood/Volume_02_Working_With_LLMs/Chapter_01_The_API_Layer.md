# Chapter 01: The API Layer

## What you will master

- The anatomy of a chat completion request: messages, roles, and system prompts, and what the server actually does with them.
- How chat templates turn a structured message list into a flat token sequence, and why this matters for debugging.
- The three dominant API shapes as of early 2026: Anthropic Messages API, OpenAI Chat Completions, and OpenAI Responses API.
- The stateless versus stateful API distinction and its consequences for cost, caching, and agent design.
- Streaming over Server-Sent Events, including the exact event sequences both providers emit.
- Error taxonomies, retry policies, and the failure modes you must design for in production.

## 1. The mental model: an API call is a rendered prompt plus a sampler

Every hosted LLM API, regardless of vendor, does the same three things.
First, it renders your structured request (messages, tools, system prompt) into a single flat token sequence using a chat template.
Second, it runs the model forward over that sequence and samples output tokens one at a time.
Third, it parses the raw output stream back into structured content (text blocks, tool calls, stop reasons) and returns it to you.

Everything else, including roles, JSON schemas, and tool definitions, is a serialization convention layered on top of next-token prediction.
Holding this model in your head demystifies most API behavior: token limits are sequence-length limits, "the model ignored my system prompt" is a rendering-plus-attention question, and streaming is just the sampler's output forwarded to you as it is produced.

## 2. Messages and roles

Both major providers represent a conversation as an ordered list of messages, each with a role and content.

The core roles as of early 2026:

- `user`: content attributed to the human or the calling application.
- `assistant`: content previously produced by the model, echoed back to give it conversational memory.
- `system` (Anthropic: a top-level `system` parameter, not a message): operator-level instructions with elevated authority.
- `developer` (OpenAI Responses API and newer Chat Completions models): OpenAI's renamed system role, ranked between platform-level instructions and user messages in their instruction hierarchy.
- `tool` (OpenAI) / `tool_result` content blocks inside a `user` message (Anthropic): results of tool executions fed back to the model.

Two structural rules trip people up.
Anthropic requires the first message to have role `user` and historically required strict user/assistant alternation; consecutive same-role messages are now merged into one turn, but a first-message assistant turn is still a 400 error.
OpenAI is more permissive about ordering but the model was trained on alternating conversations, so degenerate orderings degrade quality even when they are accepted.

The critical insight: roles are not security boundaries.
The model receives one token sequence in which roles are marked by special tokens.
A sufficiently adversarial user message can often override system instructions because both end up as tokens attended to by the same attention heads.
Role separation gives you a strong prior, trained into the model via instruction-hierarchy fine-tuning, not a guarantee.
Volume 11 covers the security consequences.

## 3. System prompts

A system prompt is operator-level context: persona, constraints, task framing, and tool usage policy.
Models are specifically post-trained to weight system content more heavily than user content, which is why the same instruction placed in the system prompt outperforms the identical instruction in a user turn.

Anthropic makes the system prompt a top-level request field, which enforces a clean mental separation:

```python
response = client.messages.create(
    model="claude-sonnet-4-6",
    max_tokens=1024,
    system="You are a terse SQL expert. Output only SQL, no prose.",
    messages=[{"role": "user", "content": "Top 5 customers by revenue, table 'orders'."}],
)
```

OpenAI keeps it as the first message in the list:

```python
response = client.chat.completions.create(
    model="gpt-4.1",
    messages=[
        {"role": "system", "content": "You are a terse SQL expert. Output only SQL, no prose."},
        {"role": "user", "content": "Top 5 customers by revenue, table 'orders'."},
    ],
)
```

Design consequence: because prompt caching on both platforms is prefix-based, the system prompt sits at the front of the cacheable prefix.
Keep it byte-stable across requests (no timestamps, no per-user interpolation) or you silently forfeit caching.
Volume 06 treats this in depth.

## 4. Chat templates under the hood

When you self-host, the chat template stops being an implementation detail and becomes your problem, so it is worth seeing exactly what happens.
A chat template is a rendering function (in the Hugging Face ecosystem, a Jinja2 template shipped inside `tokenizer_config.json`) that maps the message list to a token string.

Llama 3 style rendering of a two-turn exchange looks approximately like this:

```
<|begin_of_text|><|start_header_id|>system<|end_header_id|>

You are a helpful assistant.<|eot_id|><|start_header_id|>user<|end_header_id|>

What is 2+2?<|eot_id|><|start_header_id|>assistant<|end_header_id|>

```

ChatML style (used by many models including OpenAI-lineage and Qwen) looks like:

```
<|im_start|>system
You are a helpful assistant.<|im_end|>
<|im_start|>user
What is 2+2?<|im_end|>
<|im_start|>assistant
```

Note three things.
The template ends with an open assistant header; generation is literally "continue this document", and the model stops when it emits its end-of-turn token (`<|eot_id|>`, `<|im_end|>`).
The role markers are special tokens that cannot be produced by tokenizing ordinary user text, which is the mechanism that prevents trivial role spoofing; a template that renders roles as plain text (some fine-tunes do) is injectable by construction.
Tool definitions and tool calls are also rendered into this same stream, usually as JSON inside the system region or as dedicated sections, which is why enormous tool schemas consume your context window.

With Hugging Face `transformers` you can inspect the exact rendering:

```python
from transformers import AutoTokenizer

tok = AutoTokenizer.from_pretrained("meta-llama/Meta-Llama-3.1-8B-Instruct")
rendered = tok.apply_chat_template(
    [{"role": "user", "content": "hi"}],
    tokenize=False,
    add_generation_prompt=True,
)
print(rendered)
```

Debugging rule: when a self-hosted model behaves bizarrely (ignores the system prompt, emits role markers in its output, never stops), print the rendered template first.
A mismatched or missing chat template is the single most common self-hosting bug.

## 5. Anthropic Messages API

Endpoint: `POST https://api.anthropic.com/v1/messages`.
Authentication is an `x-api-key` header plus a required `anthropic-version: 2023-06-01` header.
`model`, `max_tokens`, and `messages` are required; `max_tokens` being mandatory is a deliberate design choice that forces you to think about output budget on every call.

```python
import anthropic

client = anthropic.Anthropic()  # reads ANTHROPIC_API_KEY

response = client.messages.create(
    model="claude-sonnet-4-6",  # current Sonnet as of early 2026
    max_tokens=1024,
    system="You are a concise technical writer.",
    messages=[
        {"role": "user", "content": "Explain idempotency in one paragraph."},
    ],
)

print(response.content[0].text)
print(response.stop_reason)   # "end_turn", "max_tokens", "tool_use", "refusal", ...
print(response.usage)         # input_tokens, output_tokens, cache_* fields
```

Distinctive properties of the Messages API:

- Content is a list of typed blocks (`text`, `tool_use`, `thinking`, `image`, `document`), not a single string.
  Your parsing code must iterate blocks and switch on `type`; `response.content[0].text` breaks the moment thinking or tool use is enabled.
- `stop_reason` is a first-class field you must branch on.
  Treating every response as final text is the root cause of many broken agent loops.
- Multi-turn state is entirely client-side: you re-send the full history every call.
- Extended thinking (reasoning) is exposed as `thinking` content blocks controlled by a `thinking` request parameter; on models from the 4.6 generation onward (early 2026) the recommended configuration is adaptive thinking rather than a manual token budget.

## 6. OpenAI Chat Completions

Endpoint: `POST https://api.openai.com/v1/chat/completions`.
Authentication is a `Authorization: Bearer $OPENAI_API_KEY` header.
This shape, dating to March 2023, is the de facto industry standard: nearly every other provider (Mistral, DeepSeek, Together, Groq, local servers like vLLM and Ollama) exposes an OpenAI-compatible endpoint.

```python
from openai import OpenAI

client = OpenAI()  # reads OPENAI_API_KEY

response = client.chat.completions.create(
    model="gpt-4.1",  # current general-purpose GPT as of early 2026
    messages=[
        {"role": "system", "content": "You are a concise technical writer."},
        {"role": "user", "content": "Explain idempotency in one paragraph."},
    ],
    max_completion_tokens=1024,
)

choice = response.choices[0]
print(choice.message.content)
print(choice.finish_reason)  # "stop", "length", "tool_calls", "content_filter"
print(response.usage)        # prompt_tokens, completion_tokens, total_tokens
```

Differences from Anthropic worth internalizing:

- The response is `choices[i].message` with a plain string `content`; tool calls live in a parallel `message.tool_calls` array rather than interleaved content blocks.
- `finish_reason` plays the role of `stop_reason`, with different vocabulary (`length` instead of `max_tokens`).
- `n > 1` lets you sample multiple completions in one call, which Anthropic does not offer.
- Output limit is optional and the parameter has migrated from `max_tokens` to `max_completion_tokens` on reasoning-capable models.

## 7. OpenAI Responses API

Endpoint: `POST https://api.openai.com/v1/responses`.
Introduced in March 2025, this is OpenAI's designated successor to Chat Completions and the required surface for their built-in tools (web search, file search, code interpreter, computer use) and for full reasoning-model features.

```python
from openai import OpenAI

client = OpenAI()

response = client.responses.create(
    model="gpt-4.1",
    instructions="You are a concise technical writer.",  # system-prompt equivalent
    input="Explain idempotency in one paragraph.",
)

print(response.output_text)  # SDK convenience accessor

# Stateful continuation: the server stores the conversation.
followup = client.responses.create(
    model="gpt-4.1",
    previous_response_id=response.id,
    input="Now give a REST API example.",
)
print(followup.output_text)
```

Key differences:

- `input` replaces `messages` and accepts either a plain string or a structured item list.
- `instructions` replaces the system message.
- The response is a list of typed output items (`message`, `reasoning`, `function_call`, tool results), structurally closer to Anthropic's content blocks than to Chat Completions.
- It is optionally stateful: with `store: true` (the default) the server persists the response, and `previous_response_id` chains turns without re-sending history.

## 8. Stateless versus stateful APIs

Anthropic Messages and OpenAI Chat Completions are stateless: the server retains nothing between calls, and you transmit the full conversation every time.
The Responses API (and Anthropic's separate Managed Agents surface, beta as of mid-2026) are stateful: the server holds the conversation and you send only the delta.

Trade-offs, stated explicitly:

- Statelessness gives you total control and portability.
  You can edit history, compact it, fork it, replay it against a different model, and store it under your own retention policy.
  The cost is re-transmitting and re-processing a growing prefix every turn, which is exactly the problem prompt caching exists to solve.
- Statefulness gives convenience and lets the provider optimize storage and caching internally.
  The costs are lock-in (conversation state lives in one vendor's datastore), reduced control over history editing and compaction, retention-policy coupling, and harder local debugging because you cannot see the full prompt that was actually rendered.

For agent engineering the stateless model is the more important one to master, because context management (Volume 06) presupposes that you own the message list.
A defensible default as of early 2026: build on stateless APIs with explicit history management, and adopt stateful surfaces only when you need provider-hosted tools or want the vendor to run the loop.

## 9. Streaming with Server-Sent Events

Without streaming, a 2,000-token answer at around 60 tokens per second means the user stares at nothing for over 30 seconds.
Streaming delivers tokens as they are sampled, cutting perceived latency to time-to-first-token.
Both providers stream over SSE: a long-lived HTTP response whose body is a sequence of `event:` / `data:` lines.

Anthropic's event sequence is rigidly structured and mirrors the content-block model:

```
message_start           # empty message shell with usage so far
content_block_start     # a block (text, tool_use, thinking) begins, with index
content_block_delta     # repeated; text_delta / input_json_delta / thinking_delta
content_block_stop      # block at that index is complete
message_delta           # stop_reason and output token usage
message_stop            # stream end
```

```python
with client.messages.stream(
    model="claude-sonnet-4-6",
    max_tokens=2048,
    messages=[{"role": "user", "content": "Write a haiku about queues."}],
) as stream:
    for text in stream.text_stream:
        print(text, end="", flush=True)
    final = stream.get_final_message()  # complete Message object
```

OpenAI Chat Completions streams a flatter shape: repeated `chat.completion.chunk` objects whose `choices[0].delta` carries partial content, terminated by a literal `data: [DONE]` sentinel.

```python
stream = client.chat.completions.create(
    model="gpt-4.1",
    messages=[{"role": "user", "content": "Write a haiku about queues."}],
    stream=True,
    stream_options={"include_usage": True},  # usage arrives in a final chunk
)
for chunk in stream:
    if chunk.choices and chunk.choices[0].delta.content:
        print(chunk.choices[0].delta.content, end="", flush=True)
```

The Responses API streams semantic events (`response.created`, `response.output_text.delta`, `response.completed`), which is closer to Anthropic's design and easier to build UIs against than raw deltas.

Engineering notes that matter in production:

- Tool-call arguments stream as partial JSON fragments (`input_json_delta` on Anthropic, `tool_calls[].function.arguments` deltas on OpenAI); you must accumulate the fragments and parse only when the block or message completes.
- Streams can die mid-response from network faults; you must handle a truncated stream as a failed request, and idempotency is on you.
- Always use streaming for large `max_tokens` values; both vendors' SDKs enforce or strongly recommend it because a non-streaming request that generates for many minutes will hit HTTP timeouts.
- SSE passes through some proxies badly; disable response buffering (for example `X-Accel-Buffering: no` behind nginx) or your "stream" arrives in one lump.

## 10. Errors and retries

The status codes that matter, common to both providers as of early 2026:

| Code | Meaning | Retry? |
| --- | --- | --- |
| 400 | Malformed request, invalid params, context overflow | No; fix the request |
| 401 | Bad or missing API key | No |
| 403 | Key lacks permission for model or feature | No |
| 404 | Wrong endpoint or model ID typo | No |
| 413 | Request body too large | No; shrink it |
| 429 | Rate limit (requests or tokens per minute) | Yes, with backoff; honor `retry-after` |
| 500 | Provider-side fault | Yes, with backoff |
| 503 / 529 | Overloaded (Anthropic uses 529 `overloaded_error`) | Yes, with backoff |

Both error bodies are structured JSON: Anthropic returns `{"type": "error", "error": {"type": "rate_limit_error", "message": ...}, "request_id": ...}`, and OpenAI returns `{"error": {"type": ..., "code": ..., "message": ...}}`.
Log the request ID on every failure; it is the handle support uses to trace your call.

Retry policy that works in practice:

- Retry only 408, 429, 5xx, and connection errors; never retry 4xx validation failures, which will fail identically forever.
- Use exponential backoff with jitter; synchronized retries from a fleet cause self-inflicted thundering herds.
- Honor the `retry-after` header when present rather than your own schedule.
- Cap total attempts (both official SDKs default to 2 retries) and cap total wall-clock time, because an interactive user will not wait through five backoff cycles.
- Treat a mid-stream disconnection as a retryable failure, but be aware you may be billed for tokens already generated.

Both official SDKs implement this automatically and expose typed exception classes; prefer catching `anthropic.RateLimitError` or `openai.RateLimitError` over string-matching messages, and order exception handlers from most specific to least.

```python
import anthropic

try:
    msg = client.messages.create(...)
except anthropic.RateLimitError:
    ...  # backoff/queue path
except anthropic.APIStatusError as e:
    log.error("api error", status=e.status_code, request_id=e.request_id)
    raise
except anthropic.APIConnectionError:
    ...  # network path; safe to retry
```

One subtlety: on recent Anthropic models a safety refusal is not an HTTP error at all but a 200 response with `stop_reason: "refusal"`.
Rate-limit handling therefore does not cover it; you need an explicit stop-reason branch.
Similarly, hitting the output cap surfaces as `stop_reason: "max_tokens"` (Anthropic) or `finish_reason: "length"` (OpenAI) on a successful response, and silently accepting truncated output is a classic production bug.

## 11. Choosing a shape, and insulating yourself from the choice

As of early 2026 the pragmatic guidance is:

- If you need maximum ecosystem compatibility (gateways, local models, third-party providers), the Chat Completions shape is the lingua franca.
- If you build on OpenAI's hosted tools or reasoning models, use the Responses API; OpenAI has stated Chat Completions remains supported but new capabilities land on Responses first.
- Anthropic's Messages API is the only surface for Claude and its block-structured design is the cleanest for agentic parsing.

Regardless of choice, put a thin internal boundary between your application and the provider SDK: your own `LLMClient` interface with `complete()` and `stream()` covering messages, tools, and usage.
The downside is a small abstraction tax and the risk of lowest-common-denominator design, so keep the boundary thin and let provider-specific escape hatches exist.
The payoff is that model migrations (a constant in this field) become one adapter change instead of a codebase-wide rewrite.

## Exercises

1. Using the `transformers` library, render the same three-message conversation through the chat templates of Llama 3.1 and Qwen 2.5.
   Diff the outputs and identify every special token; explain what each one prevents or enables.
2. Write a script that sends the identical prompt to Anthropic Messages and OpenAI Chat Completions, then normalizes both responses into a common dataclass (text, stop reason, input tokens, output tokens).
   This is the seed of your provider abstraction layer.
3. Implement SSE streaming against the raw Anthropic HTTP endpoint using `httpx` without the SDK.
   Parse the event stream yourself and reconstruct the final message; verify it matches `stream.get_final_message()` from the SDK.
4. Build a retry wrapper with exponential backoff plus jitter that honors `retry-after`, retries only retryable classes, and enforces a total deadline.
   Test it by pointing at a mock server that returns scripted sequences of 429s and 500s.
5. Write a multi-turn chat loop twice: once stateless against Chat Completions (you manage history) and once stateful against the Responses API with `previous_response_id`.
   Measure input tokens billed per turn in each design as the conversation grows to 20 turns, and explain the curve you observe.
6. Deliberately trigger and correctly handle each of the following: a context-overflow 400, a `max_tokens` truncation, a refusal stop reason, and a mid-stream disconnect (kill the connection with a proxy).
   Document what your code observed in each case.

## Godhood check

You have mastered this chapter when you can answer these without looking anything up:

- Given a raw rendered prompt string, identify the chat template family and explain why role-marking special tokens make user-level role spoofing hard, and under what template design it becomes easy.
- Explain why Anthropic's `max_tokens` is required while OpenAI's output cap is optional, and argue which is the better API design decision.
- List the full Anthropic SSE event sequence for a response containing thinking, text, and one tool call, in order, with the delta types inside each block.
- State precisely which HTTP statuses you retry, which you never retry, and why a 200 response can still require error handling on both platforms.
- Explain what breaks, and what gets cheaper, when you move a 20-turn agent conversation from a stateless API to a stateful one, covering caching, history editing, compaction, retention, and debuggability.
- Sketch the interface of a provider abstraction layer that supports both providers' streaming and tool calling without lowest-common-denominator loss.
