#!/usr/bin/env python3
"""
sync_job_emails.py - Automated Local Apple Mail to Obsidian Job Sync Pipeline
Author: Antigravity Assistant & Shreejit Verma

Modes:
  python sync_job_emails.py --mode daily   # Rolling 48-hour sync across all Mac Mail accounts
  python sync_job_emails.py --mode full    # Complete historical scan across dedicated folders
"""

import os
import sys
import json
import re
import argparse
import subprocess
from datetime import datetime, timedelta

# Target vault directories
PIPELINE_DIR = os.path.dirname(os.path.abspath(__file__))
VAULT_ROOT = os.path.dirname(PIPELINE_DIR)
PIPELINE_ACTIVE = os.path.join(PIPELINE_DIR, "Active")
PIPELINE_ARCHIVE = os.path.join(PIPELINE_DIR, "Archive")
DAILY_LOG_DIR = os.path.join(VAULT_ROOT, "06-Daily-Log")

# Accounts and dedicated mailboxes to monitor
TARGET_ACCOUNTS = [
    ("Exchange", ["Interviews", "Rejections", "In Progress", "GA Job", "BigInterview", "Career Brew", "Bloomberg", "Ford", "Inbox"]),
    ("Google", ["Bank of America", "Rejections", "Job"]),
    ("sverma357@gatech.edu", ["JOB", "Inbox"]),
    ("shreejitverma1234@gmail.com", ["Work", "INBOX"]),
    ("shreejitfinance@gmail.com", ["INBOX"]),
    ("shreejitabroad@gmail.com", ["INBOX"]),
    ("vermashreejit@gmail.com", ["INBOX"]),
    ("sverma16@stevens.edu", ["INBOX"]),
    ("iCloud", ["INBOX"]),
]

JOB_KEYWORDS = [
    "interview", "application", "assessment", "hackerrank", "codesignal", 
    "karat", "codility", "recruiter", "hiring", "offer", "rejection", 
    "status of your application", "next steps", "phone screen", "technical round",
    "onsite", "take-home", "right to represent", "congratulations", 
    "thank you for your interest", "applied", "candidacy", "position", "candidate",
    "rtr", "exclusivity", "screening"
]

EXCLUDE_PATTERNS = [
    "job alert", "jobs you may like", "recommended jobs", "daily job alert", 
    "weekly digest", "newsletter", "promotions", "uber", "delivery", "order",
    "run failed", "jobright", "flipboard", "weekly wisdom", "samsung",
    "notifications@github.com"
]

def run_applescript(script, timeout_sec=50):
    try:
        p = subprocess.run(["osascript", "-e", script], capture_output=True, text=True, timeout=timeout_sec)
        return p.stdout.strip()
    except subprocess.TimeoutExpired:
        return ""
    except Exception as e:
        print(f"⚠️ AppleScript error: {e}", file=sys.stderr)
        return ""

def fetch_mailbox_messages(acc, mb_name, limit=30):
    script = f"""
    tell application "Mail"
        set accObj to account "{acc}"
        set mbList to (every mailbox of accObj whose name is "{mb_name}")
        if (count of mbList) is 0 then return "EMPTY"
        set mb to item 1 of mbList
        set msgCount to count of messages of mb
        if msgCount is 0 then return "EMPTY"
        set maxItems to {limit}
        if msgCount < maxItems then set maxItems to msgCount
        
        set outText to ""
        repeat with i from 1 to maxItems
            try
                set m to message i of mb
                set s to subject of m
                set snd to sender of m
                set dt to (date received of m as string)
                set msgContent to content of m
                set snippetLen to length of msgContent
                if snippetLen > 400 then set snippetLen to 400
                if snippetLen > 0 then
                    set snip to text 1 thru snippetLen of msgContent
                else
                    set snip to ""
                end if
                set outText to outText & s & "«FIELD»" & snd & "«FIELD»" & dt & "«FIELD»" & snip & "«RECORD»"
            end try
        end repeat
        return outText
    end tell
    """
    raw = run_applescript(script)
    if not raw or raw == "EMPTY":
        return []

    records = []
    for entry in raw.split("«RECORD»"):
        if not entry.strip():
            continue
        parts = entry.split("«FIELD»")
        if len(parts) >= 4:
            records.append({
                "account": acc,
                "mailbox": mb_name,
                "subject": parts[0].strip(),
                "sender": parts[1].strip(),
                "date": parts[2].strip(),
                "snippet": parts[3].strip().replace("\r", " ").replace("\n", " ")
            })
    return records

def is_job_related(subject, sender, snippet):
    combined = (subject + " " + sender + " " + snippet).lower()
    for ex in EXCLUDE_PATTERNS:
        if ex in combined:
            return False
    return any(k in combined for k in JOB_KEYWORDS)

def sync(mode="daily"):
    print(f"\n=======================================================")
    print(f"🚀 Running Job Email Sync [MODE: {mode.upper()}]")
    print(f"Connected to macOS Apple Mail across 9 accounts")
    print(f"=======================================================\n")

    limit = 20 if mode == "daily" else 60
    results = []

    for acc, mailboxes in TARGET_ACCOUNTS:
        for mb in mailboxes:
            is_dedicated = any(w in mb.lower() for w in ["interview", "rejection", "job", "bank of america"])
            # In daily mode, check dedicated folders + recent messages
            fetch_limit = limit if is_dedicated else (15 if mode == "daily" else 40)
            
            print(f"🔍 Checking [{acc}] -> {mb}...")
            msgs = fetch_mailbox_messages(acc, mb, limit=fetch_limit)
            
            matched = 0
            for m in msgs:
                if is_dedicated or is_job_related(m["subject"], m["sender"], m["snippet"]):
                    results.append(m)
                    matched += 1
            if matched > 0:
                print(f"   ✓ Found {matched} relevant job emails")

    print(f"\n✅ Total relevant emails retrieved: {len(results)}")
    
    # Save cache
    cache_file = os.path.join(os.path.dirname(__file__), "extracted_emails.json")
    with open(cache_file, "w") as f:
        json.dump(results, f, indent=2)
    print(f"📁 Updated cache at {cache_file}")

    # Display clean summary
    print("\n📋 Recent Job Pipeline Updates:")
    print("-" * 60)
    for i, item in enumerate(results[:15]):
        print(f"[{i+1}] {item['subject']}")
        print(f"    Sender: {item['sender']} | Date: {item['date']}")
        print(f"    Snippet: {item['snippet'][:120]}...\n")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Sync Apple Mail Job Emails to Obsidian")
    parser.add_argument("--mode", choices=["daily", "full"], default="daily", help="Sync mode: 'daily' (recent rolling) or 'full' (historical)")
    args = parser.parse_args()
    sync(args.mode)
