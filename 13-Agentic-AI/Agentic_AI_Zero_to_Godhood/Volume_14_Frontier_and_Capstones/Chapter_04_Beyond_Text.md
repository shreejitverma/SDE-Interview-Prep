# Chapter 04 - Beyond Text

## What you will master

- Realtime voice agents: cascaded versus native speech-to-speech architectures, where the latency floor comes from, and how interruption handling actually works.
- The engineering of a production voice agent loop: VAD, turn detection, barge-in, tool calls mid-conversation, and telephony integration.
- World models: what they are, the main research lines, and their concrete (and mostly future) relevance to agents.
- Robotics and vision-language-action (VLA) models at a survey level: the lineage, the main players, and the sim-to-real gap that dominates the field.
- Video understanding agents: what long-video comprehension requires and what works as of early 2026.
- A deployment-reality map: where multimodal agency is actually shipped and earning money versus where it remains research, date-stamped early 2026.

## 1. Why agents leave text

Everything in Volumes 01-13 assumed the agent's senses and actuators are textual: tokens in, tokens and tool calls out.
Three pressures push beyond that boundary.

First, the interfaces humans already use are not textual: phone calls, meetings, physical spaces, and video are where a large fraction of economically valuable work happens, and an agent that cannot enter those channels is locked out of the work.
Second, some tasks are irreducibly non-textual: you cannot inspect a warehouse shelf, watch a security feed, or manipulate a physical object through text descriptions without an upstream perception system that is itself the hard part.
Third, latency: conversational speech has interaction rhythms measured in hundreds of milliseconds, which breaks the batch-oriented request-response assumptions your text agents were built on and forces a different systems architecture.

The organizing claim for this chapter: multimodal agency is mostly the same agent loop you already know, wrapped in perception and actuation layers whose engineering difficulty is systematically underestimated by text-first engineers.
The loop, the tools, the context economics, and the safety model transfer; the input/output machinery, the latency budgets, and the failure modes do not.

## 2. Realtime voice agents: architectures

Two architectures dominate, and the trade-off between them is the central design decision of every voice product.

### The cascaded pipeline

Speech-to-text (ASR), then the LLM, then text-to-speech (TTS), as three separate components.

- Strengths: each component is best-of-breed and independently swappable; the LLM in the middle is your ordinary text agent, so every tool, guardrail, eval, and prompt from Volumes 03-12 works unchanged; transcripts are first-class, which simplifies logging, compliance, and debugging.
- Weaknesses: latency stacks across three systems plus network hops; paralinguistic information (tone, hesitation, emotion, overlapping speech) is destroyed at the ASR boundary, so the agent literally cannot hear sarcasm or distress; errors compound, with ASR mistakes on names and numbers poisoning everything downstream.

### Native speech-to-speech

A single model consumes audio tokens and emits audio tokens, with text optionally produced as a byproduct.
OpenAI's Realtime API with GPT-4o-class voice (2024 onward), Google's Gemini Live, and Amazon's Nova Sonic are the reference commercial systems; open-weight efforts (Moshi from Kyutai, 2024, and successors) demonstrated the architecture publicly.
Date-stamped: as of early 2026 both architectures are in serious production use, and neither has displaced the other.

- Strengths: lower floor latency because there is one model and no serialization boundaries; preserved paralinguistics in and out, enabling natural prosody, emotional tone, and mid-word interruption handling; full-duplex behavior (listening while speaking) is architecturally natural rather than bolted on.
- Weaknesses: the speech-native models trail their text siblings in reasoning and instruction-following at any given date, because speech training data and compute lag text; tool-calling and guardrail ecosystems are younger; transcripts become derived artifacts that may not exactly match what was said, which complicates compliance in regulated deployments; and you inherit a much smaller vendor menu.

The pragmatic production pattern of 2025, worth knowing because it shows up everywhere: a native speech-to-speech front model handles the conversational surface, and it delegates anything requiring deep reasoning or sensitive tool use to a text-based agent backend via tool calls, recombining the strengths of both stacks at the cost of an internal handoff seam.

## 3. The latency floor and the turn-taking problem

