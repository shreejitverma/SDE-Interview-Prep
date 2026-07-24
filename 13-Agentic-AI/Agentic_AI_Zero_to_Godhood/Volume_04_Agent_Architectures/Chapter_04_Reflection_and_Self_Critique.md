# Chapter 04 - Reflection and Self-Critique

## What you will master

- The reflection family tree: self-consistency, Self-Refine, Reflexion, and critic/verifier loops, with what each actually measured.
- The evaluator-optimizer pattern in production depth: evaluator design, feedback contracts, stopping rules, and cost accounting.
- The empirical limits of self-correction: why models often cannot find their own errors, and the intrinsic self-correction results you must know before trusting any reflect-and-retry loop.
- Why external feedback beats self-feedback, and how to rank feedback sources by soundness.
- Rubber-stamping and its relatives in LLM-as-judge setups: sycophancy, self-preference, position and verbosity biases, and the mitigations that survive contact with production.

## 1. The seductive idea

Reflection is the idea that a model can improve its output by examining it: generate, critique, revise.
It is seductive for three reasons: it mirrors how humans revise drafts, it needs no new infrastructure (the critic is just another prompt), and it visibly produces more polished-sounding output, which reads as improvement whether or not correctness moved.

This chapter's thesis, stated up front because it organizes everything else: reflection is a transport mechanism for feedback, and its value is bounded by the quality of the feedback source.
When the feedback comes from ground truth (a failing test, an execution error, a checkable constraint), reflection loops are among the most reliable techniques in agent engineering.
When the feedback comes from the same model re-reading its own answer, gains are small, task-dependent, and sometimes negative.
Most published disagreement about whether reflection "works" dissolves once you ask where the feedback signal came from.

## 2. Self-consistency: reflection without a critic

Self-consistency (Wang et al., 2022) predates the critique loops and is the cheapest member of the family: sample multiple independent chains of thought at nonzero temperature and take the majority answer.
No model ever critiques anything; reliability comes from marginalizing over reasoning paths, on the observation that many wrong paths diverge to different wrong answers while correct paths converge.

What to know as an engineer:

- It applies only where answers are comparable for equality or near-equality: multiple-choice, numeric answers, short extractions, classifications; free-form prose has no majority.
- Cost is linear in samples with diminishing returns; most reported gains concentrate in the first handful of samples.
- It is the voting pattern of Chapter 01 applied to reasoning, and it remains a strong baseline that any fancier reflection method must beat at matched token cost; papers that skip this comparison are hiding something.
- Reasoning models complicate the picture (2025 era): heavy internal deliberation already performs some of this marginalization, so external self-consistency stacked on a reasoning model buys less than it did on 2022-2023 models.

## 3. Self-Refine: same model, three hats

Self-Refine (Madaan et al., 2023) is the canonical intrinsic-reflection loop: one model generates, the same model gives feedback on its output, the same model revises given the feedback, iterating until a stop condition.
The paper reported average improvements across seven tasks with GPT-3.5/GPT-4-era models, with the largest gains on tasks with soft, preference-like criteria (dialogue quality, readability, sentiment-controlled rewriting) and much weaker gains on tasks with hard correctness criteria, notably math reasoning.

That gain profile is the finding to internalize.
Feedback like "this could be more concise" is within the model's competence to produce and apply, so style-shaped tasks improve.
Feedback like "step 4 contains an arithmetic error" requires detecting a mistake the model just made with its best effort, and detection is the bottleneck; the paper's own analysis attributes most math-task failure to bad feedback rather than bad revision.
Self-Refine works where critique is easier than generation, and fails where critique requires exactly the capability whose absence caused the error.

## 4. Reflexion: reflection across episodes

