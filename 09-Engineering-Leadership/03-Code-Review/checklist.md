# Code Review Checklist for Senior Engineers

**Goal:** Improve quality, not just find bugs.

## 1. Functional Correctness
*   [ ] Does the code do what it's supposed to do?
*   [ ] Are edge cases handled? (Null inputs, empty lists, network timeouts).
*   [ ] Are there tests covering the new logic?

## 2. Design & Architecture
*   [ ] Is this change in the right place? (Architecture coherence).
*   [ ] Does it follow SOLID principles?
*   [ ] Is the code reusable or is it duplicating logic?

## 3. Readability & Maintenance
*   [ ] Are variable/function names descriptive?
*   [ ] Is the complexity justified? (Can it be simpler?).
*   [ ] Are comments explaining "Why", not "What"?

## 4. Performance & Security
*   [ ] Are there any N+1 query problems?
*   [ ] Is there potential for SQL Injection or XSS?
*   [ ] Are locks held for too long?

## 5. The "Nitpick" Rule
*   Don't block PRs on style/formatting. Use a linter (Prettier, Clang-Format) instead.
*   **Tone:** Be kind. "Have you considered..." instead of "Change this."
