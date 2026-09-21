---
tags: [gmail, automation, pipeline, prompt, ingestion, tools, daily-sync]
---

# 📬 Master Gmail Job Ingestion & Daily 48-Hour Sync System

> **Purpose:** A dual-mode system to automatically fetch, extract, and integrate all job-hunting emails from Gmail into your **Interview Command Center**:
> 1. **Full Initial Sync:** Historical ingestion of all past applications, recruiter outreach, and OAs.
> 2. **Daily Incremental Sync (Rolling 48h):** A 60-second daily routine to refresh active pipelines, advance stages, append interview rounds, record assessment links, and log rejections without creating duplicate files.

---

## ⚡ Quick Reference: The Two Gmail Search Queries

### 1. 🔄 Daily Refresh Query (Rolling Past 2 Days — Run Every Morning)
Copy and paste this into Gmail every morning to grab only updates from the past 48 hours:

```gmail
("application" OR "interview" OR "recruiter" OR "hiring team" OR "assessment" OR "coding challenge" OR "hackerrank" OR "codesignal" OR "karat" OR "codility" OR "right to represent" OR "offer" OR "rejection" OR "status of your application" OR "next steps" OR "scheduling" OR "availability" OR "phone screen" OR "technical round" OR "onsite" OR "take-home" OR "congratulations" OR "thank you for your interest" OR "applied to") -from:(jobalerts-noreply@linkedin.com OR alert@indeed.com OR messages-noreply@linkedin.com OR digest-noreply@quora.com OR "newsletter" OR "job recommendations" OR "jobs for you") -subject:("job alert" OR "jobs you may like" OR "recommended jobs" OR "daily job alert" OR "weekly digest") newer_than:2d
```

### 2. 📚 Full Initial Historical Query (All-Time / Last 60 Days)
Run this once to discover and bootstrap all existing historical pipelines:

```gmail
("application" OR "interview" OR "recruiter" OR "hiring team" OR "assessment" OR "coding challenge" OR "hackerrank" OR "codesignal" OR "karat" OR "codility" OR "right to represent" OR "offer" OR "rejection" OR "status of your application" OR "next steps" OR "scheduling" OR "availability" OR "phone screen" OR "technical round" OR "onsite" OR "take-home" OR "congratulations" OR "thank you for your interest" OR "applied to") -from:(jobalerts-noreply@linkedin.com OR alert@indeed.com OR messages-noreply@linkedin.com OR digest-noreply@quora.com OR "newsletter" OR "job recommendations" OR "jobs for you") -subject:("job alert" OR "jobs you may like" OR "recommended jobs" OR "daily job alert" OR "weekly digest")
```

---

## 🤖 The Master AI Prompt (With Dual-Mode Support)

This prompt features an automatic **Sync Mode Detector** that handles both initial onboarding and state-preserving daily refreshes.

```markdown
You are an elite Career Operations & Interview Pipeline Assistant managing my "16-Interview-Command-Center" Obsidian vault.

### OPERATIONAL SYNC MODES:
Determine the operational mode based on the user request:

1. [MODE: FULL_SYNC] -> Ingesting historical emails to create new company directories and baseline interview tracker files.
2. [MODE: DAILY_INCREMENTAL] -> Processing new emails from the past 48 hours to update, advance, or create pipeline entries.

---

### INGESTION & MATCHING RULES:

#### Step 1: Identity & Entity Resolution
From the email(s), extract:
- Company Name (standardized slug, e.g., "Bank-of-America", "Citadel", "Google", "Meta")
- Role Title & Target Track (SDE, Quant-Dev, Quant-Research, AI-Engineer, Low-Latency)
- Seniority Level (Junior, Mid, Senior, Lead, VP, Contract)
- Sender & Stakeholder Info (Recruiter name, email, agency, hiring manager)
- Event Type:
  * Application Receipt (ATS acknowledgment)
  * Recruiter Screen / Right to Represent (RTR) confirmation
  * Online Assessment (OA) / Take-home challenge invitation + deadline
  * Interview Scheduling Request / Calendar Confirmation (Date/Time/Format)
  * Interview Feedback / Next Round Progression
  * Offer Letter / Compensation Rate Lock
  * Rejection Notice

#### Step 2: State-Aware Pipeline Handling (For DAILY_INCREMENTAL)
Does this company/role already exist in my vault?
- IF EXISTING ACTIVE TRACK:
  DO NOT recreate the file. Output a structured "FILE UPDATE PATCH":
  1. Updated Frontmatter Fields:
     - Advance `stage` (e.g., `applied` -> `phone-screen`, or `phone-screen` -> `technical`)
     - Update `next_action` with the exact next requirement (e.g., "Complete HackerRank by Sept 22")
     - Update `next_deadline` (YYYY-MM-DD)
  2. Append Timeline Row to `## 📋 Interview Timeline`:
     `| YYYY-MM-DD | [Round Name] | [Interviewer/Recruiter] | [Format] | [Duration] | [Status] |`
  3. Action Checklist Items: Add any test links, prep tasks, or confirmation replies needed.

- IF BRAND NEW APPLICATION OR RECRUITER OUTREACH:
  Create the dedicated folder and full tracker note under:
  `16-Interview-Command-Center/03-Pipeline/Active/[Company-Name]/[Track-or-Role]/[Company]-[Role]-Interview-Tracker.md`
  using the standard Interview Command Center YAML frontmatter.

