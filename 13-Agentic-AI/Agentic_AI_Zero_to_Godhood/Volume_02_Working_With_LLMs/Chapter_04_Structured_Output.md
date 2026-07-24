# Chapter 04: Structured Output

## What you will master

- The reliability ladder from "please output JSON" to grammar-constrained decoding, and what each rung actually guarantees.
- JSON mode versus JSON schema / strict mode on OpenAI, and structured outputs plus strict tool use on Anthropic, with real request shapes as of early 2026.
- Constrained decoding theory: grammars, finite-state machines, token-level masking, and the engineering behind libraries like Outlines.
- The tool-calling-as-structured-output trick and when it is still the right pattern.
- Pydantic and Instructor patterns for schema definition, parsing, and validation-with-retry loops.
- The failure modes that remain even with perfect syntactic enforcement, and how to design around them.

## 1. Why structure is the load-bearing problem

Almost every serious LLM application is a text-to-structure system somewhere: extraction to JSON, classification to enums, agent steps to tool calls, UI generation to component trees.
Unstructured model output must be parsed, and parsing free text written by a stochastic generator is how you end up maintaining a museum of regexes.

The reliability ladder, from weakest to strongest:

1. Prompt-only: describe the format in words, hope.
2. Few-shot format examples: the model imitates; drift still happens under distribution shift.
3. JSON mode: the API guarantees syntactically valid JSON, nothing about its shape.
4. Schema-enforced output (strict mode / structured outputs): the API guarantees the output matches your JSON Schema, via constrained decoding.
5. Grammar-constrained decoding (self-hosted or provider grammar support): the output matches an arbitrary formal grammar, not just JSON Schema.

Each rung up removes a class of failure and adds a constraint on flexibility.
The central lesson of this chapter: climb as high as your platform allows for anything machines consume, and understand that even the top rung enforces syntax, never truth.

## 2. JSON mode

JSON mode (OpenAI `response_format={"type": "json_object"}`, and equivalents on many OpenAI-compatible providers) constrains decoding so that output is valid parseable JSON.

```python
from openai import OpenAI
import json

client = OpenAI()
resp = client.chat.completions.create(
    model="gpt-4.1",
    messages=[
        {"role": "system", "content": "Extract vendor, total, and date. Reply in JSON."},
        {"role": "user", "content": invoice_text},
    ],
    response_format={"type": "json_object"},
)
data = json.loads(resp.choices[0].message.content)  # parse cannot fail syntactically
```

What it guarantees: the string parses.
What it does not guarantee: keys you asked for exist, types are right, enums are respected, or extra keys are absent.
Two operational gotchas: OpenAI requires the word "JSON" to appear in your messages when JSON mode is on (a guard against accidental use), and truncation via `finish_reason: "length"` can still hand you an incomplete object, so check finish reasons before parsing.
JSON mode is a 2023-era tool; as of early 2026 it survives mainly for providers and models that lack full schema enforcement.

## 3. JSON Schema and strict mode

Structured Outputs (OpenAI, introduced August 2024) accepts a JSON Schema and guarantees the response conforms, using constrained decoding server-side.

```python
resp = client.chat.completions.create(
    model="gpt-4.1",
    messages=[{"role": "user", "content": invoice_text}],
    response_format={
        "type": "json_schema",
        "json_schema": {
            "name": "invoice",
            "strict": True,
            "schema": {
                "type": "object",
                "properties": {
                    "vendor": {"type": "string"},
                    "total": {"type": "number"},
                    "date": {"type": "string"},
                    "currency": {"type": "string", "enum": ["USD", "EUR", "GBP"]},
                },
                "required": ["vendor", "total", "date", "currency"],
                "additionalProperties": False,
            },
        },
    },
)
```

On the Responses API the same feature is spelled `text={"format": {"type": "json_schema", ...}}`.
The SDK also offers `client.responses.parse()` / `client.chat.completions.parse()` which accept a Pydantic model directly and return a parsed instance.

Anthropic shipped its equivalent, structured outputs via `output_config.format`, in late 2025 (public beta) with the same core idea:

```python
import anthropic

client = anthropic.Anthropic()
resp = client.messages.create(
    model="claude-sonnet-4-6",
    max_tokens=1024,
    messages=[{"role": "user", "content": invoice_text}],
    output_config={
        "format": {
            "type": "json_schema",
            "schema": {
                "type": "object",
                "properties": {
                    "vendor": {"type": "string"},
                    "total": {"type": "number"},
                    "date": {"type": "string"},
                },
                "required": ["vendor", "total", "date"],
                "additionalProperties": False,
            },
        }
    },
)
```

