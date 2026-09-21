#!/usr/bin/env python3
"""
generate_vault_trackers.py
Generates and populates Obsidian markdown files for newly discovered active pipelines,
archived pipelines, and company intelligence dossiers.
"""

import os
import json

VAULT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ACTIVE_DIR = os.path.join(VAULT_ROOT, "03-Pipeline", "Active")
ARCHIVE_DIR = os.path.join(VAULT_ROOT, "03-Pipeline", "Archive")
COMPANIES_DIR = os.path.join(VAULT_ROOT, "02-Companies")

# Active pipelines to create
ACTIVE_PIPELINES = [
    {
        "folder": "Teza-Technologies",
        "file": "Teza-Quant-Tracker.md",
        "company": "Teza Technologies",
        "company_file": "Teza-Technologies.md",
        "role": "Quantitative Researcher / Quant Developer",
        "stage": "Interview",
        "status": "Interview Scheduled / Active",
        "track": "Quant-Research / Quant-Dev",
        "priority": "P1",
        "confidence": "High",
        "date_applied": "2026-09-01",
        "last_contact": "2026-09-14",
        "next_action": "Follow up on upcoming interview schedule with Gia German",
        "next_deadline": "2026-09-21",
        "salary_range": "$250,000 - $350,000 + Bonus",
        "location": "New York, NY (Hybrid)",
        "stakeholders": [
            {"name": "Gia German", "email": "ggerman@teza.com", "role": "Talent Acquisition Coordinator"},
            {"name": "Teza Hiring Team", "email": "no-reply@ashbyhq.com", "role": "Hiring Platform"}
        ],
        "comms": [
            {"date": "2026-09-14 12:55 PM", "sender": "Gia German <ggerman@teza.com>", "subject": "Re: Reminder: Your Upcoming Interview with Teza Technologies", "note": "Interview coordination & confirmation"},
            {"date": "2026-09-14 12:03 PM", "sender": "Gia German <ggerman@teza.com>", "subject": "Re: Reminder: Your Upcoming Interview with Teza Technologies", "note": "Logistics and interview schedule details"},
            {"date": "2026-09-13 12:01 PM", "sender": "Teza Technologies Hiring Team <no-reply@ashbyhq.com>", "subject": "Reminder: Your Upcoming Interview with Teza Technologies", "note": "Ashby system calendar invite and prep details"}
        ]
    },
    {
        "folder": "ATT-Labs",
        "file": "ATT-Labs-Distributed-Systems-Tracker.md",
        "company": "AT&T Labs",
        "company_file": "ATT-Labs.md",
        "role": "Distributed Systems Software Engineer",
        "stage": "Final Round",
        "status": "Final Onsite Interview Completed",
        "track": "Low-Latency / Distributed Systems",
        "priority": "P1",
        "confidence": "High",
        "date_applied": "2026-08-20",
        "last_contact": "2026-09-10",
        "next_action": "Debrief with Benjamin Byrne on client feedback and offer decision",
        "next_deadline": "2026-09-22",
        "salary_range": "$175,000 - $210,000",
        "location": "Middletown, NJ / New York, NY",
        "stakeholders": [
            {"name": "Benjamin Byrne", "email": "Benjamin.Byrne@insightglobal.com", "role": "Insight Global Account Manager"}
        ],
        "comms": [
            {"date": "2026-09-10 08:24 AM", "sender": "Shreejit Verma <shreejitverma@gmail.com>", "subject": "Re: Final Onsite Interview - AT&T Labs - Distributed Systems Software Engineer", "note": "Post-interview follow-up and notes"},
            {"date": "2026-09-09 08:55 AM", "sender": "Shreejit Verma <shreejitverma@gmail.com>", "subject": "Re: Final Onsite Interview - AT&T Labs - Distributed Systems Software Engineer", "note": "Confirmation of final onsite schedule"},
            {"date": "2026-09-09 08:39 AM", "sender": "Benjamin Byrne <Benjamin.Byrne@insightglobal.com>", "subject": "Final Onsite Interview - AT&T Labs - Distributed Systems Software Engineer", "note": "Official final onsite schedule and panel details"},
            {"date": "2026-09-01 11:35 AM", "sender": "Benjamin Byrne <Benjamin.Byrne@insightglobal.com>", "subject": "Client Interview - AT&T Labs - Software Engineer", "note": "First client technical round with AT&T team"},
            {"date": "2026-08-27 09:00 AM", "sender": "Benjamin Byrne <Benjamin.Byrne@insightglobal.com>", "subject": "Client Interview - AT&T Labs - Software Engineer", "note": "Initial screening setup"}
        ]
    },
    {
        "folder": "DRW",
        "file": "DRW-FICC-Tools-Developer-Tracker.md",
        "company": "DRW",
        "company_file": "DRW.md",
        "role": "FICC Desk Tools Developer",
        "stage": "Interview",
        "status": "Interview Loop Completed / Awaiting Feedback",
        "track": "Quant-Dev / Market Data",
        "priority": "P1",
        "confidence": "Medium",
        "date_applied": "2026-07-20",
        "last_contact": "2026-07-29",
        "next_action": "Check in with Intec Select for latest FICC team updates",
        "next_deadline": "2026-09-25",
        "salary_range": "$220,000 - $280,000 + PnL Bonus",
        "location": "New York, NY",
        "stakeholders": [
            {"name": "Jamie Mumford", "email": "jamie.mumford@intecselect.com", "role": "Intec Select Recruitment Consultant"},
            {"name": "Jai Bahra", "email": "Jai.Bahra@intecselect.com", "role": "Intec Select Senior Partner"}
        ],
        "comms": [
            {"date": "2026-07-29 03:35 AM", "sender": "Shreejit Verma <shreejitverma@gmail.com>", "subject": "Re: Update on Your Application with DRW", "note": "Availability and follow-up sent to Jai Bahra"},
            {"date": "2026-07-29 03:29 AM", "sender": "Jai Bahra <Jai.Bahra@intecselect.com>", "subject": "Update on Your Application with DRW", "note": "Feedback update from DRW trading desk"},
            {"date": "2026-07-27 06:23 AM", "sender": "Jamie Mumford <jamie.mumford@intecselect.com>", "subject": "Re: Interview Confirmation - DRW - FICC Desk Tools Developer", "note": "Technical loop details & confirmation"},
            {"date": "2026-07-24 03:53 AM", "sender": "Jai Bahra <Jai.Bahra@intecselect.com>", "subject": "Interview Confirmation - DRW - FICC Desk Tools Developer", "note": "Interview confirmation with FICC desk heads"}
        ]
    },
    {
        "folder": "Talan",
        "file": "Talan-CPP-Market-Data-Tracker.md",
        "company": "Talan",
        "company_file": "Talan.md",
        "role": "C++ Software Engineer - Market Data",
        "stage": "Interview",
        "status": "Technical Interview Loop Completed",
        "track": "Low-Latency / Market Data",
        "priority": "P2",
        "confidence": "Medium",
        "date_applied": "2026-06-10",
        "last_contact": "2026-06-16",
        "next_action": "Review Market Data C++ notes and await client assignment matching",
        "next_deadline": "2026-09-30",
        "salary_range": "$160,000 - $190,000",
        "location": "New York, NY",
        "stakeholders": [
            {"name": "Kelley Chung", "email": "notifications@smartrecruiters.talan.com", "role": "Talan Americas Senior Recruiter"}
        ],
        "comms": [
            {"date": "2026-06-16 09:01 AM", "sender": "Talan Hiring Team", "subject": "Your application to Talan", "note": "Candidate status updated in SmartRecruiters"},
            {"date": "2026-06-10 05:26 PM", "sender": "Kelley CHUNG from Talan", "subject": "Your first interview for C++ Software Engineer - Market Data at Talan Americas – Schedule now!", "note": "Interview invitation for Market Data C++ role"}
        ]
    },
    {
        "folder": "Fidelity",
        "file": "Fidelity-Principal-Quant-Dev-Tracker.md",
        "company": "Fidelity Investments",
        "company_file": "Fidelity-Investments.md",
        "role": "Principal Quant Developer (Req 2125023 / 2126133)",
        "stage": "Interview",
        "status": "Video Interviews Completed / Archive Review",
        "track": "Quant-Dev",
        "priority": "P1",
        "confidence": "Medium",
        "date_applied": "2026-05-01",
        "last_contact": "2026-07-08",
        "next_action": "Check Workday portal for new Principal Quant openings",
        "next_deadline": "2026-10-01",
        "salary_range": "$190,000 - $235,000 + Bonus",
        "location": "Boston, MA / Jersey City, NJ",
        "stakeholders": [
            {"name": "Brianna Collums", "email": "fmr@myworkday.com", "role": "Fidelity Talent Acquisition"},
            {"name": "Anay Gonzalez", "email": "fmr@myworkday.com", "role": "Fidelity Campus & Lateral Recruiting"},
            {"name": "Ansila Antony", "email": "ansila@digipulsetech.com", "role": "DigiPulse Tech Partner"}
        ],
        "comms": [
            {"date": "2026-07-08 03:51 AM", "sender": "fmr@myworkday.com", "subject": "Follow Up to Your Fidelity Application - Candidacy Update", "note": "Candidacy update on summer loop"},
            {"date": "2026-06-03 12:11 PM", "sender": "Brianna Collums <fmr@myworkday.com>", "subject": "Fidelity Investments Video Interview for 2125023 Principal Quant Developer", "note": "Video Interview invite"},
            {"date": "2026-05-27 08:55 AM", "sender": "Ansila Antony <ansila@digipulsetech.com>", "subject": "Submission Confirmation for Fidelity Investments (RTR) and (Rate Acceptance)", "note": "RTR signed for Fidelity"}
        ]
    }
]

