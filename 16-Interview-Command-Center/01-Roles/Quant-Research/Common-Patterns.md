---
type: playbook
track: [sde, quant-dev, quant-research, low-latency, ai-eng, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# 🔄 Quant Research Common Patterns

## Probability Techniques
1. **Indicator Variables:** E[X] = Σ E[Xi]. Break complex expectations into simple indicators.
2. **Conditioning:** E[X] = Σ E[X|Y=y] P(Y=y). Condition on first step/event.
3. **Symmetry:** If outcomes are exchangeable, use symmetry to simplify.
4. **Recursion:** Define f(state) = ... f(next_state). Especially for Markov problems.
5. **Generating Functions:** Encode sequences, solve recurrences, find moments.
6. **Coupling:** Relate two processes to compare probabilities.

## Mental Math Tricks
- **Multiply by 5:** Divide by 2, multiply by 10
- **Square numbers near 50:** 50² = 2500, then adjust: 48² = 2500 - 2(50+48) = 2304
- **Percentages:** 15% of 80 = 80% of 15 = 12
- **Rule of 72:** Time to double = 72 / interest rate

## Resources
| Title | Author |
|-------|--------|
| A Practical Guide to QF Interviews (Green Book) | Xinfeng Zhou |
| Heard on The Street | Timothy Crack |
| Fifty Challenging Problems in Probability | Frederick Mosteller |
| Vault: [[05-Quantitative-Finance/01-Mathematics]] | |
