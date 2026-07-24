# Chapter 07: Multimodality

## What you will master

- How vision-language models ingest images: patching, vision encoders, resolution tiling, and image token accounting on both major APIs.
- Document and PDF understanding: the text-plus-render pipeline, when native PDF support beats your own OCR, and its limits.
- Audio in and out: transcription pipelines versus native audio models versus realtime speech APIs, and how to choose.
- Image generation in brief: where it sits in the API landscape and what an agent engineer needs from it.
- Multimodal agent use cases: screenshots for computer use, chart and dashboard reading, UI verification, and document workflows.
- The practical constraints (resolution, token cost, latency, hallucination modes) that decide whether a multimodal design ships.

## 1. How images become tokens

Modern vision-language models (VLMs) reuse the transformer by converting an image into a sequence of embeddings that sits in the same stream as text tokens.

The standard pipeline, descended from ViT (Dosovitskiy et al., 2020) through CLIP-style pretraining into today's frontier models:

1. Preprocess: the image is resized and possibly split; high-resolution handling usually means cutting the image into tiles processed at the encoder's native resolution, plus a downscaled global view for overall layout.
2. Patchify: each tile is divided into fixed-size patches (14x14 or 16x16 pixels are typical); each patch is linearly projected into an embedding, giving a grid of "image tokens".
3. Encode: a vision transformer processes the patch grid; a projection layer (linear, MLP, or a resampler that also compresses token count) maps the output into the language model's embedding space.
4. Interleave: the resulting image embeddings are spliced into the token sequence at the position of the image content block, and the language model attends over text and image tokens jointly.

Consequences you can predict from this design:

- Images are expensive in tokens, and cost scales with resolution.
  Anthropic's documented estimate is `tokens ~= (width * height) / 750`, with images capped (long edge limits, and downscaling applied above roughly 1.15 megapixels on most models as of early 2026; the newest generation raised the cap to a 2576-pixel long edge at proportionally higher token cost).
  OpenAI's detail-tiered accounting charges a base cost plus a per-512px-tile cost at high detail, with a `detail: "low"` mode that processes only a small fixed-size view for a flat low price.
- Fine detail lives or dies by resolution: text in a screenshot smaller than roughly 8 to 10 pixels of character height is unreadable after downscaling, which is the root cause of most "the model misread my dashboard" bugs.
  The fix is cropping to the region of interest rather than sending one huge image.
- Patch granularity limits spatial precision: the model perceives in patch-sized cells, so pixel-exact localization is inherently approximate; models trained for computer use output coordinates, but you should expect small-target clicks to be the weak point.
- Orientation and quality sensitivity: rotated, blurry, or heavily compressed images degrade sharply; some current models are explicitly trained to invoke cropping and image-processing tools on degraded inputs rather than answering directly.

Request shapes, both current as of early 2026.
Anthropic accepts base64 or URL image sources as content blocks:

```python
import anthropic, base64, httpx

client = anthropic.Anthropic()
img_b64 = base64.standard_b64encode(httpx.get(IMG_URL).content).decode()

resp = client.messages.create(
    model="claude-sonnet-4-6",
    max_tokens=1024,
    messages=[{
        "role": "user",
        "content": [
            {"type": "image",
             "source": {"type": "base64", "media_type": "image/png", "data": img_b64}},
            {"type": "text", "text": "What error is shown in this screenshot, and on which line?"},
        ],
    }],
)
```

OpenAI Chat Completions uses `image_url` parts (data URLs for local files), with the `detail` knob:

```python
from openai import OpenAI

client = OpenAI()
resp = client.chat.completions.create(
    model="gpt-4.1",
    messages=[{
        "role": "user",
        "content": [
            {"type": "image_url",
             "image_url": {"url": f"data:image/png;base64,{img_b64}", "detail": "high"}},
            {"type": "text", "text": "Transcribe the table in this image as Markdown."},
        ],
    }],
)
```

Ordering matters more than people expect: placing images before the question, and labeling multiple images ("Image 1:", "Image 2:") in the interleaved text, measurably improves grounding on multi-image prompts; both vendors document this.

## 2. Document and PDF understanding

Documents are the highest-value multimodal workload in enterprises, and the naive approach (run your own OCR, feed the text) throws away exactly what makes documents hard: layout, tables, figures, stamps, and handwriting.

Native PDF support, available on both platforms as of early 2026, processes each page twice: the extracted text layer and a rendered page image both go into context, so the model can read the text precisely while seeing the layout.
Anthropic's shape is a `document` content block (base64, URL, or an uploaded file ID), with limits on request size and page count (hundreds of pages per request, fewer on smaller-context models); OpenAI accepts PDFs as `input_file` parts on the Responses API with the same text-plus-image treatment.

```python
resp = client.messages.create(
    model="claude-sonnet-4-6",
    max_tokens=2048,
    messages=[{
        "role": "user",
        "content": [
            {"type": "document",
             "source": {"type": "base64", "media_type": "application/pdf", "data": pdf_b64},
             "citations": {"enabled": True}},
            {"type": "text", "text": "List every payment obligation with its due date and cite the page."},
        ],
    }],
)
```

