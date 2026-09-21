---
type: playbook
track: [sde, quant-dev, quant-research, low-latency, ai-eng, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# 🔄 AI Engineer Common Patterns

## ML System Design Framework (45 min)

### Step 1: Problem Formulation (5 min)
- What is the ML task? (classification, ranking, generation, retrieval)
- What is the business metric? What is the ML metric?
- Online vs offline? Real-time vs batch?

### Step 2: Data Pipeline (5 min)
- Data sources, volume, freshness requirements
- Feature engineering: what signals matter?
- Data quality, labeling, handling imbalance

### Step 3: Model Architecture (10 min)
- Baseline: start simple (logistic regression, gradient boosted trees)
- Advanced: deep learning, transformers, embeddings
- Training infrastructure: distributed training, GPU/TPU requirements

### Step 4: Serving & Inference (10 min)
- Latency requirements, batching strategies
- Model compression: quantization, distillation, pruning
- Caching, feature stores, online vs offline features

### Step 5: Evaluation & Iteration (10 min)
- Offline metrics (precision, recall, NDCG, perplexity)
- Online metrics (CTR, revenue, user satisfaction)
- A/B testing framework, shadow mode deployment
- Monitoring: data drift, model degradation

### Step 6: Operational Concerns (5 min)
- CI/CD for models, retraining cadence
- Fairness, bias, safety considerations
- Cost analysis: training vs serving

## Resources
| Topic | Link |
|-------|------|
| Agentic AI | [[13-Agentic-AI/Agentic_AI_Zero_to_Godhood]] |
| System Design | [[04-System-Design/README\|04-System-Design]] |