Anthropic additionally offers strict tool use: `strict: true` on a tool definition guarantees that `tool_use.input` validates against the tool's schema exactly, which matters enormously for agents (a malformed tool call is a wasted loop iteration at best).

Constraints common to both platforms, stated plainly because they shape schema design:

- Strict modes support a subset of JSON Schema: typically no recursive schemas (OpenAI supports limited recursion via `$ref`; Anthropic does not as of early 2026), no numeric range keywords (`minimum`, `maximum`), no string length or pattern constraints on Anthropic, and `additionalProperties: false` required on every object.
- Every field generally must be listed in `required` under OpenAI strict mode; optionality is expressed as a union with `null` rather than by omission.
- First use of a new schema pays a compilation cost (grammar construction server-side); subsequent uses hit a schema cache, so avoid gratuitously unique schemas per request.
- Enforcement covers syntax, not semantics; and unsupported constraints that an SDK strips out (both Python SDKs remove them and validate client-side) are silently your responsibility again.
- Refusals and truncations still bypass the schema: a `refusal` stop reason or a `length` finish reason yields non-conforming output, so those branches stay in your code.

Trade-off worth naming: schema enforcement slightly constrains the model's freedom to think in its output.
Forcing an answer-only schema on a task that benefits from visible reasoning can reduce quality; the standard fix is a `reasoning` string field ordered before the answer fields in the schema (field order is preserved in generation), or separating a free-form reasoning turn from a structured extraction turn.

## 4. Constrained decoding theory

How does an API guarantee schema conformance from a stochastic sampler?
By making invalid tokens unsampleable.

The mechanism, step by step:

1. Compile the target format into a machine that can answer "given what has been generated so far, which next characters are legal?"
   For regular languages this is a finite-state machine (FSM); for JSON Schema, the schema compiles to (roughly) a regular grammar over the JSON surface plus bookkeeping; for context-free grammars you carry a pushdown automaton or an incremental parser state instead.
2. Lift the character-level machine to token level: for each automaton state, precompute the set of vocabulary tokens whose full character sequence keeps the machine in legal states.
   This is subtle because tokens span multiple characters and token boundaries do not align with grammar boundaries; Outlines' core contribution (Willard and Louf, "Efficient Guided Generation for Large Language Models", 2023) was an efficient state-to-valid-token-set index making the per-step cost effectively O(1) lookup instead of scanning the vocabulary.
3. At each decoding step, take the model's logits, set the logits of all illegal tokens to negative infinity (masking), renormalize, and sample.
   The sample is now guaranteed legal by construction, and generation ends only when the automaton is in an accepting state (or budget runs out).

Libraries and engines implementing this as of early 2026: Outlines (FSM/regex/JSON Schema, integrated into vLLM), llguidance and Guidance (grammar-based, very fast masking), XGrammar (context-free grammars, used by vLLM and SGLang), llama.cpp GBNF grammars, and LM Format Enforcer.
OpenAI's strict mode is the same idea run server-side; their stated approach converts the schema to a context-free grammar and masks per step.

Properties and costs you should understand:

- Guarantee: syntactic validity is absolute, including resistance to prompt injection attempts to break format (the mask does not care what the text says).
- Distribution distortion: masking renormalizes the model's distribution over only legal tokens, which is not the same as conditioning the model on "output must be legal".
  Greedy legal continuation of an illegal intention can yield valid-but-wrong output; empirically, constrained decoding can slightly reduce task accuracy on some benchmarks (a finding reported in "Let Me Speak Freely?", Tam et al., 2024), and pairing constraints with a schema-aware prompt (tell the model the format too, do not rely on the mask alone) recovers most of it.
- Whitespace and token-boundary pathology: over-tight grammars that forbid whitespace where models like to emit it can force strange low-probability token paths; good libraries build flexible whitespace into the grammar.
- Performance: mask computation is cheap with a precompiled index, and constrained generation can even be faster (fewer tokens, no retries); compilation of large schemas is the cost to watch, hence schema caches.

The deep takeaway: constrained decoding turns the format problem into an automaton intersection problem (model vocabulary automaton intersected with format automaton) and solves it exactly.
This is a rare instance in LLM engineering where you get a hard guarantee, so exploit it wherever the platform allows.

## 5. The tool-calling-as-structured-output trick

Before native structured outputs existed, the standard trick was to define a function/tool whose parameters are your target schema and force the model to call it.
The model emits a tool call; you never execute anything; you just read the arguments as your extraction.

