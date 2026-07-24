# Chapter 05 - Computer Use

## What you will master

- The screenshot-action loop in full mechanical detail, including the state, latency, and cost model of each iteration.
- Coordinate-based clicking versus element grounding: why pixel coordinates are a hard perception problem and what changed between 2024 and 2026.
- The Anthropic computer use API lineage from the October 2024 beta through the tool versions that followed, with the concrete parameters that matter.
- OSWorld and desktop evaluation: construction, why scores were so low, and how to read desktop benchmark numbers.
- The reliability and latency realities that decide whether a computer-use deployment survives contact with production.
- A decision procedure for computer use versus API integration versus browser automation versus scripted RPA.
- Virtual desktop infrastructure: how to actually run these agents - containers, resolution choices, isolation, and fleet operation.

Date-stamp: API shapes, tool version strings, and benchmark figures describe early 2026 and will move; the loop, grounding, and decision-procedure material is durable.

## 1. What computer use is, and where it sits

Computer use is the most general agent modality: the agent sees a screen, moves a mouse, and types on a keyboard, exactly as a human does.
No API, no DOM, no accessibility contract - just pixels in and input events out.

Its position in the capability stack is worth fixing precisely.

| Modality | Observation | Action | Reliability | Coverage |
|---|---|---|---|---|
| API integration | Structured JSON | Typed calls | Highest | Only what the vendor exposes |
| Terminal/CLI | Text | Commands | High | Anything scriptable |
| Browser (semantic) | Accessibility tree | Element references | Medium | Anything on the web |
| Browser (vision) | Screenshots | Coordinates | Lower | Anything on the web, including canvas |
| Computer use | Screenshots | Coordinates, keys | Lowest | Anything a human can do on a computer |

The ordering is monotone and it is the single most useful thing in this chapter: **coverage and reliability trade off directly**.
Computer use buys universality at the cost of everything else.
It is the modality of last resort, and treating it as a first resort is the most common and most expensive mistake teams make with it.

Its legitimate uses are exactly where the alternatives are unavailable: legacy desktop applications with no API and no web interface, cross-application workflows that stitch together tools with no integration between them, QA of GUI applications, accessibility assistance, and the long tail of internal enterprise software that will never get an API.

## 2. The screenshot-action loop

The loop is simple to state and everything hard about it is in the details.

```
1. Capture a screenshot of the screen (or window).
2. Send the screenshot plus the task and history to the model.
3. The model returns an action: click(x, y), type("text"), key("cmd+s"),
   scroll(direction, amount), drag(from, to), wait(seconds), or done.
4. Execute the action against the OS input layer.
5. Wait for the UI to settle.
6. Go to 1.
```

Five properties of this loop determine everything downstream.

**Each iteration costs a full image.**
A 1080p screenshot costs on the order of 1,000 to 2,000 image tokens on 2024-era models, and up to roughly 3x that on the high-resolution vision models introduced in 2025-2026 (Opus 4.7 and later raised the long-edge maximum to 2576 pixels, with a correspondingly higher per-image token ceiling near 4,800 tokens).
A 50-step task therefore spends 50 images, and if you keep every screenshot in context the conversation grows quadratically in cost.
The standard mitigations: keep only the last N screenshots (typically 2 to 5) in full and elide older ones to text descriptions, or use context-editing features that clear old tool results automatically.

**Each iteration costs a model round trip.**
At a few seconds per step, a 50-step task is minutes of wall clock even before UI settling time.
Computer use is not interactive-latency technology; it is background-task technology, which is why Chapter 6's async framing matters here.

**The model has no state between screenshots.**
Everything it knows about the current UI state comes from the current image plus its conversation history.
Modal dialogs that appear and vanish, tooltips, transient toasts, and animations are perceptual hazards: the agent may act on a frame that no longer describes reality.

**Errors compound.**
A misclick at step 12 puts the agent in a state its plan did not anticipate, and recovery requires recognizing the divergence from a screenshot.
Recognition of "I am not where I expected to be" is a specific competence that improved substantially across model generations and is worth measuring separately from raw click accuracy.