Reflexion (Shinn et al., 2023) moved reflection from within one answer to across attempts of a task.
An agent attempts an episode (ReAct-style); an evaluator signal indicates failure (unit tests for code, task success flags for environments, heuristics otherwise); the model writes a verbal reflection about what went wrong and what to try differently; the reflection is stored in an episodic memory buffer and prepended to the next attempt; repeat up to a retry budget.
The paper's framing is memorable and accurate: verbal reinforcement, where the "gradient" is a natural-language note to self, no weights updated.

Reported results were strong where the evaluator was sound: the headline number was pass@1 on HumanEval with GPT-4 rising to roughly the low 90s percent versus around 80 percent without Reflexion (2023 measurement, models long since surpassed), plus large gains on ALFWorld and HotpotQA variants.
The design decomposes into three choices you will reuse everywhere:

- **Evaluator**: the results track evaluator soundness exactly; self-generated unit tests already dilute the coding gains versus true hidden tests, and heuristic evaluators dilute further; Reflexion with a broken evaluator is Self-Refine with extra steps.
- **Reflection content**: good reflections are causal and prescriptive ("I never checked the second column; next time verify both") rather than descriptive ("the answer was wrong"); prompting for the counterfactual next-time behavior is what makes the memory useful.
- **Memory scope**: a small sliding window of reflections; unbounded accumulation degrades into noise, an early sighting of the context-curation problems Volume 06 treats fully.

Reflexion's descendants are everywhere in 2025-2026 agents: coding agents that read the test failure, write a diagnosis, and retry are running the Reflexion loop with the diagnosis inlined rather than stored; the pattern became invisible by winning, like ReAct.

## 5. Critic and verifier loops

Separating the critic from the generator, even when both are LLMs, changes the dynamics in ways worth cataloguing.

- **Separate context**: a critic that sees only the artifact and the spec, not the generator's reasoning, cannot inherit the generator's framing errors as easily; fresh-context review consistently catches issues that same-context review misses (this is why human code review works better than proofreading your own diff).
- **Separate persona and objective**: a critic prompted to find problems, with permission to be wrong occasionally, behaves measurably differently from a generator asked "is this correct?", which is biased toward yes; asymmetric prompting is cheap and real.
- **Separate model**: cross-model critique (model A critiques model B) reduces correlated blind spots and self-preference bias (section 8); it costs a second vendor dependency and evaluation of a second model's critique quality.
- **Verifier proper**: a checker with soundness properties, code or formal, not an LLM; execution, type checking, schema validation, simulation; this is the LLM-Modulo critic of Chapter 03, and it is the only member of this list whose approval means anything guaranteed.

The engineering hierarchy that falls out, from strongest to weakest feedback source: sound verifier, unsound-but-objective checker (linters, heuristic detectors), fresh-context different-model critic, fresh-context same-model critic, same-context self-critique.
Design rule: push every property you care about as far up this hierarchy as it can go, and only leave to LLM judgment what cannot be checked mechanically.

## 6. Evaluator-optimizer in production depth

Chapter 01 introduced the pattern; here is what makes it work or fail when real money runs through it.

```
def evaluator_optimizer(task, spec, budget):
    best, best_score = None, -inf
    draft = generate(task)
    for round in range(budget.max_rounds):
        report = evaluate(draft, spec)        # structured verdict, see below
        if report.score > best_score:
            best, best_score = draft, report.score
        if report.verdict == "pass" or not report.actionable_items:
            break
        draft = revise(task, draft, report.actionable_items)
    return best                                # never the last draft by default
```

Evaluator design, the part that determines everything:

- The evaluator needs a spec, not vibes: explicit criteria enumerated in the prompt, each scored separately, with a structured output (per-criterion verdicts plus concrete, quotable defects); "rate this 1-10" produces noise with a confident face.
- Binary or few-level scales beat fine-grained scores; LLM judges are inconsistent at fine granularity, and a defect list is more actionable than a scalar anyway.
- Ask for the failure evidence ("quote the sentence that violates criterion 3"); requiring evidence suppresses hallucinated critiques, which otherwise send the optimizer chasing ghosts.

Loop mechanics learned the hard way:

