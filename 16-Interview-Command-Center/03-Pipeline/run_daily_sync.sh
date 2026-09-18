#!/bin/bash
export PATH="/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:$PATH"
REPO_DIR="/Users/shreejitverma/github/SDE-Interview-Prep"
SCRIPT_PATH="$REPO_DIR/16-Interview-Command-Center/03-Pipeline/sync_job_emails.py"
LOG_FILE="$REPO_DIR/16-Interview-Command-Center/03-Pipeline/sync.log"

echo "==========================================" >> "$LOG_FILE"
echo "🕒 Daily Job Sync started at $(date)" >> "$LOG_FILE"
echo "==========================================" >> "$LOG_FILE"

/usr/bin/python3 "$SCRIPT_PATH" --mode daily >> "$LOG_FILE" 2>&1

echo "✅ Sync completed at $(date)" >> "$LOG_FILE"
echo "" >> "$LOG_FILE"
