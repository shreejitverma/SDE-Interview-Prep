#!/usr/bin/env python3
import subprocess
import json
import re
import os
from datetime import datetime

ACCOUNTS_FOLDERS = [
    ("Exchange", ["Interviews", "Rejections", "In Progress", "GA Job", "BigInterview", "Career Brew", "Bloomberg", "Ford", "Inbox"]),
    ("Google", ["Bank of America", "Rejections", "INBOX"]),
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
    "thank you for your interest", "applied", "candidacy", "position"
]

def run_applescript(script):
    try:
        p = subprocess.run(["osascript", "-e", script], capture_output=True, text=True, timeout=30)
        return p.stdout.strip()
    except Exception as e:
        print(f"Error running AppleScript: {e}")
        return ""

def fetch_messages_from_mailbox(acc, mb_name, limit=60):
    script = f"""
    tell application "Mail"
        set accObj to account "{acc}"
        set mbList to (every mailbox of accObj whose name is "{mb_name}")
        if (count of mbList) is 0 then return "EMPTY"
        set mb to item 1 of mbList
        set msgCount to count of messages of mb
        if msgCount is 0 then return "EMPTY"
        set startIdx to msgCount - {limit} + 1
        if startIdx < 1 then set startIdx to 1
        
        set outText to ""
        repeat with i from msgCount to startIdx by -1
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
            subject, sender, date_str, snippet = parts[0].strip(), parts[1].strip(), parts[2].strip(), parts[3].strip()
            records.append({
                "account": acc,
                "mailbox": mb_name,
                "subject": subject,
                "sender": sender,
                "date": date_str,
                "snippet": snippet
            })
    return records

def is_job_related(subject, sender, snippet):
    text = (subject + " " + sender + " " + snippet).lower()
    return any(k in text for k in JOB_KEYWORDS)

def main():
    all_job_emails = []
    print("🔍 Scanning Apple Mail accounts...")

    for acc, mailboxes in ACCOUNTS_FOLDERS:
        for mb in mailboxes:
            print(f"Scanning {acc} -> {mb}...")
            # If it's a dedicated interview/rejection folder, fetch more
            is_dedicated = any(w in mb.lower() for w in ["interview", "rejection", "job", "bank of america"])
            limit = 100 if is_dedicated else 40
            
            msgs = fetch_messages_from_mailbox(acc, mb, limit=limit)
            print(f"  Retrieved {len(msgs)} messages.")
            
            for m in msgs:
                if is_dedicated or is_job_related(m["subject"], m["sender"], m["snippet"]):
                    # Avoid spam/job alerts
                    s_lower = m["subject"].lower()
                    if "job alert" in s_lower or "jobs you may like" in s_lower or "weekly digest" in s_lower:
                        continue
                    all_job_emails.append(m)

    output_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "extracted_emails.json")
    with open(output_path, "w") as f:
        json.dump(all_job_emails, f, indent=2)

    print(f"\n✅ Total relevant job emails extracted: {len(all_job_emails)}")
    print(f"Saved to {output_path}")

if __name__ == "__main__":
    main()