# Archived pipelines to create
ARCHIVE_PIPELINES = [
    {
        "folder": "Renaissance-Technologies",
        "file": "Rentec-Quant-Tracker.md",
        "company": "Renaissance Technologies",
        "company_file": "Renaissance-Technologies.md",
        "role": "Quantitative Researcher / Developer",
        "stage": "Rejected",
        "date_applied": "2026-07-15",
        "date_rejected": "2026-07-30",
        "rejection_reason": "Resume review post-submission; elite bar",
        "recruiter": "Denise Gennari (denise@rentec.com)",
        "track": "Quant-Research",
        "takeaway": "Apply with published math/stats papers or proven signal generation track record."
    },
    {
        "folder": "FalconX",
        "file": "FalconX-Quant-Tracker.md",
        "company": "FalconX",
        "company_file": "FalconX.md",
        "role": "Quantitative Developer / Researcher",
        "stage": "Rejected",
        "date_applied": "2026-08-10",
        "date_rejected": "2026-08-28",
        "rejection_reason": "Application reviewed via Greenhouse; position filled",
        "recruiter": "no-reply@us.greenhouse-mail.io",
        "track": "Quant-Dev",
        "takeaway": "Crypto institutional prime brokerage; strengthen digital asset low-latency trading angle."
    },
    {
        "folder": "Millennium-Management",
        "file": "Millennium-Tracker.md",
        "company": "Millennium Management",
        "company_file": "Millennium-Management.md",
        "role": "Quantitative Developer / Alpha Research",
        "stage": "Rejected",
        "date_applied": "2026-07-01",
        "date_rejected": "2026-07-28",
        "rejection_reason": "Pod-specific hiring mismatch",
        "recruiter": "Millennium Recruiting Team (millenniumrecruitingteam@careers.mlp.com)",
        "track": "Quant-Dev",
        "takeaway": "Multi-manager pod model; requires direct pod PM sponsorship."
    },
    {
        "folder": "Point72-Cubist",
        "file": "Point72-Cubist-Tracker.md",
        "company": "Point72",
        "company_file": "Point72.md",
        "role": "Quantitative Developer - Cubist Systematic Strategies",
        "stage": "Rejected",
        "date_applied": "2026-05-10",
        "date_rejected": "2026-05-29",
        "rejection_reason": "Application update received via Workday/talent@cubistsystematic.com",
        "recruiter": "talent@cubistsystematic.com",
        "track": "Quant-Dev",
        "takeaway": "Cubist has rigorous Python/C++ algorithmic rounds. Re-apply in 12 months."
    },
    {
        "folder": "DE-Shaw",
        "file": "DE-Shaw-Tracker.md",
        "company": "D. E. Shaw",
        "company_file": "DE-Shaw.md",
        "role": "Quantitative Developer / Analyst",
        "stage": "Rejected",
        "date_applied": "2026-05-25",
        "date_rejected": "2026-06-18",
        "rejection_reason": "Resume screening rejection from recruiting@deshaw.com",
        "recruiter": "recruiting@deshaw.com",
        "track": "Quant-Dev",
        "takeaway": "D. E. Shaw values Olympiad/Putnam/ICPC background; highlight low-level architecture."
    },
    {
        "folder": "Squarepoint-Capital",
        "file": "Squarepoint-Tracker.md",
        "company": "Squarepoint Capital",
        "company_file": "Squarepoint-Capital.md",
        "role": "Quantitative Developer",
        "stage": "Rejected",
        "date_applied": "2026-06-01",
        "date_rejected": "2026-06-16",
        "rejection_reason": "Greenhouse application update",
        "recruiter": "no-reply@us.greenhouse-mail.io",
        "track": "Quant-Dev",
        "takeaway": "Global quantitative investment manager; focuses on C++20 and distributed computing."
    },
    {
        "folder": "Schonfeld",
        "file": "Schonfeld-Tracker.md",
        "company": "Schonfeld",
        "company_file": "Schonfeld.md",
        "role": "Senior FX Vol Quant Strategist / Researcher",
        "stage": "Rejected",
        "date_applied": "2026-06-05",
        "date_rejected": "2026-06-17",
        "rejection_reason": "Direct Greenhouse update",
        "recruiter": "no-reply@us.greenhouse-mail.io",
        "track": "Quant-Research",
        "takeaway": "FX options pricing & volatility modeling focus required."
    },
    {
        "folder": "Old-Mission-Capital",
        "file": "Old-Mission-Tracker.md",
        "company": "Old Mission Capital",
        "company_file": "Old-Mission-Capital.md",
        "role": "Quantitative Developer / ETF Market Making",
        "stage": "Rejected",
        "date_applied": "2026-07-10",
        "date_rejected": "2026-07-30",
        "rejection_reason": "Automated notification from no-reply@oldmissioncapital.com",
        "recruiter": "no-reply@oldmissioncapital.com",
        "track": "Low-Latency",
        "takeaway": "ETF market making specialist; requires ultra-low-latency C++."
    },
    {
        "folder": "Tower-Research-Capital",
        "file": "Tower-Research-Tracker.md",
        "company": "Tower Research Capital",
        "company_file": "Tower-Research-Capital.md",
        "role": "Low-Latency C++ Developer",
        "stage": "Rejected",
        "date_applied": "2026-06-15",
        "date_rejected": "2026-06-25",
        "rejection_reason": "Agency feedback via Joseph Cooper (Huxley)",
        "recruiter": "Joseph Cooper (j.cooper@huxley.com)",
        "track": "Low-Latency",
        "takeaway": "Core market data and order gateway team has an extremely low latency bar."
    },
    {
        "folder": "The-Voleon-Group",
        "file": "Voleon-Tracker.md",
        "company": "The Voleon Group",
        "company_file": "The-Voleon-Group.md",
        "role": "Quantitative Researcher / ML Engineer",
        "stage": "Rejected",
        "date_applied": "2026-06-01",
        "date_rejected": "2026-06-19",
        "rejection_reason": "recruiting-noreply@voleon.com notification",
        "recruiter": "recruiting-noreply@voleon.com",
        "track": "AI-Engineer / Quant-Research",
        "takeaway": "Pioneers in ML statistical arbitrage. Highlight PyTorch and high-dimensional time-series."
    }
]

