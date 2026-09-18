#!/usr/bin/env python3
"""
build_complete_vault_dataset.py
Consolidates all extracted email data from:
1. google_rejections.txt (96 rejections)
2. google_interview_hits.txt (90 interview hits)
3. google_keyword_hits.json (rtr, teza, drw, talan, fidelity, next steps, feedback)
4. extracted_emails.json (Exchange & previous extractions)

Deduplicates, extracts company entity, tracks, dates, recruiters, and statuses.
"""

import json
import re
import os

IGNORE_SENDERS = [
    "notifications@github.com",
    "noreply@jobright.ai",
    "alerts@johnsonjobs.com",
    "jobs@alerts.jobot.com",
    "updates@schwarzenegger.com",
    "editorialstaff@flipboard.com",
    "no-reply@randstadusa.com",
    "naukrialerts@naukri.com",
    "hello@mindstream.news",
    "info@alerts.foundit.in",
    "noreply@medium.com",
    "thewallstreetwire@mail.beehiiv.com"
]

IGNORE_SUBJECT_KEYWORDS = [
    "run failed", "jobs you may like", "recommended jobs",
    "daily job alert", "weekly digest", "weekly wisdom", "cw ",
    "we interviewed", "why aren't i getting interviews",
    "getting interviews but no offers", "why \"we\" is costing",
    "4 interview calls in 1 week", "more interviews",
    "internet joke", "strangest job interview", "what’s actually working",
    "how do i get a job interview", "why do top 1% software engineers still fail",
    "how to use ai for your next job interview", "you’re busy...but are you getting",
    "apply less. interview more", "how to get job interviews", "this is why you get no interviews",
    "why sr. engineers still fail interviews", "[full] goodwork interviews",
    "the secrets behind successful remote interviews", "i dared to ask what a job paid",
    "kx news:", "join our 90 days interview guarantee", "9 months, 180 applications",
    "important update to your interviewready", "complete your ai interview",
    "how experienced engineers get unstuck", "cut the fluff", "mock interviews are waiting",
    "ensure feedback leads to learning", "at my first probation review", "introducing the gain framework"
]

def is_spam_or_noise(subject, sender):
    s_sub = subject.lower()
    s_snd = sender.lower()
    for ign in IGNORE_SENDERS:
        if ign in s_snd:
            return True
    for ign in IGNORE_SUBJECT_KEYWORDS:
        if ign in s_sub:
            return True
    return False

def clean_company_name(text, sender=""):
    # Common company patterns
    companies = [
        ("Bank of America", ["bank of america", "bofa", "bacjp"]),
        ("Teza Technologies", ["teza"]),
        ("DRW", ["drw"]),
        ("AT&T Labs", ["at&t", "att labs"]),
        ("Talan", ["talan"]),
        ("Fidelity Investments", ["fidelity", "fmr"]),
        ("BHFT", ["bhft"]),
        ("Sagarsoft", ["sagarsoft"]),
        ("Deloitte", ["deloitte"]),
        ("Morgan Stanley", ["morgan stanley"]),
        ("Goldman Sachs", ["goldman sachs", "goldman"]),
        ("Barclays", ["barclays"]),
        ("Renaissance Technologies", ["rentec", "renaissance"]),
        ("FalconX", ["falconx"]),
        ("Millennium Management", ["millennium", "mlp.com"]),
        ("Point72", ["point72", "cubist"]),
        ("D. E. Shaw", ["d. e. shaw", "deshaw"]),
        ("Squarepoint Capital", ["squarepoint"]),
        ("Schonfeld", ["schonfeld"]),
        ("Old Mission Capital", ["old mission", "oldmission"]),
        ("GTS", ["gts"]),
        ("Tudor Investment", ["tudor"]),
        ("3Red Partners", ["3red"]),
        ("DV Trading", ["dv trading"]),
        ("BlackEdge Capital", ["blackedge"]),
        ("Tower Research Capital", ["tower research"]),
        ("Aquatic Capital", ["aquatic"]),
        ("Garda Capital Partners", ["garda"]),
        ("Hudson River Trading", ["hrt", "hudson river"]),
        ("KKR", ["kkr"]),
        ("Radix Trading", ["radix"]),
        ("Tradeweb", ["tradeweb"]),
        ("B2C2", ["b2c2"]),
        ("Chicago Trading Company", ["chicago trading"]),
        ("TIAA", ["tiaa"]),
        ("DriveWealth", ["drivewealth"]),
        ("BMO Financial Group", ["bmo"]),
        ("Wells Fargo", ["wells fargo"]),
        ("HarbourVest Partners", ["harbourvest"]),
        ("Vanguard", ["vanguard"]),
        ("Nasdaq", ["nasdaq"]),
        ("State Street", ["state street"]),
        ("Citi", ["citi"]),
        ("Roku", ["roku"]),
        ("Summit Securities Group", ["summit securities"]),
        ("TransMarket Group", ["transmarket"]),
        ("Peak6 Apex", ["peak6", "apex fintech"]),
        ("Farther", ["farther"]),
        ("Humana", ["humana"]),
        ("DeepFin Research", ["deepfin"]),
        ("Valuematrix.AI", ["valuematrix"]),
        ("Frammer AI", ["frammer"]),
        ("The Voleon Group", ["voleon"]),
        ("LSEG", ["lseg"]),
        ("MarketAxess", ["marketaxess"]),
        ("qSpark", ["qspark"]),
        ("Altruist", ["altruist"]),
        ("Bayview Asset Management", ["bayview"]),
        ("Hudson Cove Capital", ["hudson cove"]),
        ("Xantium", ["xantium"]),
        ("Universal Creative", ["universal creative", "universalorlando"]),
        ("PermitFlow", ["permitflow"]),
        ("Mondrian Alpha", ["mondrian alpha"]),
        ("Radley James", ["radley james"]),
        ("Saragossa", ["saragossa"]),
        ("Hunter Bond", ["hunter bond"]),
        ("Selby Jennings", ["selby jennings"]),
        ("Stanford Black", ["stanford black"]),
        ("Paragon Alpha", ["paragon alpha"]),
        ("Goliath Partners", ["goliath partners"]),
        ("Analytic Recruiting", ["analytic recruiting"]),
        ("BAMM Staffing", ["bamm staffing"]),
        ("Thurn Partners", ["thurn partners"]),
        ("Tempest Vane Partners", ["tempest vane"]),
        ("Meraki Talent", ["meraki talent"]),
        ("Kelly", ["at kelly"]),
        ("Bowden Brown", ["bowden brown"]),
        ("Soho Square Solutions", ["soho square"]),
        ("Sartre Group", ["sartre"]),
        ("Pyramid Consulting", ["pyramid"]),
        ("ICONMA", ["iconma"]),
        ("Artech", ["artech"]),
        ("Matlen Silver", ["matlen silver"]),
        ("Experis", ["experis"]),
        ("Innova Solutions", ["innova solutions"]),
        ("Insight Global", ["insight global"]),
        ("Intec Select", ["intec select"])
    ]
    
    combined = (text + " " + sender).lower()
    for name, aliases in companies:
        for al in aliases:
            if al in combined:
                return name
    return "Other"