Citations (Anthropic) deserve a note: enabling them makes the model return page-anchored references for its claims, which converts "trust me" extraction into verifiable extraction, the difference that matters in legal and financial workflows.

When to use what, honestly:

- Native PDF ingestion wins for mixed text-and-layout documents at moderate volume: zero pipeline to build, layout awareness for free, and per-page token cost (text tokens plus roughly 1.5k to 3k image tokens per page) that is acceptable for interactive use.
- A dedicated parsing stage (OCR or a document-parsing model, then feed text) wins at bulk scale, where paying image tokens per page across millions of pages is untenable, and for pipelines needing deterministic intermediate artifacts (searchable text, indexed chunks) independent of any one LLM.
- Scanned-image-only PDFs work through the native path (the render carries the information) but cost the same as images and inherit resolution limits; extremely long documents exceed page caps and need splitting regardless, at which point you are building a pipeline anyway and Volume 05's chunking discipline applies.

Failure modes to design around: tables with merged cells and multi-column layouts still produce transposition errors; numbers are transcribed with high but not perfect fidelity, so reconcile totals in code; and page-cap truncation fails silently if you do not check what was actually ingested.

## 3. Audio: in and out

Three distinct architectures serve "the model hears and speaks", and conflating them is a design error.

Pipeline (ASR then LLM then TTS): transcribe with a speech-to-text model (the Whisper lineage, `gpt-4o-transcribe`, or a hosted competitor), run text through your normal LLM stack, synthesize the reply with a TTS model.
Strengths: each stage is best-of-breed and independently swappable, the middle is your existing, testable text agent, and cost is low.
Weaknesses: latency stacks up across three hops (typically well over a second before smart endpointing), and transcription discards paralinguistics (tone, emotion, emphasis, speaker overlap), so the LLM literally cannot hear sarcasm or urgency.

Native audio models: models like `gpt-4o-audio` accept audio (and emit audio) directly in a Chat Completions request; the model attends over audio representations, preserving nonverbal signal.
As of early 2026 Anthropic's API does not accept audio input, so this lane is OpenAI-and-others territory; audio tokens are billed at a substantial premium over text tokens.

Realtime speech-to-speech: OpenAI's Realtime API (WebSocket or WebRTC) streams audio both ways with server-side voice-activity detection, interruption handling, and tool calling inside the session; this is the architecture for genuinely conversational voice agents, with sub-second perceived response times.
Trade-offs: a stateful streaming session is operationally heavier than request-response, evals and logging are harder (your transcript is a reconstruction, not the ground truth the model consumed), and cost per minute is far above the pipeline approach.

Selection rule: pipeline for transcription-centric and cost-sensitive workloads (meeting notes, voicemail triage, call analytics), native audio when paralinguistic understanding changes the answer, realtime only when conversational latency is the product.
For agent engineering, the deeper point is that voice is a transport: the agent loop, tools, and context discipline from the rest of this track are unchanged underneath, and a text-first agent that also speaks is easier to build, test, and debug than a voice-native design.

## 4. Image generation, briefly

Image generation sits adjacent to agent engineering rather than inside it, but you should know the surface.
As of early 2026: OpenAI's `gpt-image-1` (the API descendant of the GPT-4o image capability) generates and edits images via the Images API and as a tool inside the Responses API; Google's Imagen and Gemini image models and open-weight diffusion families (FLUX, Stable Diffusion lineage) cover the rest of the landscape.
Anthropic offers no image generation.

What an agent engineer actually needs from this:

- Generation-as-tool: an agent that produces documents, slides, or UIs may call an image model the way it calls any tool; treat prompts to the image model as another prompt surface with its own eval discipline.
- Multimodal loops: generate, then critique with a vision model, then regenerate; this closed loop (visual self-verification) is the same pattern as code-runs-tests and is the reliable way to hit layout or brand constraints.
- Constraints: text rendering inside generated images has improved but still fails on long or small text; generation latency (seconds to tens of seconds) dominates any interactive loop it appears in; and provenance and policy handling (watermarking, C2PA metadata, content rules) are your responsibility in products.

## 5. Multimodal agent use cases

Vision is what lets agents operate in environments built for humans; these are the patterns that matter as of early 2026.

Computer use via screenshots: the agent receives a screenshot, decides an action (click coordinates, type, scroll), executes through a harness, and receives the next screenshot; both Anthropic (computer-use tool) and OpenAI (computer-use in the Responses API) ship this loop.
Engineering realities: resolution discipline dominates accuracy (1080p-class screenshots are the documented sweet spot on current models, cost roughly a few thousand image tokens each; oversized displays should be scaled or tiled), every loop iteration re-sends recent screenshots so context and cost management (Chapter 06) bind hard, and small-target precision plus latency per step (seconds) make screenshot agents the tool of last resort when an API or DOM-level interface exists.
Prefer accessibility trees, DOM access, or real APIs when available; use pixels when the environment offers nothing better, which is often.

