#!/usr/bin/env python3
"""
deep_scan_job_folder.py - Deep scan of Google -> Job mailbox
Sweeps through recent messages, filtering out automated alert spam (Jobright, GitHub, LinkedIn alerts)
and extracting real job applications, recruiter conversations, OAs, and rejections.
"""

import subprocess
import json
import re
import os
import sys

IGNORE_SENDERS = [
    "notifications@github.com",
    "noreply@jobright.ai",
    "jobs-noreply@linkedin.com",
    "newsletters-noreply@linkedin.com",
    "jobalerts-noreply@linkedin.com",
    "donotreply@jobalert.indeed.com",
    "alerts@johnsonjobs.com",
    "jobs@alerts.jobot.com",
    "updates@schwarzenegger.com",
    "samsung"
]

IGNORE_SUBJECTS = [
    "run failed", "job alert", "jobs you may like", "recommended jobs",
    "daily job alert", "weekly digest", "posted a", "match", "weekly wisdom"
]

def is_ignored(subject, sender):
    s_sub = subject.lower()
    s_snd = sender.lower()
    for ign in IGNORE_SENDERS:
        if ign in s_snd:
            return True
    for ign in IGNORE_SUBJECTS:
        if ign in s_sub:
            return True
    return False

def scan_batch(start_idx, end_idx):
    script = f"""
    tell application "Mail"
        set acc to account "Google"
        set mb to first mailbox of acc whose name is "Job"
        set outText to ""
        repeat with i from {start_idx} to {end_idx}
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
                set outText to outText & (i as string) & "«F»" & s & "«F»" & snd & "«F»" & dt & "«F»" & snip & "«R»"
            end try
        end repeat
        return outText
    end tell
    """
    try:
        p = subprocess.run(["osascript", "-e", script], capture_output=True, text=True, timeout=45)
        raw = p.stdout.strip()
    except Exception as e:
        print(f"Batch {start_idx}-{end_idx} error: {e}", file=sys.stderr)
        return []

    results = []
    for record in raw.split("«R»"):
        if not record.strip():
            continue
        parts = record.split("«F»")
        if len(parts) >= 5:
            idx = int(parts[0])
            subj = parts[1].strip()
            snd = parts[2].strip()
            dt = parts[3].strip()
            snip = parts[4].strip().replace("\r", " ").replace("\n", " ")
            
            if not is_ignored(subj, snd):
                results.append({
                    "index": idx,
                    "subject": subj,
                    "sender": snd,
                    "date": dt,
                    "snippet": snip
                })
    return results

def main():
    print("🔍 Deep scanning Google -> Job (messages 1 to 300)...")
    all_relevant = []
    
    # Process in batches of 30 to avoid AppleScript timeouts
    for b_start in range(1, 301, 30):
        b_end = min(b_start + 29, 300)
        print(f"Scanning range [{b_start}..{b_end}]...")
        batch_res = scan_batch(b_start, b_end)
        print(f"  Found {len(batch_res)} relevant messages.")
        all_relevant.extend(batch_res)

    out_file = "/Users/shreejitverma/github/SDE-Interview-Prep/16-Interview-Command-Center/03-Pipeline/google_job_scanned.json"
    with open(out_file, "w") as f:
        json.dump(all_relevant, f, indent=2)

    print(f"\n✅ Total relevant job communications found: {len(all_relevant)}")
    print(f"Saved to {out_file}")
    
    # Print summary of matches
    print("\n--- Summary of Found Emails ---")
    for item in all_relevant:
        print(f"[{item['index']}] {item['date']}")
        print(f"   From: {item['sender']}")
        print(f"   Subject: {item['subject']}")
        print(f"   Snippet: {item['snippet'][:120]}...\n")

if __name__ == "__main__":
    main()
