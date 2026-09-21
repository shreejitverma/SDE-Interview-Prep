"""Vault-wide conventions shared by the tools: tracks and folder scopes.

Frontmatter schema for knowledge notes (see CLAUDE.md):
    type: concept | pattern | problem | case-study | paper | playbook | moc
    track: [sde, quant-dev, quant-research, low-latency, ai-eng, distinguished]
    level: L3 | L4 | L5 | L6 | L7 | L8+   (blank until someone assigns it)
    status: seed | draft | solid | canonical
    last_reviewed: YYYY-MM-DD           (blank until someone reviews it)
    sources: [...]
"""

ALL_TRACKS = ["sde", "quant-dev", "quant-research", "low-latency", "ai-eng", "distinguished"]

# Top-level folder -> tracks its notes serve.
TRACKS: dict[str, list[str]] = {
    "00-Start-Here": ALL_TRACKS,
    "01-CS-Foundations": ["sde"],
    "02-Programming-Languages": ["sde", "quant-dev", "low-latency"],
    "03-Data-Structures-Algorithms": ["sde", "quant-dev"],
    "04-System-Design": ["sde", "distinguished"],
    "05-Quantitative-Finance": ["quant-dev", "quant-research"],
    "06-Interview-Prep": ["sde", "quant-dev", "quant-research", "low-latency", "ai-eng"],
    "07-Project-Portfolio": ["sde"],
    "08-Distinguished-Engineering": ["distinguished"],
    "09-Engineering-Leadership": ["distinguished"],
    "10-Development-Practices": ["sde"],
    "11-Security-And-Cryptography": ["sde"],
    "12-Performance-Engineering": ["low-latency", "sde"],
    "13-Agentic-AI": ["ai-eng"],
    "14-Low-Latency-Systems": ["low-latency", "quant-dev"],
    "15-Technical-Whitepapers": ["distinguished", "sde"],
    "16-Interview-Command-Center": ALL_TRACKS,
    "CS-Subjects": ["sde"],
}

# One-line scope for generated top-level entry notes.
SCOPES: dict[str, str] = {
    "00-Start-Here": "Where to begin: the phased roadmap and the progress checklist.",
    "01-CS-Foundations": "Core computer science: operating systems, computer networks, databases, and object-oriented programming, with notes, quizzes, and runnable code.",
    "02-Programming-Languages": 'Language deep dives: the C++ and Python "Zero to Godhood" books, plus Java and JavaScript material.',
    "03-Data-Structures-Algorithms": "Data structures and algorithms: topic notes, practice-platform solutions, resources, and gold-standard C++ reference patterns.",
    "06-Interview-Prep": "Interview process preparation: behavioral answers, resume guidance, and mock interview checklists.",
    "08-Distinguished-Engineering": "Staff-plus systems depth: advanced concurrency, distributed systems internals, database internals, architecture patterns, and distributed transactions.",
    "09-Engineering-Leadership": "Technical leadership: technical writing and code review.",
    "10-Development-Practices": "Engineering practices: testing, CI/CD, and cloud-native development.",
    "11-Security-And-Cryptography": "Security: common vulnerabilities and secure coding.",
    "12-Performance-Engineering": "Performance engineering: CPU architecture effects and profiling.",
    "13-Agentic-AI": "AI engineering: the Agentic AI Zero to Godhood curriculum.",
}


def tracks_for(path: str) -> list[str]:
    return TRACKS.get(path.split("/", 1)[0], [])
