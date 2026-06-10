# Cluster Computing with PySpark and Dask


### 78.1 PySpark: The JVM Bridge
PySpark is a Python wrapper for Apache Spark (written in Scala/JVM).
*   **The Architecture**: Python code uses the **Py4J** bridge to communicate with the Spark JVM.
*   **RDDs and DataFrames**: These are distributed data structures that partitioned across the cluster.
*   **Godhood Tip**: Avoid UDFs (User Defined Functions) in PySpark if possible, as they require moving data between the JVM and Python process, which is a massive performance bottleneck. Use Spark SQL expressions instead.

### 78.2 Dask: Native Python Parallelism
Unlike Spark, Dask is written entirely in Python.
*   **Task Graphs**: Dask creates a DAG (Directed Acyclic Graph) of operations.
*   **Schedulers**: Dask can run on a single machine (using threads/processes) or on a distributed cluster of thousands of nodes.

---
