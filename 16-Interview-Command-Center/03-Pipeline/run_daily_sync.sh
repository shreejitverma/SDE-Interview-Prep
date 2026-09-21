#!/bin/bash
export PATH="/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:$PATH"
PIPELINE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SCRIPT_PATH="$PIPELINE_DIR/sync_job_emails.py"
LOG_FILE="$PIPELINE_DIR/sync.log"

echo "==========================================" >> "$LOG_FILE"
echo "🕒 Daily Job Sync started at $(date)" >> "$LOG_FILE"
echo "==========================================" >> "$LOG_FILE"

/usr/bin/python3 "$SCRIPT_PATH" --mode daily >> "$LOG_FILE" 2>&1

echo "✅ Sync completed at $(date)" >> "$LOG_FILE"
echo "" >> "$LOG_FILE"