- IF REJECTION:
  - Update `stage: rejected` in the frontmatter.
  - Append rejection date to Timeline table.
  - Suggest creating a retrospective note in `04-Retrospectives/` if an interview was completed.

---

### OUTPUT FORMAT SPECIFICATION:

For each email or thread processed, generate:

#### 1. Action Type: [NEW_TRACK | UPDATE_EXISTING | STAGE_ADVANCEMENT | REJECTION]

#### 2. Target File Path:
`16-Interview-Command-Center/03-Pipeline/Active/[Company-Name]/...`

#### 3. Exact Markdown Output:
- If NEW_TRACK: Complete Interview Tracker Markdown with full frontmatter, timeline, technical checklist, and notes template.
- If UPDATE_EXISTING: Precise diff/snippet showing updated YAML frontmatter and the new timeline table row to insert.

#### 4. Daily Log Entry (Copy-Paste for `06-Daily-Log/YYYY-MM-DD.md`):
A clean 2-3 line summary:
- **[Company]** ([Role]): [Event Description] -> Action required: [Next Step] (Due: [Date])

---
RAW EMAIL TEXT / DAILY 48-HOUR EXPORT TO PROCESS:
"""
[PASTE EMAILS OR THREADS HERE]
"""
```

---

## 📅 The 60-Second Daily Morning Sync Routine

Integrate this into your morning routine to stay 100% on top of all recruiter messages and coding test deadlines:

```
Step 1: Open Gmail Search
   │
   │ Paste Daily Query:
   │ newer_than:2d
   ▼
Step 2: Copy Any Job Emails From Yesterday / Today
   │
   │ Copy thread text (recruiter reply, OA link, interview invite)
   ▼
Step 3: Paste into Assistant Chat
   │
   │ "Here is my daily 48h Gmail sync. Update my Interview Command Center."
   ▼
Step 4: Vault Automatically Updated
   │
   │ Tracker notes updated with new dates and stages
   │ 00-Dashboard.md reflects updated deadlines
   │ Daily log recorded
```

---

## 🐍 Automated Python Script (With `--mode daily` vs `--mode full`)

Save this script as `sync_job_emails.py`. It supports both full historical sync and daily 48-hour rolling refreshes:

```python
"""
sync_job_emails.py - Automated Dual-Mode Gmail to Obsidian Ingestion
Modes:
  python sync_job_emails.py --mode daily   # Fetches last 48 hours
  python sync_job_emails.py --mode full    # Fetches all historical
"""

import os
import sys
import argparse
from datetime import datetime
from google.auth.transport.requests import Request
from google.oauth2.credentials import Credentials
from google_auth_oauthlib.flow import InstalledAppFlow
from googleapiclient.discovery import build

SCOPES = ['https://www.googleapis.com/auth/gmail.readonly']
VAULT_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "Active")

BASE_FILTER = (
    '("application" OR "interview" OR "recruiter" OR "hiring team" OR "assessment" OR '
    '"coding challenge" OR "hackerrank" OR "codesignal" OR "right to represent" OR "offer" OR "rejection") '
    '-from:(jobalerts-noreply@linkedin.com OR alert@indeed.com OR messages-noreply@linkedin.com) '
    '-subject:("job alert" OR "jobs you may like")'
)

def build_query(mode: str) -> str:
    if mode == "daily":
        return f"{BASE_FILTER} newer_than:2d"
    return BASE_FILTER

def get_gmail_service():
    creds = None
    if os.path.exists('token.json'):
        creds = Credentials.from_authorized_user_file('token.json', SCOPES)
    if not creds or not creds.valid:
        if creds and creds.expired and creds.refresh_token:
            creds.refresh(Request())
        else:
            flow = InstalledAppFlow.from_client_secrets_file('credentials.json', SCOPES)
            creds = flow.run_local_server(port=0)
        with open('token.json', 'w') as token:
            token.write(creds.to_json())
    return build('gmail', 'v1', credentials=creds)

def sync_emails(mode: str):
    service = get_gmail_service()
    query = build_query(mode)
    print(f"🔄 Executing [{mode.upper()}] sync with query:\n{query}\n")

    results = service.users().messages().list(userId='me', q=query, maxResults=30).execute()
    messages = results.get('messages', [])

    if not messages:
        print("✅ No new job-related emails found in this time window.")
        return

    print(f"📬 Found {len(messages)} matching emails.\n" + "="*60)

    for msg_meta in messages:
        msg = service.users().messages().get(userId='me', id=msg_meta['id'], format='full').execute()
        headers = {h['name'].lower(): h['value'] for h in msg['payload']['headers']}
        
        subject = headers.get('subject', 'No Subject')
        sender = headers.get('from', 'Unknown Sender')
        date_str = headers.get('date', '')
        snippet = msg.get('snippet', '')

        print(f"\n📩 [{date_str}]")
        print(f"   From: {sender}")
        print(f"   Subject: {subject}")
        print(f"   Snippet: {snippet[:140]}...")

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Sync Gmail Job Emails to Obsidian")
    parser.add_argument("--mode", choices=["daily", "full"], default="daily", help="Sync mode: 'daily' (last 48h) or 'full' (all)")
    args = parser.parse_args()
    sync_emails(args.mode)
```