**Waiting is a first-class problem.**
There is no page-load event; the agent cannot know whether the application is thinking or finished.
Fixed sleeps waste time and still race; the practical answer is a wait action the model can choose plus screenshot-diff heuristics in the harness (re-screenshot until two consecutive frames are stable, with a timeout).

## 3. Grounding: coordinates versus elements

**Coordinate grounding** is the requirement that a model, shown an image, emit the pixel coordinates of a target.
This is a genuinely hard perception task with a demanding success criterion: the click either lands inside the button or it does not, and being 20 pixels off is a total failure with no partial credit.

Three things made it hard in 2024 and less hard by 2026.

*Resolution and scaling.*
Early computer-use APIs recommended downscaling screenshots to roughly XGA or WXGA (around 1024 to 1366 pixels wide) because models degraded on larger images, which forced the harness to scale model-emitted coordinates back up to physical pixels.
That scaling step is a classic bug source: off-by-a-factor errors, aspect-ratio distortion, and rounding drift on retina displays.
The 2025-2026 high-resolution vision generation removed most of this: models accept substantially larger images and return coordinates that map one-to-one onto image pixels, so the scale-factor math disappears.
Anthropic's own guidance for the current generation is that 1080p is a good performance-cost balance, with 720p or 1366x768 as cheaper options.

*Small and dense targets.*
Toolbar icons, checkbox targets, tightly packed table cells, and menu items in dense enterprise applications remain the hardest cases; accuracy falls sharply with target size.

*Ambiguity.*
"Click Save" is unambiguous only if there is one Save; real applications have repeated labels, disabled twins, and the same control in a toolbar and a menu.

**Element grounding** is the alternative: instead of asking the model for pixels, expose a list of interactable elements with identifiers, and let the model choose an identifier that the harness resolves to a location.
On the web this is the accessibility tree (Chapter 4).
On the desktop, the analogues are the platform accessibility APIs: UI Automation on Windows, the Accessibility API on macOS, and AT-SPI on Linux.

Element grounding is strictly better where it is available, for the same reasons as in the browser: robust to layout shift, no perception error, cheaper, and debuggable.
Its problem on the desktop is coverage.
Many applications - Electron apps with poor accessibility, custom-rendered enterprise clients, remote-desktop windows, games, anything drawing to a canvas - expose almost nothing useful.
Remote desktop is the extreme case: an RDP or VNC window is, to the host OS, one opaque rectangle of pixels.

The practical synthesis, mirroring Chapter 4: **use accessibility APIs when the application cooperates, fall back to vision when it does not, and design the tool layer so the model's action vocabulary is the same either way**.
The model should say "click the Save button"; whether that resolves through an accessibility handle or through coordinates is the harness's problem, not the model's.

Note the middle option that grew through 2025: **set-of-marks prompting**, where the harness overlays numbered boxes on the screenshot for each detected interactable element and the model picks a number.
It converts a hard regression problem (predict x, y) into an easy classification problem (pick a label), and it measurably improves accuracy when element detection is decent, at the cost of an extra detection step and a visually cluttered image.

## 4. The Anthropic computer use lineage

Anthropic shipped the first mainstream computer use API as a public beta on 2024-10-22, with Claude 3.5 Sonnet (new).
The design has been stable in shape ever since, so it is worth learning concretely.

The tool is declared as a client-side tool - Anthropic defines the schema and the model's usage pattern; **you** execute the actions in an environment you provide.
The declaration carries the display dimensions and an optional display number:

```python
tools = [
    {
        "type": "computer_20250124",      # tool version string; see note below
        "name": "computer",
        "display_width_px": 1920,
        "display_height_px": 1080,
        "display_number": 1,               # X11 display, optional
    },
    {"type": "bash_20250124", "name": "bash"},
    {"type": "text_editor_20250728", "name": "str_replace_based_edit_tool"},
]
```

