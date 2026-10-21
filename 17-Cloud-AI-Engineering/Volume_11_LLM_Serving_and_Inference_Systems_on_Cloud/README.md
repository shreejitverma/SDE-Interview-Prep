---
type: moc
track: [cloud-ai, ai-eng, sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 11 - LLM Serving and Inference Systems on Cloud

Volume 11 - LLM Serving and Inference Systems on Cloud in the Cloud AI Engineering Zero to Godhood curriculum.

## Chapters

1. [LLM Inference Mechanics: Prefill vs Decode Phases](Chapter_01_LLM_Inference_Mechanics_Prefill_vs_Decode.md) - Compute-bound GEMM during prefill, memory-bandwidth-bound GEMV during decode, TTFT, and inter-token latency math.
2. [Inference Engines: vLLM, SGLang, and TensorRT-LLM](Chapter_02_Inference_Engines_vLLM_SGLang_TensorRT_LLM.md) - Architecture comparison, PagedAttention integration, RadixAttention tree-sharing, and custom CUDA kernel dispatching.
3. [KV-Cache Management and PagedAttention](Chapter_03_KV_Cache_Management_and_PagedAttention.md) - Virtual memory translation for token key-value blocks, eliminating external memory fragmentation, and chunked prefill mechanics.
4. [Continuous Batching and Scheduling Algorithms](Chapter_04_Continuous_Batching_and_Scheduling_Algorithms.md) - Iteration-level continuous scheduling, dynamic slot assignment, priority queues, and preemption policies.
5. [Speculative Decoding and Advanced Decoding](Chapter_05_Speculative_Decoding_and_Advanced_Decoding.md) - Small draft model speculative execution, verification passes, Medusa heads, lookahead decoding, and token acceptance economics.
6. [Model Quantization and Compression](Chapter_06_Model_Quantization_and_Compression.md) - Post-training quantization: AWQ, GPTQ, FP8, and INT4; hardware kernel acceleration; and perplexity vs latency curves.
7. [Prefill and Decode Disaggregation](Chapter_07_Prefill_and_Decode_Disaggregation.md) - Decoupling prefill workers from decode workers, high-speed KV-cache network transfer over InfiniBand/RoCE, and throughput maximization.
8. [Cloud GPU Autoscaling and Cold-Start Mitigation](Chapter_08_Cloud_GPU_Autoscaling_and_Cold_Start_Mitigation.md) - Container image pre-warming, base weight caching on local NVMe, and rapid weight streaming from high-bandwidth object storage.
9. [Managed Inference: Amazon Bedrock vs Vertex AI](Chapter_09_Managed_Inference_Amazon_Bedrock_vs_Vertex_AI.md) - Model catalog architectures, on-demand vs provisioned throughput, cross-region request routing, and burst quota limits.
10. [Claude on Bedrock and Vertex AI](Chapter_10_Claude_on_Bedrock_and_Vertex_AI.md) - Anthropic Claude deployment parity on AWS and GCP, private endpoint routing, streaming APIs, prompt caching mechanics, and quotas.
11. [End-to-End Latency, Throughput, and Cost Modeling](Chapter_11_End_to_End_Latency_Throughput_and_Cost_Modeling.md) - Complete back-of-the-envelope capacity formulas, concurrency sizing, cost-per-token projections, and hardware selection decision trees.
