# Stage 1: Build stage
FROM python:3.13-slim AS builder

WORKDIR /app

# Install build dependencies for compiling native extensions
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

# Create isolated virtual environment
RUN python -m venv /opt/venv
ENV PATH="/opt/venv/bin:$PATH"

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Stage 2: Runtime stage (Debian-based slim image, no compilers)
FROM python:3.13-slim AS runner

WORKDIR /app

# Create non-root user and group
RUN useradd --create-home --user-group appuser

# Copy the virtual environment from the builder stage.
# It stays root-owned and world-readable, so appuser can run it but not modify it.
COPY --from=builder /opt/venv /opt/venv

# Copy application source code (root-owned, read-only for appuser)
COPY . .

# Set environment variables for virtual environment and unbuffered logs
ENV PATH="/opt/venv/bin:$PATH" \
    PYTHONDONTWRITEBYTECODE=1 \
    PYTHONUNBUFFERED=1

USER appuser

CMD ["python", "main.py"]