Computer use requires a beta header, and both the tool `type` string and the header are dated and versioned; the exact current values change with model generations (the original was `computer_20241022` with header `computer-use-2024-10-22`; the 2025 generation used `computer_20250124` / `computer-use-2025-01-24`; a further `computer_20251124` / `computer-use-2025-11-24` generation shipped with the Sonnet 5 era).
The rule to remember rather than the strings: **the tool version must match what the model you are calling supports, and the beta header must match the tool version**; consult the current computer use documentation for the pairing rather than reusing a string from memory.

The action vocabulary the model emits, expanded across versions:

- `screenshot` - capture the display.
- `mouse_move`, `left_click`, `right_click`, `middle_click`, `double_click`, `triple_click` - pointer actions at `coordinate: [x, y]`.
- `left_click_drag`, and in later versions `left_mouse_down` / `left_mouse_up` for precise drag control.
- `type` - type a UTF-8 string.
- `key` - press a key or chord using xdotool-style names (`Return`, `ctrl+s`, `alt+Tab`).
- `scroll` - with direction and amount, added in the 2025 version (previously agents had to emulate it with keys or drags).
- `hold_key`, `wait` - added in the 2025 version; `wait` is the sanctioned way to let the UI settle.
- `cursor_position` - query where the pointer is.

The harness contract is what you implement: receive the tool call, perform the action against a real display, take a fresh screenshot, and return it as the tool result image.
Anthropic ships a reference implementation as a Docker container (a virtual X11 desktop with a browser and basic apps, plus a Streamlit control UI) precisely because standing up that environment is most of the work; use it to learn the loop, then build your own environment for production.

Two API-level details worth internalizing:

- Computer use composes with bash and text editor tools, and should.
Many "computer use" tasks are far better done partly in a shell - navigating the filesystem, invoking a CLI, checking a result - with the GUI reserved for what only the GUI can do.
An agent that opens a terminal window and types into it visually, when it could call bash directly, is wasting steps and reliability.
- Prompting matters more here than in most tool use: telling the model to take a screenshot after actions with delayed effects, to zoom or scroll rather than guess at small text, and to verify before consequential clicks moves success rates noticeably.

OpenAI's parallel lineage - the `computer-use-preview` model and Operator (January 2025), later the Responses API computer-use tool - has the same shape: a screenshot-in, action-out loop with a client-executed environment, differing in action naming and in the product's cloud-hosted default.
The portability lesson: the loop and the environment are yours; the model and its action schema are swappable.

## 5. OSWorld and desktop evaluation

**OSWorld** (2024) is the reference desktop benchmark: 369 real computer tasks in a controlled Ubuntu VM (plus Windows and macOS variants), spanning file management, GIMP, LibreOffice, VS Code, Thunderbird, Chrome, and multi-application workflows.
Its critical design property is execution-based evaluation: each task ships an initialization script and a bespoke validation script that inspects real system state afterward - was the file created with the right contents, was the setting changed, does the spreadsheet contain the right value.
That is the same verifiability trick SWE-bench uses, transplanted to the desktop, and it is why OSWorld is trustworthy in a way that screenshot-judged benchmarks are not.

The headline result at publication was the field's second cold shower after WebArena: humans completed over 70 percent of tasks, the best systems managed roughly 12 percent, and the paper's error analysis attributed much of the gap specifically to GUI grounding and to operational-knowledge failures rather than to reasoning.
Over 2025 the number climbed steadily as models gained high-resolution vision and grounding-focused training - the frontier moved into the 40 to 60 percent band by late 2025 on OSWorld-Verified, the cleaned variant - which is real progress and still far below what an unattended production workflow needs.

Related evaluations worth knowing: **WindowsAgentArena** (2024) for the Windows ecosystem, **AndroidWorld** and related mobile suites for phone UIs, and **ScreenSpot** and successors for pure grounding accuracy (given an instruction and a screenshot, is the predicted coordinate inside the correct element).
Grounding benchmarks are the most useful diagnostic: if grounding accuracy is 85 percent per click, a 20-click task cannot exceed roughly 4 percent success without recovery, which is the arithmetic that explains why early end-to-end numbers were so low.

That arithmetic is the number to carry: **end-to-end success is roughly per-step reliability raised to the number of steps, unless the agent can detect and recover from errors**.
Every engineering lever in computer use is either raising per-step reliability, reducing step count, or adding recovery.

