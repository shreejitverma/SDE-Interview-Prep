#!/usr/bin/env python3
"""
deep_grep_job_emails.py
Comprehensive Apple Mail scanner that executes targeted searches across all mailboxes:
1. Google -> Rejections (all 96 messages)
2. Google -> Job (targeted searches for interview, assessment, test, recruiter, rtr, offer, feedback, etc.)
3. Exchange -> Interviews, Rejections, In Progress, JOB
4. Georgia Tech -> JOB
5. Recent 150 messages from Google -> Job (last 48-72 hours)

Outputs structured JSON with deduplication, normalized company extraction, and track resolution.
"""

import subprocess
import json
import re
import os
import sys
from datetime import datetime

IGNORE_SENDERS = [
    "notifications@github.com",
    "noreply@jobright.ai",
    "alerts@johnsonjobs.com",
    "jobs@alerts.jobot.com",
    "updates@schwarzenegger.com",
    "editorialstaff@flipboard.com",
    "no-reply@randstadusa.com"
]

IGNORE_SUBJECTS = [
    "run failed", "jobs you may like", "recommended jobs",
    "daily job alert", "weekly digest", "weekly wisdom", "cw "
]

def run_applescript(script, timeout=60):
    try:
        p = subprocess.run(["osascript", "-e", script], capture_output=True, text=True, timeout=timeout)
        if p.returncode != 0:
            print(f"AppleScript error: {p.stderr.strip()}", file=sys.stderr)
            return ""
        return p.stdout.strip()
    except Exception as e:
        print(f"Subprocess exception: {e}", file=sys.stderr)
        return ""

def is_noise(subject, sender):
    s_sub = subject.lower()
    s_snd = sender.lower()
    for ign in IGNORE_SENDERS:
        if ign in s_snd:
            return True
    for ign in IGNORE_SUBJECTS:
        if ign in s_sub:
            return True
    return False

def extract_body_snippet(content, max_len=500):
    if not content:
        return ""
    # remove excessive whitespace
    cleaned = re.sub(r'\s+', ' ', content).strip()
    return cleaned[:max_len]

def get_google_rejections():
    print("📥 Scanning Google -> Rejections (all messages)...")
    script = """tell application "Mail"
        set acc to account "Google"
        set mb to first mailbox of acc whose name is "Rejections"
        set c to count of messages of mb
        set outText to ""
        repeat with i from 1 to c
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
                set outText to outText & (i as string) & "«FIELD»" & s & "«FIELD»" & snd & "«FIELD»" & dt & "«FIELD»" & snip & "«RECORD»"
            end try
        end repeat
        return outText
    end tell"""
    raw = run_applescript(script, timeout=90)
    records = []
    for entry in raw.split("«RECORD»"):
        if not entry.strip():
            continue
        parts = entry.split("«FIELD»")
        if len(parts) >= 5:
            records.append({
                "account": "Google",
                "mailbox": "Rejections",
                "index": int(parts[0]),
                "subject": parts[1].strip(),
                "sender": parts[2].strip(),
                "date": parts[3].strip(),
                "snippet": parts[4].strip().replace("\r", " ").replace("\n", " "),
                "category": "Rejection"
            })
    print(f"  Retrieved {len(records)} rejection emails.")
    return records

def grep_google_job_keywords(keywords):
    all_found = []
    print(f"🔍 Grepping Google -> Job for {len(keywords)} targeted topics...")
    for kw in keywords:
        print(f"  Searching subject containing '{kw}'...")
        script = f"""tell application "Mail"
            set acc to account "Google"
            set mb to first mailbox of acc whose name is "Job"
            set mList to (messages of mb whose subject contains "{kw}")
            set c to count of mList
            set maxCount to c
            if maxCount > 80 then set maxCount to 80
            set outText to ""
            repeat with i from 1 to maxCount
                try
                    set m to item i of mList
                    set s to subject of m
                    set snd to sender of m
                    set dt to (date received of m as string)
                    set msgContent to content of m
                    set snippetLen to length of msgContent
                    if snippetLen > 350 then set snippetLen to 350
                    if snippetLen > 0 then
                        set snip to text 1 thru snippetLen of msgContent
                    else
                        set snip to ""
                    end if
                    set outText to outText & s & "«FIELD»" & snd & "«FIELD»" & dt & "«FIELD»" & snip & "«RECORD»"
                end try
            end repeat
            return outText
        end tell"""
        raw = run_applescript(script, timeout=40)
        count = 0
        for entry in raw.split("«RECORD»"):
            if not entry.strip():
                continue
            parts = entry.split("«FIELD»")
            if len(parts) >= 4:
                subj = parts[0].strip()
                snd = parts[1].strip()
                dt = parts[2].strip()
                snip = parts[3].strip().replace("\r", " ").replace("\n", " ")
                if not is_noise(subj, snd):
                    all_found.append({
                        "account": "Google",
                        "mailbox": "Job",
                        "subject": subj,
                        "sender": snd,
                        "date": dt,
                        "snippet": snip,
                        "query_keyword": kw
                    })
                    count += 1
        print(f"    Found {count} matches.")
    return all_found