Chart, dashboard, and figure reading: VLMs read trends, labels, and outliers from charts well, and exact values badly; a bar's height is a patch-grid estimate, not a datum.
Chart-reading benchmarks (the ChartQA lineage) show strong-but-imperfect numeric extraction, and the production pattern reflects it: use vision for structure and salience ("which region regressed"), then fetch exact numbers from the underlying data source, or have the model crop and zoom before answering.
Never let a dashboard screenshot be the system of record for a number that matters.

UI verification and visual QA: an agent that changes frontend code screenshots the result and judges it against the spec (or a reference design) before declaring success; this closes the loop that plain code review leaves open and is one of the highest-ROI uses of vision in coding agents (Volume 13).
The same pattern powers visual regression triage: diff screenshots, ask the model whether the change is intended.

Document workflows end to end: intake (photo or PDF), extraction with citations into schemas (Chapter 04 discipline applies unchanged: schema-enforced output, escape hatches, reconciliation), and human-review routing for low-confidence pages; multimodal extraction plus structured output is arguably the most economically active LLM workload in existence.

Multimodal RAG: index page renders and figures alongside text (multimodal embeddings such as Cohere embed-v4, or ColPali-style late-interaction page retrieval), retrieve pages as images, and let the VLM read the retrieved page; this preserves layout through the whole retrieval path and is covered properly in Volume 05.

## 6. Practical constraints: the decision table

The constraints that decide whether a multimodal design ships, gathered in one place:

- Token cost: a high-detail image costs hundreds to thousands of tokens; a PDF page, its text plus roughly 1.5k to 3k image tokens; a screenshot loop, thousands per step, every step.
  Multimodal token math belongs in your context budget (Chapter 06) from day one, and `detail: "low"`, cropping, and downscaling are your cost levers.
- Resolution versus readability: downscaling caps mean small text dies first; crop, tile, or zoom instead of sending the whole surface, and test with the worst real inputs (phone photos, 4k dashboards), not clean samples.
- Latency: image prefill adds hundreds of milliseconds to seconds; screenshot loops compound it per step; realtime voice has a hard sub-second budget that constrains every other design choice.
- Hallucination modes specific to vision: confident misreading of small text, invented table cells, plausible-but-wrong chart values, and OCR-style character confusions; the countermeasures are cropping, citations, code-side reconciliation of numbers, and never trusting a single read for high-stakes values.
- Capability asymmetry across providers: as of early 2026, Anthropic has vision and PDFs but no audio input and no image generation; OpenAI covers all four; requirements involving audio or generation therefore constrain provider choice before any quality comparison begins.
- Input hygiene and safety: images and documents are injection surfaces (instructions rendered inside a screenshot or PDF are read by the model just like text); treat multimodal content as untrusted data, fence it, and apply the Volume 11 discipline to it.
- Format limits: supported media types (JPEG, PNG, GIF, WebP images; PDF documents), per-request size caps, and per-request image counts differ by provider and change; validate and normalize inputs at the edge rather than discovering limits as 400s in production.

## Exercises

1. Measure the resolution cliff: render the same 40 lines of code at five font sizes, screenshot each, and measure transcription accuracy per size on one model.
   Find the character-height threshold where accuracy collapses, then show that cropping to a region restores it at lower total token cost than a full-resolution image.
2. Audit image token accounting: for a set of images at varied resolutions, predict token cost from each provider's documented formula, then verify against the API's reported usage.
   Produce a cost table for low versus high detail on OpenAI and full versus downscaled on Anthropic.
3. Build a cited-extraction pipeline: feed a 30-page contract PDF through native document support with citations enabled, extract obligations into a schema-enforced structure (Chapter 04), and verify every citation resolves to a page actually containing the claim.
   Report citation precision.
4. Bake off chart reading: take 25 charts with known underlying data; ask a VLM for (a) qualitative findings and (b) exact values.
   Score both, then add a crop-and-zoom step for values and report the improvement, and state your resulting policy for numbers from pixels.
5. Prototype a screenshot verification loop: have a coding agent modify a small web page, screenshot it headlessly, and judge the result against a written spec, iterating up to three times.
   Measure how often visual self-verification catches a defect that code inspection missed.
6. Compare voice architectures: implement voicemail triage twice, once as ASR-then-LLM pipeline and once with a native audio model; compare cost per call, end-to-end latency, and classification accuracy on calls where tone matters (angry versus routine).
   Write the selection rule your results support.

## Godhood check

You have mastered this chapter when you can do the following without notes:

- Trace an image from pixels to interleaved tokens (tiling, patching, encoding, projection) and use that mechanism to predict three failure modes: small-text illegibility, imprecise localization, and resolution-dependent cost.
- Compute approximate token cost for a given image on both providers, name the levers that reduce it, and state where multimodal costs live in a context budget.
- Explain the text-plus-render design of native PDF support, when it beats a self-built OCR pipeline and when it loses, and why citations change the trust model of extraction.
- Distinguish the three audio architectures, give the latency and signal-preservation trade-offs of each, and pick the right one for three named products without hesitation.
- Design a screenshot-based computer-use loop with correct resolution policy, per-step token accounting, and a justification for why you would still prefer a DOM or API interface where one exists.
- List five vision-specific hallucination or safety modes and the concrete countermeasure for each, including why a screenshot is an injection surface.