def ensure_dir(path):
    os.makedirs(path, exist_ok=True)

def create_active_trackers():
    print("Creating active trackers...")
    for item in ACTIVE_PIPELINES:
        folder_path = os.path.join(ACTIVE_DIR, item["folder"])
        ensure_dir(folder_path)
        file_path = os.path.join(folder_path, item["file"])
        
        stakeholders_yaml = "\n".join([f"  - name: \"{s['name']}\"\n    email: \"{s['email']}\"\n    role: \"{s['role']}\"" for s in item["stakeholders"]])
        comms_md = "\n".join([f"- **{c['date']}** | `{c['sender']}`: **{c['subject']}**\n  *Notes*: {c['note']}" for c in item["comms"]])
        
        content = f"""---
company: "{item['company']}"
role: "{item['role']}"
stage: "{item['stage']}"
status: "{item['status']}"
track: "{item['track']}"
priority: "{item['priority']}"
confidence: "{item['confidence']}"
date_applied: {item['date_applied']}
last_contact: {item['last_contact']}
next_action: "{item['next_action']}"
next_deadline: {item['next_deadline']}
salary_range: "{item['salary_range']}"
location: "{item['location']}"
stakeholders:
{stakeholders_yaml}
tags:
  - interview-tracker
  - active-pipeline
  - {item['track'].lower()}
---

# {item['company']} - {item['role']}

> [!INFO] Pipeline Status
> **Stage**: `{item['stage']}` | **Status**: {item['status']} | **Priority**: `{item['priority']}`
> **Target Track**: `{item['track']}` | **Company Profile**: [[{item['company_file'].replace('.md', '')}]]

---

## 1. Role Overview & Strategic Opportunity
- **Company**: [[{item['company_file'].replace('.md', '')}|{item['company']}]]
- **Target Track**: `{item['track']}`
- **Compensation Target**: `{item['salary_range']}`
- **Location**: `{item['location']}`

---

## 2. Key Stakeholders & Contacts
| Name | Role | Email |
| :--- | :--- | :--- |
"""
        for s in item["stakeholders"]:
            content += f"| **{s['name']}** | {s['role']} | `{s['email']}` |\n"
            
        content += f"""
---

## 3. Communication Audit Log
{comms_md}

---

## 4. Next Immediate Actions
- [ ] **{item['next_action']}** (Deadline: `{item['next_deadline']}`)
- [ ] Review technical track notes and core architecture for [[{item['company_file'].replace('.md', '')}|{item['company']}]]
"""
        with open(file_path, "w") as f:
            f.write(content)
        print(f"  Created active tracker: {file_path}")