```python
# Anthropic version: force a tool call, harvest its input as structured data.
resp = client.messages.create(
    model="claude-sonnet-4-6",
    max_tokens=1024,
    tools=[{
        "name": "record_invoice",
        "description": "Record the extracted invoice fields.",
        "strict": True,
        "input_schema": {
            "type": "object",
            "properties": {
                "vendor": {"type": "string"},
                "total": {"type": "number"},
                "date": {"type": "string"},
            },
            "required": ["vendor", "total", "date"],
            "additionalProperties": False,
        },
    }],
    tool_choice={"type": "tool", "name": "record_invoice"},
    messages=[{"role": "user", "content": invoice_text}],
)
data = next(b.input for b in resp.content if b.type == "tool_use")
```

Why this remains relevant even now:

- It works on models and providers whose tool calling is solid but whose structured-output support is absent or weaker; tool schemas were the best-trained structured pathway for years.
- Choosing among several tools doubles as classification-plus-extraction in one call: which tool the model picked is the label, the arguments are the payload.
- In agent code you get one uniform pathway (tools) for both real actions and pure data capture, which simplifies the loop.

Downsides: it is semantically a lie (a "tool" that is not a tool), forced tool choice can suppress the model's ability to say "this input is invalid" unless you add an explicit `report_problem` tool, and on platforms with first-class structured outputs the native feature is now the cleaner and equally reliable path.
Rule of thumb as of early 2026: use native structured outputs for pure extraction, use tools when the structure is a real action or when you want the classification-by-tool-choice pattern.

## 6. Pydantic and Instructor patterns

Hand-writing JSON Schema is error-prone; in Python the ecosystem standard is to define schemas as Pydantic models and derive everything from them.

```python
from pydantic import BaseModel, Field, field_validator

class LineItem(BaseModel):
    description: str
    amount: float = Field(ge=0)  # semantic constraint; enforced by validation, not decoding

class Invoice(BaseModel):
    vendor: str
    date: str = Field(description="ISO 8601 date")
    total: float
    line_items: list[LineItem]

    @field_validator("date")
    @classmethod
    def date_is_iso(cls, v: str) -> str:
        from datetime import date
        date.fromisoformat(v)  # raises on bad format
        return v
```

Native SDK route (OpenAI): `client.responses.parse(model=..., input=..., text_format=Invoice)` returns `resp.output_parsed` as an `Invoice` instance; the SDK converts the model to strict JSON Schema, strips unsupported keywords, and validates client-side.
The Anthropic SDK's `client.messages.parse()` plays the same role against `output_config.format`.

Instructor (an open-source library layered over provider SDKs, widely used since 2023) generalizes the pattern across providers and adds automatic validation-retry:

```python
import instructor
import anthropic

client = instructor.from_provider("anthropic/claude-sonnet-4-6")

invoice = client.chat.completions.create(
    response_model=Invoice,
    max_retries=2,   # failed Pydantic validation => re-ask with the error message
    messages=[{"role": "user", "content": invoice_text}],
)
assert isinstance(invoice, Invoice)
```

The important design idea in Instructor is that Pydantic validators become part of the control loop: when validation fails, the validation error text is appended to the conversation and the model is asked to correct itself, which converts your semantic constraints (ranges, formats, cross-field checks) into feedback the model can act on.
This composes with, rather than replaces, decoding-level enforcement: let the platform guarantee syntax, let Pydantic guarantee semantics, and let retries bridge the gap.

## 7. Validation-and-retry loops

Whatever the enforcement level, production code needs an explicit loop with a bounded budget.
The canonical shape:

```python
import json
from pydantic import ValidationError

MAX_ATTEMPTS = 3

def extract_invoice(text: str) -> Invoice:
    messages = [{"role": "user", "content": PROMPT.format(text=text)}]
    last_err = None
    for _ in range(MAX_ATTEMPTS):
        raw = call_model(messages)          # ideally schema-enforced already
        try:
            return Invoice.model_validate_json(raw)
        except (json.JSONDecodeError, ValidationError) as e:
            last_err = e
            messages.append({"role": "assistant", "content": raw})
            messages.append({
                "role": "user",
                "content": f"Your output failed validation:\n{e}\nReturn a corrected JSON object only.",
            })
    raise ExtractionFailed(text=text, error=last_err)   # surface, never swallow
```

Engineering rules for this loop:

- Feed the actual validation error back; "try again" without the error wastes the retry.
- Bound attempts (2 to 3) and treat exhaustion as a first-class outcome with its own handling path (queue for human review, fall back to a stronger model, or return a typed failure); infinite retry loops against a systematically failing input are a cost incident.
- Consider a repair step before a retry: for near-miss JSON (trailing commas, fenced code blocks around the object, single quotes), a deterministic fixer (for example the `json-repair` family of libraries) is cheaper and faster than another model call.
  The trade-off is that aggressive repair can mask real model regressions, so log every repair.