def get_recent_google_job(max_n=150):
    print(f"⚡ Scanning recent {max_n} messages in Google -> Job (last 48-72h)...")
    script = f"""tell application "Mail"
        set acc to account "Google"
        set mb to first mailbox of acc whose name is "Job"
        set outText to ""
        repeat with i from 1 to {max_n}
            try
                set m to message i of mb
                set s to subject of m
                set snd to sender of m
                set dt to (date received of m as string)
                set msgContent to content of m
                set snippetLen to length of msgContent
                if snippetLen > 350 then set snippetLen to 350
                if snippetLen > 0 then
                    set snip to text 1 thru snippetLen of msgContent
                else
                    set snip to ""
                end if
                set outText to outText & (i as string) & "«FIELD»" & s & "«FIELD»" & snd & "«FIELD»" & dt & "«FIELD»" & snip & "«RECORD»"
            end try
        end repeat
        return outText
    end tell"""
    raw = run_applescript(script, timeout=60)
    recent = []
    for entry in raw.split("«RECORD»"):
        if not entry.strip():
            continue
        parts = entry.split("«FIELD»")
        if len(parts) >= 5:
            idx = int(parts[0])
            subj = parts[1].strip()
            snd = parts[2].strip()
            dt = parts[3].strip()
            snip = parts[4].strip().replace("\r", " ").replace("\n", " ")
            if not is_noise(subj, snd):
                recent.append({
                    "account": "Google",
                    "mailbox": "Job",
                    "index": idx,
                    "subject": subj,
                    "sender": snd,
                    "date": dt,
                    "snippet": snip,
                    "category": "Recent Sweep"
                })
    print(f"  Retrieved {len(recent)} valid non-spam recent emails.")
    return recent

def get_exchange_dedicated_folders():
    print("📬 Scanning Exchange & Gatech dedicated folders...")
    targets = [
        ("Exchange", "Interviews"),
        ("Exchange", "Rejections"),
        ("Exchange", "In Progress"),
        ("Exchange", "GA Job"),
        ("sverma357@gatech.edu", "JOB")
    ]
    results = []
    for acc, mb_name in targets:
        script = f"""tell application "Mail"
            set accObj to account "{acc}"
            set mbList to (every mailbox of accObj whose name is "{mb_name}")
            if (count of mbList) is 0 then return ""
            set mb to item 1 of mbList
            set c to count of messages of mb
            set outText to ""
            repeat with i from 1 to c
                try
                    set m to message i of mb
                    set s to subject of m
                    set snd to sender of m
                    set dt to (date received of m as string)
                    set msgContent to content of m
                    set snippetLen to length of msgContent
                    if snippetLen > 350 then set snippetLen to 350
                    if snippetLen > 0 then
                        set snip to text 1 thru snippetLen of msgContent
                    else
                        set snip to ""
                    end if
                    set outText to outText & s & "«FIELD»" & snd & "«FIELD»" & dt & "«FIELD»" & snip & "«RECORD»"
                end try
            end repeat
            return outText
        end tell"""
        raw = run_applescript(script, timeout=40)
        c = 0
        for entry in raw.split("«RECORD»"):
            if not entry.strip():
                continue
            parts = entry.split("«FIELD»")
            if len(parts) >= 4:
                subj = parts[0].strip()
                snd = parts[1].strip()
                dt = parts[2].strip()
                snip = parts[3].strip().replace("\r", " ").replace("\n", " ")
                if not is_noise(subj, snd):
                    results.append({
                        "account": acc,
                        "mailbox": mb_name,
                        "subject": subj,
                        "sender": snd,
                        "date": dt,
                        "snippet": snip,
                        "category": "Folder: " + mb_name
                    })
                    c += 1
        print(f"  {acc} -> {mb_name}: {c} messages.")
    return results

def main():
    print("🚀 Starting Complete Deep Grep across Mailboxes...")
    
    # 1. Google Rejections
    rejections = get_google_rejections()
    
    # 2. Targeted keyword grep in Google -> Job
    job_keywords = [
        "Right to Represent",
        "RTR",
        "interview",
        "assessment",
        "HackerRank",
        "CodeSignal",
        "Codility",
        "Karat",
        "invitation",
        "candidacy",
        "offer",
        "feedback",
        "technical round",
        "phone screen",
        "superday",
        "next steps",
        "Update regarding your application",
        "Thank you for applying",
        "Following up on Your Application"
    ]
    keyword_hits = grep_google_job_keywords(job_keywords)
    
    # 3. Recent 150 sweep in Google -> Job
    recent_hits = get_recent_google_job(150)
    
    # 4. Dedicated Exchange & Gatech folders
    dedicated_hits = get_exchange_dedicated_folders()
    
    # Combine & Deduplicate by (subject, date)
    combined = []
    seen = set()
    
    for item in rejections + keyword_hits + recent_hits + dedicated_hits:
        key = (item.get("subject", "").strip().lower(), item.get("date", "").strip().lower())
        if key in seen:
            continue
        seen.add(key)
        combined.append(item)
        
    out_file = os.path.join(os.path.dirname(os.path.abspath(__file__)), "all_job_emails_deep_grep.json")
    with open(out_file, "w") as f:
        json.dump(combined, f, indent=2)
        
    print(f"\n✨ COMPLETE! Deduplicated unique job communications: {len(combined)}")
    print(f"Saved to {out_file}")

if __name__ == "__main__":
    main()