def create_archive_trackers():
    print("Creating archive trackers...")
    for item in ARCHIVE_PIPELINES:
        folder_path = os.path.join(ARCHIVE_DIR, item["folder"])
        ensure_dir(folder_path)
        file_path = os.path.join(folder_path, item["file"])
        
        content = f"""---
company: "{item['company']}"
role: "{item['role']}"
stage: "{item['stage']}"
status: "Archived / Rejected"
track: "{item['track']}"
date_applied: {item['date_applied']}
date_rejected: {item['date_rejected']}
rejection_reason: "{item['rejection_reason']}"
recruiter: "{item['recruiter']}"
tags:
  - interview-tracker
  - archive
  - rejection-postmortem
---

# {item['company']} - {item['role']} (Archived)

> [!WARNING] Post-Mortem & Status
> **Outcome**: `Rejected` on `{item['date_rejected']}`
> **Company Profile**: [[{item['company_file'].replace('.md', '')}]]
> **Reason**: {item['rejection_reason']}

---

## 1. Submission Details
- **Company**: [[{item['company_file'].replace('.md', '')}|{item['company']}]]
- **Role**: `{item['role']}`
- **Track**: `{item['track']}`
- **Applied Date**: `{item['date_applied']}`
- **Rejection Date**: `{item['date_rejected']}`
- **Point of Contact**: `{item['recruiter']}`

---

## 2. Post-Mortem & Strategic Key Learnings
- **Core Insight**: {item['takeaway']}
- **Actionable Adjustment**: Keep monitoring engineering openings and re-apply once cooldown expires.
"""
        with open(file_path, "w") as f:
            f.write(content)
        print(f"  Created archive tracker: {file_path}")

