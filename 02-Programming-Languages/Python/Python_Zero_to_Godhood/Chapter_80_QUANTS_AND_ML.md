---
type: concept
track: [sde, quant-dev, low-latency]
level:
status: draft
last_reviewed:
sources: []
---

# Phase XVII: Python for Quants and Data Engineers

In the modern era, Python is the lingua franca for data science and quantitative finance. This section explores the internals of the tools that power these industries.

# Chapter 80: High-Performance Data Analysis: Pandas and Polars Internals

### 80.1 Pandas: The BlockManager and NumPy
Pandas is built on top of NumPy, but it adds the **BlockManager** to handle heterogeneous data.
*   **Blocks**: Data is grouped by type (e.g., all integer columns are in one 2D NumPy array).
*   **Overhead**: While flexible, the BlockManager can be slow for operations that touch multiple columns of different types due to consolidation overhead.

### 80.2 Polars: The Rust-Accelerated Future
Polars is a modern alternative to Pandas written in Rust.
*   **Apache Arrow**: It uses Arrow as its native memory format, allowing for zero-copy data exchange.
*   **Query Optimization**: Polars has a built-in query optimizer (similar to a SQL database) that can reorder operations and prune unnecessary data before execution.

---

# Chapter 81: Large Language Models (LLMs) and Python

This section is maintained in [[Chapter_64_Large_Language_Models_LLMs_and_Python]].

# Chapter 82: Productionizing Python: Docker and Kubernetes

### 82.1 Dockerizing Python
*   **Base Images**: Always use `python:3.x-slim` or `python:3.x-alpine` to minimize the image size.
*   **Multi-stage Builds**: Compile C-extensions in a build stage and copy the binaries to the final runtime stage to keep the production image clean.

### 82.2 Observability and Monitoring
*   **Prometheus**: Exporting metrics from Python applications.
*   **OpenTelemetry**: Standardized tracing and logging across microservices.

---
