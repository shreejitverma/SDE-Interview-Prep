# ==========================================
# Stage 1: Build Stage
# ==========================================
FROM python:3.9-slim as builder

WORKDIR /app

# Install build dependencies (GCC, Make) needed for some Python libs
# We verify this in the build stage, but don't want these in Prod.
RUN apt-get update && apt-get install -y \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

COPY requirements.txt .
RUN pip install --user --no-cache-dir -r requirements.txt

# Copy source code
COPY . .

# ==========================================
# Stage 2: Production Stage (Distroless/Slim)
# ==========================================
FROM python:3.9-slim

WORKDIR /app

# Copy only installed dependencies from builder
COPY --from=builder /root/.local /root/.local
COPY --from=builder /app /app

# Add local bin to PATH
ENV PATH=/root/.local/bin:$PATH

# Non-root user for security
RUN useradd -m appuser
USER appuser

CMD ["python", "main.py"]