def main():
    items = []
    seen = set()

    def add_item(company, subject, sender, date, snippet, stage, account="Google"):
        if is_spam_or_noise(subject, sender):
            return
        key = (company.lower(), subject.strip().lower(), date.strip().lower())
        if key in seen:
            return
        seen.add(key)
        items.append({
            "company": company,
            "subject": subject.strip(),
            "sender": sender.strip(),
            "date": date.strip(),
            "snippet": snippet.strip()[:400],
            "stage": stage,
            "account": account
        })

    # 1. Parse google_rejections.txt
    rejections_file = "16-Interview-Command-Center/03-Pipeline/google_rejections.txt"
    if os.path.exists(rejections_file):
        with open(rejections_file) as f:
            for line in f:
                parts = line.strip().split(" | ")
                if len(parts) >= 4:
                    dt = parts[1]
                    snd = parts[2]
                    sub = parts[3]
                    co = clean_company_name(sub, snd)
                    add_item(co, sub, snd, dt, "Historical Rejection Notification", "Rejected", "Google")

    # 2. Parse google_interview_hits.txt
    interview_file = "16-Interview-Command-Center/03-Pipeline/google_interview_hits.txt"
    if os.path.exists(interview_file):
        with open(interview_file) as f:
            content = f.read()
            for rec in content.split("«R»"):
                parts = rec.strip().split("«F»")
                if len(parts) >= 3:
                    sub = parts[0]
                    snd = parts[1]
                    dt = parts[2]
                    co = clean_company_name(sub, snd)
                    add_item(co, sub, snd, dt, "Interview correspondence", "Interview", "Google")

    # 3. Parse google_keyword_hits.json
    kw_file = "16-Interview-Command-Center/03-Pipeline/google_keyword_hits.json"
    if os.path.exists(kw_file):
        with open(kw_file) as f:
            kw_data = json.load(f)
            for cat, records in kw_data.items():
                stage = "RTR" if "rtr" in cat else ("Interview" if cat in ["teza", "drw", "talan", "screening"] else "Application")
                for r in records:
                    co = clean_company_name(r["subject"], r["sender"])
                    add_item(co, r["subject"], r["sender"], r["date"], "", stage, "Google")

    # 4. Parse extracted_emails.json (Exchange & earlier)
    ex_file = "16-Interview-Command-Center/03-Pipeline/extracted_emails.json"
    if os.path.exists(ex_file):
        with open(ex_file) as f:
            ex_data = json.load(f)
            for r in ex_data:
                co = clean_company_name(r["subject"], r["sender"])
                mb = r.get("mailbox", "").lower()
                st = "Rejected" if "rejection" in mb else ("Assessment" if "hackerrank" in r["subject"].lower() or "assessment" in r["subject"].lower() else "Interview")
                add_item(co, r["subject"], r["sender"], r["date"], r.get("snippet", ""), st, r.get("account", "Exchange"))

    out_file = "16-Interview-Command-Center/03-Pipeline/master_job_pipeline.json"
    with open(out_file, "w") as f:
        json.dump(items, f, indent=2)

    print(f"✅ Successfully compiled {len(items)} curated job-related communications.")
    print(f"Saved master dataset to {out_file}")

    # Breakdown by Company
    from collections import Counter
    counts = Counter(x["company"] for x in items)
    print("\n--- Pipeline Items by Company ---")
    for comp, cnt in counts.most_common(25):
        print(f"  {comp:30}: {cnt}")

if __name__ == "__main__":
    main()
