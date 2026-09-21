---
type: playbook
track: [sde, quant-dev, quant-research, low-latency, ai-eng, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Quant Dev Question Bank

---

## C++ Coding
| # | Question | Difficulty | Freq | Status |
|---|---------|-----------|------|--------|
| 1 | Implement a lock-free SPSC queue | Hard | High | ☐ |
| 2 | Implement an order book (price-time priority) | Hard | High (must-know) | ☐ |
| 3 | Implement a memory pool allocator | Hard | Med | ☐ |
| 4 | What is move semantics? Implement a move-aware class | Medium | High | ☐ |
| 5 | Implement shared_ptr from scratch | Hard | Med (must-know) | ☐ |
| 6 | Explain CRTP and implement a static polymorphism example | Medium | Med | ☐ |
| 7 | Implement a thread-safe LRU cache | Hard | Med | ☐ |
| 8 | What happens when you call `std::vector::push_back`? Full lifecycle | Medium | High | ☐ |
| 9 | Implement a compile-time type map (variadic templates) | Hard | Low | ☐ |
| 10 | Cache-friendly matrix multiplication | Medium | Med | ☐ |

## System Design (Trading)
| # | Question | Difficulty | Freq | Status |
|---|---------|-----------|------|--------|
| 1 | Design a real-time market data distribution system | Hard | High (must-know) | ☐ |
| 2 | Design an order management system | Hard | High | ☐ |
| 3 | Design a matching engine | Hard | Med (must-know) | ☐ |
| 4 | Design a risk management engine (real-time PnL) | Hard | Med | ☐ |
| 5 | Design a backtesting framework | Medium | Med | ☐ |

## Probability & Brain Teasers
| # | Question | Difficulty | Freq | Status |
|---|---------|-----------|------|--------|
| 1 | Expected flips to get HH vs HT | Medium | High | ☐ |
| 2 | You have 100 balls, 50 red 50 blue, 2 buckets. Maximize chance of red | Easy | High | ☐ |
| 3 | Gambler's ruin: probability of ruin starting with $k | Hard | Med | ☐ |
| 4 | Expected value of max of N uniform [0,1] random variables | Medium | Med | ☐ |
| 5 | Monty Hall problem - explain and generalize to N doors | Medium | Med | ☐ |
| 6 | Random walk on a line - expected return time to origin | Hard | Med | ☐ |
| 7 | Dice game: roll die, you can keep or re-roll. Optimal strategy? | Medium | High | ☐ |
| 8 | How many trailing zeros in 100! ? | Easy | High | ☐ |
| 9 | You break a stick at 2 random points - probability of triangle? | Medium | Med | ☐ |
| 10 | Secretary problem: optimal stopping rule | Hard | Med (must-know) | ☐ |

## Market Microstructure
| # | Question | Difficulty | Freq | Status |
|---|---------|-----------|------|--------|
| 1 | Explain the bid-ask spread and why it exists | Easy | High | ☐ |
| 2 | What is adverse selection in market making? | Medium | High | ☐ |
| 3 | How does a matching engine handle limit vs market orders? | Medium | High | ☐ |
| 4 | What is queue position and why does it matter? | Medium | Med | ☐ |
| 5 | Explain maker-taker vs inverted fee models | Medium | Med | ☐ |
