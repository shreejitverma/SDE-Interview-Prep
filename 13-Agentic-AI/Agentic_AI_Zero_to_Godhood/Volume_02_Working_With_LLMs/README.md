# Volume 02: Working With LLMs

How to actually drive a large language model over an API: the request layer, the sampler, the prompt, the output contract, the vector space, the window, and the modalities.
This volume turns the foundations of Volume 01 into working engineering practice; everything later in the track (tools, agents, RAG, evals) builds on these mechanics.
API shapes and model references are current as of early 2026 and date-stamped where they will rot.

## Chapters

- [Chapter 01: The API Layer](Chapter_01_The_API_Layer.md) - Messages, roles, system prompts, and chat templates under the hood; Anthropic Messages versus OpenAI Chat Completions versus Responses API; stateless versus stateful design, SSE streaming, and error-and-retry engineering.
- [Chapter 02: Sampling and Decoding](Chapter_02_Sampling_and_Decoding.md) - The logits-to-token pipeline; temperature, top-k, top-p, and min-p; logprobs and the systems you can build on them; penalties, why temperature 0 is not determinism, and sampling recipes per task type.
- [Chapter 03: Prompt Engineering](Chapter_03_Prompt_Engineering.md) - Prompts as programs in natural language: zero-shot, few-shot, and chain-of-thought with evidence; roles, XML and Markdown structure, an anti-pattern catalog, the eval loop, and prompt versioning discipline.
- [Chapter 04: Structured Output](Chapter_04_Structured_Output.md) - The reliability ladder from JSON mode to strict schemas; constrained decoding theory (grammars, FSMs, Outlines); the tool-calling trick, Pydantic and Instructor patterns, validation-and-retry loops, and the failures that survive perfect syntax.
- [Chapter 05: Embeddings](Chapter_05_Embeddings.md) - What embeddings are and the contrastive training that shapes them; similarity measures and calibration; Matryoshka dimensionality; dedup, clustering, routing, semantic caching, and drift detection beyond RAG; model selection as of early 2026.
- [Chapter 06: Context Windows](Chapter_06_Context_Windows.md) - What the window physically is; the cost and latency economics of long context; needle-in-a-haystack and its limits, lost-in-the-middle, and context rot; effective versus advertised context, long context versus retrieval, and context-budget engineering.
- [Chapter 07: Multimodality](Chapter_07_Multimodality.md) - How images become tokens; document and PDF understanding with citations; audio pipelines versus native and realtime speech; image generation in brief; screenshots for computer use, chart reading, and the constraints that decide what ships.
