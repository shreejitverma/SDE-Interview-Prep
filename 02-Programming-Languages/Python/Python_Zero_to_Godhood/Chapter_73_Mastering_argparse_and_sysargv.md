# Mastering `argparse` and `sys.argv`


### 98.1 Low-Level Argument Handling
*   **`sys.argv`**: A raw list of strings. It requires manual parsing and error checking.
*   **Positional vs. Optional**: Managing the index shifts in `argv`.

### 98.2 Advanced `argparse` Features
*   **Exclusive Groups**: Ensure that only one of a set of arguments is provided.
*   **Argument Defaults**: Defining intelligent fallbacks for missing inputs.

---



## Phase XXIV: Cloud Native and Distributed Architectures

# Chapter 99: Cloud Native Python: Serverless and Containers

The modern senior engineer must know how Python scales in the cloud.

### 99.1 Python in AWS Lambda and Cloud Functions
*   **The Execution Environment**: Lambda uses a frozen Python runtime. The main constraint is the "Cold Start" time, which can be mitigated by minimizing imports and using layers.
*   **Event Driven**: Connecting Python to SQS, S3, and DynamoDB triggers.

### 99.2 Containerization and Orchestration
*   **Distroless Images**: Using Google's distroless images to reduce the attack surface and size of Python containers.
*   **Kubernetes Operators**: Writing custom Kubernetes controllers in Python using the `kopf` or `python-kubernetes` client.

---