## 6. Latency, cost, and reliability in practice

Concrete shape of a real deployment, using order-of-magnitude figures you should re-measure for your own stack:

- Screenshot capture and encode: tens to a few hundred milliseconds.
- Model round trip with an image: on the order of seconds, more with extended thinking.
- Action execution: milliseconds.
- UI settling: highly variable, from instant to many seconds for slow enterprise applications.

So a step is roughly 2 to 10 seconds, and a 30-step task is minutes.
Cost per step is dominated by image tokens; a long task with a naive full-history context can cost dollars, and this is the single most common budget surprise.

The mitigations that matter, in order of impact:

1. **Reduce steps.** Every step avoided is a full step's cost, latency, and failure probability.
Use keyboard shortcuts instead of menu navigation, use the shell instead of the GUI where possible, and give the agent direct starting URLs or file paths rather than making it navigate there.
2. **Trim context.** Keep only the most recent screenshots at full fidelity; elide older ones.
Context-editing features that clear stale tool results are designed exactly for this.
3. **Choose resolution deliberately.** 1080p is a reasonable default; drop to 720p for cost-sensitive workloads and measure the accuracy cost rather than assuming it.
4. **Add recovery, not just accuracy.** A verification step ("screenshot and confirm the dialog closed") is cheaper than a failed task.
5. **Cache the deterministic parts.** If the first eight steps are always the same, script them and start the agent at step nine.
6. **Escalate to humans on ambiguity** rather than letting the agent guess on consequential actions.

The reliability posture: computer use as of early 2026 is production-viable for **supervised, retryable, non-consequential, low-volume** work, and is not yet production-viable for unattended high-volume workflows with irreversible effects.
Deploy it where a failure means a retry, not a refund.

## 7. Computer use versus the alternatives

Use this decision procedure in order; stop at the first yes.

1. **Is there an API?** Use the API.
It is orders of magnitude faster, cheaper, and more reliable, and it fails loudly with structured errors.
The only reasons to skip an existing API are missing coverage for your specific operation or a licensing wall.
2. **Is there a CLI or scriptable interface?** Use it, with a coding agent driving.
This is Chapters 2 and 3's territory and it is the highest-leverage automation available.
3. **Is it in a browser?** Use browser automation with semantic representations (Chapter 4), not desktop computer use.
Accessibility trees and element references beat pixels every time, and the browser tooling is far more mature.
4. **Is it a repeated, stable workflow in a GUI app?** Consider scripted automation (AutoHotkey, AppleScript, platform accessibility APIs, traditional RPA tools) - or better, have an agent write that script once and run the script thereafter.
A deterministic script has zero per-run model cost and near-perfect reliability; the agent's value is authoring and maintaining it.
5. **Is it varied, ad hoc, or in an application nothing else can reach?** Now use computer use.

The strategic reframing that resolves most arguments: **computer use is a bridge technology**.
Its highest value is often not in performing the task repeatedly but in performing it once well enough to produce a durable artifact - a script, an API integration, a documented procedure - that replaces it.
Teams that treat computer use as the permanent execution layer for high-volume work end up with an expensive, slow, flaky system; teams that treat it as the universal fallback and the paving-the-cowpath tool get value immediately and shrink their dependence on it over time.

The counter-consideration, stated honestly: some workflows genuinely cannot be scripted - vendor software that changes its UI unpredictably, one-off migrations, workflows requiring judgment at each step - and for those computer use is not a bridge but the destination, and its reliability limits are simply the constraint you design around.

## 8. Virtual desktop infrastructure

Running computer-use agents at any scale is an infrastructure problem, and the design has converged.

**Never run on the user's primary machine for autonomous work.**
The agent's actions are indistinguishable from the user's, it can see and act on everything on screen including other applications' data, and a mistake or an injected instruction operates with the user's full identity.
Supervised assistance on a real desktop is defensible; unattended work is not.