- Keep best-so-far and return it, because revisions non-monotonically wander; a revision that fixes criterion 2 routinely breaks criterion 1.
- Most gain arrives in round one; two to three rounds is the economic ceiling for almost all tasks, and unbounded loops are the pattern's signature cost incident.
- Detect oscillation (draft similarity to a previous round) and stop; two drafts alternating under a conflicted spec will burn the whole budget.
- Log evaluator verdicts against final human judgment; an evaluator that passes everything or fails everything is not evaluating, and you find out only by measuring agreement (Volume 10 covers judge calibration properly).

## 7. The limits of self-correction

The optimistic 2023 story, extrapolated from Self-Refine and Reflexion, was that models could bootstrap reliability by checking their own work.
The corrective literature landed almost immediately, and its findings replicated well enough that they should be treated as load-bearing engineering facts.

- "Large Language Models Cannot Self-Correct Reasoning Yet" (Huang et al., 2023, Google DeepMind): under intrinsic self-correction, meaning no external feedback and no oracle telling the model when it is wrong, prompting GPT-4-class models to review and revise their reasoning answers made accuracy worse on the tested benchmarks, because models changed correct answers to incorrect ones more often than the reverse.
- The oracle confound: many positive self-correction results secretly used ground-truth knowledge of which answers were wrong to decide when to trigger revision; remove the oracle and the gains largely vanish; any reflection paper you read should be interrogated for this confound first.
- Error detection is the bottleneck, not error repair: given the location of a bug, models fix it well; asked whether their own output contains a bug, they miss their own errors at high rates; detection and generation fail on correlated inputs because they are the same weights applying the same biases.
- Sycophantic collapse: asked "are you sure?", models frequently abandon correct answers; revision pressure without evidence produces answer churn, not error correction; this interacts with user-facing agents that treat any user pushback as a correction signal.
- What did survive scrutiny: self-correction with external, informative feedback (execution results, test failures, retrieved documents, tool errors) robustly helps, which is exactly the Reflexion-with-sound-evaluator configuration and exactly the LLM-Modulo claim.

The design consequence in one line: never build a loop whose only error signal is the generator's own opinion of its work; find an external signal or expect no gain.
Reasoning-model caveat (2025-era): models trained with RL to deliberate do internally revise during thinking, and they self-correct better than the 2023 cohort; but the relative ordering stands, external signal still dominates, and the intrinsic-only gains remain the smallest and least reliable.

## 8. Rubber-stamping and LLM-judge pathologies

Evaluator-optimizer, critic loops, and eval pipelines (Volume 10) all lean on LLM judges, and judges fail in patterned, exploitable ways; the biases below are documented across the 2023-2025 judge literature and recur in production audits.

- **Rubber-stamping**: the judge approves near-everything, especially fluent, confident, well-formatted output; surface competence is the easiest feature to detect and correlates weakly with correctness; a judge whose pass rate is high and flat across known-quality tiers is measuring polish.
- **Self-preference and self-recognition**: models score their own generations above equal-quality text from other models or humans; using the same model as both generator and judge inflates scores by construction, which is a standing argument for cross-model judging.
- **Position bias**: in pairwise comparison, judges systematically favor one position (commonly the first); the standard mitigation, swap the order and keep only consistent verdicts, is cheap and non-optional.
- **Verbosity bias**: longer answers score higher at equal correctness; length-control the comparison or the score, or your optimizer will learn to pad.
- **Sycophancy toward the prompt**: judges lean toward whatever the evaluation prompt seems to hope is true; describing the artifact as "our improved system's output" measurably shifts scores upward; blind the judge to provenance.
- **Critique dilution in-loop**: in evaluator-optimizer specifically, judges soften over rounds; round-3 output looks better relative to round-1 memory of complaints, so the loop halts on fatigue rather than quality; fresh evaluator context per round, with the spec restated and no memory of prior verdicts, mitigates this.

