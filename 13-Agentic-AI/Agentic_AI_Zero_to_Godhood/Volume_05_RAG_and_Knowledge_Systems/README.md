# Volume 05 - RAG and Knowledge Systems

How to connect language models to knowledge they were not trained on: the theory, the pipeline, the infrastructure, the evaluation discipline, and the agentic evolution beyond the classic pipeline.
Product and model observations in this volume are stated as of early 2026.

## Chapters

- [Chapter 01 - Why Retrieval](Chapter_01_Why_Retrieval.md): knowledge cutoffs, hallucination mechanics, parametric versus non-parametric knowledge, the RAG paper lineage, and a decision framework for RAG versus fine-tuning versus long context versus search tools.
- [Chapter 02 - Chunking and Indexing](Chapter_02_Chunking_and_Indexing.md): chunking strategies from fixed-size to AST and layout-aware, chunk size trade-offs, metadata and filtered retrieval, contextual retrieval, and the realities of production parsing pipelines.
- [Chapter 03 - Vector Search Internals](Chapter_03_Vector_Search_Internals.md): embedding-space geometry, brute force versus ANN, IVF and HNSW in algorithmic detail, scalar/binary/product quantization, and the recall/latency/memory triangle.
- [Chapter 04 - The Vector Database Landscape](Chapter_04_The_Vector_Database_Landscape.md): pgvector, Qdrant, Weaviate, Milvus, Pinecone, Chroma, Turbopuffer, LanceDB, and Elasticsearch/OpenSearch compared on the axes that matter, plus the pgvector-first argument and its exact limits.
- [Chapter 05 - Hybrid Retrieval and Reranking](Chapter_05_Hybrid_Retrieval_and_Reranking.md): why BM25 still matters, reciprocal rank fusion, cross-encoder reranking, ColBERT late interaction, query rewriting and HyDE, and the assembled multi-stage pipeline with code.
- [Chapter 06 - RAG Evaluation](Chapter_06_RAG_Evaluation.md): recall@k, MRR, and nDCG for retrieval, faithfulness and relevance for generation, RAGAS-style LLM judges and their calibration, golden dataset construction, and a systematic procedure for localizing failures.
- [Chapter 07 - Agentic RAG and Beyond](Chapter_07_Agentic_RAG_and_Beyond.md): agent-driven iterative retrieval, Self-RAG and CRAG, GraphRAG and knowledge graphs, why coding agents grep, file-system-as-context, and the "RAG is dead" debate stated fairly.