**The standard unit is a containerized virtual desktop.**
The Anthropic reference container is the archetype: a Linux container running Xvfb (a virtual framebuffer), a lightweight window manager, the target applications, a VNC server for observation, and a small HTTP service that executes screenshot and input actions via xdotool or equivalent.
The agent talks to that service; the service talks to X11.

Design decisions and their trade-offs:

- **Resolution**: fix it explicitly and match the tool declaration; 1280x800 or 1920x1080 are common.
Never let it vary between screenshot and action.
- **Isolation**: one container per session, torn down after.
This is the primary security control - it bounds what a compromised or confused agent can touch, and it makes state reset trivial.
- **Persistence**: mount only what the task needs; treat everything else as ephemeral.
Persistent profiles are how login state is reused, and are also how contamination between tasks happens.
- **Networking**: default-deny egress with an allowlist.
This is the same egress control as Chapter 4's injection defense and it is the highest-value network control available.
- **Credentials**: inject at the proxy or via pre-authenticated sessions, never as text the agent can read.
An agent that can read a password can leak it.
- **Observability**: record the screen (video or screenshot sequence) plus the action log for every session.
Computer-use failures are impossible to debug from text alone, and the recording is also the audit trail.
- **Scaling**: containers are cheap but a GUI stack plus a browser is hundreds of megabytes of RAM each; plan for tens per host, not thousands, and expect the model API to be the cost bottleneck rather than compute.

For Windows and macOS targets, containers are not available in the same way; the practical answers are VM fleets (with the attendant licensing costs) or hosted virtual-desktop providers, and this is a real reason Linux-targeted computer use is disproportionately represented in research and demos relative to where enterprise legacy applications actually live.

Chapter 6 takes this infrastructure and generalizes it: once you have isolated, observable, reset-able execution environments, you can run many agents at once, and the engineering problem becomes queueing and verification rather than perception.

## Exercises

1. Stand up the Anthropic computer use reference container, run three tasks of increasing complexity, and instrument the loop to log per-step latency, image tokens, and total cost; produce the per-step cost breakdown and identify the dominant term.
2. Measure grounding accuracy directly: assemble 50 screenshot-plus-instruction pairs from applications you use, record whether the model's predicted coordinate lands inside the correct element, and compute per-step accuracy; then predict end-to-end success for a 15-step task and test the prediction.
3. Implement set-of-marks prompting - overlay numbered boxes on detected interactable elements - and re-run exercise 2; report the accuracy delta and the added per-step cost.
4. Take one task and implement it three ways: computer use, browser automation with accessibility references, and a direct API call; measure success rate over ten runs, wall-clock, and cost, and write the decision rule your data supports.
5. Build the settling detector: replace fixed sleeps with a screenshot-diff loop that waits for two consecutive stable frames with a timeout, and measure how many steps it saves and how many races it prevents across twenty runs.
6. Design and document the container spec for a computer-use fleet handling invoice data entry into a legacy desktop app: specify resolution, isolation, network policy, credential handling, recording, and the human-escalation trigger, and justify each against a named failure it prevents.
7. Have an agent perform a repetitive GUI task once, then have it write an AppleScript, AutoHotkey script, or accessibility-API script for the same task; compare ten runs of each on time, cost, and failures, and state the break-even volume at which authoring the script pays for itself.

## Godhood check

You have mastered this chapter when you can:

- Draw the screenshot-action loop and name, for each of its five properties, one production problem it causes and the mitigation.
- Explain why coordinate grounding is hard, what changed with high-resolution vision models, and when set-of-marks or accessibility grounding should replace raw coordinates.
- Declare a computer use tool correctly from memory in shape (not string), explain why the tool version and beta header must be paired, and say why bash and text editor tools belong alongside it.
- Describe OSWorld's execution-based validation, state why early scores were near 12 percent against 70-plus percent human performance, and derive end-to-end success from per-step reliability and step count.
- Run the five-step decision procedure on a novel automation request and defend stopping where you stop.
- Argue the bridge-technology framing, including the honest cases where computer use is the destination rather than a bridge.
- Specify a production virtual desktop environment covering isolation, resolution, egress, credentials, recording, and escalation, and name the failure each control prevents.