Mitigation kit, in the order to apply it: replace judge criteria with mechanical checks wherever possible; blind the judge to provenance and strip identifying framing; use structured per-criterion rubrics with required evidence quotes; randomize and swap positions in pairwise setups; use a different model family for judging than for generation when stakes justify it; calibrate the judge against a small human-labeled set and re-check on distribution shifts.
The trade-off to state honestly: each mitigation adds latency, cost, or pipeline complexity, and over-hardened judge pipelines become their own maintenance burden; calibrate hardening to the cost of a wrong verdict, not to completeness.

## 9. Placement: where reflection belongs in an architecture

Pulling the chapter together into placement rules:

- Inside a step, prefer native deliberation: a reasoning model's thinking budget captures most intrinsic-revision value at the lowest orchestration cost (as of early 2026).
- At step boundaries, reflect only on external signals: tool errors, test output, verifier reports; wire the signal into the retry prompt verbatim, since models repair well from concrete evidence.
- At artifact boundaries, use evaluator-optimizer with a spec'd, hardened judge, budget two to three rounds, keep best-so-far.
- Across episodes, use Reflexion-style memory only when a sound evaluator exists to label episodes failed, and cap the memory window.
- At system boundaries, human review remains the strongest general critic; spend design effort making the human's verdict cheap to give and hard to rubber-stamp (short diffs, surfaced risks, defaults that require an active choice), because the human gate degrades exactly like the LLM one when reviewing is made boring.

## 10. Claims that will rot

Numbers cited (Reflexion's HumanEval figures, Self-Refine's task-level gains) are 2023 measurements on models of that era, kept here as historical anchors, not current capability claims.
The intrinsic self-correction limits were measured on 2023-2024 models; reasoning-trained models have shifted the magnitudes and will shift them further, but no result as of early 2026 overturns the ordering that external feedback dominates self-feedback.
Judge-bias findings have replicated across several model generations and are the safest bets in this chapter to still hold; even so, re-run the calibration on your own stack rather than trusting the literature's effect sizes.

## Exercises

1. Reproduce the self-consistency baseline: take 50 grade-school math problems, sample 1, 5, and 15 chains at temperature 0.8, majority-vote the answers, and plot accuracy versus cost; then add a Self-Refine loop at matched token budget and report which wins.
2. Build the Reflexion loop for a coding task against a real hidden test suite: attempt, run tests, write a reflection, retry with the reflection in context, budget three episodes; then swap the hidden tests for model-generated tests and measure how much of the gain evaporates.
3. Design a structured evaluator prompt for "internal design document quality" with five criteria, per-criterion verdicts, and required evidence quotes; run it on three documents you know well and grade the judge's agreement with your own ranking.
4. Demonstrate two judge biases on your own stack: measure position bias by swapping pairwise order on 40 comparisons, and verbosity bias by scoring a correct-short versus padded-equal answer set; report effect sizes and apply one mitigation for each.
5. Audit any reflection loop you currently run (or a published one) for the oracle confound: identify exactly what signal triggers revision, classify it on the section 5 hierarchy, and predict from that classification alone whether the loop helps; then check.
6. Write the stopping policy for an evaluator-optimizer that writes SQL migrations: rounds budget, oscillation detection, best-so-far rule, and the condition that escalates to a human; justify each element with a failure it prevents.

## Godhood check

You have mastered this chapter when you can:

- Draw the feedback-source hierarchy from memory and place any proposed reflection scheme on it before estimating whether it will work.
- Explain why Self-Refine helps on style and fails on math, and generalize that explanation to a novel task in one sentence about critique-versus-generation difficulty.
- State the intrinsic self-correction results precisely, including the oracle confound, and interrogate a new reflection paper for it in five minutes.
- Design an evaluator-optimizer loop with spec'd criteria, best-so-far, and a defensible budget, and predict its cost envelope before running it.
- List five LLM-judge biases with a mitigation each, and articulate the point at which further judge-hardening stops paying.