def create_company_dossiers():
    print("Creating company dossiers...")
    companies = [
        {"name": "Teza Technologies", "file": "Teza-Technologies.md", "tier": "Tier 1 Quant Prop", "type": "High-Frequency & Systematic Trading", "comp": "$300k - $500k+", "hq": "Chicago / New York", "desc": "Founded by Misha Malyshev, Teza is an elite quantitative trading firm focused on algorithmic trading, statistical arbitrage, and machine learning across equities, futures, and FX."},
        {"name": "AT&T Labs", "file": "ATT-Labs.md", "tier": "Tier 1 Telecommunications R&D", "type": "Distributed Systems & Network Architecture", "comp": "$180k - $230k", "hq": "Middletown, NJ / Dallas, TX", "desc": "AT&T Labs Research is one of the birthplace institutions of Unix, C/C++, and modern distributed networking, operating massive scale low-latency infrastructure."},
        {"name": "DRW", "file": "DRW.md", "tier": "Tier 1 Proprietary Trading", "type": "Principal Trading & Market Making", "comp": "$250k - $450k+", "hq": "Chicago, IL / New York, NY", "desc": "DRW is a diversified principal trading firm trading across equities, fixed income (FICC), commodities, energy, and cryptocurrency (Cumberland)."},
        {"name": "Talan", "file": "Talan.md", "tier": "Tier 2 Financial IT Consulting", "type": "Capital Markets Technology", "comp": "$160k - $200k", "hq": "Paris / New York", "desc": "Talan is an international consulting and technology integration firm specializing in market data, low-latency execution engines, and investment banking infrastructure."},
        {"name": "Fidelity Investments", "file": "Fidelity-Investments.md", "tier": "Tier 1 Asset Management", "type": "Quantitative Asset Management & Brokerage", "comp": "$190k - $260k", "hq": "Boston, MA", "desc": "Fidelity Investments is a multi-trillion dollar asset manager with major quantitative development groups in Fixed Income, Equity Quantitative Research, and Trading Systems."},
        {"name": "Renaissance Technologies", "file": "Renaissance-Technologies.md", "tier": "Tier S Quant Hedge Fund", "type": "Systematic Alpha / Medallion Fund", "comp": "$500k - $1M+", "hq": "East Setauket, NY", "desc": "Renaissance Technologies (Rentec) is widely regarded as the most successful quantitative hedge fund in history, famous for the Medallion Fund."},
        {"name": "FalconX", "file": "FalconX.md", "tier": "Tier 1 Crypto Prime Brokerage", "type": "Digital Assets Institutional Execution", "comp": "$200k - $320k", "hq": "San Mateo, CA / New York, NY", "desc": "FalconX is an institutional crypto prime brokerage and algorithmic liquidity provider powering institutional trading in digital assets."},
        {"name": "Millennium Management", "file": "Millennium-Management.md", "tier": "Tier 1 Multi-Manager Hedge Fund", "type": "Multi-Strategy Alpha Pods", "comp": "$300k - $600k+", "hq": "New York, NY", "desc": "Millennium Management is a premier $60B+ multi-strategy hedge fund operating independent quantitative trading and portfolio manager pods."},
        {"name": "Point72", "file": "Point72.md", "tier": "Tier 1 Multi-Manager Hedge Fund", "type": "Systematic & Fundamental Equities", "comp": "$300k - $600k+", "hq": "Stamford, CT / New York, NY", "desc": "Point72 Asset Management operates Cubist Systematic Strategies, its quantitative investing business deploying automated computer-driven trading models."},
        {"name": "D. E. Shaw", "file": "DE-Shaw.md", "tier": "Tier S Quant Hedge Fund", "type": "Systematic & Discretionary Multi-Asset", "comp": "$350k - $700k+", "hq": "New York, NY", "desc": "The D. E. Shaw group is a global investment and technology development firm with an elite reputation for mathematical rigor and computational depth."},
        {"name": "Squarepoint Capital", "file": "Squarepoint-Capital.md", "tier": "Tier 1 Systematic Hedge Fund", "type": "Quantitative Trading", "comp": "$250k - $450k+", "hq": "New York / London / Paris", "desc": "Squarepoint Capital is a systematic investment manager executing quantitative strategies across global financial markets with advanced technology infrastructure."},
        {"name": "Schonfeld", "file": "Schonfeld.md", "tier": "Tier 1 Multi-Manager Hedge Fund", "type": "Quantitative & Fundamental Equities / Macro", "comp": "$280k - $500k+", "hq": "New York, NY", "desc": "Schonfeld Strategic Advisors operates quantitative statistical arbitrage and fundamental investing pods with advanced execution technology."},
        {"name": "Old Mission Capital", "file": "Old-Mission-Capital.md", "tier": "Tier 1 ETF Market Maker", "type": "Quantitative Market Making", "comp": "$250k - $450k+", "hq": "Chicago, IL / New York, NY", "desc": "Old Mission Capital is an institutional quantitative trading firm and market maker in global ETFs, fixed income, and equities."},
        {"name": "Tower Research Capital", "file": "Tower-Research-Capital.md", "tier": "Tier 1 High Frequency Trading", "type": "Ultra Low Latency Prop Trading", "comp": "$300k - $600k+", "hq": "New York, NY", "desc": "Tower Research Capital is one of the oldest and most successful high-frequency algorithmic trading firms in the world."},
        {"name": "The Voleon Group", "file": "The-Voleon-Group.md", "tier": "Tier 1 Machine Learning Hedge Fund", "type": "Statistical Arbitrage & AI", "comp": "$300k - $550k+", "hq": "Berkeley, CA / New York, NY", "desc": "The Voleon Group applies cutting-edge machine learning and statistical physics principles to global financial markets trading."}
    ]
    
    for c in companies:
        file_path = os.path.join(COMPANIES_DIR, c["file"])
        content = f"""---
company: "{c['name']}"
tier: "{c['tier']}"
industry: "{c['type']}"
headquarters: "{c['hq']}"
compensation_tier: "{c['comp']}"
tags:
  - company-intel
  - financial-engineering
  - {c['tier'].lower().replace(' ', '-')}
---

# {c['name']}

> [!INFO] Profile Overview
> **Tier**: `{c['tier']}` | **Focus**: {c['type']}
> **HQ**: {c['hq']} | **Compensation Expectation**: `{c['comp']}`

---

## 1. Executive Summary
{c['desc']}

---

## 2. Core Technical Stacks & Interview Focus
- **Languages**: Modern C++ (C++17/20/23), High-performance Python, Cython, SQL.
- **Systems Core**: Low-latency networking (Kernel bypass, Solarflare OpenOnload, DPDK), lock-free data structures, cache optimization, NUMA architecture.
- **Financial Engineering**: Order matching algorithms, Market Data Feeds (ITCH/OUCH, FIX, CME MDP 3.0), Options pricing (Black-Scholes, Greeks), Statistical Arbitrage, High-dimensional time series.

---

## 3. Related Command Center Trackers
```dataview
TABLE role, stage, priority, date_applied, next_action
FROM "16-Interview-Command-Center/03-Pipeline"
WHERE company = "{c['name']}"
```
"""
        with open(file_path, "w") as f:
            f.write(content)
        print(f"  Created company dossier: {file_path}")

def main():
    ensure_dir(ACTIVE_DIR)
    ensure_dir(ARCHIVE_DIR)
    ensure_dir(COMPANIES_DIR)
    
    create_active_trackers()
    create_archive_trackers()
    create_company_dossiers()
    print("\n✅ All active/archive trackers and company dossiers successfully generated!")

if __name__ == "__main__":
    main()