- Escalation beats repetition: retrying the same model at the same settings resamples the same distribution; if attempt one and two both fail semantically, attempt three on the same model rarely differs.
  A cheaper-model-first, stronger-model-on-failure cascade usually dominates on cost and success rate.
- Log the full loop (attempts, errors, repairs, final status) as structured telemetry; your retry rate is a leading indicator of prompt or model drift.

## 8. Failure modes that survive perfect enforcement

Schema enforcement eliminates parse errors; these remain, and your design must own them:

- Semantically wrong but valid: the schema says `total: number`, the model outputs the subtotal.
  Only task-level validation (cross-field checks, reconciliation against source, evals) catches this.
- Hallucinated compliance: required fields force an answer even when the source lacks one, so the model invents a plausible vendor name rather than leaving it out.
  Fix by designing escape hatches into the schema: nullable fields, an explicit `"confidence"` or `"not_found"` enum, or a separate `report_problem` tool; a schema with no way to say "I cannot" is a hallucination press.
- Truncation: `max_tokens` hit mid-object; strict decoding cannot conjure the closing braces it was never allowed to emit.
  Check stop/finish reasons before trusting output, and budget output tokens for worst-case payloads (long arrays are the usual offender).
- Refusals and safety interventions: on Anthropic a `stop_reason: "refusal"` response will not match your schema; branch on it explicitly.
- Constraint-induced quality loss: overly rigid schemas (deeply nested, exhaustively enumerated) can measurably degrade extraction accuracy versus a looser schema plus validation; when accuracy drops after tightening a schema, suspect the schema.
- Enum coverage gaps: a closed enum forces the nearest legal label for out-of-distribution inputs; always include an `"other"` arm if the real world is open-ended, and monitor its rate.
- Unsupported-keyword illusions: `pattern`, `minimum`, `maxLength` written into a schema that the platform ignores give you documentation, not enforcement; know your platform's supported subset and validate the rest client-side.
- Schema drift between producer and consumer: the model conforms to the schema you sent, which is not automatically the schema your downstream service expects this week; generate both from one source of truth (the Pydantic model) and version it.

The compact philosophy: constrained decoding gives you syntax, validation gives you local semantics, evals give you global semantics, and only all three together give you reliability.

## Exercises

1. Build the ladder empirically: run the same extraction task 200 times at each rung (prompt-only, JSON mode, strict schema) on one provider.
   Report parse failure rate, schema violation rate, and semantic error rate separately, and observe which rung eliminates which class.
2. Implement toy constrained decoding: using a small open model via `transformers`, write a logits processor that masks tokens to enforce the regex `(yes|no)` and then a minimal JSON object grammar.
   Handle the multi-character-token problem and document where naive character-level masking breaks.
3. Reproduce the distribution-distortion effect: on a multiple-choice reasoning task, compare accuracy with unconstrained output plus parsing versus schema-constrained answer-only output versus a schema with a leading `reasoning` field.
   Explain the ordering you observe.
4. Build the forced-tool-call extractor on Anthropic with strict tool use, including a second `report_problem` tool; feed it 20 valid invoices and 10 garbage inputs, and verify the garbage lands in `report_problem` instead of hallucinated fields.
5. Wire up Instructor (or hand-roll its loop) with a Pydantic model containing three semantic validators (date format, non-negative totals, line items summing to total within tolerance).
   Measure how often retry-with-error-feedback fixes a failure versus escalation to a stronger model.
6. Design and stress-test the escape hatch: give your schema nullable fields plus a `not_found` marker, then run inputs that genuinely lack fields; measure hallucinated-compliance rate with and without the escape hatch.

## Godhood check

You have mastered this chapter when you can do the following without reference:

- Recite the reliability ladder and state, for each rung, exactly what is and is not guaranteed, including the truncation and refusal bypasses at the top rung.
- Explain token-level constrained decoding end to end: schema to grammar to automaton, the character-to-token lifting problem, the Outlines indexing idea, and the logit-masking step, plus why the result can distort the model's distribution.
- Write from memory the strict-mode request shape on OpenAI and the `output_config.format` shape on Anthropic, and name five JSON Schema keywords that strict modes typically do not enforce.
- Argue when tool-calling-as-structured-output still beats native structured outputs, and design the two-tool (extract, report_problem) pattern.
- Design a production extraction pipeline: schema-enforced call, deterministic repair, validation with error-feedback retry, bounded attempts, model escalation, typed failure, full telemetry, and defend each stage's budget.
- List six failure modes that survive perfect syntactic enforcement and the specific countermeasure for each, with hallucinated compliance and its escape-hatch fix first among them.