Human conversational turn-taking operates around 200 milliseconds of gap, with anything beyond roughly 500-800 milliseconds perceived as hesitation and anything over a second as system failure.
Your latency budget decomposes, and knowing the decomposition tells you where to spend:

- Audio capture and transport: tens of milliseconds with WebRTC or raw sockets; hundreds if you naively use HTTP request-response, which is why every serious voice stack is a streaming stack.
- Turn detection: the system must decide the user has finished speaking; a fixed silence threshold of 500-700 milliseconds is the naive approach, and it alone can consume your entire perceived-latency budget, which is why semantic turn detection (a small model predicting end-of-turn from content and prosody, shipped in commercial stacks through 2025) matters so much.
- Model time-to-first-token: the dominant model-side term; native speech models and aggressively streamed cascades both target getting the first audio out in a few hundred milliseconds while the rest of the response is still being generated.
- TTS synthesis start (cascaded only): modern streaming TTS begins emitting audio within roughly 100-200 milliseconds of receiving first text.

The floor, in practice and as of early 2026: well-engineered cascades achieve roughly 500-800 milliseconds voice-to-voice; native speech-to-speech systems achieve roughly 300-600 milliseconds; a naive cascade over HTTP sits at 2-4 seconds and is a failed product.
These numbers rot; the decomposition does not.

Two systemic complications deserve explicit callout.

Tool calls break the rhythm: a database lookup or API call mid-conversation inserts seconds into a 300-millisecond rhythm, so production agents use verbal fillers ("let me check that"), speculative prefetching, and async tool patterns where the agent continues conversing and delivers the result when it lands; this is a genuine agent-loop design change, not a cosmetic one.
Interruption (barge-in) is mandatory: users interrupt agents constantly, so the system must detect user speech during agent playback (echo cancellation so the agent does not hear itself, then VAD), stop playback within roughly 100-200 milliseconds, and - critically for the agent loop - truncate its own conversation history to what was actually heard rather than what was generated, or the model's context silently diverges from the user's reality.
That last truncation step is the detail most first-time voice engineers miss, and providers expose explicit truncation APIs for it (API shape current as of 2025).

## 4. Voice agents in production

What the deployed landscape actually looks like, date-stamped early 2026: customer-service phone automation is the largest commercial category, with vendors reporting containment rates (calls resolved without human handoff) as the core metric; outbound scheduling, ordering, and reminder calls are widespread; voice interfaces on consumer assistants are ubiquitous but mostly shallow; and healthcare intake, insurance verification, and logistics dispatch are the fast-growing verticals because they are phone-heavy, script-adjacent, and expensive to staff.

Engineering realities that differentiate voice from text agents in production:

- Telephony integration (SIP, PSTN) is its own subsystem, with 8 kHz audio narrowing ASR quality and carrier-side latency you do not control.
- Evaluation requires audio-native harnesses: simulated callers with accents, background noise, and interruptions; word-error-rate on entities (names, account numbers, medication doses) matters far more than average WER, because a single misheard digit is a failed task.
- Safety inherits everything from Volume 11 plus new surface: voice cloning and caller impersonation on the input side, and the agent's own realistic voice on the output side, which is why disclosure requirements ("this is an automated assistant") are regulatory reality in several jurisdictions as of early 2026.
- Cost structure differs: you pay per-minute for realtime model sessions and telephony simultaneously, and idle listening time is billable, so session management (when to hang up, when to hand off) is a first-order economic control.

## 5. World models

