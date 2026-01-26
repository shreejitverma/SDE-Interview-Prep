# RFC: [Title of Proposal]

**Author:** Shreejit Verma  
**Status:** Draft / Review / Approved  
**Date:** [Date]

## 1. Summary
A concise paragraph describing the problem and the proposed solution.

## 2. Motivation
*   **Problem:** Why are we doing this? (e.g., latency is too high, current DB can't scale).
*   **Impact:** What happens if we don't do this?

## 3. Detailed Design
### 3.1 Architecture
*   [Diagram Link or Mermaid Chart]
*   Description of components (e.g., "New Redis Cluster for caching").

### 3.2 API Changes
*   `POST /api/v2/orders`
*   Request/Response payload.

### 3.3 Data Model
*   Schema changes (SQL vs NoSQL).

## 4. Alternatives Considered
*   **Option A:** [Describe alternative]
    *   *Pros:* ...
    *   *Cons:* ...
*   **Why we chose the current proposal:** ...

## 5. Risks and Mitigation
*   **Scalability:** Will this handle 10x load?
*   **Migration:** How do we migrate old data without downtime?

## 6. Implementation Plan
*   [ ] Phase 1: Prototype
*   [ ] Phase 2: Staging rollout
*   [ ] Phase 3: Production (10% traffic)