A world model is a learned simulator: a system that predicts how an environment's state evolves in response to actions.
The idea is old (Ha and Schmidhuber's World Models, 2018; the Dreamer line of latent-imagination RL agents, 2019-2023), and it became a frontier-lab priority in 2024-2025 with interactive video generators: DeepMind's Genie 2 (2024) and Genie 3 (2025) generate playable, action-conditioned environments from prompts, and the surrounding debate about whether video generators like Sora learn physics or merely learn to render plausible pixels remains genuinely unresolved as of early 2026; report both positions rather than picking one on vibes.

Why agents should care, in increasing order of speculativeness:

- Training environments: world models could manufacture unlimited, diverse, action-conditioned environments for the RL pipelines of Chapter 01, attacking the environment-scarcity bottleneck directly; this is the most concrete near-term thesis and the stated motivation behind much of the investment.
- Planning substrate: an agent with an internal simulator can roll out candidate action sequences before committing, which is model-based RL's classic promise; for embodied agents this is plausibly essential, and for text agents the analogous capability already exists in degenerate form (the model imagining "if I run this command, the test will probably fail") inside chain-of-thought.
- Sample-efficient robotics: learning policies inside a learned simulator and transferring them out, which collides with the sim-to-real gap discussed below.

The honest state as of early 2026: no production agent system you are likely to build this year has a world model in its loop; the relevance is real, forward-looking, and concentrated in labs.
Marked as speculation: whether world models become a standard agent component or remain a training-infrastructure technology is an open question on which serious people disagree.

## 6. Robotics and VLA models, at survey level

Vision-language-action models apply the foundation-model recipe to robot control: pretrain on internet-scale vision-language data, fine-tune on robot demonstration trajectories, and emit low-level actions (end-effector poses, joint commands) as tokens or via a diffusion/flow head.

The lineage to know:

- RT-1 and RT-2 (Google, 2022-2023): established that web-scale vision-language pretraining transfers to manipulation, with RT-2 coining the VLA framing.
- Open X-Embodiment and OpenVLA (2023-2024): pooled demonstration data across dozens of robot types and released open-weight VLAs, doing for robotics roughly what Llama did for text.
- pi0 and successors (Physical Intelligence, 2024-2025): flow-matching action generation over a VLM backbone, aimed at general manipulation across embodiments; the company's stated bet is a single generalist robot foundation model.
- Gemini Robotics (DeepMind, 2025) and comparable frontier-lab efforts: frontier VLMs coupled to action decoders, with embodied reasoning variants that plan in language before acting.
- Humanoid-platform efforts (Figure's Helix, Tesla's Optimus program, several Chinese platforms through 2025): high-profile, capital-intensive, and the correct engineering posture toward their demo videos is the same skepticism you apply to any capability announcement (Chapter 06).

What separates robotics from the text-agent world you know:

- Data scarcity is the binding constraint: there is no internet of robot trajectories; demonstrations are collected by teleoperation at a cost of dollars per episode, which is why cross-embodiment pooling and sim-to-real transfer dominate the research agenda.
- The sim-to-real gap: policies trained in simulation exploit simulator physics the way RL policies exploit reward bugs (Chapter 01, Section 7), and domain randomization is the standard partial remedy.
- Latency and safety are physical: control loops run at tens to hundreds of hertz, errors break objects or injure people, and there is no retry button, which makes the human-approval and guardrail patterns of Volume 11 load-bearing rather than advisory.

Deployment reality, date-stamped early 2026: economically deployed robot autonomy remains concentrated in structured environments (warehouse picking and transport, manufacturing cells, some commercial cleaning and delivery); general-purpose manipulation in unstructured homes and workplaces is pilot-stage; humanoids are in small factory pilots with public claims well ahead of verified productivity data.
Marked as speculation: the field's own leaders disagree publicly on whether general home robotics is a few years out or well over a decade out; hold both dates loosely.

## 7. Video understanding agents

Video is the perception modality most likely to enter your agent systems before robotics does, because the actuator side stays textual (reports, alerts, edits) while only the input side changes.

What long-video comprehension requires, and why it is not "images plus more frames":

- Token economics: naive frame sampling at one frame per second, with hundreds of visual tokens per frame, means an hour of video is on the order of a million tokens; every practical system therefore lives on an aggressive compression curve (keyframe selection, temporal pooling, hierarchical summarization), trading recall of fine-grained moments for feasibility.
- Temporal reasoning: ordering, causality, and state change across minutes ("who left the room before the alarm") stress exactly the long-range dependency machinery that per-frame captioning cannot fake.
- Retrieval within video: production systems treat long video as a corpus, indexing segments with embeddings and letting the agent retrieve-then-inspect, which is Volume 05's RAG architecture transplanted onto a new medium; this framing - agentic RAG over time-indexed segments - is the single most reusable design pattern in this section.

State of practice, date-stamped early 2026: frontier multimodal models (the Gemini line has been the most aggressive on long-video context, with the GPT and Claude lines expanding vision capability through 2025) handle multi-minute video with useful accuracy and hour-scale video through retrieval-and-sampling scaffolds; deployed applications cluster in media logging and search, meeting and lecture summarization, security and safety monitoring with human review, sports and broadcast analytics, and dashcam or fleet incident triage.
Benchmarks to know by name rather than number, because scores rot: Video-MME, EgoSchema, and LongVideoBench for long-form comprehension.

## 8. The deployment-reality map

A compressed, dated summary you can carry into planning meetings, stated with the confidence appropriate to early 2026.

Shipped and earning revenue at scale:

- Voice agents for customer service, scheduling, and intake, in both cascaded and native architectures.
- Document and image understanding inside text agents (screenshots, PDFs, charts), so standard by now that it barely reads as multimodal.
- Video summarization, search, and monitoring with humans reviewing agent output.
- Structured-environment robotics that predates the VLA wave and is increasingly retrofitted with it.

Pilot-stage, real but narrow:

- Computer-use agents driving GUIs from pixels (covered in Volume 13, and still pilot-stage for high-stakes workflows as of early 2026).
- VLA-driven manipulation in commercial settings; humanoid factory trials.
- Full-duplex voice agents handling complex multi-tool workflows end-to-end without human fallback.

Research, not deployment:

- World models in agent loops; agents planning inside learned simulators.
- General-purpose home robotics.
- Unified any-to-any models acting agentically across all modalities simultaneously; the models exist, the agentic deployments do not.

The meta-lesson, which is stable even as every entry above rots: modality expansions reach deployment in the order that their error costs are recoverable, and their latency budgets are compatible with existing infrastructure; text and vision came first because retries are free, voice came next because a bad utterance is embarrassing but recoverable, and physical actuation comes last because it is neither.

## Exercises

1. Build a cascaded voice agent from open components: streaming ASR, a text agent you built earlier in this track, and streaming TTS, connected over WebSockets; measure voice-to-voice latency at each pipeline stage and produce the decomposition table from Section 3 for your own system.
2. Add barge-in to your voice agent: implement VAD-during-playback, stop audio within 200 milliseconds, and truncate the conversation history to what was actually played; write a test that proves the context matches the audible reality after an interruption.
3. Design (on paper, at production depth) a phone-based insurance-verification agent: architecture choice with justification, latency budget, entity-accuracy strategy for policy numbers, tool-call filler strategy, handoff criteria, disclosure compliance, and the eval harness including simulated adversarial callers.
4. Take one hour of video (a recorded meeting or lecture), build a retrieval-over-segments pipeline (segment, embed, index), and put an agent on top that answers temporal questions ("what was decided after the budget discussion"); evaluate on ten questions you author before building, and report where compression destroyed the evidence.
5. Write a two-page survey memo on VLA models for a robotics-curious engineering leader: the lineage, the data bottleneck, sim-to-real, what is deployed versus piloted as of early 2026, and the three claims from vendor demos you would insist on verifying before believing.
6. Argue both sides, one page each: "world models will be standard components of production agents within five years" versus "world models will remain training infrastructure"; date-stamp your evidence and mark your own speculation as such.

## Godhood check

You are at godhood level for this chapter when you can do the following without notes.

- Draw both voice architectures, state the four strengths and weaknesses of each, and describe the hybrid front-model-plus-text-backend pattern and its handoff seam.
- Decompose voice-to-voice latency into its four components with rough magnitudes, explain semantic turn detection, and specify correct barge-in handling including history truncation.
- Explain how tool calls break conversational rhythm and give three production mitigations.
- Define a world model, name the research lineage from Dreamer to Genie 3, and state the three relevance theses for agents in increasing order of speculativeness.
- Sketch the VLA recipe and lineage, name the data-scarcity and sim-to-real constraints, and give the early-2026 deployment reality for robotics without inflating it.
- Explain why long video is a token-economics problem, describe the RAG-over-segments pattern, and name the long-video benchmarks.
- Reproduce the deployment-reality map's three tiers and articulate the error-cost-recoverability principle that orders them.
